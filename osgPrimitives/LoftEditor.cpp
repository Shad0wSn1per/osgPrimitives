#include "stdafx.h"
#include "LoftEditor.h"


using namespace osg;
using namespace Utility::GeometryFactory;

//bool isPoint( LoftEditor::PATH_POINT &point, osg::Vec3& pos )
//{
//	if( !point.Vaild() )
//		return false;
//	return point.GetPosition() == pos;
//}

LoftEditor::LoftEditor( ILoft* loft )
	: m_bVisible( false )
	, m_Loft( loft )
{
	//if( m_Loft->`
}

osg::Vec3Array *LoftEditor::GetVertexArray()
{
	ref_ptr< Vec3Array > arr = new Vec3Array;
	PATH_TRAECTORY::iterator itr = m_Traectory.begin();
	while( itr != m_Traectory.end() )
		arr->push_back( (*itr).GetPosition());
	return arr.get();
}

void LoftEditor::AddPoint( const Vec3 &pos )
{
	PATH_POINT pt;
	pt.SetPosition( pos );
	m_Traectory.push_back( pt );
}

bool LoftEditor::InsertPoint( size_t after, const osg::Vec3 &pos )
{
	if( m_Loft )
	{
		m_Loft->GetPath()->InsertPoint( after, pos );
		return true;
	}
	return false;
}

bool LoftEditor::RemovePoint( const osg::Vec3 &pos )
{
	//TODO

	//PATH_TRAECTORY::iterator itr = GetPoint( pos );
	//if( itr != m_Traectory.end() )
	//{
	//	m_Traectory.erase( itr );
		return true;
	//}
	//return false;
}


std::vector< osg::Vec3 > LoftEditor::GetControlPoints()
{
	std::vector< osg::Vec3 > v;
	PATH_TRAECTORY::iterator itr = m_Traectory.begin();
	while( itr != m_Traectory.end())
	{
		v.push_back( itr->GetPosition() );
		++itr;
	}
	return v;
}

void LoftEditor::SetVisible( bool state )
{
	if( state )
	{
		
	}
	else
	{
	}
}


