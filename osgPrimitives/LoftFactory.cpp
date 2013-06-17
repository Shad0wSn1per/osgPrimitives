#include "stdafx.h"
#include "LoftFactory.h"
#include "Geometry.h"

using namespace osg;
using namespace Utility::GeometryFactory;

Loft::Path& Loft::Path::AddPoint( const osg::Vec3 &point )
{
	if( point.valid() )
		m_Path.push_back( point );
	return *this;
}

Loft::Path& Loft::Path::AddPoint( const float x, const float y,const float z )
{
	return AddPoint( Vec3( x, y, z ) );
}

Loft::Path::PATH Loft::Path::Get(){ return m_Path; }

void Loft::Path::Clear(){ m_Path.clear(); }

Loft::Path& Loft::Path::SetPath( const osg::Vec3Array &path )
{
	m_Path.clear();
	m_Path.assign( path.begin(), path.end());
	return *this;
}


Loft::Shape& Loft::Shape::AddPoint( const osg::Vec3 &point )
{
	if( point.valid() )
		m_Shape.push_back( point );
	return *this;
}

Loft::Shape& Loft::Shape::AddPoint( const float x, const float y,const float z )
{
	return AddPoint( Vec3( x, y, z ));
}

Loft::Shape& Loft::Shape::SetCloseShape( bool close )
{
	m_bCloseShape = close;
	return *this;
}

Loft::Shape& Loft::Shape::SetShape( const Vec3Array &shape )
{
	m_Shape.clear();
	m_Shape.assign( shape.begin(), shape.end());
	return *this;
}
void Loft::Shape::Clear(){ m_Shape.clear(); }

Loft::Shape::SHAPE Loft::Shape::Get(){ return m_Shape; }

Loft& Loft::SetPath( Path *p )
{
	m_pPath = p;
	return *this;
}

Loft& Loft::SetShape( Shape *s )
{
	m_pShape = s;
	return *this;
}

bool Loft::Realize( osg::Geode *geode )
{
	if( !m_pPath || !m_pShape )
		return false;


}
