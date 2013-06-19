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
		m_Path.push_back( point );
	return this;
}

Loft::Path* Loft::Path::AddPoint( const float x, const float y,const float z )
{
	return AddPoint( Vec3( x, y, z ) );
}

Loft::Path::PATH Loft::Path::Get(){ return m_Path; }

void Loft::Path::Clear(){ m_Path.clear(); }

Loft::Path* Loft::Path::SetPath( const osg::Vec3Array &path )
{
	m_Path.clear();
	m_Path.assign( path.begin(), path.end());
	return this;
}

Vec3& Loft::Path::operator[]( size_t idx )
{
	if( !m_Path.empty() && idx < m_Path.size() )
	{
		return m_Path[idx];
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

bool Loft::Realize( osg::Group *parentGroup )
{
	int num_path_points = m_pPath->Get().size();
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

	
	makeGeometry( parentGroup );
	return true;
}

void Loft::makeGeometry( osg::Group *g )
{
	int total_points = m_pShape->Get()->size();
	int num_slices = m_ShapeSlices.size();
	for( size_t slice_index = 0; slice_index < num_slices-1 ; ++slice_index )
	{
		Geode *geo = Geometry3D::Get().CreateShape( m_ShapeSlices[slice_index], m_ShapeSlices[slice_index+1], m_pShape->IsClosed() );
		//Geometry2D::Get().GenerateTriangleStripTextureCoordinates( (Geometry*)geo->getDrawable(0), slice_index, num_slices );
		g->addChild( geo );
		/*for( int point_index = 0; point_index < total_points-1; ++point_index )
		{
			Geode *geo = Geometry3D::Get().CreateGeometryQuad( 
				(*m_ShapeSlices[ slice_index ])[ point_index ],
				(*m_ShapeSlices[ slice_index ])[ point_index+1 ],
				(*m_ShapeSlices[ slice_index+1 ])[ point_index+1 ],
				(*m_ShapeSlices[ slice_index+1 ])[ point_index ] );
			Geometry2D::Get().GenerateQuadTextureCoordinates( (Geometry*)geo->getDrawable(0), point_index, total_points );
			g->addChild( geo );

		}
		if( m_pShape->IsClosed() )
		{
			int sl0 = (*m_ShapeSlices[ slice_index ]).size();
			int sl1 = (*m_ShapeSlices[ slice_index+1 ]).size();
			Geode *geo =Geometry3D::Get().CreateGeometryQuad( 
				(*m_ShapeSlices[ slice_index ])[ sl0-1 ],
				(*m_ShapeSlices[ slice_index ])[ 0 ],
				(*m_ShapeSlices[ slice_index+1 ])[ 0 ],
				(*m_ShapeSlices[ slice_index+1 ])[ sl1-1 ] );
			g->addChild( geo );
			Geometry2D::Get().GenerateQuadTextureCoordinates( (Geometry*)geo->getDrawable(0), total_points-1, total_points );
		}*/
	}

	if( m_bClosed )
	{
		Geode *geo = Geometry3D::Get().CreateShape( m_ShapeSlices[ num_slices-1 ], m_ShapeSlices[ 0 ], m_pShape->IsClosed() );
		//Geometry2D::Get().GenerateTriangleStripTextureCoordinates( (Geometry*)geo->getDrawable(0), total_points-1, total_points );
		g->addChild( geo );
		/*int num_slices = m_ShapeSlices.size();
		for( int point_index = 0; point_index < total_points-1; ++point_index )
		{
			Geode *geo = Geometry3D::Get().CreateGeometryQuad( 
				(*m_ShapeSlices[ num_slices-1 ])[ point_index ],
				(*m_ShapeSlices[ num_slices-1 ])[ point_index+1 ],
				(*m_ShapeSlices[ 0 ])[ point_index+1 ],
				(*m_ShapeSlices[ 0 ])[ point_index ] );
			Geometry2D::Get().GenerateQuadTextureCoordinates( (Geometry*)geo->getDrawable(0), point_index, total_points );
			g->addChild( geo );
		}
		if(  m_pShape->IsClosed() )
		{
			int sl0 = (*m_ShapeSlices[ num_slices-1 ]).size();
			int sl1 = (*m_ShapeSlices[ 0 ]).size();
			Geode *geo =Geometry3D::Get().CreateGeometryQuad( 
				(*m_ShapeSlices[ num_slices-1 ])[ sl0-1 ],
				(*m_ShapeSlices[ num_slices-1 ])[ 0 ],
				(*m_ShapeSlices[ 0 ])[ 0 ],
				(*m_ShapeSlices[ 0 ])[ sl1-1 ] );
			g->addChild( geo );
			Geometry2D::Get().GenerateQuadTextureCoordinates( (Geometry*)geo->getDrawable(0), total_points-1, total_points );
		}*/
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
