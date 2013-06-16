#include "stdafx.h"
#include "Geometry2D.h"
#include "osg/Math"

using namespace osg;
using namespace Utility::Geometry2D;


Utility::Geometry2D::GeometryFactory * Utility::Geometry2D::GeometryFactory::m_pInstance = 0;

osg::Vec3Array *GeometryFactory::GetCircle( double radius, UT_PIVOT_PLANE basePlane, int numSegments )
{
	ref_ptr< Vec3Array > arr = new Vec3Array;
	for( double i = 0; i <= 2*osg::PI; i += 2*osg::PI / numSegments )
	{
		arr->push_back( calcArcPoint( i, basePlane ) * radius );
	}
	return arr.release();
}