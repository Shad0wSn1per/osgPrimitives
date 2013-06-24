#pragma once
#ifndef __PROCEDURAL_GEOMETRY_PATH_EDITOR_H__
#define __PROCEDURAL_GEOMETRY_PATH_EDITOR_H__
#include "osg/Vec3"
#include "../GeometryLib/IProcedural.h"
namespace Utility
{
	namespace GeometryFactory
	{
		class LoftEditor
		{
		public:

			LoftEditor( ILoft * loft );

			osg::Vec3Array *GetVertexArray();
			void AddPoint( const osg::Vec3 &pos );
			bool InsertPoint( size_t after, const osg::Vec3 &pos );
			bool RemovePoint( const osg::Vec3 &pos );
			void SetVisible( bool state );
			bool IsVisible(){ return m_bVisible; }
		private:
			osg::ref_ptr< osg::Vec3Array > m_VertexArray;
			bool m_bVisible;
			ILoft* m_Loft;
		};
	}
}
#endif