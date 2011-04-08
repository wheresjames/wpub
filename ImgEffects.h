/*******************************************************************
// Copyright (c) 2002, 2003, 2004, 2005, 
// Robert Umbehant
// mailto:rumbehant@wheresjames.com
//
// This program is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as 
// published by the Free Software Foundation; either version 2 of 
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA 02111-1307 USA
//
*******************************************************************/
// ImgEffects.h: interface for the CImgEffects class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGEFFECTS_H__B186F394_B4CF_4E62_B8C8_E1644AED9938__INCLUDED_)
#define AFX_IMGEFFECTS_H__B186F394_B4CF_4E62_B8C8_E1644AED9938__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Params.h"
#include "Fx.h"

#define IMGFX_NONE			0
#define IMGFX_MIRROR		1
#define IMGFX_FLIP			2
#define IMGFX_GRAYSCALE		3
#define IMGFX_ROTATE		4
#define IMGFX_NEGATIVE		5
#define IMGFX_RESAMPLE		6
#define IMGFX_DITHER		7
#define IMGFX_CROP			8
#define IMGFX_NOISE			9
#define IMGFX_MEDIAN		10
#define IMGFX_GAMMA			11
#define IMGFX_SHIFT			12
#define IMGFX_THRESHOLD		13
#define IMGFX_COLORIZE		14
#define IMGFX_LIGHT			15
#define IMGFX_ERODE			16
#define IMGFX_DILATE		17


class CImgEffects  
{
public:
	CImgEffects();
	virtual ~CImgEffects();



public:
	static LPCTSTR GetFxParams( DWORD i );
	static LPCTSTR GetFxName( DWORD i );
	BOOL Edit( CWnd *pWnd );

	
	BOOL Save( CCfgFile *pCfg, HGROUP hGroup, LPCTSTR pName );
	BOOL Save( CCfgFile *pCfg, LPCTSTR pGroup, LPCTSTR pName )
	{	
		if ( pCfg == NULL ) return FALSE;
		HGROUP hGroup = pCfg->FindGroup( pGroup );
		if ( hGroup == NULL ) return FALSE;
		return Save( pCfg, hGroup, pName );		
	}
	BOOL Load( CCfgFile *pCfg, HGROUP hGroup, LPCTSTR pName );
	BOOL Load( CCfgFile *pCfg, LPCTSTR pGroup, LPCTSTR pName )
	{	if ( pCfg == NULL ) return FALSE;
		HGROUP hGroup = pCfg->FindGroup( pGroup );
		if ( hGroup == NULL ) return FALSE;
		return Load( pCfg, hGroup, pName );
	}
	
	BOOL DoEffects( CWinImg *pImg );

	CParams		m_params;

//	void SetFx( LPFX pfx ) { m_pfx = pfx; }

	DWORD GetEffectsCount() { return m_params.Size(); }

	void SetFx( LPCTSTR pTitle, long w, long h )
	{	strcpy_sz( m_szTitle, pTitle ); m_w = w; m_h = h; }

private:

	char	m_szTitle[ 256 ];
	long	m_w, m_h;

//	LPFX	m_pfx;

public:
	BOOL Save( CRKey *pRk, LPCTSTR pName );
	BOOL Load( CRKey *pRk, LPCTSTR pName );
	void Destroy();
};

#endif // !defined(AFX_IMGEFFECTS_H__B186F394_B4CF_4E62_B8C8_E1644AED9938__INCLUDED_)
