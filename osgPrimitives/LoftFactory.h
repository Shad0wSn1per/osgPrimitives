#pragma once
#ifndef __LOFT_FACTORY_H__
#define __LOFT_FACTORY_H__
#include "Singleton.h"
namespace Utility
{
	namespace GeometryFactory
	{

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
				void Clear();
				PATH Get();
				Path(){};
				~Path(){};
			};

			class Shape
			{
				typedef std::vector< osg::Vec3 > SHAPE;
				SHAPE m_Shape;
				bool m_bCloseShape;
				

			public:
				Shape& AddPoint( const osg::Vec3 &point );
				Shape& AddPoint( const float x, const float y,const float z );
				Shape& SetCloseShape( bool close );
				Shape& SetShape( const osg::Vec3Array &shape );
				void Clear();
				SHAPE Get();
				
			};

		public:

			Loft()
				: m_pPath(0)
				, m_pShape(0)
			{}
			bool Realize( osg::Geode *geode );
			Loft& SetPath( Path *p );
			Loft& SetShape( Shape *p );

		private:

			typedef std::vector< osg::Vec3 > POINTS;
			
			std::vector< osg::MatrixTransform* > m_Transforms;
			
			Path * m_pPath;
			Shape * m_pShape;
			
		};
	}
}

#endif