#include "stdafx.h"
#include "LoftFactory.h"
#include "Geometry.h"

using namespace osg;
using namespace Utility::GeometryFactory;


namespace Utility
{
	namespace GeometryFactory
	{
#define RADIUS_SLICE_DEGREE_STEP 4.0

		template< class T >
		void deleteUsedObject ( T ptr )
		{
			if( ptr )
			{
				delete ptr;
				ptr = nullptr;
			}
		}

		void incrementIndex( Loft::CONTROL_POINT &p )
		{
			p.Index( p.Index()+1 );
		}

		void decrementIndex( Loft::CONTROL_POINT &p )
		{
			p.Index( p.Index()-1 );
		}
	}
	
	bool isPoint( const osg::Vec3&point, osg::Vec3& pos )
	{
		return point == pos;
	}
}

Loft::Path* Loft::Path::AddPoint( const osg::Vec3 &point )
{
	if( point.valid() )
	{
		m_Anchors.get()->push_back( point );
		m_ControlPoints.push_back( CONTROL_POINT());
		m_ControlPoints.back().Index( m_ControlPoints.size()-1 );
		_dirty = true;
		//generatePath();
	}
	return this;
}

Loft::Path* Loft::Path::AddPoint( const float x, const float y,const float z )
{
	return AddPoint( Vec3( x, y, z ) );
}


Loft::Path* Loft::Path::InsertPoint( size_t after, const osg::Vec3 &point )
{
	if( after > m_Anchors->size()-1 )
		throw std::out_of_range( "Index out of range. Array to small or you try to insert after last element" );
	m_Anchors->insert( m_Anchors->begin() + after, point );
	CONTROL_POINT cp;
	cp.Index( after );
	m_ControlPoints.insert( m_ControlPoints.begin() + after ,cp);
	reindexPoints( m_ControlPoints.begin() + after );
	_dirty = true;
	//generatePath();
	return this;
}

Loft::Path* Loft::Path::AddPoint( const osg::Vec3 &point, POINT_TYPE type, float radius )
{
	AddPoint( point );
	CONTROL_POINT& cp = m_ControlPoints.back();
	cp.SetType( type );
	//cp.SetCornerRadius( radius );
	SetCornerRadius( &cp, radius );
	_dirty = true;
	//generatePath();
	return this;
}

Loft::Path* Loft::Path::AddPoint( const float x, const float y,const float z, POINT_TYPE type, float radius )
{
	AddPoint( osg::Vec3( x,y,z ), type, radius );
	_dirty = true;
	//generatePath();
	return this;
}

Loft::Path* Loft::Path::InsertPoint( size_t after, const float x, const float y,const float z )
{
	return InsertPoint( after, Vec3( x,y,z ));
}

osg::Vec3Array * Loft::Path::Get(){ return m_Path.get(); }

void Loft::Path::Clear(){ m_Path->clear(); }


void Loft::Path::generatePath()
{
	if( _dirty )
	{
		m_Path->clear();

		CONTROL_POINTS::iterator itr = m_ControlPoints.begin();
		CONTROL_POINTS::iterator itr_e = m_ControlPoints.end();
		int index = 0;
		while( itr != itr_e )
		{
			//CONTROL_POINT cp = m_ControlPoints.at( index );
			switch( itr->Type() )
			{
			case PT_ROUND:
				if( !createRoundedCorner( m_Path, index ) )
					m_Path->push_back(  m_Anchors->at( itr->Index() ) );
				break;
			case PT_CORNER: default:
				m_Path->push_back( m_Anchors->at( itr->Index() ) );
				break;
			};
			++index;
			++itr;
		}
		_dirty = false;
	}
}


bool Loft::PickPoint( const osg::Vec3& pos, IControlPoint **point )
{
	*point = 0;
	Vec3Array *cp = m_pPath->GetControlPointsArray();
	Vec3Array::iterator itr = std::find_if( cp->begin(), cp->end(), std::bind( isPoint, std::placeholders::_1, pos ));
	if ( itr != cp->end() )
	{
		*point = &m_pPath->GetControlPoints().at( itr - cp->begin() );
		return true;
	}
	else
		return false;
}

bool Loft::Path::createRoundedCorner( osg::Vec3Array *arr, size_t corner_index )
{
	if( corner_index == 0 || corner_index == m_ControlPoints.size()-1 )
		return false;
	CONTROL_POINT &current = m_ControlPoints.at( corner_index );
	CONTROL_POINT &prev = m_ControlPoints.at( corner_index -1 );
	CONTROL_POINT &next = m_ControlPoints.at( corner_index + 1 );

	/*if( prev.Type() == PT_ROUND || next.Type() == PT_ROUND )
		return false;*/

	/*Vec3 currPos = *current.Position();
	Vec3 prevPos = *prev.Position();
	Vec3 nextPos = *next.Position();*/

	Vec3 currPos = m_Anchors->at( corner_index ) ;
	Vec3 prevPos = m_Anchors->at( corner_index-1 );
	Vec3 nextPos = m_Anchors->at( corner_index+1 );

	Vec3 AB = currPos - prevPos;
	double AB_len = AB.length();
	Vec3 BC =  nextPos - currPos;
	double BC_len = BC.length();
	Vec3 ABC_norm = ( AB ^ BC );
	Vec3 HO = ABC_norm ^ AB;
	HO.normalize();
	double R = current.GetCornerRadius();
	HO *= R;
	double corner_angle = osg::PI - acos( AB*BC/( AB_len * BC_len)) ;
	double HB = R / tan( corner_angle/2.0 );
	AB.normalize();
	Vec3 AH = AB*( AB_len - HB );
	Vec3 WH = prevPos + AH;
	Vec3 WO = WH + HO;
	
	arr->push_back( WH );
	Vec3 OH = WH - WO;
	BC.normalize();
	Vec3 final =currPos + BC * HB;
	Vec3 OH_ = final - WO;
	double kink_angle = acos( OH*OH_/( R * R )) ;
	double angle_step = kink_angle / 16.0 ;
	double radial_shift = osg::DegreesToRadians( RADIUS_SLICE_DEGREE_STEP );
	double steps = kink_angle / radial_shift;

	for( int  i = 0; i < (int)steps; i++ )
	{
		Matrixd rm = Matrixd::rotate( -radial_shift, ABC_norm );
		OH = Matrixd::transform3x3( rm, OH );
		arr->push_back( WO + OH );
	}
	if( (int)steps != steps ) // 
	{
		double lastDelta = (int)steps*radial_shift - kink_angle;
		Matrixd rm = Matrixd::rotate( lastDelta , ABC_norm );
		OH = Matrixd::transform3x3( rm, OH );
		arr->push_back( WO + OH );
	}

	/*for( double i = 0; i < kink_angle ; i += angle_step )
	{
		
		Matrixd rm = Matrixd::rotate( -angle_step, ABC_norm );
		OH = Matrixd::transform3x3( rm, OH );
		arr->push_back( WO + OH );
	}*/
	return true;
}


Vec3& Loft::Path::operator[]( size_t idx )
{
	if( !m_Path->empty() && idx < m_Path->size() )
	{
		return (*m_Path)[idx];
	}
	throw std::out_of_range(  "Index out of range" );
}


Loft::Shape* Loft::Shape::AddPoint( const osg::Vec3 &point )
{
	if( point.valid() )
		m_Shape->push_back( point );
	return this;
}

Loft::Shape* Loft::Shape::AddPoint( const float x, const float y,const float z )
{
	return AddPoint( Vec3( x, y, z ));
}

Loft::Shape* Loft::Shape::CloseShape( bool close )
{
	m_bCloseShape = close;
	return this;
}

Loft::Shape* Loft::Shape::SetShape( Vec3Array *shape )
{
	m_Shape = shape;
	return this;
}
void Loft::Shape::Clear(){ m_Shape->clear(); }

osg::Vec3Array *Loft::Shape::Get(){ return m_Shape; }

Loft* Loft::SetPath( ILoftPath *p )
{
	m_pPath = (Path*)p;
	return this;
}

Loft* Loft::SetShape( ILoftShape *s )
{
	m_pShape = (Shape*)s;
	return this;
}

//bool Loft::Realize( osg::Group *parentGroup )
bool Loft::Realize(  )
{
	
	
	if( !m_pPath || !m_pShape  )
		return false;

	m_pPath->generatePath();
	int num_path_points = m_pPath->Get()->size();
	if( num_path_points <= 1 )
		return false;
	
	// Control points
	// First and next_first for the zero slice shape points
	// Last and pre_last for the last slice shape points
	Vec3Array *cpar = m_pPath->GetControlPointsArray();

	Vec3 first = (*m_pPath)[0];
	Vec3 next_first = (*m_pPath)[1];
	Vec3 last = (*m_pPath)[ num_path_points-1 ];
	Vec3 pre_last = (*m_pPath)[ num_path_points-2 ];
	
	//Vec3 first = *(cpar->begin()); //������ ����� ����
	//Vec3 next_first = *(cpar->begin()+1); // ������ ����� ����
	//Vec3 last = *(cpar->end()-1); // ��������� �����
	//Vec3 pre_last = *(cpar->end()-2); // ������������� �����

	// ���� ���� ������� �� 2� �����, �� first == pre_last, next_first == last

	m_ShapeSlices.clear();
	ref_ptr< MatrixTransform > mt_first = new MatrixTransform;
	Matrixd all_translate = Matrixd::translate( first ); // ������������� �������� �������
	mt_first->setMatrix( Matrixd::rotate( m_pShape->GetDirection(), next_first - first ) * all_translate );
	ref_ptr< Vec3Array> ar = static_cast< Vec3Array*>( m_pShape->Get()->clone( CopyOp::DEEP_COPY_ARRAYS ));
	m_ShapeSlices.push_back( ar );
	Geometry2D::Get().TransformPoints( m_ShapeSlices[0].get(), mt_first );

	if( num_path_points > 2 ) // More then two points - complex path
	{
		for( int i = 1; i < num_path_points-1 ; ++i ) // �������� �� ���� ������, ����� ���������
		{
			Vec3 i_minus_1 = (*m_pPath)[i-1]; // ���������� �����
			Vec3 i_plus_1 = (*m_pPath)[i+1]; // �������� �����
			Vec3 i_curr = (*m_pPath)[i]; // ������� �����
			Vec3 dir0 = i_curr- i_minus_1; 
			Vec3 dir1 = -i_curr + i_plus_1;
			Vec3 direction = dir0 + dir1; // ����������� dir0 � dir1
			m_ShapeSlices.push_back( static_cast< Vec3Array*>( m_pShape->Get()->clone( CopyOp::DEEP_COPY_ARRAYS )));
			ref_ptr< MatrixTransform > mt = new MatrixTransform;
			all_translate = all_translate*Matrixd::translate( dir0 );
			mt->setMatrix( Matrixd::rotate( m_pShape->GetDirection(), direction ) * all_translate );
			Geometry2D::Get().TransformPoints( m_ShapeSlices.back().get() , mt );
		}
		//m_ShapeSlices.push_back( static_cast< Vec3Array*>( m_ShapeSlices.back().get()->clone( CopyOp::DEEP_COPY_ARRAYS )));
		m_ShapeSlices.push_back( static_cast< Vec3Array*>( m_pShape->Get()->clone( CopyOp::DEEP_COPY_ARRAYS )));
		ref_ptr< MatrixTransform > mt = new MatrixTransform;
		all_translate *=Matrixd::translate( last - pre_last );
		mt->setMatrix(  Matrixd::rotate( m_pShape->GetDirection(), last - pre_last ) *  all_translate );
		Geometry2D::Get().TransformPoints( m_ShapeSlices.back().get() , mt );
	}
	else
	{
		Vec3Array *prev_shape = m_ShapeSlices.back().get();
		ref_ptr< MatrixTransform > mt_last = new MatrixTransform;
		mt_last->setMatrix(  mt_last->getMatrix() *  Matrixd::translate( last - pre_last ) );
		m_ShapeSlices.push_back( static_cast< Vec3Array*>( prev_shape->clone( CopyOp::DEEP_COPY_ARRAYS )));
		Geometry2D::Get().TransformPoints( m_ShapeSlices[m_ShapeSlices.size()-1] , mt_last );
	}

	
	makeGeometry( m_ModelGroup );
	return true;
}

void Loft::makeGeometry( osg::Group *g )
{
	g->removeChild( 0, g->getNumChildren());
	int total_points = m_pShape->Get()->size();
	int num_slices = m_ShapeSlices.size();
	for( int slice_index = 0; slice_index < num_slices-1 ; ++slice_index )
	{
		Geode *geo = Geometry3D::Get().CreateShape( m_ShapeSlices[slice_index], m_ShapeSlices[slice_index+1], m_pShape->IsClosed(), EditMode() );
		g->addChild( geo );
	}

	if( m_bClosed )
	{
		Geode *geo = Geometry3D::Get().CreateShape( m_ShapeSlices[ num_slices-1 ], m_ShapeSlices[ 0 ], m_pShape->IsClosed(), EditMode() );
		g->addChild( geo );
	}
	if( EditMode() )
	{
		g->addChild( Geometry3D::Get().DrawLine( m_pPath->GetControlPointsArray(), Vec4( 1.0,1.0,0.0,1.0 ), true ));
		//g->addChild( Geometry3D::Get().DrawPoint( m_pPath->GetControlPointsArray(), Vec4( 1.0,1.0,0.0,1.0 )));
	}
}
// create a Loft::Path new instance
ILoftPath *Loft::NewPath()
{
	Path *obj = new Loft::Path();
	m_PathObjects.push_back( obj );
	return obj ;
}
			// create a Loft::Shape new instance
ILoftShape *Loft::NewShape( UT_PIVOT_PLANE pivotPlane )
{
	Shape *obj = new Loft::Shape( pivotPlane );
	m_ShapeObjects.push_back( obj );
	return obj;
}

Loft::~Loft()
{
	std::for_each( m_PathObjects.begin(), m_PathObjects.end(), deleteUsedObject< Path* > );
	std::for_each( m_ShapeObjects.begin(), m_ShapeObjects.end(), deleteUsedObject< Shape* > );
}


void Loft::Path::reindexPoints( CONTROL_POINTS::iterator from )
{
	std::for_each( from+1, m_ControlPoints.end(), incrementIndex );
	_asm nop;
	/*int start_idx = from - m_ControlPoints.begin();
	for( ; from != m_ControlPoints.end(); ++from )
		from->Index( start_idx++ );*/
}

bool Loft::IsValidSegment( const osg::Vec3& p0, const osg::Vec3& p1 )
{
	IControlPoint *cp0 = 0;
	IControlPoint *cp1 = 0;
	if( PickPoint( p0, &cp0 ) && PickPoint( p1, &cp1 ))
		return !( cp0->Type() == PT_ROUND || cp1->Type() == PT_ROUND );
	return false;
}

void Loft::SetEditMode( bool editMode )
{
	EditMode() = editMode;
	Realize();
}

bool Loft::GetPointPosition( size_t index, osg::Vec3 &point )
{
	if( m_pPath )
	{
		Vec3Array *ar = m_pPath->GetControlPointsArray();
		if( index >= ar->size() )
			return false;
		point = ar->at( index );
		return true;
	}
	return false;
}

bool Loft::Path::RemovePoint( size_t idx )
{
	if( idx >= m_Anchors->size() )
		return false;
	m_Anchors->erase( m_Anchors->begin() + idx );
	m_ControlPoints.erase( m_ControlPoints.begin()+idx );
	std::for_each( m_ControlPoints.begin()+idx-1, m_ControlPoints.end(), decrementIndex );
	return true;
}

void Loft::Path::SetCornerRadius( IControlPoint *p, float R )
{
	int idx = p->Index();
	if( idx == 0 || idx == m_ControlPoints.size()-1 )
		return;
	CONTROL_POINT prev_cp = m_ControlPoints.at( idx - 1 );
	CONTROL_POINT next_cp = m_ControlPoints.at( idx + 1 );
	Vec3 prev, next;
	Vec3 current = m_Anchors->at( idx );

	double Rleft = MAXDWORD , Rright = MAXDWORD;

	prev = m_Anchors->at( idx-1 );
	next = m_Anchors->at( idx+1 );
	
	if( prev_cp.Type() == PT_ROUND )
		Rleft = getFreeLength( prev_cp, true );
	else
		Rleft = (current - prev).length();
	
	if( next_cp.Type() == PT_ROUND )
		Rright = getFreeLength( next_cp, false );
	else
		Rright = (next - current ).length();

	double maxR = 0.9*getMaxAvaliableRadius( current-prev, Rleft, next-current, Rright );
	dynamic_cast< Loft::ControlPoint*>( p )->SetCornerRadius( min( maxR , R ));
}

double Loft::Path::getFreeLength( CONTROL_POINT &pt, bool prevPoint )
{
	int i = pt.Index();
	CONTROL_POINT prev = m_ControlPoints.at( i-1 );
	CONTROL_POINT next = m_ControlPoints.at( i+1 );
	Vec3 c = m_Anchors->at( i );
	Vec3 p = c - m_Anchors->at( i-1 );
	Vec3 n = m_Anchors->at( i+1 ) - c;
	
	double corner_angle = osg::PI - acos( p*n/( p.length() * n.length())) ;
	double HB = pt.Radius() / tan( corner_angle/2.0 );
	return prevPoint ? n.length() - HB : HB;
}

double Loft::Path::getMaxAvaliableRadius(  osg::Vec3 left, const double &freeLeft,  osg::Vec3 right, const double &freeRight )
{
	left.normalize();
	right.normalize();
	double L = min( freeLeft, freeRight );
	double corner_angle = osg::PI - acos( left * right ) ;
	return L * tan( corner_angle / 2.0 );
}

bool Loft::SetCornerPosition( size_t cornerIndex, const osg::Vec3 &newPos )
{
	if( m_pPath )
	{
		Vec3Array *arr = m_pPath->GetControlPointsArray();
		if( cornerIndex >= arr->size() )
			return false;
		(*arr)[ cornerIndex ].set( newPos );
		m_pPath->setDirty();
		return true;
	}
	return false;
}