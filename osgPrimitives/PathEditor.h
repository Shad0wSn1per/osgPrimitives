#pragma once
#ifndef __PROCEDURAL_GEOMETRY_PATH_EDITOR_H__
#define __PROCEDURAL_GEOMETRY_PATH_EDITOR_H__
#include "osg/Vec3"
namespace Utility
{
	namespace GeometryFactory
	{
		

		class PathEditor
		{
		public:

			typedef enum
			{
				PT_CORNER,
				PT_ROUND,
				PT_UNDEFINED = 0xFF
			} POINT_TYPE;
			
			typedef class PathPoint
			{
				osg::Vec3 m_Position;
				POINT_TYPE m_Type;
				bool m_bValid;
				float m_Radius;
				
			public:
				PathPoint()
					: m_Type( PT_CORNER )
					, m_bValid( false )
					, m_Radius( 1.0 )
				{}

				void SetPosition( const osg::Vec3& p )
				{ 
					m_Position = p;
					if( !m_bValid )
						m_bValid = true;
				}

				osg::Vec3 GetPosition(){ return m_Position; }

				bool Vaild(){ return m_bValid; }

				void SetType( POINT_TYPE t ){ m_Type = t;}

				POINT_TYPE Type(){ return m_Type; }

				void SetCornerRadius( float r ){ m_Radius = r; };
				float GetCornerRadius() { return m_Radius; }


			} PATH_POINT;

			typedef std::vector < PATH_POINT > PATH_TRAECTORY;
			bool IsValidSegment( const osg::Vec3& p0, const osg::Vec3& p1 );
			
			int Find( const osg::Vec3& p );
			PATH_POINT &GetPoint( int index );


			osg::Vec3Array *GetVertexArray();
			void AddPoint( const osg::Vec3 &pos );
			bool InsertPoint( int after, const osg::Vec3 &pos );
			bool RemovePoint( const osg::Vec3 &pos );
			osg::Vec3Array * GetPath();

		private:
			PATH_TRAECTORY::iterator GetPoint( const osg::Vec3& p );
			PATH_TRAECTORY m_Traectory;
			osg::ref_ptr< osg::Vec3Array > m_VertexArray;
			void createRoundedCorner( osg::Vec3Array *path, PATH_TRAECTORY::iterator corner );
		};
	}
}
#endif