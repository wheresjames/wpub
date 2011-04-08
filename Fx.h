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
// Fx.h: interface for the CFx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FX_H__4E653190_2E7A_4AF5_A656_5C89CE626674__INCLUDED_)
#define AFX_FX_H__4E653190_2E7A_4AF5_A656_5C89CE626674__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define FXTYPE_NONE			0
#define FXTYPE_TEXT			1
#define FXTYPE_TIME			2
#define FXTYPE_DATE			3
#define FXTYPE_RECT			4
#define FXTYPE_IMAGE		5
#define FXTYPE_WATERMARK	6
#define FXTYPE_VIDEO		7
#define FXTYPE_URL			8
#define FXTYPE_SCREENSHOT	9
#define FXTYPE_STEXT		10

#define FXF1_VISIBLE		0x00000001
#define FXF1_GMT			0x00000002
#define FXF1_BCK			0x00000004
#define FXF1_NOAUTOSIZE		0x00000008
#define FXF1_BORDER			0x00000010
#define FXF1_FOUNDERED		0x00000020
#define FXF1_ASPECT			0x00000040

// Time flags
#define FXF1_24HR			0x00010000
#define FXF1_ANALOG			0x00020000

// Date flags
#define FXF1_DATEFORM1		0x00010000
#define FXF1_DATEFORM2		0x00020000
#define FXF1_DATEFORM3		0x00040000
#define FXF1_DATEFORM4		0x00080000
#define FXF1_DATEFORM5		0x00100000
#define FXF1_DATEFORM6		0x00200000
#define FXF1_DATEFORM7		0x00400000
#define FXF1_ANALOGCAL		0x10000000

// Image flags
#define FXF1_TRANSPARENCY	0x00010000
#define FXF1_REALSIZE		0x00020000
#define FXF1_NOSMOOTHING	0x00040000

#define STEXT_AWINDOW		0
#define STEXT_CPU			1
#define STEXT_NUMCPU		2
#define STEXT_CPUSPEED		3
#define STEXT_DISK			4
#define STEXT_MEM			5
#define STEXT_BANDWIDTH		6

class CImgEffects;

typedef struct tagFX
{
	DWORD		type;
	DWORD		f1;
	
	RECT		rect;
	char		sep[ CFG_STRSIZE ];
	char		data[ CFG_STRSIZE ];
	char		preloaded[ CFG_STRSIZE ];
	LOGFONT		lf;	
	COLORREF	trans;
	COLORREF	rgb;
	COLORREF	bck;
	DWORD		flags;
	DWORD		translucency;
	LONG		cpos;
	LONG		cneg;
	LONG		shadow;
	RECT		scale;
	DWORD		driver;
	DWORD		format;

	long		orw;
	long		orh;

	// Not saved
//	CBmp		*bmp;
	CImgEffects	*imgfx;
	CWinDc  	*os;
	CNetFile	*nf;
	DWORD		lastflags;
	DWORD		loadtimeout;

	tagFX		*pNext;
	tagFX		*pPrev;

} FX; // end typedef struct
typedef FX* LPFX;


class CFx  
{
public:
	static BOOL OnInternetImage( DWORD dwUser, WPARAM wParam, LPARAM lParam );
	BOOL GetInternetImage( LPFX pfx );
	BOOL GetFxString( LPFX pfx, LPSTR pStr );
	static double GetCPU();
	static LPCTSTR GetSpecialTextName( DWORD i );
	static BOOL GetSpecialText( DWORD i, LPSTR buf, DWORD size );
	BOOL DrawBorder( HDC hDC, LPRECT pRect, LPFX pfx );
	BOOL CaptureScreen( LPFX pFx, CWinDc *pos );
	BOOL DrawAnalogCalender( LPFX pFx, HDC hDC, LPRECT pRect, LPSYSTEMTIME pTime );
	BOOL MoveToBottom( LPFX node );
	BOOL Append( LPFX node, LPFX pAfter );
	BOOL Insert( LPFX node, LPFX pBefore );
	BOOL MoveToTop( LPFX node );
	BOOL ScaleFx( LPFX pFx );
	BOOL DrawClock( HDC hDC, LPRECT pRect, LPFX pFx, LPSYSTEMTIME pst );
	void DrawHand(	LPFX pfx, HDC hDC, COLORREF col, COLORREF ol, COLORREF fc, COLORREF bc, 
						long width, long mwidth, long scx, long scy, long ox, long oy, 
						double angle, double len, DWORD gap );
	void DrawLine(HDC hDC, COLORREF col, COLORREF ol, DWORD size, long sx, long sy, long ex, long ey);
	void DrawCircle(HDC hDC, COLORREF col, COLORREF ol, long left, long top, long right, long bottom);
	void GetClockMetrics(LPRECT pRect, long * cx, long * cy, long * rx, long * ry, BOOL bSquare );
	BOOL DrawTicks( LPFX pfx, HDC hDC, LPRECT pRect );
	BOOL DrawHands( LPFX pfx, HDC hDC, LPRECT pRect, LPSYSTEMTIME pTime );
	static BOOL AdjustRect( LPFX pFx );
	BOOL VerifyFx( LPFX node );
	LPFX HitTest( LPPOINT pt, LPFX plast = NULL );
	static HCURSOR GetInsertCursor( DWORD type );
	HCURSOR GetInsertCursor() { return GetInsertCursor( m_dwInsert ); }
	BOOL Draw( LPCTSTR pBmp, LPCTSTR pNew = NULL );
	BOOL Save( LPCTSTR pFile );
	BOOL Load( LPCTSTR pFile, BOOL bMerge = FALSE );
	BOOL DrawText( LPFX pFx, HDC hDC, LPRECT pRect, BOOL bPlaceholders = FALSE );
	BOOL DrawFx( LPFX pFx, HDC hDC, LPRECT pRect, BOOL bPlaceholders = FALSE, BOOL bAll = FALSE );
	BOOL Draw( HDC hDC, LPRECT pRect, BOOL bPlaceholders = FALSE, BOOL bAll = FALSE );
	BOOL MoveDown( LPFX node );
	BOOL MoveUp( LPFX node );
	BOOL RemoveFx( LPFX node, BOOL bDelete = TRUE );
	LPFX AddFx( LPFX pFx );
	void Destroy();
	CFx();
	virtual ~CFx();

	DWORD Size() { return m_dwSize; }
	DWORD GetNumFx() { return Size(); }

	LPFX GetNext( LPFX ptr )
	{	if ( ptr != NULL ) return ptr->pNext; return m_pHead; }
	LPFX GetPrev( LPFX ptr )
	{	if ( ptr != NULL ) return ptr->pPrev; return m_pTail; }

	void SetInsert( DWORD type ) { m_dwInsert = type; }
	DWORD GetInsert() { return m_dwInsert; }
	void ClearInsert() { m_dwInsert = FXTYPE_NONE; }

	void SetScreenRect( LPRECT pRect )
	{	if ( pRect != NULL ) CopyRect( &m_rectScreen, pRect ); 
		else ZeroMemory( &m_rectScreen, sizeof( m_rectScreen ) ); }
	DWORD GetScreenWidth() { return ( m_rectScreen.right - m_rectScreen.left ); }
	DWORD GetScreenHeight() { return ( m_rectScreen.bottom - m_rectScreen.top ); }

private:

	RECT		m_rectScreen;

	DWORD		m_dwInsert;

	DWORD		m_dwSize;
	LPFX		m_pHead;
	LPFX		m_pTail;

	char		m_szFileName[ CWF_STRSIZE ];
};

#endif // !defined(AFX_FX_H__4E653190_2E7A_4AF5_A656_5C89CE626674__INCLUDED_)
