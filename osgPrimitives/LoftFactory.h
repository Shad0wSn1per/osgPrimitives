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
			class Path
			{
				typedef std::vector< osg::Vec3 > PATH;
				PATH m_Path;

			public:
				Path& AddPoint( const osg::Vec3 &point );
				void Clear();
				PATH Get();
			};

			template < class T > class Shape
			{
				typedef std::vector< T > SHAPE;
				SHAPE m_Shape;
			public:
				Shape& AddPoint( const T &point );
				void Clear();
				SHAPE Get();
			};

		public:
			
		};
	}
}

#endif