#include "stdafx.h"
#include "LoftFactory.h"
#include "Geometry.h"

using namespace osg;
using namespace Utility::GeometryFactory;

template< class T >
 void deleteUsedObject ( T ptr )
{
	if( ptr )
	{
		delete ptr;
		ptr = nullptr;
	}
}

Loft::Path* Loft::Path::AddPoint( const osg::Vec3 &point )
{
	if( point.valid() )
	{
		//TODO : Remove m_Path from here
		m_Path.get()->push_back( point );
		m_Anchors.get()->push_back( point );
		m_ControlPoints.push_back( CONTROL_POINT( &m_Anchors->back() ));
	}
	return this;
}

Loft::Path* Loft::Path::AddPoint( const float x, const float y,const float z )
{
	return AddPoint( Vec3( x, y, z ) );
}


Loft::Path* Loft::Path::InsertPoint( size_t after, const osg::Vec3 &point )
{
	if( after > m_Anchors->size()-2 )
		throw std::out_of_range( "Index out of range. Array to small or you try to insert after last element" );
	m_Anchors->insert( m_Anchors->begin() + after + 1, point );
	m_ControlPoints.insert( m_ControlPoints.begin() + after + 1,CONTROL_POINT( &m_Anchors->at( after + 1 )));
	return this;
}

Loft::Path* Loft::Path::AddPoint( const osg::Vec3 &point, POINT_TYPE type, float radius )
{
	AddPoint( point );
	CONTROL_POINT& cp = m_ControlPoints.back();
	cp.SetType( type );
	cp.SetCornerRadius( radius );
	return this;
}

Loft::Path* Loft::Path::AddPoint( const float x, const float y,const float z, POINT_TYPE type, float radius )
{
	AddPoint( osg::Vec3( x,y,z ), type, radius );
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
	m_Path->clear();
	Vec3Array::iterator itr = m_Anchors->begin();
	Vec3Array::iterator itr_e = m_Anchors->end();
	int index = 0;
	while( itr != itr_e )
	{
		CONTROL_POINT cp = m_ControlPoints.at( index );
		if( cp.Type() == PT_CORNER )
		{
			m_Path->push_back( *itr );
		}
		else
		{
		}
		++index;
		++itr;
	}
}

void Loft::Path::createRoundedCorner( osg::Vec3Array *arr, size_t corner_index )
{
	//int index = corner - m_Traectory.begin();
	
	if( corner_index == 0 || corner_index == m_ControlPoints.size()-2 )
		return;
	CONTROL_POINT current = m_ControlPoints.at( corner_index );
	CONTROL_POINT prev = m_ControlPoints.at( corner_index -1 );
	CONTROL_POINT next = m_ControlPoints.at( corner_index + 1 );

	Vec3 currPos = *current.Position();
	Vec3 prevPos = *prev.Position();
	Vec3 nextPos = *next.Position();

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
	for( double i = 0; i < kink_angle ; i += angle_step )
	{
		Matrixd rm = Matrixd::rotate( -angle_step, ABC_norm );
		OH = Matrixd::transform3x3( rm, OH );
		arr->push_back( WO + OH );
	}
}


Vec3& Loft::Path::operator[]( size_t idx )
{
	if( !m_Path->empty() && idx < m_Path->size() )
	{
		return (*m_Path)[idx];
	}
	throw( "Index out of range" );
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
	int num_path_points = m_pPath->Get()->size();
	if( !m_pPath || !m_pShape ||  num_path_points <= 1 )
		return false;
	
	// Control points
	// First and next_first for the zero slice shape points
	// Last and pre_last for the last slice shape points
	Vec3 first = (*m_pPath)[0];
	Vec3 next_first = (*m_pPath)[1];
	Vec3 last = (*m_pPath)[ num_path_points-1 ];
	Vec3 pre_last = (*m_pPath)[ num_path_points-2 ];

	
	ref_ptr< MatrixTransform > mt_first = new MatrixTransform;
	Matrixd all_translate = Matrixd::translate( first );
	mt_first->setMatrix( Matrixd::rotate( m_pShape->GetDirection(), next_first - first ) * all_translate );

	
	m_ShapeSlices.clear();
	ref_ptr< Vec3Array> ar = static_cast< Vec3Array*>( m_pShape->Get()->clone( CopyOp::DEEP_COPY_ARRAYS ));
	m_ShapeSlices.push_back( ar );
	Geometry2D::Get().TransformPoints( m_ShapeSlices[0].get(), mt_first );

	if( num_path_points > 2 ) // More then two points - complex path
	{
		for( int i = 1; i < num_path_points-1 ; ++i )
		{
			Vec3 i_minus_1 = (*m_pPath)[i-1];
			Vec3 i_plus_1 = (*m_pPath)[i+1];
			Vec3 i_curr = (*m_pPath)[i];
			Vec3 dir0 = i_curr- i_minus_1;
			Vec3 dir1 = -i_curr + i_plus_1;
			Vec3 direction = dir0 + dir1;
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
	int total_points = m_pShape->Get()->size();
	int num_slices = m_ShapeSlices.size();
	for( int slice_index = 0; slice_index < num_slices-1 ; ++slice_index )
	{
		Geode *geo = Geometry3D::Get().CreateShape( m_ShapeSlices[slice_index], m_ShapeSlices[slice_index+1], m_pShape->IsClosed() );
		g->addChild( geo );
	}

	if( m_bClosed )
	{
		Geode *geo = Geometry3D::Get().CreateShape( m_ShapeSlices[ num_slices-1 ], m_ShapeSlices[ 0 ], m_pShape->IsClosed() );
		g->addChild( geo );
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
