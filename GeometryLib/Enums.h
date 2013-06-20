#pragma once
#ifndef __GEOMETRY_FACTORY_ENUMS_H__
#define __GEOMETRY_FACTORY_ENUMS_H__

namespace Utility
{
	namespace GeometryFactory
	{
		typedef enum 
		{
			PIVOT_PLANE_XY,
			PIVOT_PLANE_YZ,
			PIVOT_PLANE_XZ,
			PIVOT_NONE = 0xFF

		} UT_PIVOT_PLANE;

		typedef enum
		{
			PT_CORNER,
			PT_ROUND,
			PT_UNDEFINED = 0xFF
		} POINT_TYPE;
	}
}

#endif