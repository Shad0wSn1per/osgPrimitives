// osgPrimitives.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "../GeometryLib/IProcedural.h"
#include <Windows.h>
#include "LoftEditor.h"
#include <assert.h>

using namespace osg;
using namespace Utility::GeometryFactory;

class EditorHandler :public osgGA::GUIEventHandler
{
public:
	EditorHandler(  ILoft *loftModel ) 
		: m_pLoft( loftModel )
	{
		assert( loftModel );
	} 
	virtual bool handle( const osgGA::GUIEventAdapter& ea,
		osgGA::GUIActionAdapter& aa )
	{
		switch ( ea.getEventType() )
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
			switch ( ea.getKey() )
			{
			case '[': 
				m_pLoft->SetEditMode( true );
				break;
			case ']':
				m_pLoft->SetEditMode( false);
				break;
			case '1':
				{
					int count = m_pLoft->GetPath()->GetControlPointsArray()->size();
					int idx = (int)std::rand() % count;
					Vec3 prev, next;
					if( m_pLoft->GetPointPosition( idx-1, prev ) && m_pLoft->GetPointPosition( idx+1, next ) )
					{
						Vec3 in = ( next - prev ) / 2.0;
						double z = ( 50 - rand()%100 );
						in.z() = z;
						m_pLoft->GetPath()->InsertPoint( idx, in );
						IControlPoint *cp = 0;
						if( m_pLoft->PickPoint( in, &cp ) )
						{
							cp->Type() = (POINT_TYPE)( rand()%1 );
						}

						m_pLoft->Realize();
					}
					
					break;
				}
			case '2':

				int count = m_pLoft->GetPath()->GetControlPointsArray()->size();
				if( count>1 )
				{
					m_pLoft->GetPath()->RemovePoint ( count-2 );
					m_pLoft->Realize();
				}
				break;
			};
			break;
		};
		return false;
	}
protected:
	ILoft *m_pLoft;

};



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
	root->addChild( cyl_grp );
	ILoft *loft = factory->Loft( cyl_grp );
	ILoftPath *path = loft->NewPath();
	ILoftShape *shape = loft->NewShape( PIVOT_PLANE_YZ );
	//path->AddPoint( Vec3( 10,0,0 ))->AddPoint( Vec3( 50, 25, 0 ) )->AddPoint( 100, 25, 20 )->AddPoint( 200,50,0)->AddPoint( 250, 50, 15 );
	float radius = 2;
	float pr = radius * 1.3;
	float rad = sqrt( 2*radius*radius ) - radius;
	path->AddPoint( Vec3( 35.05,43.83,0 ) );
	path->AddPoint( Vec3( 163.27,117.75,0 ));
	path->AddPoint( Vec3( 100,-20.0,0 ));
	path->InsertPoint( 2, Vec3( 273.12,0.05,0 ));
	shape->SetShape( factory->Geometry2D()->GetCircleShapePoints( radius, PIVOT_PLANE_YZ, 8 ) )->CloseShape( true );
	loft->SetPath( path )->SetShape( shape );
	IControlPoint *cp = 0;
	if( loft->PickPoint( Vec3( 163.27,117.75,0 ), &cp ))
	{
		cp->Type() = PT_ROUND;
		path->SetCornerRadius( cp, 200 );
		//cp->Radius() = 60.5;
	}

	if( loft->PickPoint( Vec3( 273.12,0.05,0 ), &cp ))
	{
		//cp->Type() = PT_ROUND;
		//path->SetCornerRadius( cp, 345 );
		//cp->Radius() = 30.5;
	}
	loft->Realize();
	root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 200,0,0 ), Vec4( 0,1,0,1)));
	root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 0,200,0 ), Vec4( 1,0,0,1)));
	root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 0,0,200 ), Vec4( 0,0,1,1)));
	osg::ref_ptr< EditorHandler > ehandler = new EditorHandler( loft );
	viewer.addEventHandler( ehandler );
	//root->addChild( factory->Geometry3D()->DrawLine( loft->GetPath()->GetControlPointsArray(), Vec4( 1.0,1.0,0.0,0.0 )));
	//	root->addChild( cyl_grp );
	//}
	/*cyl_grp->getOrCreateStateSet()->setTextureAttributeAndModes(
	0, new Texture2D( osgDB::readImageFile( "Dirt.jpg" )), StateAttribute::ON );*/
	viewer.setSceneData( root.get() );
	viewer.run();
	freeInterface( factory );
	return 0;
}
