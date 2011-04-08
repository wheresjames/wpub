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
// WinButton.h: interface for the CWinButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINBUTTON_H__6EF3A418_FC61_4F9B_B2A4_1ADA58E9F487__INCLUDED_)
#define AFX_WINBUTTON_H__6EF3A418_FC61_4F9B_B2A4_1ADA58E9F487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WBIF1_PRESSED		0x00000001
#define WBIF1_CHECKED		0x00000002
#define WBIF1_DISABLED		0x00000004
#define WBIF1_HOT			0x00000008
#define WBIF1_DEFAULTED		0x00000010
#define WBIF1_SELECTED		0x00000020
#define WBIF1_FOCUS			0x00000040
#define WBIF1_LEFTTEXT		0x00000080
#define WBIF1_BOTTOMTEXT	0x00000100
#define WBIF1_TOPTEXT		0x00000200
#define WBIF1_NOBORDER		0x00000400
#define WBIF1_NOBCK			0x00000800
#define WBIF1_NOTHEME		0x00001000
#define WBIF1_CUSTOMFONT	0x00002000

typedef struct tagWINBUTTONINFO
{
	DWORD		f1;
	
	HWND		hWnd;
	DWORD		id;

	RECT		rect;

	HICON		hIcon;
	LPSTR		pText;
	LPLOGFONT	plf;

	COLORREF	rgbtxt;
	COLORREF	rgbbck;

	CWinDc		*pbckimg;

	RECT		win;

} WINBUTTONINFO, *LPWINBUTTONINFO; // end typedef struct


#define WBFLAGFUNCTION( fn, flag )		\
	void Set##fn( BOOL b ) { Set##fn( m_pwbi, b ); }		\
	static void Set##fn( LPWINBUTTONINFO pwbi, BOOL b )		\
	{	if ( b ) pwbi->f1 |= WBIF1_##flag; 					\
		else pwbi->f1 &= ~WBIF1_##flag; }					\
	BOOL Get##fn() { return Get##fn( m_pwbi ); }			\
	static BOOL Get##fn( LPWINBUTTONINFO pwbi ) 			\
	{	return ( pwbi->f1 & WBIF1_##flag ) != 0; }

class CWinButton  
{
public:

	static void Init( LPWINBUTTONINFO pWbi );
	static LPWINBUTTONINFO NewObject();
	static void ReleaseObject( LPWINBUTTONINFO pWbi );
	
	static BOOL Draw( LPWINBUTTONINFO pWbi, HWND hWnd, HDC hDC, LPRECT pRect );
	BOOL Draw( HWND hWnd, HDC hDC, LPRECT pRect )
	{	return Draw( m_pwbi, hWnd, hDC, pRect ); }

	CWinButton();
	CWinButton( LPWINBUTTONINFO pwbi, BOOL bAutoDelete = FALSE );
	virtual ~CWinButton();

	WBFLAGFUNCTION( Check, CHECKED );
	WBFLAGFUNCTION( Press, PRESSED );
	WBFLAGFUNCTION( Disabled, DISABLED );
	WBFLAGFUNCTION( Hot, HOT );
	WBFLAGFUNCTION( Defaulted, DEFAULTED );
	WBFLAGFUNCTION( Selected, SELECTED );
	WBFLAGFUNCTION( CustomFont, CUSTOMFONT );
	WBFLAGFUNCTION( Focus, FOCUS );

	void SetIcon( HICON hIcon ) { SetIcon( m_pwbi, hIcon ); }
	static void SetIcon( LPWINBUTTONINFO pwbi, HICON hIcon ) 
	{	if ( pwbi != NULL ) pwbi->hIcon = hIcon; }

	void SetColor( COLORREF bck, COLORREF txt ) { SetColor( m_pwbi, bck, txt ); }
	static void SetColor( LPWINBUTTONINFO pwbi, COLORREF bck, COLORREF txt ) 
	{	if ( pwbi != NULL ) pwbi->rgbbck = bck, pwbi->rgbtxt = txt; }

	// Create blank background image
	CWinDc* CreateBckImg( HDC hDC, long x, long y ) { return CreateBckImg( m_pwbi, hDC, x, y ); }
	CWinDc* CreateBckImg( LPWINBUTTONINFO pwbi, HDC hDC, long x, long y )
	{	if ( pwbi->pbckimg == NULL ) pwbi->pbckimg = new CWinDc( hDC, 0, 0, x, y );
		else pwbi->pbckimg->Create( hDC, 0, 0, x, y ); return pwbi->pbckimg; }

	// Create background image from CWinImg object
	BOOL CreateBckImg( CWinImg *pImg ) { return CreateBckImg( m_pwbi, pImg ); }
	BOOL CreateBckImg( LPWINBUTTONINFO pwbi, CWinImg *pImg )
	{	if ( pwbi == NULL || pImg == NULL ) return FALSE;
		CWinDc* pdc = CreateBckImg( pwbi, NULL, pImg->GetWidth(), pImg->GetHeight() ); 
		if ( pdc == NULL ) return FALSE; return pImg->Draw( *pdc, *pdc ); }

	BOOL SetText( LPCTSTR pText ) { return SetText( m_pwbi, pText ); }
	BOOL SetText( LPWINBUTTONINFO pwbi, LPCTSTR pText )
	{	if ( pwbi == NULL ) return FALSE;
		if ( pwbi->pText != NULL ) { delete [] pwbi->pText; pwbi->pText = NULL; }
		if ( pText == NULL || *pText == 0 ) return TRUE;
		pwbi->pText = new char[ strlen( pText ) + 1 ];
		if ( pwbi->pText == NULL ) return FALSE;
		strcpy( pwbi->pText, pText ); return TRUE; }

	BOOL OnLButtonUp( UINT nFlags, LPPOINT pt ) { return OnLButtonUp( m_pwbi, nFlags, pt ); }
	static BOOL OnLButtonUp(LPWINBUTTONINFO pwbi, UINT nFlags, LPPOINT pt );
	BOOL OnLButtonDown( UINT nFlags, LPPOINT pt ) { return OnLButtonDown( m_pwbi, nFlags, pt ); }
	static BOOL OnLButtonDown(LPWINBUTTONINFO pwbi, UINT nFlags, LPPOINT pt );
	BOOL OnMouseMove( UINT nFlags, LPPOINT pt ) { return OnMouseMove( m_pwbi, nFlags, pt ); }
	static BOOL OnMouseMove(LPWINBUTTONINFO pwbi, UINT nFlags, LPPOINT pt );

private:

	BOOL				m_bRelease;
	LPWINBUTTONINFO		m_pwbi;

};

#endif // !defined(AFX_WINBUTTON_H__6EF3A418_FC61_4F9B_B2A4_1ADA58E9F487__INCLUDED_)
