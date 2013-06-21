 /*
This source file is part of Integra Earth

Module: Procedural Library interface
File: IProcedural.h
Created: 18.06.2013
Updated:
Autor: Golodov Aleksey
mailto:golodov@integra-s.com

Copyright (c) 2013 Integra-S JSC
-----------------------------------------------------------------------------*/

#pragma once 
#ifndef __OSG_PROCEDURAL_LIBRARY_INTERFACE_H__
#define __OSG_PROCEDURAL_LIBRARY_INTERFACE_H__
#include "Enums.h"
#include "osg/Array"
#include "osg/Vec3"


namespace Utility
{
	namespace GeometryFactory
	{
		
		struct ILoftShape
		{
			virtual ILoftShape* AddPoint( const osg::Vec3 &point ) = 0;
			virtual ILoftShape* AddPoint( const float x, const float y,const float z ) = 0;
			virtual ILoftShape* CloseShape( bool close ) = 0;
			virtual ILoftShape* SetShape( osg::Vec3Array *shape ) = 0;
			virtual osg::Vec3 GetDirection() = 0;
			virtual void Clear() = 0;
			virtual osg::Vec3Array *Get() = 0;
			virtual bool IsClosed() = 0;
		};

		struct ILoftPath
		{
			virtual ILoftPath* AddPoint( const osg::Vec3 &point ) = 0;
			virtual ILoftPath* AddPoint( const float x, const float y,const float z ) = 0;
			virtual ILoftPath* AddPoint( const osg::Vec3 &point, POINT_TYPE type , float radius = 1 ) = 0;
			virtual ILoftPath* AddPoint( const float x, const float y,const float z, POINT_TYPE type , float radius = 1 ) = 0;
			virtual ILoftPath* InsertPoint( size_t after, const osg::Vec3 &point ) = 0;
			virtual ILoftPath* InsertPoint( size_t after, const float x, const float y,const float z ) = 0;
			virtual osg::Vec3& operator[]( size_t idx ) = 0;
			virtual void Clear() = 0;
			virtual osg::Vec3Array * Get() = 0;
			virtual osg::Vec3Array *GetControlPointsArray() = 0;
			virtual bool RemovePoint( int idx ) = 0;
		};

		struct IGeometry2D
		{
			virtual osg::Vec3Array *GetCircleShapePoints( double radius, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ, int numSegments = 16 ) = 0;
			virtual osg::Vec3Array *GetArcPoints( double radius,double fromDegree, double toDegree, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ, int stepDegree = 2 ) = 0;
			virtual osg::Vec3d calcArcPoint( double  pointAngle, UT_PIVOT_PLANE pivotPlane ) = 0;
			virtual void TranslatePoints( osg::Vec3Array *arr, const osg::Vec3 &translate ) = 0;
			virtual void GenerateQuadTextureCoordinates( osg::Geometry* geometry, float index, float count ) = 0;
			virtual void TransformPoints( osg::Vec3Array *arr, osg::MatrixTransform *mt ) = 0;
		};

		struct IGeometry3D
		{
			virtual osg::Group *GetCylinder( double R, double L, bool CapEnds = true, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ, int numSegments = 16 ) = 0;

			virtual osg::Group *CreateCylinder( 
				const osg::Vec3 &from
				,const osg::Vec3 &to
				, double Radius
				, bool CapEnds = true
				, UT_PIVOT_PLANE basePlane = PIVOT_PLANE_YZ	, int numSegments = 16 ) = 0;

			virtual osg::Geode* CreateGeometryQuad( const osg::Vec3d &p0, const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &p3 ) = 0;

			virtual osg::Geode *DrawLine( const osg::Vec3 &from	,const osg::Vec3 &to, const osg::Vec4 &color = osg::Vec4(1.0,1.0,1.0,1.0) ) = 0;
			virtual osg::Geode *DrawLine( osg::Vec3Array *from, const osg::Vec4 &color = osg::Vec4(1.0,1.0,1.0,1.0), bool showPoints = false ) = 0;

			virtual osg::Geode *CreateShape( osg::Vec3Array *slice0, osg::Vec3Array *slice1, bool closed, bool wireframe = false ) = 0;
		};

		struct IControlPoint
		{
			//virtual osg::Vec3 GetPosition() = 0;
			virtual POINT_TYPE &Type() = 0;
			virtual bool Valid() = 0;
			virtual float &Radius() = 0;
			virtual int Index() = 0;
		};
		
		struct ILoft
		{
		public:
			// create a Loft::Path new instance
			virtual ILoftPath *NewPath() = 0;
			// create a Loft::Shape new instance
			virtual ILoftShape *NewShape( UT_PIVOT_PLANE pivotPlane ) = 0;

			//virtual bool Realize( osg::Group *group ) = 0;
			virtual bool Realize(  ) = 0;
			virtual ILoft* SetPath( ILoftPath *p ) = 0;
			virtual ILoft* SetShape( ILoftShape *p ) = 0;
			virtual ILoft* CloseContour( bool value ) = 0;
			virtual bool IsClosed() = 0;
			// returns a current Path, nullptr othercase
			virtual ILoftPath* GetPath() = 0;
			// returns a current Shape, nullptr othercase
			virtual ILoftShape* GetShape() = 0;
			virtual osg::Group* GetModelGroup() = 0;
			virtual bool PickPoint( const osg::Vec3& pos, IControlPoint **point ) = 0;
			virtual bool IsValidSegment( const osg::Vec3& p0, const osg::Vec3& p1 ) = 0;
			//virtual bool &EditMode() = 0;
			virtual void SetEditMode( bool editMode ) = 0;
			virtual bool GetPointPosition( int index, osg::Vec3 &point )=0;
			
		};

		struct  IFactory
		{ 
			// returns an singleton instane of a Geometry2D
			virtual Utility::GeometryFactory::IGeometry2D *Geometry2D()=0;
			// returns an singleton instane of a Geometry3D
			virtual IGeometry3D *Geometry3D() = 0;
			// returns new instance of a Loft
			virtual ILoft *Loft( osg::Group * g ) = 0;
			
		};
	}
}

#endif