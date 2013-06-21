#pragma once
#ifndef __LOFT_FACTORY_H__
#define __LOFT_FACTORY_H__
#include "Singleton.h"
//#include "Enums.h"

#include "IProcedural.h"
namespace Utility
{
	namespace GeometryFactory
	{
		using namespace Utility::GeometryFactory;
		class Loft : public ILoft
		{
		public:
			

			typedef class ControlPoint : public IControlPoint
			{
				//osg::Vec3 *m_Position;
				POINT_TYPE m_Type;
				bool m_bValid;
				float m_Radius;
				int m_Index;
				
			public:
				ControlPoint()
					: m_Type( PT_CORNER )
					, m_bValid( false )
					, m_Radius( 1.0 )
					//, m_Position( 0 )
				{}

				//ControlPoint( osg::Vec3 * point)
				//	: m_Type( PT_CORNER )
				//	, m_bValid( false )
				//	, m_Radius( 1.0 )
				//	//, m_Position( point )
				//{
				//}
				/*void SetPosition( const osg::Vec3& p )
				{ 
					m_Position->set( p );
					if( !m_bValid )
						m_bValid = true;
				}*/

				/*osg::Vec3 *Position(){ return m_Position; }
				osg::Vec3 GetPosition(){ return *m_Position; }*/


				bool Valid(){ return m_bValid; }

				void SetType( POINT_TYPE t ){ m_Type = t;}

				POINT_TYPE &Type(){ return m_Type; }
				float &Radius(){ return m_Radius; }

				void SetCornerRadius( float r ){ m_Radius = r; };
				float GetCornerRadius() { return m_Radius; }
				int Index(){ return m_Index; };
				void Index( int val ){ m_Index = val; }


			} CONTROL_POINT;
			
			class Path : public ILoftPath
			{
				osg::ref_ptr< osg::Vec3Array > m_Path;
				osg::ref_ptr< osg::Vec3Array> m_Anchors;
				typedef std::vector< ControlPoint > CONTROL_POINTS;
				CONTROL_POINTS m_ControlPoints;
					
			public:
				Path* AddPoint( const osg::Vec3 &point );
				Path* AddPoint( const float x, const float y,const float z );
				Path* AddPoint( const osg::Vec3 &point, POINT_TYPE type , float radius = 1 );
				Path* AddPoint( const float x, const float y,const float z, POINT_TYPE type , float radius = 1 );
				Path* InsertPoint( size_t after, const osg::Vec3 &point );
				Path* InsertPoint( size_t after, const float x, const float y,const float z );
				bool RemovePoint( int idx );


				osg::Vec3& operator[]( size_t idx );
				void Clear();
				osg::Vec3Array *Get();
				osg::Vec3Array *GetControlPointsArray(){ return m_Anchors.get(); }
				CONTROL_POINTS  &GetControlPoints(){ return m_ControlPoints; }
				Path()
				{
					m_Path = new osg::Vec3Array;
					m_Anchors = new osg::Vec3Array;
				};

				~Path(){};
				void generatePath();
			private:
				
				bool createRoundedCorner( osg::Vec3Array *arr, size_t corner_index );
				void reindexPoints( CONTROL_POINTS::iterator from );
			};

			class Shape : public ILoftShape
			{
				osg::ref_ptr< osg::Vec3Array > m_Shape;
				
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

			Loft( osg::Group* g )
				: m_pPath(0)
				, m_pShape(0)
				, m_bClosed( false )
				, m_ModelGroup( g )
				, m_bEditMode( false )
			{
				assert( g );
			}

			~Loft();
			bool Realize();
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

			// returns current set of Path / Shape
			// returns nullptr if is not set before
			virtual ILoftPath* GetPath(){ return m_pPath; }
			virtual ILoftShape* GetShape(){ return m_pShape; }
			virtual osg::Group* GetModelGroup(){ return m_ModelGroup.get(); }

			bool PickPoint( const osg::Vec3& pos, IControlPoint **point );
			bool GetPointPosition( int index, osg::Vec3 &point );
			bool IsValidSegment( const osg::Vec3& p0, const osg::Vec3& p1 );
			bool &EditMode(){ return m_bEditMode;}
			void SetEditMode( bool editMode );

		private:
			
			typedef std::vector< osg::ref_ptr< osg::Vec3Array >> SLICES;
			SLICES m_ShapeSlices;
			std::vector< osg::MatrixTransform* > m_Transforms;
			Path * m_pPath;
			Shape * m_pShape;
			std::vector< Path* > m_PathObjects;
			std::vector< Shape* >m_ShapeObjects;
			bool m_bClosed;
			osg::ref_ptr< osg::Group > m_ModelGroup;
			bool m_bEditMode;

		private:
			void makeGeometry( osg::Group *parentGroup );
			
		};
	}
}

#endif