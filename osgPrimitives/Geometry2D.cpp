#include "stdafx.h"
#include "Geometry2D.h"
#include "osg/Math"

using namespace osg;
using namespace Utility::Geometry2D;


Utility::Geometry2D::GeometryFactory * Utility::Geometry2D::GeometryFactory::m_pInstance = 0;

osg::Vec3Array *GeometryFactory::GetCircleShapePoints( double radius, UT_PIVOT_PLANE basePlane, int numSegments )
{
	ref_ptr< Vec3Array > arr = new Vec3Array;
	for( double i = 0; i <= 2*osg::PI; i += 2*osg::PI / numSegments )
	{
		arr->push_back( calcArcPoint( i, basePlane ) * radius );
	}
	return arr.release();
}

osg::Group *GeometryFactory::GetCylinder( double R, double L, bool CapEnds, UT_PIVOT_PLANE basePlane, int numSegments )
{
	Vec3Array *P1 = GetCircleShapePoints( R );
	if( P1->size() > 3 )
	{
		Group *basement = new Group;
		
		
		Vec3Array *P2 = (Vec3Array*)P1->clone( CopyOp::DEEP_COPY_ARRAYS );
		TranslatePoints( P2, Vec3( L, 0, 0 ));
		ref_ptr< Vec3Array > quad_points = new Vec3Array;
		
		int total_points = P1->size();
		for( int i =0; i < total_points-1; ++i  )
		{
			Geode *geo = createGeometryQuad( P1->at( i ), P1->at( i+1 ), P2->at( i+1 ), P2->at( i ) );
			basement->addChild( geo );
			/*ref_ptr< Geode> geo = new Geode;
			ref_ptr< Geometry > quad = new Geometry;
			quad_points->push_back( P1->at( i ) );
			quad_points->push_back( P1->at( i+1 ) );
			quad_points->push_back( P2->at( i+1 ) );
			quad_points->push_back( P2->at( i ) );
			quad->setVertexArray( quad_points );
			quad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,i*4,4));
			geo->addDrawable( quad.get() );
			basement->addChild( geo.get() );*/
		}

		basement->addChild( createGeometryQuad( P1->at( total_points-1 ), P1->at( 0 ), P2->at( 0 ), P2->at( total_points-1 ) ) );

		if( CapEnds )
		{
			ref_ptr<Geode> geo_b = new Geode;
			ref_ptr<Geode> geo_t = new Geode;
			Geometry *bottom = new Geometry;
			Geometry *top = new Geometry;
			Vec3d norm;
			switch( basePlane )
			{
			case PIVOT_PLANE_XY:
				norm = Vec3( 0,1,0 );
				break;
			case PIVOT_PLANE_YZ:
				norm = Vec3( 1,0,0);
				break;
			case PIVOT_PLANE_XZ:
				norm = Vec3( 0,0,1);
			};
			bottom->setVertexArray( P1 );
			bottom->addPrimitiveSet( new DrawArrays( PrimitiveSet::TRIANGLE_FAN, 0, P1->size() ));
			geo_b->addDrawable( bottom );
			basement->addChild( geo_b.get() );

			top->setVertexArray( P2 );
			top->addPrimitiveSet( new DrawArrays( PrimitiveSet::TRIANGLE_FAN, 0, P2->size() ));
			geo_t->addDrawable( top );
			basement->addChild( geo_t );
		}
		return basement;
	}
}

void GeometryFactory::TranslatePoints( osg::Vec3Array *arr, const osg::Vec3 &translate )
{
	for( size_t i = 0; i < arr->size(); ++i )
		(*arr)[i] += translate;
}

osg::Geode* GeometryFactory::createGeometryQuad( const osg::Vec3d &p0, const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &p3)
{
	Geode *geo = new Geode;
	ref_ptr< Vec3Array > quad_points = new Vec3Array;
	ref_ptr< Geometry > quad = new Geometry;
	quad_points->push_back( p0 );
	quad_points->push_back( p1 );
	quad_points->push_back( p2 );
	quad_points->push_back( p3 );
	quad->setVertexArray( quad_points );
	quad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
	geo->addDrawable( quad.get() );
	return geo;
}