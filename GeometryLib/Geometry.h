#pragma once
#ifndef __OSG_GEOMETRY_2D_H__
#define __OSG_GEOMETRY_2D_H__
#include "Singleton.h"

#include "IProcedural.h"


namespace Utility
{

	namespace GeometryFactory
	{
		using namespace Utility::GeometryFactory;
		


		class Geometry2D : public Singleton< Utility::GeometryFactory::Geometry2D >, public IGeometry2D
		{
		public :
			osg::Vec3Array *GetCircleShapePoints( double radius, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ, int numSegments = 16 );

			osg::Vec3Array *GetArcPoints( double radius,double fromDegree, double toDegree, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ, int stepDegree = 2 );

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

			void TranslatePoints( osg::Vec3Array *arr, const osg::Vec3 &translate );

			void GenerateQuadTextureCoordinates( osg::Geometry* geometry, float index, float count );

			void TransformPoints( osg::Vec3Array *arr, osg::MatrixTransform *mt );

			/*
			*
			* slice0    0(1)---1(3)---2(5)---3(7)
			*           |     /|     /|     /|
			*           |    / |    / |    / |
			*           |   /  |   /  |   /  |
			*           |  /   |  /   |  /   |    -> current Stripe
			*           | /    | /    | /    |
			*           |/     |/     |/     |
			* slice1    0(0)---1(2)---2(4)---3(8)
			*
			* in brackets () vertex index
			*/
			
			void GenerateTriangleStripTextureCoordinates( osg::Geometry* geom,float curStripeIndex,float totalStripes );


		};


		class Geometry3D : public Singleton< Geometry3D >, public IGeometry3D
		{
		public:
			osg::Group *GetCylinder( double R, double L, bool CapEnds = true, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ, int numSegments = 16 );

			osg::Group *CreateCylinder( 
				const osg::Vec3 &from
				,const osg::Vec3 &to
				, double Radius
				, bool CapEnds = true
				, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ	, int numSegments = 16 );

			osg::Geode* CreateGeometryQuad( const osg::Vec3d &p0, const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &p3 );

			osg::Geode *DrawLine( const osg::Vec3 &from	,const osg::Vec3 &to, const osg::Vec4 &color = osg::Vec4(1.0,1.0,1.0,1.0) );

			osg::Geode* CreateShape( osg::Vec3Array *slice0, osg::Vec3Array *slice1, bool closed );
		};
	};
};


#endif
