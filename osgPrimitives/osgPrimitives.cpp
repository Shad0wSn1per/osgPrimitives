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
	Vec3Array *arr = GeometryFactory::Get().GetCircle( 10, PIVOT_PLANE_YZ );
	viewer.run();
	return 0;
}