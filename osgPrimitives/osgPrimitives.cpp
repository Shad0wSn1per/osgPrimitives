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
	EditorHandler( osg::Group* object, LoftEditor* editor, ILoft *loftModel ) 
		: _model(object) 
		, m_pEditor( editor )
		, m_pLoft( loftModel )
	{
		assert( object || editor );
	} 
	virtual bool handle( const osgGA::GUIEventAdapter& ea,
		osgGA::GUIActionAdapter& aa )
	{
		switch ( ea.getEventType() )
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
			switch ( ea.getKey() )
			{
			case 'q': case 'Q':

				if( m_pEditor->IsVisible() )
				{
					m_pEditor->SetVisible( false );
					m_pLoft->Realize();
				}
				else
				{
					m_pEditor->SetVisible( true );
					_model->removeChild(0, _model->getNumChildren() );
				}
				break;
			};
			break;
		};
	}
protected:
	osg::ref_ptr<osg::Group> _model;
	Utility::GeometryFactory::LoftEditor* m_pEditor;
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
	ILoft *loft = factory->Loft( cyl_grp );
	ILoftPath *path = loft->NewPath();
	ILoftShape *shape = loft->NewShape( PIVOT_PLANE_YZ );
	//path->AddPoint( Vec3( 10,0,0 ))->AddPoint( Vec3( 50, 25, 0 ) )->AddPoint( 100, 25, 20 )->AddPoint( 200,50,0)->AddPoint( 250, 50, 15 );
	float radius = 2;
	float pr = radius * 1.3;
	float rad = sqrt( 2*radius*radius ) - radius;
	LoftEditor editor( loft );
	editor.AddPoint( Vec3( 35.05,43.83,0 ));
	editor.AddPoint( Vec3( 163.27,117.75,0 ));
	editor.AddPoint( Vec3( 273.12,0.05,0 ));
	//PathEditor::PATH_TRAECTORY::iterator point = editor.GetPoint( Vec3( 163.27,117.75,0 ) );
	int idx = editor.Find(  Vec3( 163.27,117.75,0 ) );
	if( idx != -1 )
	{
		LoftEditor::PATH_POINT &point = editor.GetPoint( idx );
		point.SetType( LoftEditor::PT_ROUND );
		point.SetCornerRadius( 100.5 );
		//path->SetPath( editor.GetPath() );
		/*path->AddPoint( 40, 0 , 0)
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
		*/


		shape->SetShape( factory->Geometry2D()->GetCircleShapePoints( radius, PIVOT_PLANE_YZ, 32 ) )->CloseShape( true );
		loft->SetPath( path )->SetShape( shape )->CloseContour(false)->Realize(  );
		root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 200,0,0 ), Vec4( 0,1,0,1)));
		root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 0,200,0 ), Vec4( 1,0,0,1)));
		root->addChild( factory->Geometry3D()->DrawLine( Vec3(), Vec3( 0,0,200 ), Vec4( 0,0,1,1)));
		root->addChild( cyl_grp );
	}
	/*cyl_grp->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new Texture2D( osgDB::readImageFile( "Dirt.jpg" )), StateAttribute::ON );*/
	viewer.setSceneData( root.get() );
	viewer.run();
	freeInterface( factory );
	return 0;
}