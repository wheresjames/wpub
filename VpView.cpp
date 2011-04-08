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
// VpView.cpp : implementation of the CVpView class
//

#include "stdafx.h"
#include "Vp.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVpView

IMPLEMENT_DYNCREATE(CVpView, CView)

BEGIN_MESSAGE_MAP(CVpView, CView)
	//{{AFX_MSG_MAP(CVpView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVpView construction/destruction

CVpView::CVpView()
{
	m_pPrintInfo = NULL;

}

CVpView::~CVpView()
{
}

BOOL CVpView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVpView drawing

void CVpView::OnDraw(CDC* pDC)
{
	if ( pDC == NULL ) return;
	CVpDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if ( pDC->IsPrinting() && m_pPrintInfo != NULL )
	{
		if (	::IsWindow( FRAME()->m_simple.GetSafeHwnd() ) &&
				FRAME()->m_simple.m_img.GetSafeHdc() != NULL )
		{
			RECT rect, aspect;

			// Get rect
			CopyRect( &rect, m_pPrintInfo->m_rectDraw );

			// 1/16th margin
			rect.right -= RW( rect ) >> 4;
			rect.bottom -= RH( rect ) >> 4;
			CGrDC::Center( &rect, &m_pPrintInfo->m_rectDraw );

			// Correct aspect ratio
			FRAME()->m_simple.m_img.GetRect( &aspect );
			CGrDC::AspectCorrect( &rect, &aspect );

			// Draw the image
			FRAME()->m_simple.m_img.Draw( pDC->GetSafeHdc(), &rect );

		} // end if
		
		else
		{
			// Ensure there is an image to print
			CPubImgList *pil = m_winedit.GetImageList();
			if ( pil == NULL ) return;

			// Get image object
			LPPUBIMGINFO ppii = NULL;
			ppii = pil->GetSelImage();

			// Ensure valid image
			if ( ppii == NULL || ppii->os == NULL ) return;

			RECT rect, aspect;

			// Get rect
			CopyRect( &rect, m_pPrintInfo->m_rectDraw );

			// 1/16th margin
			rect.right -= RW( rect ) >> 4;
			rect.bottom -= RH( rect ) >> 4;
			CGrDC::Center( &rect, &m_pPrintInfo->m_rectDraw );

			// Correct aspect ratio
			ppii->os->GetRect( &aspect );
			CGrDC::AspectCorrect( &rect, &aspect );

			// Draw the image
			ppii->os->Draw( pDC->GetSafeHdc(), &rect );

		} // end else 

	} // end if
}

/////////////////////////////////////////////////////////////////////////////
// CVpView printing

BOOL CVpView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVpView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	pInfo->SetMaxPage( 1 );
	pInfo->m_nNumPreviewPages = 1;

	m_pPrintInfo = pInfo;
}

void CVpView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CVpView diagnostics

#ifdef _DEBUG
void CVpView::AssertValid() const
{
	CView::AssertValid();
}

void CVpView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVpDoc* CVpView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVpDoc)));
	return (CVpDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVpView message handlers

BOOL CVpView::OnEraseBkgnd(CDC* pDC) 
{
	if ( pDC == NULL ) return FALSE;

	RECT rect;
	GetClientRect( &rect );

	if ( !m_bck.Tile( pDC->GetSafeHdc(), &rect ) )
		pDC->FillSolidRect( &rect, GetSysColor( COLOR_APPWORKSPACE ) );

	// Punt if no window
	if ( !::IsWindow( m_winedit.GetSafeHwnd() ) ) return TRUE;

	HDC hDC = pDC->GetSafeHdc();

	RECT crect;
	m_winedit.GetClientRect( &crect );

	// Center the rect
	CGrDC::Center( &crect, &rect );
	InflateRect( &crect, 2, 2 );

	CGrDC::Box3d(	hDC, &crect, 2, 
					CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), 100 ),
					CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), -100 ),
					TRUE );
	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}

void CVpView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	// Set the image list pointer
	m_winedit.SetImgList( &IMGLIST() );

	RECT rect;
	GetClientRect( &rect );
	m_winedit.Init( GetSafeHwnd(), &rect );

	// Create background pattern
//	m_bck.Create( NULL, 0, 0, 64, 64 );
//	m_bck.DeepFill( NULL, GetSysColor( COLOR_APPWORKSPACE ), 8 );
	m_bck.Create( NULL, 0, 0, 64, 64 );
	CGrDC::FillSolidRect( m_bck, m_bck, GetSysColor( COLOR_APPWORKSPACE ) );
	m_bck.Texturize( 20 );
	
}

void CVpView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	RECT rect;
	GetClientRect( &rect );
	m_winedit.Size( &rect );

}

void CVpView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( nChar == VK_DELETE )
	{
		LPFX pfx = m_winedit.GetEditFx();
		if ( pfx != NULL )
		{	
			// Get current fx object
			CFx *Fx = APP()->GetActiveFx();
			if ( Fx != NULL )
			{	m_winedit.SetEditFx( NULL );
				Fx->RemoveFx( pfx );
				APP()->ReGen();
				APP()->RefreshProperties();
			} // end if
		} // end if	

	} // end if

	else if ( nChar == VK_INSERT )
	{
		LPFX pfx = m_winedit.GetEditFx();
		if ( pfx != NULL )
		{
			// Get current fx object
			CFx *Fx = APP()->GetActiveFx();
			if ( Fx != NULL )
			{	m_winedit.SetEditFx( NULL );
				LPFX n = Fx->AddFx( pfx );
				if ( n != NULL )
				{	OffsetRect( &n->rect, 5, 5 );
					m_winedit.SetEditFx( n );
					APP()->ReGen();
					APP()->RefreshProperties();
				} // end if
			} // end if
		} // end if	
	} // end else if
	
	if ( nChar == VK_BACK )
	{
		LPFX pfx = m_winedit.GetEditFx();
		if ( pfx != NULL && pfx->type == FXTYPE_TEXT )
		{
			DWORD i = strlen( pfx->data );
			if ( i > 0 )
			{	// Check for CRLF
				if ( i > 1 && pfx->data[ i - 1 ] == '\n' && pfx->data[ i - 2 ] == '\r' )
					pfx->data[ --i ] = 0;

				// Lose this character
				pfx->data[ --i ] = 0;

				IMGLIST().Update( MAXDWORD, FALSE );
				EDIT().Update();	
				FRAME()->SetUpdateTimer();

			} // end if
		} // end if

	} // end else if
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CVpView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( ( nChar >= ' ' && nChar <= '~' ) ||
		 nChar == '\r' )
	{
		LPFX pfx = m_winedit.GetEditFx();
		if ( pfx != NULL && pfx->type == FXTYPE_TEXT )
		{			
			// Get current fx object
			CFx *Fx = APP()->GetActiveFx();
			if ( Fx != NULL )
			{
				
				DWORD i = strlen( pfx->data );
				if ( i < 256 )
				{	pfx->data[ i++ ] = nChar;
					if ( nChar == '\r' ) pfx->data[ i++ ] = '\n';
					pfx->data[ i ] = 0;
					Fx->AdjustRect( pfx );
					IMGLIST().Update( MAXDWORD, FALSE );
					EDIT().Update();	
					FRAME()->SetUpdateTimer();

				} // end if

			} // end if

		} // end if

	} // end else if
	
	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CVpView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Clear the selection
	m_winedit.SetEditFx( NULL );
	
	CView::OnLButtonDown(nFlags, point);
}

void CVpView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Clear any flyby rect
	m_winedit.ClearFlyby();
	
	CView::OnMouseMove(nFlags, point);
}

void CVpView::OnSysColorChange() 
{
	CView::OnSysColorChange();
	
	// Create background pattern
//	m_bck.Create( NULL, 0, 0, 64, 64 );
//	m_bck.DeepFill( NULL, GetSysColor( COLOR_APPWORKSPACE ), 8 );
	m_bck.Create( NULL, 0, 0, 64, 64 );
	CGrDC::FillSolidRect( m_bck, m_bck, GetSysColor( COLOR_APPWORKSPACE ) );
	m_bck.Texturize( 20 );
	
}
