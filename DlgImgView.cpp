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
// DlgImgView.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "DlgImgView.h"
#include "PubImgList.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_TOOLBARCMD		( WM_USER + 102 )
#define WM_TOOLBARUPDATE	( WM_USER + 103 )

/////////////////////////////////////////////////////////////////////////////
// CDlgImgView dialog


CDlgImgView::CDlgImgView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImgView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImgView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	SetRect( &m_rect, 0, 0, 0, 0 );
	SetRect( &m_imgrect, 0, 0, 0, 0 );
}


void CDlgImgView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImgView)
	DDX_Control(pDX, IDC_IMG, m_img);
	DDX_Control(pDX, IDC_TOOLBAR, m_tb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImgView, CDialog)
	//{{AFX_MSG_MAP(CDlgImgView)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EFFECTS_FLIPHORIZONTALLY, OnEffectsFliphorizontally)
	ON_COMMAND(ID_EFFECTS_FLIPVERTICALLY, OnEffectsFlipvertically)
	ON_COMMAND(ID_EFFECTS_GRAYSCALE, OnEffectsGrayscale)
	ON_COMMAND(ID_EFFECTS_ROTATELEFT, OnEffectsRotateleft)
	ON_COMMAND(ID_EFFECTS_ROTATERIGHT, OnEffectsRotateright)
	ON_COMMAND(ID_EFFECTS_NEGATIVE, OnEffectsNegative)
	ON_COMMAND(ID_EFFECTS_LIGHTEN, OnEffectsLighten)
	ON_COMMAND(ID_EFFECTS_DARKEN, OnEffectsDarken)
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_TOOLBARCMD, OnToolbarCmd )
	ON_MESSAGE( WM_TOOLBARUPDATE, OnToolbarUpdate )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImgView message handlers

BOOL CDlgImgView::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bCapture = TRUE;

	SetWindowText( "No Image" );
	m_tb.SetTitle( "No Image" );

	m_mover.Size( TRUE, GetSafeHwnd() );
//	m_mover.SetDock( 25, 25 );
	m_mover.SetMinTrackSize( 64, 64 );

	m_tb.EmptyMenu();
	m_tb.ParentEcho( TRUE );
//	m_tb.TextArea( FALSE );
	m_tb.SetMinTextArea( 80 );
	m_tb.ForceTooltips( TRUE );
	m_tb.HiLight( TRUE );
	m_tb.SetMessageTarget( GetSafeHwnd(), WM_TOOLBARCMD, WM_TOOLBARUPDATE );
	m_tb.Add( 1, CGrDC::LoadIcon( IDI_X ), NULL, "Close", NULL, NULL, STIF1_RIGHTBTN );
	m_tb.Add( 2, CGrDC::LoadIcon( IDI_PINOUT ), NULL, "On Top", NULL, CGrDC::LoadIcon( IDI_PININ ), STIF1_RIGHTBTN );
//	m_tb.Add( 0, NULL, NULL, NULL, NULL, NULL );
	m_tb.Add( 3, CGrDC::LoadIcon( IDI_SMALL_CAMERA ), NULL, "Capture", NULL, NULL, STIF1_HILIGHT );
	m_tb.Add( 4, CGrDC::LoadIcon( IDI_SUNKENDISK ), NULL, "Save Still Image", NULL, NULL );
//	m_tb.Add( 5, CGrDC::LoadIcon( IDI_RECORD ), NULL, "Record Video", NULL, NULL, STIF1_NOCLICK );
	m_tb.Add( 6, CGrDC::LoadIcon( IDI_FX ), NULL, "Image Effects", NULL, NULL );
	m_tb.Add( 0, NULL, NULL, NULL, NULL, NULL );
	m_tb.Add( 7, CGrDC::LoadIcon( IDI_FULLSIZE ), NULL, "Full Size", NULL, NULL );

	SizeDlg();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImgView::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	Size();	

	RedrawWindow();
}

void CDlgImgView::Size()
{
	// Punt if no window
	if ( !::IsWindow( GetSafeHwnd() ) ) return;

	RECT rect, ctrl;
	GetClientRect( &rect );

	CopyRect( &ctrl, &rect );
	ctrl.bottom = ctrl.top + 32;
	CWnd *pCtrl = GetDlgItem( IDC_TOOLBAR );
	if ( pCtrl != NULL ) 
	{	pCtrl->MoveWindow( &ctrl ); pCtrl->RedrawWindow(); }

	ctrl.top = ctrl.bottom + 1;
	ctrl.bottom = rect.bottom;
	pCtrl = GetDlgItem( IDC_IMG );
	if ( pCtrl != NULL ) 
	{	
		CopyRect( &m_imgrect, &ctrl );

		CWinDc *pdc = m_img.GetShowDc();
		if ( pdc != NULL ) CopyRect( &m_imgrect, *pdc );
		else
		{	CWinImg *pimg = m_img.GetImg();
			if ( pimg != NULL ) CopyRect( &m_imgrect, *pimg );
		} // end if

		CGrDC::Center( &m_imgrect, &ctrl );
		if ( m_imgrect.top < ctrl.top ) OffsetRect( &m_imgrect, 0, ctrl.top - m_imgrect.top );

		pCtrl->MoveWindow( &m_imgrect );

		pCtrl->RedrawWindow(); 

	} // end if

}


void CDlgImgView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_mover.OnLButtonDown( GetSafeHwnd(), point );
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgImgView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_mover.OnLButtonUp();
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgImgView::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_mover.OnMouseMove( nFlags, point );
	
	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgImgView::OnToolbarCmd(WPARAM wParam, LPARAM lParam)
{
	LPSLANTTOOLBARINFO psti = (LPSLANTTOOLBARINFO)lParam;
	if ( psti == NULL ) return 0;

	switch ( wParam )
	{
		case 1 : OnOK(); break;

		case 2 :
		{
			// Toggle topmost state
			DWORD exstyle = GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
			if ( ( exstyle & WS_EX_TOPMOST ) != 0 )
				::SetWindowPos( GetSafeHwnd(),
								HWND_NOTOPMOST, 0, 0, 0, 0,
								SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE );
			else
				::SetWindowPos( GetSafeHwnd(),
								HWND_TOPMOST, 0, 0, 0, 0,
								SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE );

		} break;

		case 3 : EnableCapture( !m_bCapture ); break;

		case 4 : SaveImg(); break;

		case 6 :
		{
			POINT pt = { psti->rect.left, psti->rect.bottom };
			POINT pt2 = { psti->rect.right, psti->rect.bottom };
			ClientToScreen( &pt ); ClientToScreen( &pt2 );
			ShowFxMenu( pt.x, pt.y + 2, pt2.x - m_tb.GetButtonSlant() + 2, pt2.y + 2 );
		} break;

		case 7 :
			// Toggle zoomed state
			if ( IsZoomed() ) ShowWindow( SW_SHOWNORMAL );
			else ShowWindow( SW_SHOWMAXIMIZED );
			break;

	} // end switch

	return 1;
}

LRESULT CDlgImgView::OnToolbarUpdate(WPARAM wParam, LPARAM lParam)
{
	LPSLANTTOOLBARINFO psti = (LPSLANTTOOLBARINFO)lParam;
	if ( psti == NULL ) return 0;

	switch ( wParam )
	{
		case 2 :
		{	DWORD exstyle = GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
			if ( ( exstyle & WS_EX_TOPMOST ) != 0 ) psti->f1 |= STIF1_CHECKED;
			else psti->f1 &= ~STIF1_CHECKED;
		} break;

		case 3 :
			if ( m_bCapture ) psti->f1 |= STIF1_CHECKED;
			else psti->f1 &= ~STIF1_CHECKED;
			break;

		case 6 :
			if ( m_bCapture ) psti->f1 |= STIF1_DISABLED;
			else psti->f1 &= ~STIF1_DISABLED;
			break;

	} // end switch

	return 0;
}

void CDlgImgView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Toggle zoomed state
	if ( IsZoomed() ) ShowWindow( SW_SHOWNORMAL );
	else ShowWindow( SW_SHOWMAXIMIZED );
	
	CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL CDlgImgView::ShowFxMenu(long x1, long y1, long x2, long y2)
{
	// Lose the old menu
	m_menu.Destroy();

	// Set check/uncheck icons
//	m_menu.SetCheckIcons( m_hChecked, m_hUnchecked, m_hHotChecked, m_hHotUnchecked );
//	m_menu.SetTickIcon( CGrDC::LoadIcon( IDI_TICK ) );

//	m_menu.AddMenuItem( NULL, 0, NULL, NULL );
	m_menu.AddMenuItem( "Flip Horizontally", ID_EFFECTS_FLIPHORIZONTALLY, CGrDC::LoadIcon( IDI_MIRROR ) );
	m_menu.AddMenuItem( "Flip Vertically", ID_EFFECTS_FLIPVERTICALLY, CGrDC::LoadIcon( IDI_FLIP ) );
	m_menu.AddMenuItem( "Rotate Right", ID_EFFECTS_ROTATERIGHT, CGrDC::LoadIcon( IDI_ROTATERIGHT ) );
	m_menu.AddMenuItem( "Rotate Left", ID_EFFECTS_ROTATELEFT, CGrDC::LoadIcon( IDI_ROTATELEFT ) );
	m_menu.AddMenuItem( NULL, 0, NULL, NULL );
	m_menu.AddMenuItem( "Grayscale", ID_EFFECTS_GRAYSCALE, CGrDC::LoadIcon( IDI_GRAYSCALE ) );
	m_menu.AddMenuItem( "Negative", ID_EFFECTS_NEGATIVE, CGrDC::LoadIcon( IDI_NEGATIVE ) );
	m_menu.AddMenuItem( NULL, 0, NULL, NULL );
	m_menu.AddMenuItem( "Lighten", ID_EFFECTS_LIGHTEN, CGrDC::LoadIcon( IDI_LIGHTEN ) );
	m_menu.AddMenuItem( "Darken", ID_EFFECTS_DARKEN, CGrDC::LoadIcon( IDI_DARKEN ) );
//	m_menu.AddMenuItem( "Start With Windows", ID_POPUP_STARTUP, NULL, NULL, &m_bAutoStart );

	// Run the menu
	m_menu.SetShowPos( FALSE, TRUE );
	m_menu.SetShading( 3 );

	if ( m_menu.IsMenuRight( x1, y1 ) )
		m_menu.ShowMenu( GetSafeHwnd(), x1, y1 );	
	else m_menu.ShowMenu( GetSafeHwnd(), x2, y2 );

	return TRUE;
}

void CDlgImgView::OnEffectsFliphorizontally() 
{
	m_img.GetImg()->Mirror();
	m_img.Regen();
}

void CDlgImgView::OnEffectsFlipvertically() 
{
	m_img.GetImg()->Flip();
	m_img.Regen();
}

void CDlgImgView::OnEffectsGrayscale() 
{
	m_img.GetImg()->GrayScale();
	m_img.Regen();
}

void CDlgImgView::OnEffectsRotateleft() 
{
	m_img.GetImg()->Rotate( -90 );
	m_img.Regen();
}

void CDlgImgView::OnEffectsRotateright() 
{
	m_img.GetImg()->Rotate( 90 );
	m_img.Regen();
}

void CDlgImgView::OnEffectsNegative() 
{
	m_img.GetImg()->Negative();
	m_img.Regen();
}

void CDlgImgView::OnEffectsLighten() 
{
	m_img.GetImg()->Light( 40, 128 );
	m_img.Regen();
}

void CDlgImgView::OnEffectsDarken() 
{
	m_img.GetImg()->Light( -20, 128 );
	m_img.Regen();
}

BOOL CDlgImgView::OnEraseBkgnd(CDC* pDC) 
{
	if ( pDC == NULL ) return FALSE;

	RECT rect;
	GetClientRect( &rect );

	// Create background texture
	if ( m_bck.GetSafeHdc() == NULL )
	{	m_bck.Create( pDC->GetSafeHdc(), 0, 0, 64, 64 );
		CGrDC::FillSolidRect( m_bck, m_bck, GetSysColor( COLOR_APPWORKSPACE ) );
		m_bck.Texturize( 20 );
	} // end if

	// Tile in the bck
	m_bck.Tile( pDC->GetSafeHdc(), &rect );

	if ( ::IsWindow( m_img.GetSafeHwnd() ) )
	{
		RECT ctrl;
		m_img.GetWindowRect( &ctrl );
		ScreenToClient( &ctrl );
		InflateRect( &ctrl, 2, 2 );
		CGrDC::Box3d(	pDC->GetSafeHdc(), &ctrl, 2, 
						CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), -120 ),
						CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), 120 ) );

	} // end if

	return TRUE;
//	return CDialog::OnEraseBkgnd(pDC);
}

void CDlgImgView::OnSysColorChange() 
{
	CDialog::OnSysColorChange();
	m_bck.Destroy();
}

void CDlgImgView::OnTimer(UINT nIDEvent) 
{
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CDlgImgView::EnableCapture(BOOL enable)
{
	// Do we already have the required status?
	if ( enable == m_bCapture ) return TRUE;
	m_bCapture = enable;

	if ( !enable ) m_img.CaptureShowDc();
	else m_img.ClearImg();

	return TRUE;
}

void CDlgImgView::SizeDlg()
{
	// Ensure window
	if ( !::IsWindow( GetSafeHwnd() ) ) return;
	if ( RW( m_rect ) <= 32 || RH( m_rect ) <= 32 ) return;

	RECT win;
	GetWindowRect( &win );
	win.right = win.left + RW( m_rect ) + 16;
	win.bottom = win.top + RH( m_rect ) + 48;
	MoveWindow( &win, TRUE );

	Size();
}

void CDlgImgView::Shutdown()
{
	char txt[ 64 ];
	if ( GetWindowText( txt, sizeof( txt ) ) )
		CFG().SaveWindowPos( GetSafeHwnd(), "RemoteVideoPos", txt, TRUE );

	DestroyWindow();
}

BOOL CDlgImgView::RestorePosition()
{
	char txt[ 64 ];
	if ( GetWindowText( txt, sizeof( txt ) ) )
		return CFG().RestoreWindowPos( GetSafeHwnd(), "RemoteVideoPos", txt, 
										FALSE, TRUE, TRUE, TRUE );
	return FALSE;
}

BOOL CDlgImgView::SaveImg()
{
	CWinImg img, *pimg = &img;

	if ( m_bCapture )
	{
		char title[ CWF_STRSIZE ];
		GetWindowText( title, sizeof( title ) );
		if ( !IMGLIST().GetImage( title, &img ) )
		{	_Log( MB_ICONERROR, "Error loading image", title );
			return FALSE;
		} // end if
		
	} // end if

	else
	{
		pimg = m_img.GetImg();
		if ( pimg == NULL || !pimg->IsValid() )
		{	_Log( MB_ICONERROR, "Save Image", "Image not available" );
			return FALSE;
		} // end if

	} // end else

	// Get the file name
	char filename[ CWF_STRSIZE ] = { 0 };
	if ( !CWinFile::GetSaveFileName(	filename, "Save Image As",
										"All Images (*.bmp;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wmf;*.emf;*.jp2)\x0*.bmp;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wmf;*.emf;*.jp2\x0"
										"Windows Bitmap (*.bmp;*.dib)\x0*.bmp;*.dib\x0"
										"JPEG (*.jpg;*.jpeg)\x0*.jpg;*.jpeg\x0"
										"Portable Network Graphic (*.png)\x0*.png\x0"
										"Icon (*.ico)\x0*.ico\x0"
										"Tagged Image File (*.tif; *.tiff)\x0*.tif;*.tiff\x0"
										"Targa (*.tga)\x0*.tga\x0"
										"PC Paintbrush (*.pcx)\x0*.pcx\x0"
										"Windows Metafile (*.wmf;*.emf)\x0*.wmf;*.emf\x0"
										"JPEG 2000 (*.jp2)\x0*.jp2\x0"
										"All Files (*.*)\x0*.*\x0",
										"jpg",
									 GetSafeHwnd() ) ) return FALSE;

	// Save the image
	FRAME()->SetQuality( pimg );
	if ( !pimg->Save( filename ) )
	{	_Log( MB_ICONERROR, "Error saving image", pimg->GetLastError() );
		return FALSE;
	} // end if

	return TRUE;
}
