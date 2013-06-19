// osgPrimitives.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "../GeometryLib/IProcedural.h"
#include <Windows.h>

using namespace osg;
using namespace Utility::GeometryFactory;
int _tmain(int argc, _TCHAR* argv[])
{
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow( 25,25,1024,768 );
	//viewer.getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
	viewer.realize();
	typedef bool (__stdcall *GetGeomFactoryInterface)( Utility::GeometryFactory::IFactory** pInterface );
	typedef void (__stdcall *FreeFactoryInterface)( Utility::GeometryFactory::IFactory* pInterface );
	GetGeomFactoryInterface getFactoryIFace;
	FreeFactoryInterface freeInterface;
	Utility::GeometryFactory::IFactory *factory = 0; 

	HMODULE hlib = LoadLibrary(L"GeometryLib");
	if( hlib )
	{
		getFactoryIFace = (GetGeomFactoryInterface)::GetProcAddress( hlib, "_GetInterface@4" );
		freeInterface = ( FreeFactoryInterface )::GetProcAddress( hlib, "_FreeInterface@4" );
		
		if( getFactoryIFace )
			getFactoryIFace( &factory );
	}
	if( !factory )
		return -1;

	ref_ptr< Group > root = new Group;
	ref_ptr< Group > cyl_grp = new Group;
	ILoft *loft = factory->Loft();
	ILoftPath *path = loft->NewPath();
	ILoftShape *shape = loft->NewShape( PIVOT_PLANE_YZ );
	//path->AddPoint( Vec3( 10,0,0 ))->AddPoint( Vec3( 50, 25, 0 ) )->AddPoint( 100, 25, 20 )->AddPoint( 200,50,0)->AddPoint( 250, 50, 15 );
	float radius = 4;
	float pr = radius * 1.3;
	float rad = sqrt( 2*radius*radius ) - radius;
	path->AddPoint( 40, 0 , 0)
		->AddPoint( pr, 0 , 0)
		->AddPoint( rad, rad, 0 )
		->AddPoint( 0, pr, 0 )
		->AddPoint( 0, 38.5, 0 )
		->AddPoint( 0, 40, 0 )
		->AddPoint( 1.5, 40, 0 )
		->AddPoint( 120, 40, 0 )
		->AddPoint( 120,0,0 )
		->AddPoint( 100,0,0 );
	//path->SetPath( *factory->Geometry2D()->GetCircleShapePoints( 50, PIVOT_PLANE_XY, 16 ) );
	

	
	shape->SetShape( factory->Geometry2D()->GetCircleShapePoints( radius, PIVOT_PLANE_YZ, 32 ) )->CloseShape( true );
	loft->SetPath( path )->SetShape( shape )->CloseContour(true)->Realize( cyl_grp );
	root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 200,0,0 ), Vec4( 0,1,0,1)));
	root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 0,200,0 ), Vec4( 1,0,0,1)));
	root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 0,0,200 ), Vec4( 0,0,1,1)));
	root->addChild( cyl_grp );
	/*cyl_grp->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new Texture2D( osgDB::readImageFile( "Dirt.jpg" )), StateAttribute::ON );*/
	viewer.setSceneData( root.get() );
	viewer.run();
	freeInterface( factory );
	return 0;
}