// osgPrimitives.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Geometry.h"
#include "LoftFactory.h"

using namespace osg;
using namespace Utility::GeometryFactory;
int _tmain(int argc, _TCHAR* argv[])
{
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow( 25,25,1024,768 );
	viewer.getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
	viewer.realize();
	ref_ptr< Group > root = new Group;
	//Group  *cyl = Geometry3D::Get().GetCylinder( 5, 30, true );
	MatrixTransform *mt = new MatrixTransform;
	Vec3 v1( 0,10,15 );
	Vec3 v2( 25,-5,0 );

	/*cyl->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new Texture2D( osgDB::readImageFile( "Dirt.jpg" )), StateAttribute::ON );*/

	mt->setMatrix( Matrixd::identity().rotate( Vec3( 1,0,0), v2-v1 )*Matrixd::translate( v1 ) );
	//mt->addChild( cyl );
	root->addChild( Geometry3D::Get().DrawLine( Vec3(), Vec3( 100,0,0 ), Vec4( 0,1,0,1)));
	root->addChild( Geometry3D::Get().DrawLine( Vec3(), Vec3( 0,100,0 ), Vec4( 1,0,0,1)));
	root->addChild( Geometry3D::Get().DrawLine( Vec3(), Vec3( 0,0,100 ), Vec4( 0,0,1,1)));
	//root->addChild( mt );
	int width = 512;
	int depth = 50;

	Loft::Path path;
	Loft::Shape shape( Utility::GeometryFactory::PIVOT_PLANE_YZ );
	shape.SetShape( Geometry2D::Get().GetCircleShapePoints( 10 )).CloseShape( true );
	path.AddPoint( Vec3( 10,0,0 )).AddPoint( Vec3( 50, 25, 0 ) ).AddPoint( 100, 25, 100 ).AddPoint( 200,50,0).AddPoint( 250, 50, 15 );
	Loft loft_cyl;
	Group *cyl_grp = new Group;
	if( loft_cyl.SetPath( &path ).SetShape( &shape ).Realize( cyl_grp ) )
	{
		root->addChild( cyl_grp );
		cyl_grp->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new Texture2D( osgDB::readImageFile( "Dirt.jpg" )), StateAttribute::ON );
	}
	
	

	viewer.setSceneData( root.get() );
	viewer.run();
	return 0;
}