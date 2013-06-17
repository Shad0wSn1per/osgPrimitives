// osgPrimitives.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Geometry.h"

using namespace osg;
using namespace Utility::Geometry2D;
int _tmain(int argc, _TCHAR* argv[])
{
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow( 25,25,1024,768 );
	viewer.getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
	viewer.realize();
	ref_ptr< Group > root = new Group;
	Group  *cyl = GeometryFactory::Get().GetCylinder( 5, 30, false );
	MatrixTransform *mt = new MatrixTransform;
	Vec3 v1( 0,0,15 );
	Vec3 v2( 25,-5,0 );

	mt->setMatrix( Matrixd::identity().rotate( Vec3( 1,0,0), v2-v1 )*Matrixd::translate( v1 ) );
	mt->addChild( cyl );
	root->addChild( GeometryFactory::Get().DrawLine( Vec3(), Vec3( 100,0,0 ), Vec4( 0,1,0,1)));
	root->addChild( GeometryFactory::Get().DrawLine( Vec3(), Vec3( 0,100,0 ), Vec4( 1,0,0,1)));
	root->addChild( GeometryFactory::Get().DrawLine( Vec3(), Vec3( 0,0,100 ), Vec4( 0,0,1,1)));
	root->addChild( mt );
	//root->addChild( GeometryFactory::Get().CreateCylinder( Vec3(), Vec3( 30,0,0 ), 5, false ));
	viewer.setSceneData( root.get() );
	viewer.run();
	return 0;
}