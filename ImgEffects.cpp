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
// ImgEffects.cpp: implementation of the CImgEffects class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "ImgEffects.h"	

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImgEffects::CImgEffects()
{
//	m_pfx = NULL;

	*m_szTitle = 0;
	m_w = 0;
	m_h = 0;
}

CImgEffects::~CImgEffects()
{
	Destroy();
}

void CImgEffects::Destroy()
{
	m_params.Destroy();
}

BOOL CImgEffects::DoEffects(CWinImg *pImg)
{
	if ( pImg == NULL ) return FALSE;

	// Any effects?
	if ( m_params.Size() == 0 ) return TRUE;

	LPPARAMINFO	ppi = NULL;

	while ( ( ppi = (LPPARAMINFO)m_params.GetNext( ppi ) ) != NULL )
	{
		switch( ppi->function )
		{

			case IMGFX_MIRROR :
				pImg->Mirror();
				break;
			
			case IMGFX_FLIP :
				pImg->Flip();
				break;
			
			case IMGFX_GRAYSCALE :
				pImg->GrayScale();
				break;
			
			case IMGFX_NEGATIVE :
				pImg->Negative();
				break;

			case IMGFX_ROTATE :
			{	float	angle = 0;
				m_params.GetParams( ppi, &angle );
				pImg->Rotate( angle );
			} // end case
			break;
			
			case IMGFX_RESAMPLE :
			{	long newx = 0, newy = 0;
				m_params.GetParams( ppi, &newx, &newy );
				pImg->Resample( newx, newy );
			} // end case
			break;

			case IMGFX_DITHER :
			{	bool b = 0;
				m_params.GetParams( ppi, &b );
				pImg->Dither( b );
			} // end case
			break;

			case IMGFX_CROP :
			{	long left = 0, top = 0, right = 0, bottom = 0;
				m_params.GetParams( ppi, &left, &top, &right, &bottom );
				pImg->Crop( left, top, right, bottom );
			} // end case
			break;

/*			case IMGFX_REPAIR :
			{	float radius = 0;
				long niterations = 0, colorspace = 0;
				m_params.GetParams( ppi, &radius, &niterations, &colorspace );
				pImg->Repair( radius, niterations, colorspace );
			} // end case
			break;
*/
			case IMGFX_NOISE :
			{	long	level = 0;
				m_params.GetParams( ppi, &level );
				pImg->Noise( level );
			} // end case
			break;

			case IMGFX_MEDIAN :
			{	long	level = 0;
				m_params.GetParams( ppi, &level );
				pImg->Median( level );
			} // end case
			break;

			case IMGFX_GAMMA :
			{	float	gamma = 0;
				m_params.GetParams( ppi, &gamma );
				pImg->Gamma( gamma );
			} // end case
			break;

			case IMGFX_SHIFT :
			{	long r, g, b;
				m_params.GetParams( ppi, &r, &g, &b );
				pImg->ShiftRGB( r, g, b );
			} // end case
			break;

			case IMGFX_THRESHOLD :
			{	BYTE level = 0;
				m_params.GetParams( ppi, &level );
				pImg->Threshold( level );
			} // end case
			break;

			case IMGFX_COLORIZE :
			{	BYTE hue = 0, sat = 0;
				m_params.GetParams( ppi, &hue, &sat );
				pImg->Colorize( hue, sat );
			} // end case
			break;

			case IMGFX_LIGHT :
			{	long	level = 0, contrast = 0;
				m_params.GetParams( ppi, &level, &contrast );
				pImg->Light( level, contrast );
			} // end case
			break;

			case IMGFX_ERODE :
			{	long	level = 0;
				m_params.GetParams( ppi, &level );
				pImg->Erode( level );
			} // end case
			break;

			case IMGFX_DILATE :
			{	long	level = 0;
				m_params.GetParams( ppi, &level );
				pImg->Dilate( level );
			} // end case
			break;

		} // end switch

	} // end while

	return TRUE;
}

LPCTSTR CImgEffects::GetFxParams(DWORD i)
{
	switch( i )
	{
		case IMGFX_ROTATE : return "float Angle <0,359.99>"; break;
		case IMGFX_RESAMPLE : return "long New X <1,%width,%width>, long New Y<1,%height,%height>"; break;
		case IMGFX_DITHER : return "bool Function"; break;
		case IMGFX_CROP : return "long Left <0,%width>, long Top <0,%height>, long Right <0,%width, %width>, long Bottom <0,%height, %height>"; break;
//		case IMGFX_REPAIR : return "float Radius <0,%width,0>, long Iterations<0,4,1>, long Colorspace<0,4>"; break;
		case IMGFX_NOISE : return "long Level <0,1000>"; break;
		case IMGFX_MEDIAN : return "long Median <1,8,1>"; break;
		case IMGFX_GAMMA : return "float Gamma <0,10,1>"; break;
		case IMGFX_SHIFT : return "long Red <0,255>, long Green <0,255>, long Blue <0,255>"; break;
		case IMGFX_THRESHOLD : return "uchar Level <0,255,128>"; break;
		case IMGFX_COLORIZE : return "uchar Hue <0,255,128>, uchar Saturation<0,255,128>"; break;
		case IMGFX_LIGHT : return "long Level <0,255,80>, long Contrast<0,255,128>"; break;
//		case IMGFX_FILTER : return ""; break;
		case IMGFX_ERODE : return "long Level <1,20,1>"; break;
		case IMGFX_DILATE : return "long Level <1,20,1>"; break;
//		case IMGFX_HUE : return "float Correction <0,100>"; break;

	} // end switch

	return "";
}

BOOL CImgEffects::Load(CCfgFile * pCfg, HGROUP hGroup, LPCTSTR pName)
{
	// Sanity checks
	if ( pCfg == NULL || hGroup == NULL || pName == NULL )
		return FALSE;

	LPBYTE buf = (LPBYTE)pCfg->GetValuePtr( hGroup, pName );
	DWORD size = pCfg->GetValueSize( hGroup, pName );
	if ( buf == NULL || size == 0 ) return FALSE;

	// Load the params
	return m_params.Load( buf, size );
}

BOOL CImgEffects::Save(CCfgFile * pCfg, HGROUP hGroup, LPCTSTR pName)
{
	// Sanity checks
	if ( pCfg == NULL || hGroup == NULL || pName == NULL )
		return FALSE;

	// Get data size
	DWORD size = m_params.Save( NULL, 0 );
	if ( size == 0 ) return FALSE;

	TMem< BYTE > buf;
	if ( !buf.allocate( size + 1 ) ) return FALSE;

	// Get the data
	size = m_params.Save( buf, size );
	buf[ size ] = 0;

	// Save the settings
	return pCfg->SetValue( hGroup, pName, buf, size );
}

BOOL CImgEffects::Load(CRKey *pRk, LPCTSTR pName)
{
	// Sanity checks
	if ( pRk == NULL || pName == NULL ) return FALSE;

	// Get value pointers
	LPBYTE buf = (LPBYTE)pRk->GetValuePtr( pName );
	DWORD size = pRk->GetValueSize( pName );
	if ( buf == NULL || size == 0 ) return FALSE;

	// Load the params
	return m_params.Load( buf, size );
}

BOOL CImgEffects::Save(CRKey *pRk, LPCTSTR pName)
{
	// Sanity checks
	if ( pRk == NULL || pName == NULL ) return FALSE;

	// Get data size
	DWORD size = m_params.Save( NULL, 0 );
	if ( size == 0 ) return FALSE;

	TMem< BYTE > buf;
	if ( !buf.allocate( size + 1 ) ) return FALSE;

	// Get the data
	size = m_params.Save( buf, size );
	buf[ size ] = 0;

	// Save the settings
	return pRk->Set( pName, buf, size ) != NULL;
}

BOOL CImgEffects::Edit( CWnd *pWnd )
{
	// Set fx info
	APP()->m_dlgimgfxeditor.SetFx( &m_params, m_szTitle, m_w, m_h );

	// Set edit object
/*	if ( m_pfx != NULL ) 
	{
		char fname[ CWF_STRSIZE ];
		CWinFile::GetFileNameFromPath( m_pfx->data, fname );
		APP()->m_dlgimgfxeditor.SetTitle( fname );
		APP()->m_dlgimgfxeditor.SetFx( m_pfx );
	} // end if
*/
//	else 
//	{
//		APP()->m_dlgimgfxeditor.SetTitle( "Live Video" );
//		APP()->m_dlgimgfxeditor.SetParams( &m_params );
//	} // end else

	if ( IsWindow( APP()->m_dlgimgfxeditor.GetSafeHwnd() ) )
		APP()->m_dlgimgfxeditor.OnOK();

	// Create / refresh window
	if ( !IsWindow( APP()->m_dlgimgfxeditor.GetSafeHwnd() ) )
		APP()->m_dlgimgfxeditor.Create( CDlgImgFxEditor::IDD, pWnd );
	else APP()->m_dlgimgfxeditor.OnRefresh( 0, 0L );

	// Show the window
	APP()->m_dlgimgfxeditor.ShowWindow( SW_SHOWNORMAL );

	return TRUE;
}

LPCTSTR CImgEffects::GetFxName(DWORD i)
{
	switch( i )
	{
		case IMGFX_MIRROR : return "Mirror"; break;
		case IMGFX_FLIP : return "Flip"; break;
		case IMGFX_GRAYSCALE : return "Grayscale"; break;
		case IMGFX_NEGATIVE : return "Negative"; break;
		case IMGFX_ROTATE : return "Rotate"; break;
		case IMGFX_RESAMPLE : return "Resample"; break;
		case IMGFX_DITHER : return "Dither"; break;
		case IMGFX_CROP : return "Crop"; break;
//		case IMGFX_REPAIR : return "Repair"; break;
		case IMGFX_NOISE : return "Noise"; break;
		case IMGFX_MEDIAN : return "Median"; break;
		case IMGFX_GAMMA : return "Gamma"; break;
		case IMGFX_SHIFT : return "Shift"; break;
		case IMGFX_THRESHOLD : return "Thresh-hold"; break;
		case IMGFX_COLORIZE : return "Colorize"; break;
		case IMGFX_LIGHT : return "Lighten"; break;
//		case IMGFX_FILTER : return "Filter"; break;
		case IMGFX_ERODE : return "Erode"; break;
		case IMGFX_DILATE : return "Dilate"; break;
//		case IMGFX_HUE : return "Hue"; break;

	} // end switch

	return "Unknown Effect";

}


