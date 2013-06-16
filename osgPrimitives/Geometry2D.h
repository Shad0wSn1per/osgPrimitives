#pragma once
#ifndef __OSG_GEOMETRY_2D_H__
#define __OSG_GEOMETRY_2D_H__


namespace Utility
{
	namespace Geometry2D
	{
		typedef enum 
		{
			PIVOT_PLANE_XY,
			PIVOT_PLANE_YZ,
			PIVOT_PLANE_XZ
		} UT_PIVOT_PLANE;
		class GeometryFactory
		{
			static Utility::Geometry2D::GeometryFactory *m_pInstance;
			GeometryFactory(){}

		public:

			static Utility::Geometry2D::GeometryFactory& Get()
			{
				if( !m_pInstance )
					m_pInstance = new GeometryFactory();
				return *m_pInstance;
			}

			static GeometryFactory *GetPtr()
			{
				if( !m_pInstance )
					m_pInstance = new GeometryFactory();
				return m_pInstance;
			}

			~GeometryFactory()
			{
				if( m_pInstance )
					delete m_pInstance;
			}

			osg::Vec3Array *GetCircle( double radius, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ, int numSegments = 16  );

			inline osg::Vec3d calcArcPoint( double  pointAngle, UT_PIVOT_PLANE pivotPlane )
			{
				switch( pivotPlane )
				{
				default:
					return osg::Vec3d( cos( pointAngle), sin( pointAngle), 0);
				case PIVOT_PLANE_XZ:
					return osg::Vec3d( cos( pointAngle), 0, sin( pointAngle ) );
				case PIVOT_PLANE_YZ:
					return osg::Vec3d( 0, sin( pointAngle), cos( pointAngle));
				}
			}


		};
	};
};


#endif
