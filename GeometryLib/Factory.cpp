#include "stdafx.h"
#include "IProcedural.h"
#include "Factory.h"
#include "Geometry.h"
#include "LoftFactory.h"


void deleteLoft( Utility::GeometryFactory::Loft* obj )
{
	delete obj;
}

using namespace Utility::GeometryFactory;

IGeometry2D * Factory::Geometry2D()
{
	return &Geometry2D::Get();
}

IGeometry3D * Factory::Geometry3D()
{
	return &Geometry3D::Get();
}

ILoft * Factory::Loft()
{
	Utility::GeometryFactory::Loft *obj = new Utility::GeometryFactory::Loft();
	m_LoftObjects.push_back( obj );
	return obj;
}

Factory::~Factory()
{
	std::for_each( m_LoftObjects.begin(), m_LoftObjects.end(), deleteLoft );
	m_LoftObjects.clear();
}