#pragma once
#ifndef __LOFT_FACTORY_H__
#define __LOFT_FACTORY_H__
#include "Singleton.h"
//#include "Enums.h"
//#include "osg/Array"
#include "IProcedural.h"
namespace Utility
{
	namespace GeometryFactory
	{
		using namespace Utility::GeometryFactory;
		class Loft : public ILoft
		{
		public:
			
			class Path : public ILoftPath
			{
				std::vector< osg::Vec3 > m_Path;
			public:
				typedef std::vector< osg::Vec3 > PATH;
				Path* AddPoint( const osg::Vec3 &point );
				Path* AddPoint( const float x, const float y,const float z );
				Path* SetPath( const osg::Vec3Array &path );
				Path* SetPath( const osg::Vec3Array *path );
				osg::Vec3& operator[]( size_t idx );
				void Clear();
				std::vector< osg::Vec3 > Get();
				Path(){};
				~Path(){};
			};

			class Shape : public ILoftShape
			{
				typedef std::vector< osg::Vec3 > SHAPE;
				osg::Vec3Array *m_Shape;
				
				bool m_bCloseShape;
				UT_PIVOT_PLANE m_PivotPlane;
				osg::Vec3 m_MainDirection;

			public:
				Shape* AddPoint( const osg::Vec3 &point );
				Shape* AddPoint( const float x, const float y,const float z );
				Shape* CloseShape( bool close );
				Shape* SetShape( osg::Vec3Array *shape );
				osg::Vec3 GetDirection(){ return m_MainDirection; }
				void Clear();
				osg::Vec3Array *Get();
				bool IsClosed(){ return m_bCloseShape; }
				explicit Shape( UT_PIVOT_PLANE basePlane )
				{
					m_bCloseShape = false;
					m_Shape = 0;
					switch( basePlane )
					{
					case PIVOT_PLANE_XY:
						m_MainDirection = osg::Vec3( 0,0,1 );
						break;
					case PIVOT_PLANE_YZ:
						m_MainDirection = osg::Vec3( 1,0,0 );
						break;
					case PIVOT_PLANE_XZ:
						m_MainDirection = osg::Vec3( 0,1,0 );
						break;
					default:
						throw( "[ Utility::GeometryFactory::Loft::Shape ] CTOR FAIL ! Pivot plane must be defined." );
					}
				}
				
			};

		public:

			Loft()
				: m_pPath(0)
				, m_pShape(0)
				, m_bClosed( false )
			{}

			~Loft();
			bool Realize( osg::Group *group );
			Loft* CloseContour( bool value )
			{
				m_bClosed = value;
				return this;
			} 
			bool IsClosed(){ return m_bClosed; }
			

			Loft* SetPath( ILoftPath *p );
			Loft* SetShape( ILoftShape *p );

			// create a Loft::Path new instance
			virtual ILoftPath *NewPath();
			// create a Loft::Shape new instance
			virtual ILoftShape *NewShape( UT_PIVOT_PLANE pivotPlane );

		private:
			
			typedef std::vector< osg::ref_ptr< osg::Vec3Array >> SLICES;
			SLICES m_ShapeSlices;
			std::vector< osg::MatrixTransform* > m_Transforms;
			Path * m_pPath;
			Shape * m_pShape;
			std::vector< Path* > m_PathObjects;
			std::vector< Shape* >m_ShapeObjects;
			bool m_bClosed;

		private:
			void makeGeometry( osg::Group *parentGroup );
			
		};
	}
}

#endif