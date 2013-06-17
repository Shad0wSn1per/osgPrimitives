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
	ref_ptr< Group > root = new Group;
	root->addChild( GeometryFactory::Get().GetCylinder( 10, 30, false ));
	viewer.setSceneData( root.get() );
	viewer.run();
	return 0;
}