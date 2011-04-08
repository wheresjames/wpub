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
// PubViewDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "PubViewDlgBar.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPubViewDlgBar dialog


CPubViewDlgBar::CPubViewDlgBar(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CPubViewDlgBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPubViewDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPubViewDlgBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPubViewDlgBar, CDialogBar)
	//{{AFX_MSG_MAP(CPubViewDlgBar)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPubViewDlgBar message handlers

void CPubViewDlgBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);

	RECT rect;
	GetClientRect( &rect );

	// Make room for gripper bars if not floating
	if ( !IsFloating() ) rect.left += 18;
		
	// Size the view
	m_pubimgview.Size( &rect );
	
}

int CPubViewDlgBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Attach image class
	m_pubimgview.Init( GetSafeHwnd(), NULL );
	
	return 0;
}

void CPubViewDlgBar::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if ( lpMMI == NULL ) return;

	CDialogBar::OnGetMinMaxInfo(lpMMI);

//	lpMMI->ptMaxSize.x = 1000;
//	lpMMI->ptMaxSize.y = 1000;
}

BOOL CPubViewDlgBar::Create( CWnd *pParent, DWORD style)
{
	style &= ~CBRS_SIZE_FIXED;
	style |= CBRS_SIZE_DYNAMIC;

//	style |= CCS_NOPARENTALIGN | CCS_NOMOVEY| CCS_NODIVIDER | CCS_NORESIZE;

	// Create the dialog bar
	if ( !CDialogBar::Create( pParent, CG_IDD_PUBIMGVIEW, style, CG_ID_VIEW_PUBIMGVIEW ) )
		return FALSE;

	m_bChangeDockedSize = TRUE;
	m_sizeFloating = m_sizeDocked = m_sizeDefault;
	
	style = GetBarStyle();

	style &= ~CBRS_SIZE_FIXED;
	style |= CBRS_SIZE_DYNAMIC;

	SetBarStyle( style );

	return TRUE;
}

BOOL CPubViewDlgBar::OnEraseBkgnd(CDC* pDC) 
{
	CDialogBar::OnEraseBkgnd(pDC);

	RECT rect;
	GetClientRect( &rect );

	CGrDC	dc( pDC->GetSafeHdc(), FALSE );
//	CGrPen	dkpen( PS_SOLID, 1, GetSysColor( COLOR_3DDKSHADOW ) );
//	CGrPen	ltpen( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );
	CGrPen	dkpen( PS_SOLID, 1, CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), -60 ) );
	CGrPen	ltpen( PS_SOLID, 1, CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), 40 ) );

	long xoff = 10;

	dc.SelectObject( ltpen );
	dc.MoveTo( xoff - 4, rect.top );
	dc.LineTo( xoff - 4, rect.bottom );
	dc.MoveTo( xoff - 3, rect.top );
	dc.LineTo( xoff - 3, rect.bottom );
	dc.MoveTo( xoff + 2, rect.top );
	dc.LineTo( xoff + 2, rect.bottom );
	dc.MoveTo( xoff + 1, rect.top );
	dc.LineTo( xoff + 1, rect.bottom );

	dc.SelectObject( dkpen );
	dc.MoveTo( xoff - 2, rect.top );
	dc.LineTo( xoff - 2, rect.bottom );
	dc.MoveTo( xoff + 3, rect.top );
	dc.LineTo( xoff + 3, rect.bottom );

/*
	HPEN hDkPen = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DDKSHADOW ) );
	HPEN hLtPen = CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );

	HDC hDC = pDC->GetSafeHdc();
	long xoff = 10;
	
	HPEN hOldPen = (HPEN)SelectObject( hDC, hLtPen );
	MoveToEx( hDC, xoff - 3, rect.top, NULL );
	LineTo( hDC, xoff - 3, rect.bottom );
	MoveToEx( hDC, xoff + 2, rect.top, NULL );
	LineTo( hDC, xoff + 2, rect.bottom );

	SelectObject( hDC, hDkPen );
	MoveToEx( hDC, xoff - 2, rect.top, NULL );
	LineTo( hDC, xoff - 2, rect.bottom );
	MoveToEx( hDC, xoff + 3, rect.top, NULL );
	LineTo( hDC, xoff + 3, rect.bottom );

	SelectObject( hDC, hOldPen );
	DeleteObject( hLtPen );
	DeleteObject( hDkPen );
*/						   
	return TRUE;
}

CSize CPubViewDlgBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	return CDialogBar::CalcFixedLayout( bStretch, bHorz );
}

CSize CPubViewDlgBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{

	// Return default if it is being docked or floated
	if ( ( dwMode & LM_VERTDOCK) || ( dwMode & LM_HORZDOCK ) )
	{
		if ( FRAME() != NULL && ::IsWindow( FRAME()->GetSafeHwnd() ) )
		{
			RECT rect;
			::GetClientRect( FRAME()->GetSafeHwnd(), &rect );

			rect.top = 0;
			rect.bottom = m_sizeDocked.cy;
			if ( !IsFloating() ) rect.left += 18;
			m_pubimgview.Size( &rect );
			if ( !IsFloating() ) rect.left -= 18;

			return CSize( ( rect.right - rect.left ), m_sizeDocked.cy );

		} // end if

		if (dwMode & LM_STRETCH) // if not docked stretch to fit
			return CSize((dwMode & LM_HORZ) ? 32767 : m_sizeDocked.cx,
							(dwMode & LM_HORZ) ? m_sizeDocked.cy : 32767);

		else return m_sizeDocked;

	}
	if (dwMode & LM_MRUWIDTH)
	   return m_sizeFloating;
	// In all other cases, accept the dynamic length
	if (dwMode & LM_LENGTHY)
	   return CSize(m_sizeFloating.cx, (m_bChangeDockedSize) ?
					m_sizeFloating.cy = m_sizeDocked.cy = nLength :
					m_sizeFloating.cy = nLength);
	else
	   return CSize((m_bChangeDockedSize) ?
					m_sizeFloating.cx = m_sizeDocked.cx = nLength  :
					m_sizeFloating.cx = nLength, m_sizeFloating.cy);
  
}

