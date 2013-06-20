#include "stdafx.h"
#include "PathEditor.h"

using namespace osg;
using namespace Utility::GeometryFactory;

bool isPoint( PathEditor::PATH_POINT &point, osg::Vec3& pos )
{
	if( !point.Vaild() )
		return false;
	return point.GetPosition() == pos;
}

bool PathEditor::IsValidSegment( const osg::Vec3& p0, const osg::Vec3& p1 )
{
	PATH_POINT pt0 = (*GetPoint( p0 ));
	PATH_POINT pt1 = (*GetPoint( p1 ));
	return pt0.Vaild() && pt1.Vaild() && pt0.Type() != PT_ROUND && pt1.Type() != PT_ROUND;
}


PathEditor::PATH_TRAECTORY::iterator PathEditor::GetPoint( const osg::Vec3& p )
{
	//PATH_POINT pt;
	return std::find_if( m_Traectory.begin(), m_Traectory.end(), std::bind( isPoint, std::placeholders::_1, p ) );
	/*if( itr == m_Traectory.end() )
		return itr;
	else
		return (*itr );*/
	/*PATH_POINT pt;
	PATH_TRAECTORY::iterator itr = m_Traectory.begin();
	PATH_TRAECTORY::iterator itr_e = m_Traectory.end();
	for( ; itr != itr_e; ++itr )
		if( (*itr).Vaild() && (*itr).GetPosition() == p )
			return (*itr);*/
	
}

osg::Vec3Array *PathEditor::GetVertexArray()
{
	ref_ptr< Vec3Array > arr = new Vec3Array;
	PATH_TRAECTORY::iterator itr = m_Traectory.begin();
	while( itr != m_Traectory.end() )
		arr->push_back( (*itr).GetPosition());
	return arr.get();
}

void PathEditor::AddPoint( const Vec3 &pos )
{
	PATH_POINT pt;
	pt.SetPosition( pos );
	m_Traectory.push_back( pt );
}

bool PathEditor::InsertPoint( int after, const osg::Vec3 &pos )
{
	if( m_Traectory.size() > after )
	{
		PATH_POINT pt;
		pt.SetPosition( pos );
		m_Traectory.insert( m_Traectory.begin() + after + 1, pt );
		return true;
	}
	return false;
}

bool PathEditor::RemovePoint( const osg::Vec3 &pos )
{
	PATH_TRAECTORY::iterator itr = GetPoint( pos );
	if( itr != m_Traectory.end() )
	{
		m_Traectory.erase( itr );
		return true;
	}
	return false;
}

osg::Vec3Array * PathEditor::GetPath()
{
	ref_ptr< Vec3Array > path = new Vec3Array;
	PATH_TRAECTORY::iterator itr = m_Traectory.begin();
	while( itr != m_Traectory.end() )
	{
		switch( itr->Type() )
		{
		case PT_ROUND:
			createRoundedCorner( path.get(), itr );
			break;
		default:
			path->push_back( itr->GetPosition() );
		}
		++itr;
	}
	return path.release();
}

void PathEditor::createRoundedCorner( osg::Vec3Array *path, PATH_TRAECTORY::iterator corner )
{
	int index = corner - m_Traectory.begin();
	
	if( index == 0 || index == m_Traectory.size()-1 )
		return;
	PATH_POINT prev = m_Traectory.at( index - 1 );
	PATH_POINT next = m_Traectory.at( index + 1 );

	Vec3 AB = corner->GetPosition() - prev.GetPosition();
	double AB_len = AB.length();
	Vec3 BC = next.GetPosition() - corner->GetPosition();
	double BC_len = BC.length();
	Vec3 ABC_norm = ( AB ^ BC );
	Vec3 HO = ABC_norm ^ AB;
	HO.normalize();
	double R = corner->GetCornerRadius();
	HO *= R;
	double corner_angle = osg::PI - acos( AB*BC/( AB_len * BC_len)) ;
	double HB = corner->GetCornerRadius() / tan( corner_angle/2.0 );
	AB.normalize();
	Vec3 AH = AB*( AB_len - HB );
	Vec3 WH = prev.GetPosition() + AH;
	Vec3 WO = WH + HO;
	
	path->push_back( WH );
	Vec3 OH = WH - WO;
	BC.normalize();
	Vec3 final =corner->GetPosition() + BC * HB;
	Vec3 OH_ = final - WO;
	double kink_radius = acos( OH*OH_/( R * R )) ;
	double angle_step = kink_radius / 32.0 ;
	for( double i = 0; i < kink_radius ; i += angle_step )
	{
		Matrixd rm = Matrixd::rotate( -angle_step, ABC_norm );
		OH = Matrixd::transform3x3( rm, OH );
		Vec3 f  = WO + OH;
		path->push_back( f );
	}
	_asm nop;
}

int PathEditor::Find( const osg::Vec3& p )
{
	PATH_TRAECTORY::iterator obj = find_if( m_Traectory.begin(), m_Traectory.end(), std::bind( isPoint, std::placeholders::_1, p ));
	
	if( obj != m_Traectory.end() )
	{
		int idx = obj - m_Traectory.begin();
		return idx;
	}
	return -1;
}

PathEditor::PATH_POINT &PathEditor::GetPoint( int index )
{
	if( index < 0 || m_Traectory.size() <= index )
		throw "Index out of range";
	return m_Traectory.at( index );
}

