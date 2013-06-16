// osgPrimitives.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Geometry2D.h"

using namespace osg;
using namespace Utility::Geometry2D;
int _tmain(int argc, _TCHAR* argv[])
{
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow( 25,25,1024,768 );
	viewer.getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
	viewer.realize();
	ref_ptr< Geometry > geom = new Geometry;
	Vec3Array *arr = GeometryFactory::Get().GetCircle( 10, PIVOT_PLANE_YZ, 6 );
	geom->setVertexArray( arr );
	geom->addPrimitiveSet( new DrawArrays( PrimitiveSet::TRIANGLE_FAN, 0, arr->size() ));
	ref_ptr< Group > root = new Group;
	ref_ptr< Geode > geode = new Geode;
	ref_ptr< Geode > geode2 = new Geode;
	geode->addDrawable( geom );
	ref_ptr< MatrixTransform > mt2 = new MatrixTransform;
	mt2->setMatrix( Matrixd::translate( 30,0,0));
	mt2->addChild( geode2 );
	geode2->addDrawable( geom );

	root->addChild( geode );
	root->addChild( mt2 );
	viewer.setSceneData( root.get() );

	viewer.run();
	return 0;
}