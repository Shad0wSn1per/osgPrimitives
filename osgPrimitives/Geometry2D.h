#pragma once
#ifndef __OSG_GEOMETRY_2D_H__
#define __OSG_GEOMETRY_2D_H__


namespace Utility
{
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

			std::string GetName(){ return "Utility::Geometry2D::GeometryFactory"; }


		};
	};
};


#endif
