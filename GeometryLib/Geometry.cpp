#include "stdafx.h"
#include "Geometry.h"
#include "osg/Math"
#include "osg/BlendFunc"
#include "osgUtil/SmoothingVisitor"
#include "osg/ShadeModel"
#include "osg/Material"
#include "osg/Point"

using namespace osg;
using namespace Utility;
using namespace Utility::GeometryFactory;


osg::Vec3Array *Geometry2D::GetCircleShapePoints( double radius, UT_PIVOT_PLANE basePlane, int numSegments )
{
	ref_ptr< Vec3Array > arr = new Vec3Array;
	for( double i = 0; i <= 2*osg::PI; i += 2*osg::PI / numSegments )
		arr->push_back( calcArcPoint( i, basePlane ) * radius );
	return arr.release();
}

osg::Vec3Array *Geometry2D::GetArcPoints( double radius, double fromDegree, double toDegree, UT_PIVOT_PLANE basePlane, int stepDegree )
{
	double F0 = osg::DegreesToRadians( fromDegree );
	double F1 = osg::DegreesToRadians( toDegree );
	double STEP = osg::DegreesToRadians( (double)stepDegree );
	ref_ptr<Vec3Array> arr =  new Vec3Array;

	for( F0; F0 < F1; F0 = F0 + STEP)
		arr->push_back( calcArcPoint( F0, basePlane )*radius );

	if ( F0 != F1 )
		arr->push_back( calcArcPoint( F1, basePlane  )*radius );
	return arr.release();
}

osg::Group *Geometry3D::GetCylinder( double R, double L, bool CapEnds, UT_PIVOT_PLANE basePlane, int numSegments )
{
	Vec3Array *P1 = Geometry2D::Get().GetCircleShapePoints( R );
	if( P1->size() > 3 )
	{
		Group *basement = new Group;
		
		
		Vec3Array *P2 = (Vec3Array*)P1->clone( CopyOp::DEEP_COPY_ARRAYS );
		Geometry2D::Get().TranslatePoints( P2, Vec3( L, 0, 0 ));
		ref_ptr< Vec3Array > quad_points = new Vec3Array;
		
		int total_points = P1->size();
		for( int i =0; i < total_points-1; ++i  )
		{
			Geode *geo = CreateGeometryQuad( P1->at( i ), P1->at( i+1 ), P2->at( i+1 ), P2->at( i ) );
			Geometry2D::Get().GenerateQuadTextureCoordinates( (Geometry*)geo->getDrawable(0), i, total_points );
			basement->addChild( geo );
		}
		Geode *last = CreateGeometryQuad( P1->at( total_points-1 ), P1->at( 0 ), P2->at( 0 ), P2->at( total_points-1 ) );
		Geometry2D::Get().GenerateQuadTextureCoordinates( (Geometry*)last->getDrawable(0), total_points-1 , total_points );
		basement->addChild( last );

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
			geo_b->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED );
			basement->addChild( geo_b.get() );

			top->setVertexArray( P2 );
			top->addPrimitiveSet( new DrawArrays( PrimitiveSet::TRIANGLE_FAN, 0, P2->size() ));
			geo_t->addDrawable( top );
			geo_t->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED );

			basement->addChild( geo_t );
		}
		return basement;
	}
	return nullptr;
}

void Geometry2D::TranslatePoints( osg::Vec3Array *arr, const osg::Vec3 &translate )
{
	for( size_t i = 0; i < arr->size(); ++i )
		(*arr)[i] += translate;
}

void Geometry2D::TransformPoints( osg::Vec3Array *arr, osg::MatrixTransform *mt )
{
	Matrix m = mt->getMatrix();
	for( size_t i = 0; i < arr->size(); ++i )
		(*arr)[i] = (*arr)[i]* m;
}



osg::Geode* Geometry3D::CreateGeometryQuad( const osg::Vec3d &p0, const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &p3 )
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
	geo->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED );
	geo->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	geo->getOrCreateStateSet()->setAttributeAndModes( new osg::BlendFunc( osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA ) );
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	(*colors)[0] = Vec4( 1.0,1.0,1.0,1.0);
	quad->setColorArray(colors.get());
	quad->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::Vec2Array> texcoord = new osg::Vec2Array(4);
	
	(*texcoord)[0] = osg::Vec2(0.0f, 0.0f);
	(*texcoord)[1] = osg::Vec2(0.0f, 1.0f); 
	(*texcoord)[2] = osg::Vec2(1.0f, 1.0f);
	(*texcoord)[3] = osg::Vec2(1.0f, 0.0f);
	quad->setTexCoordArray( 0, texcoord.get() );


	return geo;
}

osg::Group *Geometry3D::CreateCylinder( 
				const osg::Vec3 &from
				, const osg::Vec3 &to
				, double Radius
				, bool CapEnds
				, UT_PIVOT_PLANE basePlane
				, int numSegments )
{
	Group *cyl = GetCylinder( Radius, (to-from).length() , CapEnds, basePlane, numSegments );
	if( cyl )
	{
		MatrixTransform *mt =  new MatrixTransform;
		mt->setMatrix(  Matrixd::rotate( Vec3(), to - from ) * Matrixd::translate( from ) );
		mt->addChild( cyl );
		Group* root_grp = new Group;
		root_grp->addChild( mt );
		return root_grp;
	}
	return nullptr;
}

osg::Geode* Geometry3D::CreateShape( osg::Vec3Array *slice0, osg::Vec3Array *slice1, bool closed, bool wireframe )
{
	int arr_size = slice0->size();
	if( arr_size < 2 || arr_size != slice1->size() )
		return nullptr;
	Geode *geo = new Geode;
	Geometry *geom = new Geometry;
	ref_ptr< Vec3Array > strip_points = new Vec3Array;
	for( int idx = 0; idx < arr_size-1; ++idx )
	{
		strip_points->push_back( (*slice1 )[ idx ] );
		strip_points->push_back( (*slice0 )[ idx ] );
		strip_points->push_back( (*slice1 )[ idx+1 ] );
		strip_points->push_back( (*slice0 )[ idx+1 ] );
	}
	if( closed )
	{
		strip_points->push_back( (*slice1 )[ arr_size-1 ] );
		strip_points->push_back( (*slice0 )[ arr_size-1 ] );
		strip_points->push_back( (*slice1 )[ 0 ] );
		strip_points->push_back( (*slice0 )[ 0 ] );
	}
	geom->setVertexArray( strip_points );
	geom->addPrimitiveSet( new DrawArrays( PrimitiveSet::TRIANGLE_STRIP, 0, strip_points->size() ));
	osg::ref_ptr<osg::Vec4Array> colours (new osg::Vec4Array()); 
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX );
	geom->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED );
	if( wireframe )
	{
		geom->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
		colours->assign( strip_points->size(), Vec4( 0.45,0.48, 0.65,1.0  ) );
	}
	else
	{
		colours->assign( strip_points->size(), Vec4( 0.45,0.8, 1.0,1.0  ) );
	}
	geom->setColorArray(colours.get());
	geo->addDrawable( geom );
	
	//geo->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::ON | StateAttribute::OVERRIDE );
	return geo;

}

osg::Geode *Geometry3D::DrawLine( const osg::Vec3 &from	,const osg::Vec3 &to, const osg::Vec4f &color )
{
	Geometry *geom = new Geometry;
	geom->setUseDisplayList( false );
	geom->setUseVertexBufferObjects(true);
	Vec3Array *arr = new osg::Vec3Array();
	arr->push_back( from );
	arr->push_back( to );
	geom->setVertexArray( arr );
	osg::DrawArrays *m_primSet = new osg::DrawArrays;
	m_primSet->set(GL_LINES, 0, arr->size());
	geom->addPrimitiveSet(m_primSet);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(2);
	(*colors)[0] = color;
	(*colors)[1] = color;
	geom->setColorArray(colors.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX );

	Geode* geo = new Geode;
	geo->addDrawable( geom );
	geo->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED );
	return geo;
}

osg::Geode *Geometry3D::DrawLine( osg::Vec3Array *from, const osg::Vec4 &color, bool showPoints )
{
	Geometry *geom = new Geometry;
	geom->setUseDisplayList( false );
	geom->setUseVertexBufferObjects(true);
	int size = from->size();
	geom->setVertexArray( from );
	osg::DrawArrays *m_primSet = new osg::DrawArrays;
	m_primSet->set(GL_LINE_STRIP, 0, size );
	geom->addPrimitiveSet(m_primSet);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(size);
	colors->assign( size, color );
	geom->setColorArray(colors.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX );

	Geode* geo = new Geode;
	geo->addDrawable( geom );
	geo->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED );
	if( showPoints )
	{
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,size ));
		geom->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED );
		geom->getOrCreateStateSet()->setAttributeAndModes( new osg::Point( 7.0 ));
	}
	return geo;
}

void Geometry2D::GenerateQuadTextureCoordinates( osg::Geometry* geometry, float index, float count )
{
	osg::ref_ptr<osg::Vec2Array> texcoord = new osg::Vec2Array(4);
	float step = 1. / count;
	float V = index / count;
	(*texcoord)[0] = osg::Vec2( 0.0 , V );
	(*texcoord)[1] = osg::Vec2( 0.0 , V + step ); 
	(*texcoord)[2] = osg::Vec2( 1.0 , V + step );
	(*texcoord)[3] = osg::Vec2( 1.0, V );
	geometry->setTexCoordArray( 0, texcoord.get() );
}

void Geometry2D::GenerateTriangleStripTextureCoordinates( osg::Geometry* geom,float curStripeIndex,float totalStripes )
{
	osg::Vec2Array * texcoord = new osg::Vec2Array;
	float Ustep = 1 / totalStripes;
	//float Vstep = 1 / numSlicePoints;
	float currU = curStripeIndex / totalStripes;
	texcoord->push_back( osg::Vec2( currU, 0 ));
	texcoord->push_back( osg::Vec2( currU, 1 ));
	texcoord->push_back( osg::Vec2( currU+Ustep, 0 ));
	texcoord->push_back( osg::Vec2( currU+Ustep, 1 ));
	geom->setTexCoordArray( 0, texcoord );
}

osg::Geode *Geometry3D::DrawPoint( const osg::Vec3 &pos, const osg::Vec4 &color )
{
	Geode* geode = new Geode;
	Geometry *geometry = new Geometry;
	osg::ref_ptr<osg::Vec3Array> vertices (new osg::Vec3Array()); 
	vertices->push_back( pos );
	osg::ref_ptr<osg::Vec4Array> colours (new osg::Vec4Array()); 
	colours->push_back( color );
	geometry->setColorArray(colours.get());
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX );

	geometry->setVertexArray( vertices );
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,1));
	geometry->getOrCreateStateSet()->setAttributeAndModes( new osg::Point( 3.0 ));
	geode->addDrawable( geometry );
	return geode;
}

osg::Geode *Geometry3D::DrawPoint( osg::Vec3Array *from, const osg::Vec4 &color )
{
	Geode* geode = new Geode;
	Geometry *geometry = new Geometry;
	osg::ref_ptr<osg::Vec4Array> colours (new osg::Vec4Array()); 
	colours->assign( from->size(), color );
	geometry->setColorArray(colours.get());
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX );
	geometry->setVertexArray( from );
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,from->size()));
	geometry->getOrCreateStateSet()->setMode( GL_LIGHTING, StateAttribute::OFF | StateAttribute::PROTECTED );
	geometry->getOrCreateStateSet()->setAttributeAndModes( new osg::Point( 7.0 ));
	geode->addDrawable( geometry );
	return geode;
}
