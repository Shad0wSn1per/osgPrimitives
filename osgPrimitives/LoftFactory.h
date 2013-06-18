#pragma once
#ifndef __LOFT_FACTORY_H__
#define __LOFT_FACTORY_H__
#include "Singleton.h"
#include "Enums.h"
#include "osg/Array"
namespace Utility
{
	namespace GeometryFactory
	{
		using namespace Utility::GeometryFactory;
		class Loft
		{
		public:
			
			class Path
			{
				typedef std::vector< osg::Vec3 > PATH;
				
				PATH m_Path;

			public:
				Path& AddPoint( const osg::Vec3 &point );
				Path& AddPoint( const float x, const float y,const float z );
				Path& SetPath( const osg::Vec3Array &path );
				osg::Vec3& operator[]( size_t idx );
				void Clear();
				PATH Get();
				Path(){};
				~Path(){};
			};

			class Shape
			{
				typedef std::vector< osg::Vec3 > SHAPE;
				osg::Vec3Array *m_Shape;
				
				bool m_bCloseShape;
				UT_PIVOT_PLANE m_PivotPlane;
				osg::Vec3 m_MainDirection;

			public:
				Shape& AddPoint( const osg::Vec3 &point );
				Shape& AddPoint( const float x, const float y,const float z );
				Shape& CloseShape( bool close );
				Shape& SetShape( osg::Vec3Array *shape );
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
			{}
			bool Realize( osg::Group *group );
			Loft& SetPath( Path *p );
			Loft& SetShape( Shape *p );

		private:
			//typedef std::vector< osg::Vec3 > POINTS;
			typedef std::vector< osg::ref_ptr< osg::Vec3Array >> SLICES;
			SLICES m_ShapeSlices;
			std::vector< osg::MatrixTransform* > m_Transforms;
			Path * m_pPath;
			Shape * m_pShape;

		private:
			void makeGeometry( osg::Group *parentGroup );
			
		};
	}
}

#endif