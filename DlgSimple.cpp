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
// DlgSimple.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgSimple.h"

#include "DlgSimpleSettings.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_TOOLBAR			( WM_USER + 101 )
#define WM_LOADNEXTOVERLAY	( WM_USER + 102 )
#define WM_LOADNEXTBCK		( WM_USER + 103 )

/////////////////////////////////////////////////////////////////////////////
// CDlgSimple dialog


CDlgSimple::CDlgSimple(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSimple::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSimple)
	m_bEnableBackground = FALSE;
	m_bEnableOverlay = FALSE;
	m_bMaskImg = FALSE;
	//}}AFX_DATA_INIT

	*m_szBackgroundPath = 0;
	*m_szOverlayPath = 0;
	*m_szBackgroundImg = 0;
	*m_szOverlayImg = 0;

	m_bCapture = TRUE;

	m_hExit = NULL;

	m_bClose = FALSE;

	m_bMaskLoaded = FALSE;

}


void CDlgSimple::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSimple)
	DDX_Control(pDX, IDC_MASKCOLOR, m_colorMask);
	DDX_Control(pDX, IDC_CHOOSE_MASKCOLOR, m_btnChooseMaskColor);
	DDX_Control(pDX, IDC_CHOOSE_COLOR, m_btnChooseColor);
	DDX_Control(pDX, IDC_SLIDEERR, m_slideErr);
	DDX_Control(pDX, IDC_COLORBOX, m_colorbox);
	DDX_Control(pDX, IDC_BACKGROUNDS, m_listBackgrounds);
	DDX_Control(pDX, IDC_OVERLAYS, m_listOverlays);
	DDX_Control(pDX, IDC_IMAGE, m_image);
	DDX_Control(pDX, IDC_TOOLBAR, m_toolbar);
	DDX_Check(pDX, IDC_ENABLE_BACKGROUND, m_bEnableBackground);
	DDX_Check(pDX, IDC_ENABLE_OVERLAY, m_bEnableOverlay);
	DDX_Check(pDX, IDC_MASKIMG, m_bMaskImg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSimple, CDialog)
	//{{AFX_MSG_MAP(CDlgSimple)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ENABLE_BACKGROUND, OnEnableBackground)
	ON_BN_CLICKED(IDC_ENABLE_OVERLAY, OnEnableOverlay)
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOLORCHANGE()
	ON_NOTIFY(NM_CLICK, IDC_BACKGROUNDS, OnClickBackgrounds)
	ON_NOTIFY(NM_CLICK, IDC_OVERLAYS, OnClickOverlays)
	ON_WM_GETMINMAXINFO()
	ON_WM_CHAR()
	ON_BN_CLICKED(IDC_CHOOSE_COLOR, OnChooseColor)
	ON_BN_CLICKED(IDC_MASKIMG, OnMaskimg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHOOSE_MASKCOLOR, OnChooseMaskcolor)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_TOOLBAR, OnToolbar )

	ON_MESSAGE( ( WM_USER + 201 ), OnFrameMsg )

	ON_MESSAGE( WM_LOADNEXTOVERLAY, OnLoadNextOverlay )
	ON_MESSAGE( WM_LOADNEXTBCK, OnLoadNextBck )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSimple message handlers

BOOL CDlgSimple::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );

	CWinFile::GetExePath( m_szOverlayPath );
	CWinFile::BuildPath( m_szOverlayPath, m_szOverlayPath, "Frame Images" );
	CWinFile::GetExePath( m_szBackgroundPath );
	CWinFile::BuildPath( m_szBackgroundPath, m_szBackgroundPath, "Background Images" );

	// Create background pattern
	m_bckfill.Create( NULL, 0, 0, 64, 64 );
	m_bckfill.DeepFill( NULL, GetSysColor( COLOR_3DFACE ), 6 );

	m_btnChooseColor.SetIcon( CGrDC::LoadIcon( IDI_COLOR ) );
	m_btnChooseMaskColor.SetIcon( CGrDC::LoadIcon( IDI_COLOR ) );

	if ( m_hExit == NULL )
	{
		// Are we using XP themes?
		if ( CXpTheme::IsWindowThemed( GetSafeHwnd() ) )
		{
			COLORREF bck = GetSysColor( COLOR_3DFACE );

			CXpTheme xpthm( GetSafeHwnd(), "WINDOW" );
			CWinDc dc( NULL, 0, 0, 24, 24 );		

			// Draw the close button
			CGrDC::FillSolidRect( dc, dc, bck );
			xpthm.DrawThemeBackground( dc, WP_CLOSEBUTTON, CBS_NORMAL, dc, NULL );
			m_hExit = dc.CreateIcon( 24, 24 );

		} // end if

		else m_hExit = CGrDC::LoadIcon( IDI_CLOSE );

	} // end if

	// Set color box
	m_colorbox.SetColor( CFG().GetDword( "CMSettings", "TransColor", 0 ) );
	m_colorMask.SetColor( CFG().GetDword( "CMSettings", "MaskColor", RGB( 255, 255, 255 ) ) );
	m_bMaskImg = CFG().GetDword( "CMSettings", "MaskImg", 0 );
	m_bEnableBackground = CFG().GetDword( "CMSettings", "EnableBackground", 0 );
	m_bEnableOverlay = CFG().GetDword( "CMSettings", "EnableOverlay", 0 );

	m_slideErr.SetRange( 0, 255 );
	m_slideErr.SetPos( CFG().GetDword( "CMSettings", "BckErr", 10 ) );

	UpdateData( FALSE );

	FillOverlayList();
	FillBackgroundList();

//	m_image.SetImage( (DWORD)0 );
	m_image.SetImage( &m_img );

	LoadToolbar();

	DoEnable();			 

	CreateImage();

	// Restore window pos
	CFG().RestoreWindowPos( GetSafeHwnd(), "Positions", "CaptureModeDlg" );

	Size();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSimple::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	Size();	

	Invalidate();
}

void CDlgSimple::Size()
{
	try	{

		// Ensure window
		if ( !::IsWindow( GetSafeHwnd() ) ) return;

		RECT rect, org, ctrl;
		GetClientRect( &rect );
		CopyRect( &org, &rect );

		// Get max control area size
		long xmax = CFG().GetDword( "CMSettings", "XMax", 800 );
		long ymax = CFG().GetDword( "CMSettings", "YMax", 600 );

		// Set max control area size
		if ( RW( rect ) > xmax ) rect.right = rect.left + xmax;
		if ( RH( rect ) > ymax ) rect.bottom = rect.top + ymax;
		CGrDC::Center( &rect, &org );

		ctrl.left = rect.left + 5;
		ctrl.right = ctrl.left + 120;
		ctrl.top = rect.top + 10;
		ctrl.bottom = ctrl.top + 24;

		CWnd *pWnd = GetDlgItem( IDC_ENABLE_OVERLAY );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );

		ctrl.left = ctrl.right + 5;
		ctrl.right = ctrl.left + 120;
		pWnd = GetDlgItem( IDC_ENABLE_BACKGROUND );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );

		ctrl.top = ctrl.bottom + 2;
		ctrl.bottom = rect.bottom - 80;
		ctrl.left = rect.left + 5;
		ctrl.right = ctrl.left + 120;
		pWnd = GetDlgItem( IDC_OVERLAYS );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
		{	pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );
			pWnd->RedrawWindow();
		} // end if

		ctrl.left = ctrl.right + 5;
		ctrl.right = ctrl.left + 120;
		pWnd = GetDlgItem( IDC_BACKGROUNDS );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
		{	pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );
			pWnd->RedrawWindow();
		} // end if

		ctrl.top = ctrl.bottom + 10;
		ctrl.bottom = ctrl.top + 24;
		ctrl.left = rect.left + 10;
		ctrl.right = ctrl.left + 115;
		pWnd = GetDlgItem( IDC_MASKIMG );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );
		
		ctrl.left = ctrl.right + 5;
		ctrl.right = ctrl.left + 80;
		pWnd = GetDlgItem( IDC_COLORBOX );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );

		ctrl.left = ctrl.right + 2;
		ctrl.right = ctrl.left + 38;
		pWnd = GetDlgItem( IDC_CHOOSE_COLOR );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );

		ctrl.top = ctrl.bottom + 5;
		ctrl.bottom = ctrl.top + 24;
		ctrl.left = rect.left + 10;
		ctrl.right = ctrl.left + 80;
		pWnd = GetDlgItem( IDC_MASKCOLOR );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );

		ctrl.left = ctrl.right + 2;
		ctrl.right = ctrl.left + 38;
		pWnd = GetDlgItem( IDC_CHOOSE_MASKCOLOR );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );
		
		ctrl.left = ctrl.right + 5;
		ctrl.right = ctrl.left + 120;
		ctrl.bottom = ctrl.top + 32;
		pWnd = GetDlgItem( IDC_SLIDEERR );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );

		ctrl.left = ctrl.right + 5;
		ctrl.right = rect.right - 5;
		ctrl.top = rect.top + 35;
		ctrl.bottom = rect.bottom - 80;
		pWnd = GetDlgItem( IDC_IMAGE );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );

		ctrl.top = ctrl.bottom + 10;
		ctrl.bottom = rect.bottom - 10;
		pWnd = GetDlgItem( IDC_TOOLBAR );
		if ( pWnd != NULL && ::IsWindow( pWnd->GetSafeHwnd() ) )
			pWnd->SetWindowPos(		NULL, ctrl.left, ctrl.top,
									RW( ctrl ), RH( ctrl ),
									SWP_NOZORDER | SWP_NOACTIVATE );

	} catch( ... ) { ASSERT( 0 ); }
}

LRESULT CDlgSimple::OnToolbar(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{	if ( m_bCapture ) 
		{	DWORD driver = CFG().GetDword( "CMSettings", "Driver", 0 );
			VIDEOLIST().SetFrameRate( driver, VIDEOLIST().GetFrameRate( driver ) );
			CreateImage(); 
		} // end if
	}

	else if ( wParam == 1 ) EmailImage();

	else if ( wParam == 2 ) VIEW()->SendMessage( WM_COMMAND, ID_FILE_PRINT, NULL );

	else if ( wParam == 3 ) Ftp();

	else if ( wParam == 4 ) SaveToFile();

	else if ( wParam == 5 ) 
	{
		CDlgSimpleSettings dlg( this ); 
		dlg.DoModal(); 

		LoadToolbar(); 
		Size();
		RedrawWindow();

	} // end else if
	
	else if ( wParam == 6 ) 
	{	FRAME()->ShowWindow( SW_SHOWNORMAL );
		m_bClose = TRUE; SendMessage( WM_CLOSE, 0, 0L );
	} // end else if

	return 0;
}

void CDlgSimple::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pCtrl = GetDlgItem( IDC_OVERLAYS );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnableOverlay );
	pCtrl = GetDlgItem( IDC_MASKIMG );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnableOverlay );
	pCtrl = GetDlgItem( IDC_CHOOSE_MASKCOLOR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnableOverlay && m_bMaskImg );
	pCtrl = GetDlgItem( IDC_MASKCOLOR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnableOverlay && m_bMaskImg );

	pCtrl = GetDlgItem( IDC_BACKGROUNDS );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnableBackground );
	pCtrl = GetDlgItem( IDC_CHOOSE_COLOR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnableBackground );
	pCtrl = GetDlgItem( IDC_COLORBOX );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnableBackground );
	pCtrl = GetDlgItem( IDC_SLIDEERR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnableBackground );
}

void CDlgSimple::OnEnableBackground() 
{	DoEnable();
	FillBackgroundList();
	CFG().SetValue( "CMSettings", "EnableBackground", 
					(DWORD)( m_bEnableBackground ? 1 : 0 ) );

}

void CDlgSimple::OnEnableOverlay() 
{	DoEnable();
	FillOverlayList();
	CFG().SetValue( "CMSettings", "EnableOverlay",
					(DWORD)( m_bEnableOverlay ? 1 : 0 ) );
}

BOOL CDlgSimple::OnEraseBkgnd(CDC* pDC) 
{
	// Ensure dc
	if ( pDC == NULL ) return FALSE;

	RECT rect;
	GetClientRect( &rect );
	m_bckfill.Tile( pDC->GetSafeHdc(), &rect );

	return TRUE;
//	return CDialog::OnEraseBkgnd(pDC);
}

void CDlgSimple::OnSysColorChange() 
{
	CDialog::OnSysColorChange();
	
	// Create background pattern
	m_bckfill.Create( NULL, 0, 0, 64, 64 );
	m_bckfill.DeepFill( NULL, GetSysColor( COLOR_3DFACE ), 6 );
	
}

void CDlgSimple::FillOverlayList()
{
	m_ovthread.StopThread();

	m_listOverlays.DeleteAllItems();
	m_listOverlays.SetImageList( NULL, LVSIL_NORMAL );
	if ( m_thm_overlays.GetSafeHandle() != NULL )
		m_thm_overlays.DeleteImageList();

	if ( !m_bEnableOverlay ) return;

	m_thm_overlays.Create( 48, 48, ILC_MASK | ILC_COLOR24, 0, 1024 );
	m_listOverlays.SetImageList( &m_thm_overlays, LVSIL_NORMAL );

	// Load images
	m_ovthread.EnumImages( m_szOverlayPath, GetSafeHwnd(), WM_LOADNEXTOVERLAY );

}

void CDlgSimple::FillBackgroundList()
{
	m_bckthread.StopThread();

	m_listBackgrounds.DeleteAllItems();
	m_listBackgrounds.SetImageList( NULL, LVSIL_NORMAL );
	if ( m_thm_backgrounds.GetSafeHandle() != NULL )
		m_thm_backgrounds.DeleteImageList();

	if ( !m_bEnableBackground ) return;

	m_thm_backgrounds.Create( 48, 48, ILC_MASK | ILC_COLOR24, 0, 1024 );
	m_listBackgrounds.SetImageList( &m_thm_backgrounds, LVSIL_NORMAL );

	// Load images
	m_bckthread.EnumImages( m_szBackgroundPath, GetSafeHwnd(), WM_LOADNEXTBCK );
}

BOOL CDlgSimple::CreateImage( LPVIDEOINFO pvi )
{
	UpdateData( TRUE );

	if ( m_capture.GetSafeHdc() == NULL )
	{
		DWORD driver = CFG().GetDword( "CMSettings", "Driver", 0 );

		// Get pvi
		if ( pvi == NULL ) pvi = VIDEOLIST().Add( driver );
		if ( pvi == NULL ) { m_img.Destroy(); return FALSE; }

		// Init capture if needed
		if ( m_bCapture ) VIDEOLIST().SetFrameRate( driver, pvi->framerate );

		// Get dc pointer
		CWinDc *os = pvi->os;

		// Used averaged frame if needed
		if ( ( pvi->f1 & VIF1_FRAMEAVG ) != 0 && pvi->tavg->GetSafeHdc() != NULL )
			os = pvi->tavg;

		// Draw to capture buffer
		os->SetTransparency( FALSE );
		os->SetAntiAlias( FALSE );
		if ( !m_capture.Create( *os, *os ) ) return FALSE;

	} // end if

	// Create image
	if ( !m_img.Create( m_capture, m_capture ) ) return FALSE;

	if ( m_bEnableBackground )
	{
		char fname[ CWF_STRSIZE ];
		CFG().GetValue( "CMSettings", "BackgroundImg", fname, sizeof( fname ) );

		// Load new file if needed
		if ( 	m_backgroundimg.GetSafeHdc() == NULL ||
				strcmpi( fname, m_szBackgroundImg ) )
		{	CWinImg img;
			if ( img.Load( fname ) ) 
			{	strcpy_sz( m_szBackgroundImg, fname );
				CFG().SetValue( "CMSettings", "BackgroundImg", m_szBackgroundImg );
				m_backgroundimg.Create( m_capture, m_capture ); 
				img.Draw( m_backgroundimg, m_backgroundimg );
			} // end if
			else *m_szBackgroundImg = 0;
		} // end if

		// Did we get an image?
		if ( m_backgroundimg.GetSafeHdc() != NULL )
			m_backgroundimg.Draw( m_img, m_img );

	} // end if
		
	if ( m_bEnableBackground )
	{
		// Set up for chroma keying
		m_capture.SetTransparency( TRUE );
		m_capture.SetTransparentColor( CFG().GetDword( "CMSettings", "TransColor", RGB( 0, 0, 0 ) ) );
		DWORD err = CFG().GetDword( "CMSettings", "BckErr", 10 );
		m_capture.SetChromaErr( err, err );
		m_capture.SetAntiAlias( TRUE );

	} // end if
	else 
	{	m_capture.SetTransparency( FALSE );
		m_capture.SetAntiAlias( FALSE );
	} // end else

	// Draw the video
	m_capture.Draw( m_img, m_img );

	if ( m_bEnableOverlay )
	{
		char path[ CWF_STRSIZE ];
		CFG().GetValue( "CMSettings", "OverlayImg", path, sizeof( path ) );

		// Load new file if needed
		if ( 	m_bMaskLoaded != m_bMaskImg ||
				m_overlayimg.GetSafeHdc() == NULL ||
				strcmpi( path, m_szOverlayImg ) )
		{	
			// Save mask status
			m_bMaskLoaded = m_bMaskImg;
			
			CWinImg img;
			if ( img.Load( path ) ) 
			{	
				// Save new file name
				strcpy_sz( m_szOverlayImg, path );

				// Create offscreen buffer
				m_overlayimg.Create( m_capture, m_capture ); 
				m_overlayimg.ReleaseMask();

				// Grayscale image if mask
				if ( m_bMaskImg ) img.GrayScale();

				// Draw the image
				img.Draw( m_overlayimg, m_overlayimg );

			} // end if

		} // end if

		m_overlayimg.SetTransparency( TRUE );	
		m_overlayimg.SetTransparentColor( RGB( 255, 255, 255 ) );
//			m_overlayimg.SetChromaErr( 10, 10 );
		m_overlayimg.SetAntiAlias( TRUE );

		// Did we get an image?
		if ( m_overlayimg.GetSafeHdc() != NULL )
		{
			if ( m_bMaskImg ) 
				m_img.Mask( &m_overlayimg, NULL, NULL, CFG().GetDword( "CMSettings", "MaskColor", RGB( 255, 255, 255 ) ) );
			else
			{	m_overlayimg.SetTransparency( TRUE );
				m_overlayimg.SetTransparentColor( RGB( 255, 255, 255 ) );
				m_overlayimg.Draw( m_img, m_img );
			} // end else
		} // end if

	} // end if

	CWnd *pCtrl = GetDlgItem( IDC_IMAGE );
	if ( pCtrl != NULL ) pCtrl->RedrawWindow();

	return TRUE;
}

void CDlgSimple::OnClickBackgrounds(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int sel = CComCtrl::ListGetFirstSelectedItem( m_listBackgrounds.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	char fname[ CWF_STRSIZE ];
	m_listBackgrounds.GetItemText( sel, 0, fname, sizeof( fname ) );
	CWinFile::BuildPath( fname, m_szBackgroundPath, fname );	

	CFG().SetValue( "CMSettings", "BackgroundImg", fname );

	CreateImage();
	
	*pResult = 0;
}

void CDlgSimple::OnClickOverlays(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	int sel = CComCtrl::ListGetFirstSelectedItem( m_listOverlays.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	char fname[ CWF_STRSIZE ];
	m_listOverlays.GetItemText( sel, 0, fname, sizeof( fname ) );

	// Save mask status
	if ( !strnicmp( fname, "mask_", 5 ) ) m_bMaskImg = TRUE;
	else m_bMaskImg = FALSE;
	CFG().SetValue( "CMSettings", "MaskImg", 
					(DWORD)( m_bMaskImg ? 1 : 0 ) );

	UpdateData( FALSE ); DoEnable();

	// Build full path
	CWinFile::BuildPath( fname, m_szOverlayPath, fname );	
	CFG().SetValue( "CMSettings", "OverlayImg", fname );

	// Recreate the image
	CreateImage();
	
	*pResult = 0;
}

LRESULT CDlgSimple::OnFrameMsg(WPARAM wParam, LPARAM lParam)
{
	// Ensure visible
	if ( !::IsWindow( GetSafeHwnd() ) || !IsWindowVisible() ) return 0;

	if ( !m_bCapture ) return 0;

	LPFRAMECALLBACKINFO	pfci = (LPFRAMECALLBACKINFO)lParam;	
	if ( pfci == NULL ) return 0;

	// Get pvi
	DWORD driver = pfci->pCap->GetDriverIndex();
	LPVIDEOINFO pvi = VIDEOLIST().FindByIndex( driver );
	if ( pvi == NULL ) return 0;

	// Ensure proper driver
	if ( pvi->index != driver ) return 0;

	// Get dc pointer
	CWinDc *os = pvi->os;

	// Used averaged frame if needed
	if ( ( pvi->f1 & VIF1_FRAMEAVG ) != 0 && pvi->tavg->GetSafeHdc() != NULL )
		os = pvi->tavg;

	// Ensure os
	if ( os == NULL || os->GetSafeHdc() == NULL )
		return FALSE;

	// Create dc if needed
	if ( m_capture.GetSafeHdc() == NULL )
		m_capture.Create( *os, *os );

	// Draw the image
	os->Draw( m_capture, m_capture );

	// Recreate our image
	CreateImage( pvi );

	return 1;
}


void CDlgSimple::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = 640;
	lpMMI->ptMinTrackSize.y = 480;
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CDlgSimple::EmailImage()
{
	char from[ CWF_STRSIZE ] = { 0 };
	char fname[ CWF_STRSIZE ] = { 0 };
	CFG().GetValue( "Settings", "EmailFrom", from, sizeof( from ) );
	CFG().GetValue( "Settings", "EmailFilename", fname, sizeof( fname ) );

	CMime	mime;
	if ( !mime.Create( NULL, from, NULL ) ) return FALSE;
	
	// Use default file name if none specified
	if ( *fname == 0 ) strcpy( fname, "webcam.jpg" );

	// Attach file if needed
	if ( *fname != 0 )
	{
		CWinImg		img;
		LPBYTE		buf = NULL;
		DWORD		size = 0;
		
		// Get the image
		if (	img.CreateFromHBITMAP( m_img ) && 
				FRAME()->SetQuality( &img ) &&
				img.Encode( &buf, &size, fname ) &&
				buf != NULL && size > 0 )
			mime.AddFile( fname, buf, size );

	} // end if

	// Flag message as unsent
	mime.AddToHeader( "X-Unsent", "1" );

	char path[ CWF_STRSIZE ];

	// Save the message
	mime.Save( CWinFile::BuildPath( path, CWinFile::GetExePath( path ), "_Email.eml" ) );

	// Open default mailer
	ShellExecute( NULL, "open", path, NULL, NULL, SW_SHOWNORMAL );

	return TRUE;
}

void CDlgSimple::OnClose() 
{	Shutdown();	
//	CDialog::OnClose();
}

void CDlgSimple::OnDestroy() 
{
	Shutdown();	
//	CDialog::OnDestroy();	
}

void CDlgSimple::OnOK() 
{	Shutdown();	
//	CDialog::OnOK();
}

void CDlgSimple::OnCancel() 
{	Shutdown();	
//	CDialog::OnCancel();
}

void CDlgSimple::Shutdown()
{
	// Save window pos
	CFG().SaveWindowPos( GetSafeHwnd(), "Positions", "CaptureModeDlg" );

	// Are we closing the app?
	if ( !m_bClose ) FRAME()->SendMessage( WM_CLOSE, 0, 0L );
	else { m_bClose = FALSE; DestroyWindow(); }
}


void CDlgSimple::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CDlgSimple::SaveToFile()
{
	CWinImg img;
	if ( !img.CreateFromHBITMAP( m_img ) )
	{	::MessageBox(	GetSafeHwnd(), img.GetLastError(), 
						"Unable to save image", MB_OK | MB_ICONEXCLAMATION );
		return;
	} // end if

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
									 GetSafeHwnd() ) ) return;

	// Save the image
	FRAME()->SetQuality( &img );
	if ( !img.Save( filename ) )
	{	::MessageBox(	GetSafeHwnd(), img.GetLastError(), 
						"Unable to save image", MB_OK | MB_ICONEXCLAMATION );
		return;
	} // end if
}

BOOL CDlgSimple::Ftp()
{
	LPBYTE buf = NULL;
	DWORD size = 0;
	CWinImg img;

	// Get the image
	if ( !img.CreateFromHBITMAP( m_img ) )
		return FALSE;

	// Set jpeg quality
	FRAME()->SetQuality( &img );

	char fname[ CWF_STRSIZE ] = { 0 };
	if ( !CFG().GetValue( "CMSettings", "FtpFilename", fname, sizeof( fname ) ) || *fname == 0 )
		strcpy( fname, "webcam.jpg" );

	// Encode the file
	if ( !img.Encode( &buf, &size, fname ) ) return FALSE;

	// Copy the memory
	if ( !m_mem.put( buf, size ) ) return FALSE;

	// Upload the image
	FtpImage(	CFG().GetSz( "CMSettings", "FtpServer", "" ), 
				&m_ftp, m_mem, m_mem.size(), fname );

	return TRUE;
}

BOOL CDlgSimple::FtpImage(LPCTSTR pServer, CFtp *pFtp, LPBYTE buf, DWORD size, LPCTSTR pPath)
{
	// Sanity checks
	if ( pServer == NULL || pFtp == NULL || buf == NULL || size == 0 || pPath == NULL )
		return FALSE;

	// Is the FTP already working?
	if ( pFtp->IsConnecting() || pFtp->IsConnected() )
		return FALSE;
	
	// Ensure we have server information
	HGROUP hGroup = FTPSERVERS().FindGroup( pServer );
	if ( hGroup == NULL ) return FALSE;

	char user[ CWF_STRSIZE ];
	char password[ CWF_STRSIZE ];
	char addr[ CWF_STRSIZE ];
	char folder[ CWF_STRSIZE ];
	DWORD port = 21;

	// Get server information
	FTPSERVERS().GetValue( hGroup, "Address", addr, sizeof( addr ) );
	FTPSERVERS().GetValue( hGroup, "Folder", folder, sizeof( folder ) );
	FTPSERVERS().GetValue( hGroup, "Port", &port );
	FTPSERVERS().GetValue( hGroup, "Username", user, sizeof( user ) );
	FTPSERVERS().GetValue( hGroup, "Password", password, sizeof( password ) );

	// Connect to server
	pFtp->SetUsernamePassword( user, password );
	if ( !pFtp->Connect( addr, port ) ) return FALSE;

	// Build full path to remote file
	CWinFile::BuildPath( folder, folder, pPath );

	// Upload file
	pFtp->AutoClose( TRUE );
	if ( !pFtp->Upload( buf, size, folder ) ) return FALSE;

	return TRUE;
}

void CDlgSimple::LoadToolbar()
{
	m_toolbar.Empty();
	m_toolbar.GetToolbar()->SetHeight( 42 );
	m_toolbar.GetToolbar()->SetMessageTarget( GetSafeHwnd(), WM_TOOLBAR );
	m_toolbar.GetToolbar()->AddItem( "Start Capture", 0, CGrDC::LoadIcon( IDI_CAMERA ), NULL, &m_bCapture, "Enable Capture" );
	if ( CFG().GetDword( "CMSettings", "EmailBtn", 1 ) != 0 )
		m_toolbar.GetToolbar()->AddItem( "E-mail", 1, CGrDC::LoadIcon( IDI_LETTER ), NULL, NULL, "E-Mail" );
	if ( CFG().GetDword( "CMSettings", "PrintBtn", 1 ) != 0 )
		m_toolbar.GetToolbar()->AddItem( "Print", 2, CGrDC::LoadIcon( IDI_PRINTER ), NULL, NULL, "Print" );
	if (	CFG().GetDword( "CMSettings", "FtpBtn", 1 ) != 0 &&
			*CFG().GetSz( "CMSettings", "FtpServer", "" ) != 0 )
		m_toolbar.GetToolbar()->AddItem( "FTP Upload", 3, CGrDC::LoadIcon( IDI_SERVERUPLOAD ), NULL, NULL, "FTP Upload" );
	if ( CFG().GetDword( "CMSettings", "SaveBtn", 1 ) != 0 )
		m_toolbar.GetToolbar()->AddItem( "Save", 4, CGrDC::LoadIcon( IDI_SAVEDISK ), NULL, NULL, "Save As File" );
	if ( CFG().GetDword( "CMSettings", "SetupBtn", 1 ) != 0 )
		m_toolbar.GetToolbar()->AddItem( "Settings", 5, CGrDC::LoadIcon( IDI_LARGEWRENCH ), NULL, NULL, "Settings" );
	m_toolbar.GetToolbar()->AddItem( "Advanced Mode", 6, CGrDC::LoadIcon( IDI_LARGEWHITEKNIGHT ), NULL, NULL, "Advanced Mode" );
	m_toolbar.GetToolbar()->AddItem( "Advanced Mode", 7, CGrDC::LoadIcon( IDI_HELP ), NULL, NULL, "Help" );

	m_toolbar.RedrawWindow();
}

void CDlgSimple::OnChooseColor() 
{
	COLORREF rgb = m_colorbox.GetColor();
	if ( !CWinFile::ChooseColor( &rgb, GetSafeHwnd() ) )
		return;

	// Update new color
	m_colorbox.SetColor( rgb );
	CFG().SetValue( "CMSettings", "TransColor", rgb );

	CreateImage();

}

void CDlgSimple::OnMaskimg() 
{	UpdateData( TRUE );
	DoEnable();
	CFG().SetValue( "CMSettings", "MaskImg", 
					(DWORD)( m_bMaskImg ? 1 : 0 ) );
	CreateImage();
}

LRESULT CDlgSimple::OnLoadNextOverlay(WPARAM wParam, LPARAM lParam)
{
	CWinImg	*pImg = (CWinImg*)lParam;
	if ( pImg == NULL ) return 0;

	// Get this file name
	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( pImg->GetFileName(), fname );

	// Add to list
	HICON hIcon = pImg->CreateIcon( 48, 48 );
	int icon = m_thm_overlays.Add( hIcon );
	int item = m_listOverlays.InsertItem(	m_listOverlays.GetItemCount(),
											fname, icon );

	// Get current file name
	char cname[ CWF_STRSIZE ];
	CFG().GetValue( "CMSettings", "OverlayImg", cname, sizeof( cname ) );
	CWinFile::GetFileNameFromPath( cname, cname );

	// Is this the file?
	if ( !strcmpi( cname, fname ) )
	{	m_listOverlays.SetItemState( item, LVIS_SELECTED, LVIS_SELECTED );
		m_listOverlays.EnsureVisible( item, FALSE );
	} // end if

	return 1;
}

void CDlgSimple::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	long pos = pScrollBar->GetScrollPos();

	int min = 0, max = 100;
//	pScrollBar->GetScrollRange( &min, &max );

	switch( nSBCode )
	{
		case SB_LEFT : pos = min;							break;
		case SB_ENDSCROLL :									break;
		case SB_LINELEFT : pos -= 1;						break;
		case SB_LINERIGHT : pos += 1;						break;
		case SB_PAGELEFT : pos -= ( max - min ) / 10;		break;
		case SB_PAGERIGHT : pos += ( max - min ) / 10;		break;
		case SB_RIGHT : pos = max;							break;
		case SB_THUMBPOSITION : pos = long( nPos );			break;
		case SB_THUMBTRACK : pos = long( nPos );			break;
		default : return; break;

	} // end switch
	
	if ( (CSliderCtrl*)pScrollBar == &m_slideErr )
	{
		if ( nSBCode == SB_ENDSCROLL )
			pos = m_slideErr.GetPos();
		else m_slideErr.SetPos( pos );

		// Set err value
		CFG().SetValue( "CMSettings", "BckErr", (DWORD)pos );

		UpdateData( FALSE );

//		if ( nSBCode == SB_ENDSCROLL )
		{
			CreateImage();
		} // end if
	} // end if
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CDlgSimple::OnLoadNextBck(WPARAM wParam, LPARAM lParam)
{
	CWinImg	*pImg = (CWinImg*)lParam;
	if ( pImg == NULL ) return 0;

	// Get this file name
	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( pImg->GetFileName(), fname );

	HICON hIcon = pImg->CreateIcon( 48, 48 );
	int icon = m_thm_backgrounds.Add( hIcon );
	int item = m_listBackgrounds.InsertItem(	m_listBackgrounds.GetItemCount(),
												fname, icon );

	// Get current file name
	char cname[ CWF_STRSIZE ];
	CFG().GetValue( "CMSettings", "BackgroundImg", cname, sizeof( cname ) );
	CWinFile::GetFileNameFromPath( cname, cname );

	// Is this the file?
	if ( !strcmpi( cname, fname ) )
	{	m_listBackgrounds.SetItemState( item, LVIS_SELECTED, LVIS_SELECTED );
		m_listBackgrounds.EnsureVisible( item, FALSE );
	} // end if

	return 1;
}

BOOL CDlgSimple::DestroyWindow() 
{
	m_ovthread.StopThread();	
	m_bckthread.StopThread();	
	
	return CDialog::DestroyWindow();
}

void CDlgSimple::OnChooseMaskcolor() 
{
	COLORREF rgb = m_colorMask.GetColor();
	if ( !CWinFile::ChooseColor( &rgb, GetSafeHwnd() ) )
		return;

	// Update new color
	m_colorMask.SetColor( rgb );
	CFG().SetValue( "CMSettings", "MaskColor", rgb );
	CreateImage();
}
