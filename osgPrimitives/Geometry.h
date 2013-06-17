#pragma once
#ifndef __OSG_GEOMETRY_2D_H__
#define __OSG_GEOMETRY_2D_H__
#include "Singleton.h"


namespace Utility
{
	typedef enum 
		{
			PIVOT_PLANE_XY,
			PIVOT_PLANE_YZ,
			PIVOT_PLANE_XZ,
			PIVOT_NONE = 0xFF

		} UT_PIVOT_PLANE;

	namespace Geometry3D 
	{
		class GeometryFactory: public Singleton< Utility::Geometry3D::GeometryFactory >
		{
			
		};

	};

	namespace Geometry2D
	{
		
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

			osg::Vec3Array *GetCircleShapePoints( double radius, Utility::UT_PIVOT_PLANE basePlane = Utility::PIVOT_PLANE_YZ, int numSegments = 16  );

			inline osg::Vec3d calcArcPoint( double  pointAngle, Utility::UT_PIVOT_PLANE pivotPlane )
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

			osg::Group *GetCylinder( double R, double L, bool CapEnds = true, Utility::UT_PIVOT_PLANE basePlane = Utility::PIVOT_PLANE_YZ, int numSegments = 16 );

			void TranslatePoints( osg::Vec3Array *arr, const osg::Vec3 &translate );

			osg::Group *CreateCylinder( 
				const osg::Vec3 &from
				,const osg::Vec3 &to
				, double Radius
				, bool CapEnds = true
				, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ	, int numSegments = 16 );

			osg::Geode *DrawLine( const osg::Vec3 &from	,const osg::Vec3 &to, const osg::Vec4 &color = osg::Vec4(1.0,1.0,1.0,1.0) );

		private:

			osg::Geode* createGeometryQuad( const osg::Vec3d &p0, const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &p3 );
		};
	};
};


#endif
