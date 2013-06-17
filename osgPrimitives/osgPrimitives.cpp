// osgPrimitives.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Geometry.h"
#include "osg/Image"
#include "osg/ImageStream"

using namespace osg;
using namespace Utility::GeometryFactory;
int _tmain(int argc, _TCHAR* argv[])
{
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow( 25,25,1024,768 );
	viewer.getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
	viewer.realize();
	ref_ptr< Group > root = new Group;
	Group  *cyl = Geometry3D::Get().GetCylinder( 5, 30, true );
	MatrixTransform *mt = new MatrixTransform;
	Vec3 v1( 0,0,15 );
	Vec3 v2( 25,-5,0 );

	cyl->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new Texture2D( osgDB::readImageFile( "Dirt.jpg" )), StateAttribute::ON );

	mt->setMatrix( Matrixd::identity().rotate( Vec3( 1,0,0), v2-v1 )*Matrixd::translate( v1 ) );
	mt->addChild( cyl );
	root->addChild( Geometry3D::Get().DrawLine( Vec3(), Vec3( 100,0,0 ), Vec4( 0,1,0,1)));
	root->addChild( Geometry3D::Get().DrawLine( Vec3(), Vec3( 0,100,0 ), Vec4( 1,0,0,1)));
	root->addChild( Geometry3D::Get().DrawLine( Vec3(), Vec3( 0,0,100 ), Vec4( 0,0,1,1)));
	root->addChild( mt );
	int width = 512;
	int depth = 50;
	//root->addChild( GeometryFactory::Get().CreateCylinder( Vec3(), Vec3( 30,0,0 ), 5, false ));
	viewer.setSceneData( root.get() );
	viewer.run();
	return 0;
}