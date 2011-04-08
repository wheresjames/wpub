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
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Vp.h"

#include <AfxAdv.h>
#include <AfxPriv.h>

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#include "ChooseDriverDlg.h"
#include "DlgOnClose.h"
#include "DlgNewPubImg.h"

#include "DlgTreeFrame.h"
#include "DlgGeneral.h"
#include "DlgServer.h"
#include "DlgServerLog.h"
#include "DlgServerMap.h"
#include "DlgFtp.h"
#include "DlgPublishing.h"
#include "DlgEmailSettings.h"
#include "DlgMotion.h"
#include "DlgLogFiles.h"
#include "DlgCgi.h"
#include "DlgJpegSettings.h"
#include "DlgChat.h"
#include "DlgVideoSettings.h"

#include "WzFrame.h"
#include "WzWelcome.h"
#include "WzCam.h"
#include "WzPubType.h"
#include "WzLocalServer.h"

#include "DlgFtpList.h"
#include "DlgEmailList.h"
#include "WzSave.h"
#include "WzEmailMsgInfo.h"

#include "DlgTabFrame.h"
#include "PgPubWhat.h"
#include "PgPubWhere.h"
#include "PgPubWhen.h"
#include "PgPubHow.h"
#include "PgPubThumbnail.h"
#include "PgPubSounds.h"

#include "DlgSplash.h"
#include "DlgInform.h"

#include "HttpSession.h"

#include "DlgProductUpdate.h"

#include "DlgHTMLBuild.h"

#include "DlgSimple.h"
#include "DlgSimpleSettings.h"

#include "DlgMsgBox.h"

#include "DlgInform.h"

#include "HttpSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMERID						101
#define GRAPHTIMER					102

#define ID_CONTROLBAR1				201
#define ID_CONTROLBAR2				202
#define ID_CONTROLBAR3				203
#define ID_CONTROLBAR4				204
#define ID_CONTROLBAR5				205
#define ID_CONTROLBAR6				206
#define ID_CONTROLBAR7				207
#define ID_CONTROLBAR8				208
#define ID_CONTROLBAR9				209
#define ID_CONTROLBAR10				210

#define WM_IMGLIST					( WM_USER + 101 )
#define WM_WEBSERVER				( WM_USER + 102 )
#define WM_WEBSESSION				( WM_USER + 103 )
#define WM_TRAYICON					( WM_USER + 104 )
#define WM_FRAMEREFRESH				( WM_USER + 105 )
#define WM_EXITAPP					( WM_USER + 106 )
#define WM_TOOLBAR					( WM_USER + 107 )
#define WM_SETSTATUS				( WM_USER + 108 )
#define WM_MAINTOOLBARCMD			( WM_USER + 109 )
#define WM_MAINTOOLBARUPDATE		( WM_USER + 110 )
#define WM_OVERLAYTOOLBARCMD		( WM_USER + 111 )
#define WM_OVERLAYTOOLBARUPDATE		( WM_USER + 112 )
#define WM_WEBSERVERTOOLBARCMD		( WM_USER + 113 )
#define WM_WEBSERVERTOOLBARUPDATE	( WM_USER + 114 )

#define ID_VIDMENUBASE				4000


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_COMMAND_EX(CG_ID_VIEW_IMAGE, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_IMAGE, OnUpdateControlBarMenu)
	ON_COMMAND_EX(CG_ID_VIEW_PUBIMGVIEW, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_PUBIMGVIEW, OnUpdateControlBarMenu)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ENDSESSION()
	ON_WM_QUERYENDSESSION()
	ON_COMMAND(ID_POPUP_COPY, OnPopupCopy)
	ON_COMMAND(ID_POPUP_CUT, OnPopupCut)
	ON_COMMAND(ID_POPUP_DELETE, OnPopupDelete)
	ON_COMMAND(ID_POPUP_DUPLICATE, OnPopupDuplicate)
	ON_COMMAND(ID_POPUP_PASTE, OnPopupPaste)
	ON_COMMAND(ID_POPUP_PROPERTIES, OnPopupProperties)
	ON_COMMAND(ID_OPTIONS_SETTINGS, OnOptionsSettings)
	ON_COMMAND(ID_TRAYICON_ABOUT, OnTrayiconAbout)
	ON_COMMAND(ID_TRAYICON_EMAILIMAGE, OnTrayiconEmailimage)
	ON_COMMAND(ID_TRAYICON_ENABLEPUBLISHING, OnTrayiconEnablepublishing)
	ON_COMMAND(ID_TRAYICON_EXIT, OnTrayiconExit)
	ON_COMMAND(ID_SAVEIMAGE, OnSaveimage)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_OPTIONS_CHECKFORUPDATE, OnOptionsCheckforupdate)
	ON_COMMAND(ID_ENABLEPUBLISHING, OnEnablepublishing)
	ON_UPDATE_COMMAND_UI(ID_ENABLEPUBLISHING, OnUpdateEnablepublishing)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_UPDATE_COMMAND_UI(ID_REFRESH, OnUpdateRefresh)
	ON_COMMAND(ID_HELP_REGISTRATION, OnHelpRegistration)
	ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
	ON_COMMAND(ID_VIEW_OVERLAYBAR, OnViewOverlaybar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OVERLAYBAR, OnUpdateViewOverlaybar)
	ON_COMMAND(ID_VIEW_WEBSERVER, OnViewWebserver)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WEBSERVER, OnUpdateViewWebserver)
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateViewProperties)
	ON_COMMAND(ID_OPTIONS_HTMLBUILDER, OnOptionsHtmlbuilder)
	ON_COMMAND(ID_VIEW_PUBIMGVIEW, OnViewPubimgview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PUBIMGVIEW, OnUpdateViewPubimgview)
	ON_COMMAND(ID_OPTIONS_SETUPWIZARD, OnOptionsSetupwizard)
	ON_COMMAND(ID_FILE_LOADOVERLAY, OnFileLoadoverlay)
	ON_COMMAND(ID_FILE_SAVEOVERLAY, OnFileSaveoverlay)
	ON_COMMAND(ID_FILE_MERGEOVERLAY, OnFileMergeoverlay)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_SIMPLE, OnSimple)
	ON_COMMAND(ID_SETUPSIMPLE, OnSetupsimple)
	ON_COMMAND(ID_VIEW_EVENTLOG, OnViewEventlog)
	ON_COMMAND(ID_HELP_HELP, OnHelpHelp)
	ON_COMMAND(ID_VIEW_GRAPHBAR, OnViewGraphbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRAPHBAR, OnUpdateViewGraphbar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_COMMAND(ID_NEWIMAGE, OnNewimage)
	ON_COMMAND(ID_DELETEIMAGE, OnDeleteimage)
	ON_COMMAND(ID_VIEWIMAGE, OnViewimage)
	//}}AFX_MSG_MAP

	ON_REGISTERED_MESSAGE( m_uTaskBarCreated, OnTaskBarCreated )

	ON_MESSAGE( WM_TRAYICON, OnTrayIcon )
	ON_MESSAGE( WM_IMGLIST, OnImgList )
	ON_MESSAGE( WM_FRAMEMSG, OnFrameMsg )
	ON_MESSAGE( WM_WEBSERVER, OnWebServer )
	ON_MESSAGE( WM_WEBSESSION, OnWebSession )
	ON_MESSAGE( WM_FRAMEREFRESH, OnFrameRefresh )

	ON_MESSAGE( WM_EXITAPP, OnExitApp )

	ON_COMMAND_RANGE( ID_VIDMENUBASE, ID_VIDMENUBASE + ( 4 * 128 ), OnVideoMenu ) 
	ON_UPDATE_COMMAND_UI_RANGE( ID_VIDMENUBASE, ID_VIDMENUBASE + ( 4 * 128 ), OnVideoMenuUpdate )

	ON_BN_CLICKED(IDC_RUN, OnRun)

	ON_MESSAGE( WM_TOOLBAR, OnToolbar )

	ON_MESSAGE( WM_SETSTATUS, OnSetStatus )

	ON_MESSAGE( WM_SETEVENT, OnEvent )

	ON_MESSAGE( WM_MAINTOOLBARCMD, OnMainToolbarCmd )
	ON_MESSAGE( WM_MAINTOOLBARUPDATE, OnMainToolbarUpdate )

	ON_MESSAGE( WM_OVERLAYTOOLBARCMD, OnOverlayToolbarCmd )
	ON_MESSAGE( WM_OVERLAYTOOLBARUPDATE, OnOverlayToolbarUpdate )

	ON_MESSAGE( WM_WEBSERVERTOOLBARCMD, OnWebServerToolbarCmd )
	ON_MESSAGE( WM_WEBSERVERTOOLBARUPDATE, OnWebServerToolbarUpdate )

	END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

// We need to know when the task bar reinitializes
UINT CMainFrame::m_uTaskBarCreated = ::RegisterWindowMessage( "TaskbarCreated" );

CMainFrame::CMainFrame()
{
	m_bExit = FALSE;
	m_bPaused = FALSE;
	m_dwHits = 0;

	m_bEnablePublishing = FALSE;
	m_bEnableRefresh = TRUE;

	m_hMotionWnd = NULL;

	m_hRefreshWnd = NULL;
	m_uRefreshMsg = 0;

	m_dwLastPage = 0;

	m_dwStatusTimeout = 0;

	m_uTimer = 0;
	m_uGraphTimer = 0;
	m_dwUpdateTimer = 0;

	m_bResize = FALSE;

	m_dwExpectDelays = 0;
}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Are we using XP themes?
	if ( CXpTheme::IsWindowThemed( GetSafeHwnd() ) )
	{
		COLORREF bck = GetSysColor( COLOR_3DFACE );

		// Open button themes
		CXpTheme xpthm( GetSafeHwnd(), "BUTTON" );
		CWinDc dc( NULL, 0, 0, 16, 16 );		
		
		CGrDC::FillSolidRect( dc, dc, bck );
		xpthm.DrawThemeBackground( dc, BP_CHECKBOX, CBS_CHECKEDNORMAL, dc, NULL );
		m_hChecked = dc.CreateIcon( 16, 16 );

		CGrDC::FillSolidRect( dc, dc, bck );
		xpthm.DrawThemeBackground( dc, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, dc, NULL );
		m_hUnchecked = dc.CreateIcon( 16, 16 );

	} // end if
	else
	{
		m_hChecked = CGrDC::LoadIcon( IDI_CHECK );
		m_hUnchecked = CGrDC::LoadIcon( IDI_UNCHECK );
	} // end else

	// Get html icon
	m_hHtml = CWinFile::GetIconForFile( "Index.htm", FALSE );
	if ( m_hHtml == NULL ) m_hHtml = CGrDC::LoadIcon( IDI_WEB );

	if (!m_wndStatusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if ( !m_wndMainToolbar.Create(	this, IDD_MAINTOOLBAR, 
									WS_CHILD | WS_VISIBLE |
									CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, 
									ID_CONTROLBAR9 ) )
	{
		TRACE0("Failed to create dialog bar m_wndServerBar\n");
		return -1;		// fail to create
	}
//	m_wndServerBar.EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar( &m_wndServerBar, AFX_IDW_DOCKBAR_TOP );
	AddControlBar( &m_wndMainToolbar );

/*
//	if (!m_wndToolBar.Create(this) ||
//		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY,
		CRect( 0, 0, 0, 0 ), ID_CONTROLBAR2) ||
		!m_wndToolBar.LoadToolBar( IDR_MAINFRAME ) )
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar( &m_wndToolBar, AFX_IDW_DOCKBAR_TOP );
	AddControlBar( &m_wndToolBar );
*/
/*	if ( !m_wndServerBar.Create(	this, CG_IDD_SERVER, 
									WS_CHILD | WS_VISIBLE |
									CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, 
									ID_CONTROLBAR3 ) )
	{
		TRACE0("Failed to create dialog bar m_wndServerBar\n");
		return -1;		// fail to create
	}
//	m_wndServerBar.EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar( &m_wndServerBar, AFX_IDW_DOCKBAR_TOP );
	AddControlBar( &m_wndServerBar );

*/

/*	// Create Insert Bar
	if (!m_wndInsertBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
		CRect( 0, 0, 0, 0 ), ID_CONTROLBAR4 ) ||
		!m_wndInsertBar.LoadToolBar( IDT_INSERT ) )
	{
		TRACE0("Failed to create insert toolbar\n");
		return -1;      // fail to create
	}

	m_wndInsertBar.EnableDocking( CBRS_ALIGN_ANY );
	DockControlBar( &m_wndInsertBar, AFX_IDW_DOCKBAR_LEFT );
*/
	if ( !m_wndWebServerToolbar.Create(	this, IDD_MAINTOOLBAR, 
										WS_CHILD | WS_VISIBLE |
										CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, 
										ID_CONTROLBAR10 ) )
	{
		TRACE0("Failed to create dialog bar m_wndServerBar\n");
		return -1;		// fail to create
	}

//	m_wndOverlayToolbar.EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar( &m_wndOverlayToolbar, AFX_IDW_DOCKBAR_LEFT );
	AddControlBar( &m_wndWebServerToolbar );

	// Enable docking to main window
	EnableDocking(CBRS_ALIGN_ANY);

	// Create publishing image bar
	m_pubimgbar.SetHorzSize( CSize( 100, 160 ) );
	m_pubimgbar.SetVertSize( CSize( 100, 160 ) );
	m_pubimgbar.SetFloatSize( CSize( 400, 160 ) );
    if (!m_pubimgbar.Create(_T("Image List"), this, ID_CONTROLBAR5))
    {   TRACE0("Failed to create propbar\n");
        return -1;      // fail to create
	}
    m_pubimgbar.SetBarStyle(m_propbar.GetBarStyle() |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    m_pubimgbar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_pubimgbar, AFX_IDW_DOCKBAR_BOTTOM);

	// Set the property view
	m_pubimgbar.SetSCBStyle(m_pubimgbar.GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	m_imglist.SetMessageTarget( GetSafeHwnd(), WM_IMGLIST );
	m_imglist.SetImgList( &m_pubimglist );
	m_imglist.Init( m_pubimgbar.GetSafeHwnd() );

//	m_dlgpubimg.Init( m_pubimgbar.GetSafeHwnd(), NULL );
//	m_dlgpubimg.ShowWindow( SW_SHOWNORMAL );

	// Set the image list
//	m_dlgpubimg.SetImgList( &m_pubimglist );
//	m_dlgpubimg.SetCallback( GetSafeHwnd(), WM_IMGLIST );	

	m_propbar.SetVertSize( CSize( 300, 260 ) );
	m_propbar.SetFloatSize( CSize( 260, 400 ) );
    if (!m_propbar.Create(_T("Properties"), this, ID_CONTROLBAR6))
    {   TRACE0("Failed to create propbar\n");
        return -1;      // fail to create
	} // end if
    m_propbar.SetBarStyle(m_propbar.GetBarStyle() |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    m_propbar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    DockControlBar(&m_propbar, AFX_IDW_DOCKBAR_RIGHT);

	// Set the property view
	m_propbar.SetSCBStyle(m_propbar.GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);
	m_dlgprop.Create( CDlgProp::IDD, &m_propbar );
	m_dlgprop.ShowWindow( SW_SHOWNORMAL );
	
	// Create the graph bar
	m_graphbar.SetHorzSize( CSize( 100, 60 ) );
	m_graphbar.SetVertSize( CSize( 100, 60 ) );
	m_graphbar.SetFloatSize( CSize( 400, 60 ) );
    if (!m_graphbar.Create(_T("Properties"), this, ID_CONTROLBAR7))
    {   TRACE0("Failed to create propbar\n");
        return -1;      // fail to create
	} // end if
    m_graphbar.SetBarStyle(m_propbar.GetBarStyle() |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    m_graphbar.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
    DockControlBar(&m_graphbar, AFX_IDW_DOCKBAR_BOTTOM);

	// Set the property view
	m_graphbar.SetSCBStyle(m_graphbar.GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	RECT rect;
	m_graphbar.GetClientRect( &rect );
	m_graph.Create( NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, rect, &m_graphbar );
	m_graph.ShowWindow( SW_SHOWNORMAL );

	if ( !m_wndOverlayToolbar.Create(	this, IDD_OVERLAYTOOLBAR, 
										WS_CHILD | WS_VISIBLE |
										CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY, 
										ID_CONTROLBAR9 ) )
	{
		TRACE0("Failed to create dialog bar m_wndServerBar\n");
		return -1;		// fail to create
	}

//	m_wndOverlayToolbar.EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar( &m_wndOverlayToolbar, AFX_IDW_DOCKBAR_LEFT );
	AddControlBar( &m_wndOverlayToolbar );

	// Restore toolbar positinos if needed
	if ( CFG().GetDword( "Settings", "RestoreToolbars", 0 ) != 0 )
	{
		try
		{
			// Restore toolbar settings
			if ( VerifyBarState( "ToolBarPositions" ) )
				LoadBarState( "ToolBarPositions" );
			else CRKey::DeleteKey( HKEY_SETTINGS, SZKEY_SETTINGS );
		} // end try
		catch( ... ) 
		{
			CRKey::DeleteKey( HKEY_SETTINGS, SZKEY_SETTINGS );
			ASSERT( 0 ); 
		}

	} // end if

	// Create video menu
	CreateVideoMenu();

	// Setup the webserver
	m_isession.SetCreateDelete( CHttpSession::sCreate, CHttpSession::sDelete );
	m_isession.StartThread();
	m_iserver.SetSessionList( &m_isession );

	// Install the tray icon
	TrayIcon( APPNAME );

//	m_servertoolbar.Empty();
//	m_servertoolbar.GetToolbar()->SetHeight( 26 );
//	m_servertoolbar.GetToolbar()->SetMessageTarget( GetSafeHwnd(), WM_TOOLBAR );
//	m_servertoolbar.GetToolbar()->AddItem( "Home", 1, CGrDC::LoadIcon( IDI_WEB ), NULL, NULL, "Open Home Page" );
//	m_servertoolbar.GetToolbar()->AddItem( "Open", 2, CGrDC::LoadIcon( IDI_FOLDER ), NULL, NULL, "Open Root Folder" );
/*
	// Position toolbar
	if ( ::IsWindow( m_wndServerBar.GetSafeHwnd() ) )
	{
		CWnd *pCtrl = m_wndServerBar.GetDlgItem( IDC_TOOLBAR );
		if ( pCtrl != NULL ) 
		{	RECT rect;
			pCtrl->GetWindowRect( &rect );
			m_wndServerBar.ScreenToClient( &rect );
			m_servertoolbar.Create( NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
									rect, &m_wndServerBar );
		} // end if

	} // end if
*/
	// Position toolbar
	if ( ::IsWindow( m_wndMainToolbar.GetSafeHwnd() ) )
	{
		RECT rect;
		SetRect( &rect, 0, 0, 10, 10 );
		m_maintoolbar.Create( NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
								rect, &m_wndMainToolbar );
	} // end if

	m_maintoolbar.SetMessageTarget( GetSafeHwnd(), WM_MAINTOOLBARCMD, WM_MAINTOOLBARUPDATE );
	m_maintoolbar.Add( 1, CGrDC::LoadIcon( IDI_NEWIMAGEWIZARD ), NULL, "New Image" );
	m_maintoolbar.Add( 2, CGrDC::LoadIcon( IDI_X ), NULL, "Delete Image" );
	m_maintoolbar.Add( 0, NULL, NULL, NULL );
	m_maintoolbar.Add( 3, CGrDC::LoadIcon( IDI_PUBLISHING ), NULL, "Enable Publishing" );
	m_maintoolbar.Add( 4, CGrDC::LoadIcon( IDI_GREENREFRESH ), NULL, "Enable Video Refresh" );
	m_maintoolbar.Add( 0, NULL, NULL, NULL );
	m_maintoolbar.Add( 5, CGrDC::LoadIcon( IDI_PROPERTIES ), NULL, "Properties" );
	m_maintoolbar.Add( 13, CGrDC::LoadIcon( IDI_EYEBALL ), NULL, "View Image" );
	m_maintoolbar.Add( 0, NULL, NULL, NULL );
	m_maintoolbar.Add( 6, CGrDC::LoadIcon( IDI_SMALL_LETTER ), NULL, "E-Mail Image" );
	m_maintoolbar.Add( 7, CGrDC::LoadIcon( IDI_DISK ), NULL, "Save Image" );
	m_maintoolbar.Add( 0, NULL, NULL, NULL );
	m_maintoolbar.Add( 8, CGrDC::LoadIcon( IDI_WRENCH ), NULL, "Application Settings" );
	m_maintoolbar.Add( 9, CGrDC::LoadIcon( IDI_SMALL_BLOCKS ), NULL, "HTML Publisher" );
	m_maintoolbar.Add( 0, NULL, NULL, NULL );
	m_maintoolbar.Add( 10, CGrDC::LoadIcon( IDI_SMALL_CAMERA ), NULL, "Capture Mode" );
	m_maintoolbar.Add( 11, CGrDC::LoadIcon( IDI_SMALL_CAMERA_SETTINGS ), NULL, "Capture Mode Settings" );
	m_maintoolbar.Add( 0, NULL, NULL, NULL );
	m_maintoolbar.Add( 12, CGrDC::LoadIcon( IDI_ABOUT ), NULL, "About" );

	// Position toolbar
	if ( ::IsWindow( m_wndWebServerToolbar.GetSafeHwnd() ) )
	{
		RECT rect;
		SetRect( &rect, 0, 0, 10, 10 );
		m_webservertoolbar.Create(	NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
									rect, &m_wndWebServerToolbar );
	} // end if

	m_webservertoolbar.TextArea( FALSE );
	m_webservertoolbar.SetMessageTarget( GetSafeHwnd(), WM_WEBSERVERTOOLBARCMD, WM_WEBSERVERTOOLBARUPDATE );
	m_webservertoolbar.Add( 1, m_hUnchecked, "Run Web Server", NULL, NULL, m_hChecked );
	m_webservertoolbar.Add( 0, NULL, NULL, NULL );
	m_webservertoolbar.Add( 3, CGrDC::LoadIcon( IDI_FOLDER ), "Open Root", NULL, NULL );
	m_webservertoolbar.Add( 0, NULL, NULL, NULL );
	m_webservertoolbar.Add( 4, CGrDC::LoadIcon( IDI_SMALL_USERS ), "Users: 0", NULL, NULL, NULL, STIF1_NOCLICK );
	m_webservertoolbar.Add( 0, NULL, NULL, NULL );
	m_webservertoolbar.Add( 5, m_hHtml, "Hits: 0", NULL, NULL, NULL, STIF1_NOCLICK );
	m_webservertoolbar.Add( 0, NULL, NULL, NULL );
	m_webservertoolbar.Add( 6, CGrDC::LoadIcon( IDI_WEB ), "Server Address: ", NULL, NULL, NULL );
//	m_webservertoolbar.Add( 5, CGrDC::LoadIcon( IDI_FOLDER ), NULL, NULL, NULL );
	
	// Position toolbar
	if ( ::IsWindow( m_wndOverlayToolbar.GetSafeHwnd() ) )
	{
		RECT rect;
		SetRect( &rect, 0, 0, 10, 10 );
		m_overlaytoolbar.Create(	NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
									rect, &m_wndOverlayToolbar );
	} // end if

	m_overlaytoolbar.SetMessageTarget( GetSafeHwnd(), WM_OVERLAYTOOLBARCMD, WM_OVERLAYTOOLBARUPDATE );
	m_overlaytoolbar.Add( 1, CGrDC::LoadIcon( IDI_OV_TEXT ), NULL, "Insert Text" );
	m_overlaytoolbar.Add( 2, CGrDC::LoadIcon( IDI_OV_SPECIALTEXT ), NULL, "Insert Special Text" );
	m_overlaytoolbar.Add( 0, NULL, NULL, NULL );
	m_overlaytoolbar.Add( 3, CGrDC::LoadIcon( IDI_OV_RECT ), NULL, "Insert Rect" );
	m_overlaytoolbar.Add( 0, NULL, NULL, NULL );
	m_overlaytoolbar.Add( 4, CGrDC::LoadIcon( IDI_OV_TIME ), NULL, "Insert Time" );
	m_overlaytoolbar.Add( 5, CGrDC::LoadIcon( IDI_OV_DATE ), NULL, "Insert Date" );
	m_overlaytoolbar.Add( 0, NULL, NULL, NULL );
	m_overlaytoolbar.Add( 6, CGrDC::LoadIcon( IDI_OV_IMAGE ), NULL, "Insert Image" );
	m_overlaytoolbar.Add( 7, CGrDC::LoadIcon( IDI_OV_WATERMARK ), NULL, "Insert Watermark" );
	m_overlaytoolbar.Add( 0, NULL, NULL, NULL );
	m_overlaytoolbar.Add( 8, CGrDC::LoadIcon( IDI_OV_VIDEO ), NULL, "Insert Video" );
	m_overlaytoolbar.Add( 9, CGrDC::LoadIcon( IDI_OV_URL ), NULL, "Insert Internet Image" );
	m_overlaytoolbar.Add( 10, CGrDC::LoadIcon( IDI_OV_SCREENSHOT ), NULL, "Insert Screenshot" );


	// Set status to idle
	SetStatus( "Idle" );

	// Ok to shutdown
//	m_once = FALSE;

	return 0;
}

BOOL CMainFrame::VerifyBarState(LPCTSTR pProfileName)
{
    CDockState state;
    state.LoadState( pProfileName );

    for ( int i = 0; i < state.m_arrBarInfo.GetSize(); i++ )
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[ i ];
		if ( pInfo == NULL ) return FALSE;

        int nDockedCount = pInfo->m_arrBarID.GetSize();
        
		// Docked bars
		if ( nDockedCount > 0 )
        {
            // dockbar
            for ( int j = 0; j < nDockedCount; j++ )
            {
                UINT nID = (UINT)pInfo->m_arrBarID[ j ] & 0xffff;

				if ( nID != 0 )
				{
					// Verify control bar exists
					CControlBar *pCb = GetControlBar( nID );
					if ( pCb == NULL ) return FALSE;

					// Ensure docking context
					if ( pInfo->m_bDocking && pCb->m_pDockContext == NULL )
						return FALSE;

				} // end if

            } // end for

        } // end if
        
		// Verify floating bar
        if ( !pInfo->m_bFloating && GetControlBar( pInfo->m_nBarID ) == NULL )
                return FALSE;
    }

    return TRUE;
}

BOOL CMainFrame::RestoreSettings()
{
	// This could take a while
	APP()->ExpectDelays( TRUE );

	// Inform user
	CDlgInform inform;
	inform.Run( "Restoring Settings..." );

	// Get last selected image
	DWORD sel = 0;
	CFG().GetValue( "Settings", "SelectedImage", &sel );
	IMGLIST().SetCurSel( sel );

	// Restore the images
	inform.SetStatus( "Restoring Images" );
	IMGLIST().Restore();

	// Update vierport size
	inform.SetStatus( "Initializing Edit Window" );
	EDIT().Size( NULL );

	// Restore remote view dialog status
	inform.SetStatus( "Restoring Views" );
	LPPUBIMGINFO ppii = NULL;
	while ( ( ppii = (LPPUBIMGINFO)IMGLIST().GetNext( ppii ) ) != NULL )
		if ( ( ppii->f1 & PIIF1_IMGDLG ) != 0 ) OpenViewImgDialog( ppii );

	// Update all views
	inform.SetStatus( "Updating view" );
	APP()->ReGen();

	// Start the video back up
	inform.SetStatus( "Starting Video Capture" );
	SetFrameRates();

	// Restore webserver
	if ( CFG().GetDword( "Settings", "WebServerRunning" ) != 0 )
		StartServer();

	// Start the publishing thread
	inform.SetStatus( "Initializing Publishing Threads" );
	m_pubthread.StartThread();

	// Enable refresh if needed
	m_bEnableRefresh = CFG().GetDword( "Settings", "EnableRefresh", 1 ) != 0;
	
	// Enable publishing if needed
	m_bEnablePublishing = CFG().GetDword( "Settings", "EnablePublishing", 1 ) != 0;
	CFG().SetValue( "Settings", "EnablePublishing", (DWORD)( m_bEnablePublishing ? 1 : 0 ) );

	// Update try icon
	if ( m_bEnablePublishing ) TrayIcon( NULL, NIM_MODIFY, APP()->LoadIcon( IDI_TV_GREEN ) );
	else TrayIcon( NULL, NIM_MODIFY, APP()->LoadIcon( IDI_TV_RED ) );

	// Show new image box if no image
	if ( IMGLIST().Size() == 0 ) OnNewimage();

	// Restore simple mode if needed
	if ( CFG().GetDword( "Settings", "CaptureMode" ) != 0 )
		OnSimple();

	// Update the toolbar
	m_maintoolbar.RedrawWindow();

	// Fill properties combo box
	FRAME()->m_dlgprop.FillCombo();

	// Generate images
	inform.SetStatus( "Generating Thumbnails" );
	m_imglist.Regen();

	// Lose the splash box
	inform.SetStatus( "Initialization Complete" );
	inform.DestroyWindow();

	// Do we want to check for updates?
	if ( CFG().GetDword( "Settings", "AutoUpdateCheck", 1 ) != 0 )
	{
		// Check for update
		m_productupdate.DoUpdateCheck( this, UPDATE_LINK, VER, TRUE );

	} // end if

	// Set timer
	m_uTimer = SetTimer( TIMERID, 3000, NULL );
	m_uGraphTimer = SetTimer( GRAPHTIMER, 1000, NULL );

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnClose() 
{
	if ( !m_bExit )
	{
		DWORD onclose = 0;
		CFG().GetValue( "Settings", "OnCloseAction", &onclose );

		if ( onclose == 0 )
		{	
			CDlgOnClose	dlg;

			// What does the user want to do?
			if ( dlg.DoModal() != IDOK ) return;

			if ( dlg.m_nAction == 0 )
			{	ShowWindow( SW_SHOWMINIMIZED );
				return;
			} // end if

		} // end if

		// Just want to minimize?
		else if ( onclose == 1 )
		{	ShowWindow( SW_SHOWMINIMIZED ); return; }

	} // end if

//	CDlgSplash		splash;
//	splash.Run( GetSafeHwnd(), IDR_SHUTDOWN, NULL );

	try
	{
		// Shutdown the app
		Shutdown();

		// Close the app
		CFrameWnd::OnClose();

	} catch( ... ) { ASSERT( 0 ); }
}

void CMainFrame::OnEndSession(BOOL bEnding) 
{
	Shutdown();

	CFrameWnd::OnEndSession(bEnding);	
}

BOOL CMainFrame::OnQueryEndSession() 
{
	Shutdown();

	if (!CFrameWnd::OnQueryEndSession())
		return FALSE;
	
	return TRUE;
}

void CMainFrame::Shutdown()
{
	// Inform user
	CDlgInform inform;
	inform.Run( "Shutting Down..." );
	inform.SetStatus( "Restoring view" );

	try
	{	
		// Close print preview if open
		VIEW()->SendMessage( WM_COMMAND, AFX_ID_PREVIEW_CLOSE, 0 );

		// Lose the timers
		if ( m_uTimer != 0 ) KillTimer( m_uTimer );
		if ( m_uGraphTimer != 0 ) KillTimer( m_uGraphTimer );

		// Restore toolbar positions
//		SaveToolbarPositions();

	} catch( ... ) { ASSERT( 0 ); }

	inform.SetStatus( "Gathering Settings" );

	// Save simple mode status
	CFG().SetValue( "Settings", "CaptureMode", (DWORD)( ( ::IsWindow( m_simple.GetSafeHwnd() ) ) ? 1 : 0 ) );

	// Save web server status
	CFG().SetValue( "Settings", "WebServerRunning", (DWORD)( ( m_iserver.IsRunning() ) ? 1 : 0 ) );

	// Save currently selected image
	CFG().SetValue( "Settings", "SelectedImage", IMGLIST().GetCurSel() );

	// Save window position
	CFG().SaveWindowPos( FRAME()->GetSafeHwnd(), "Positions", "MainWindow" );

	// Save properties dialog position
	APP()->m_dlgproperties.SavePosition();
	
	// Save remote view dialog status
	inform.SetStatus( "Saving remote view settings" );
	LPPUBIMGINFO ppii = NULL;
	while ( ( ppii = (LPPUBIMGINFO)IMGLIST().GetNext( ppii ) ) != NULL )
		if ( ::IsWindow( ppii->imgview->GetSafeHwnd() ) )
		{	ppii->imgview->Shutdown(); ppii->f1 |= PIIF1_IMGDLG; }
		else ppii->f1 &= ~PIIF1_IMGDLG;

	// Write the settings to disk
	inform.SetStatus( "Writing settings to disk" );
	APP()->SaveSettings();

	try
	{	
		inform.SetStatus( "Closing dialog boxes" );

		// Close simple view if open
		if ( m_simple.GetSafeHwnd() != NULL ) m_simple.DestroyWindow();

		// Close the properties box
		m_dlgprop.DestroyWindow();

		// Stop the publishing thread
		inform.SetStatus( "Halting publishing tasks" );
		m_pubthread.StopThread();

		// Lose the tray icon
		TrayIcon( NULL, NIM_DELETE );	

	} catch( ... ) { ASSERT( 0 ); }


	// --- Now for the dangerous stuff --- //

	try
	{
		// Save the toolbar position
		SaveBarState( "ToolbarPositions" );

	} catch( ... ) { ASSERT( 0 ); }

	try
	{
		// Save server log if needed
		if ( m_iserver.IsRunning() )
		{	char log[ CWF_STRSIZE ];
			inform.SetStatus( "Saving server log" );
			CWinFile::BuildPath( log, CWinFile::GetExePath( log ), "Weblog.txt" );
			SERVERLOG().Save( log );
		} // end if

		inform.SetStatus( "Halting image server" );
		StopServer();
		m_isession.Destroy();

		// Must be shut down in this order
		inform.SetStatus( "Releasing image list" );
		IMGLIST().Destroy();

		inform.SetStatus( "Releasing capture drivers" );
		VIDEOLIST().Destroy();

		inform.SetStatus( "Releasing edit buffers" );
		EDIT().FreeMemory();

	} catch( ... ) { ASSERT( 0 ); }
}

LRESULT CMainFrame::OnImgList(WPARAM wParam, LPARAM lParam)
{
	// Update edit window if sel changed
	if ( wParam == 1 ) 
	{	EDIT().SetEditFx( NULL );
		EDIT().Size( NULL );
		APP()->ReGen();
		EDIT().Update();	
		APP()->RefreshProperties();
		FRAME()->m_dlgprop.FillCombo();
	} // end if

	// If they double click
	else if ( wParam == 2 ) OnViewimage();

	return 0;
}

void CMainFrame::OnNewimage() 
{
	RunSetupWizard();
/*

	CDlgNewPubImg	dlg;

	// Get the user params for the new image
	if ( dlg.DoModal() != IDOK ) return;

	// Add a new image to the list
	LPPUBIMGINFO ppii = m_pubimglist.Add( dlg.m_sName );
	if ( ppii == NULL ) return;

	// Get image width and height
	long w = strtoul( dlg.m_sWidth, NULL, 10 );
	long h = strtoul( dlg.m_sHeight, NULL, 10 );

	// Correct size
	if ( !dlg.m_bCustomSize ) w = 320, h = 240;
	else
	{	if ( w <= 0 || w >= 3000 ) w = 320;
		if ( h <= 0 || h >= 3000 ) h = 240;
	} // end else
	
	// Create blank image of specified size
	ppii->os->Create( NULL, 0, 0, w, h );
	SetRect( &ppii->rect, 0, 0, w, h );

	// Save bck type
	ppii->cfg->SetValue( "Settings", "Type", (DWORD)dlg.m_nBckType );

	if ( dlg.m_nBckType == 1 )
		ppii->cfg->SetValue( "Settings", "Driver", dlg.m_dwDriver );

	else if ( dlg.m_nBckType == 2 )
		ppii->cfg->SetValue( "Settings", "BckImage", dlg.m_sImageFile );

	// Update the image view
	m_pubimglist.SetCurSel( m_pubimglist.Size() - 1 );
	m_pubimglist.Update();
	EDIT().SetEditFx( NULL );
	EDIT().Size( NULL );
	EDIT().Update();	
	m_dlgpubimg.Regen();
*/
}

void CMainFrame::OnDeleteimage() 
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Confirm delete
	if ( ::MessageBox( GetSafeHwnd(), "Are you sure you want to delete the current image?",
						"Confirm delete current image", MB_YESNO ) != IDYES ) return;

	EDIT().SetEditFx( NULL );

	// Delete this image
	IMGLIST().Delete( ppii );

	DWORD size = IMGLIST().Size();
	DWORD sel = IMGLIST().GetCurSel();
	if ( sel >= size )
	{	if ( size > 0 ) IMGLIST().SetCurSel( size - 1 );
		else IMGLIST().SetCurSel( 0 );
	} // end if

	// Redraw everything
	EDIT().Size( NULL );
	APP()->ReGen();
}


void CMainFrame::OnPopupCopy() 
{	EDIT().OnPopupCopy(); }

void CMainFrame::OnPopupCut() 
{	EDIT().OnPopupCut(); }

void CMainFrame::OnPopupDelete() 
{	EDIT().OnPopupDelete(); }

void CMainFrame::OnPopupDuplicate() 
{	EDIT().OnPopupDuplicate(); }

void CMainFrame::OnPopupPaste() 
{	EDIT().OnPopupPaste(); }

void CMainFrame::OnPopupProperties() 
{
	APP()->ShowProperties( 1 );
}

LRESULT CMainFrame::OnFrameMsg(WPARAM wParam, LPARAM lParam)
{	
	SendMessage( WM_FRAMEREFRESH, wParam, lParam );
	return 0;
}

LRESULT CMainFrame::OnFrameRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFRAMECALLBACKINFO	pfci = (LPFRAMECALLBACKINFO)lParam;
	LPVIDEOINFO pvi = NULL;

	if ( pfci == NULL || pfci->pCap == NULL ) return 0;

	DWORD driver = pfci->pCap->GetDriverIndex();
	pvi = VIDEOLIST().FindByIndex( driver );
	if ( pvi != NULL && pvi->os != NULL )
	{
		// Ensure effects are off
		pvi->os->SetTransparency( FALSE );
		pvi->os->SetAntiAlias( FALSE );

		if ( pvi != NULL && pvi->motion.bEnable )
		{
			// Detect motion and copy motion frame if needed
			if ( pvi->os->DetectMotion( &pvi->motion ) )
			{
				char msg[ 256 ];
				wsprintf( msg, "Motion Detected!  Driver %lu", pvi->index );
				SetStatus( msg );

				// Make a copy of the frame with motion
//				pvi->mos->Copy( *pvi->os, *pvi->os );

				// Do we want to play a sound?
				if ( ( pvi->f1 & VIF1_MOTIONSOUND ) != 0 )
				{
					// Default sound or user supplied?
					if ( *pvi->motionsnd == 0 || !CWinFile::IsFile( pvi->motionsnd ) )
						PLAYSOUND( IDW_ALARM );
					else PLAYSOUND( pvi->motionsnd );

				} // end if

			} // end if

			// Notify setup window of motion
			if ( m_hMotionWnd != NULL && IsWindow( m_hMotionWnd ) )
				::SendMessage( m_hMotionWnd, WM_MOTIONDETECT, (WPARAM)driver, (LPARAM)pvi );
			else m_hMotionWnd = NULL;

			// Overlay if needed
			if ( ( pvi->f1 & VIF1_MOTIONOVERLAY ) != 0 )
				pvi->os->DrawMotion( &pvi->motion );

		} // end if

		// Apply image effects (so slow)
		if ( pvi->imgfx->GetEffectsCount() )
		{	CWinImg img;
			img.CreateFromHBITMAP( *pvi->os );
			pvi->imgfx->DoEffects( &img );
			img.Draw( *pvi->os, *pvi->os );
		} // end if

		// Image averaging
		if ( pvi->tavg != NULL )
		{
			if ( ( pvi->f1 & VIF1_FRAMEAVG ) != 0 )
			{
				if ( pvi->tavg->GetSafeHdc() == NULL )
				{	pvi->tavg->CreateDIBSection( *pvi->os, pvi->os->GetWidth(), pvi->os->GetHeight() );
					pvi->os->Draw( *pvi->tavg, *pvi->tavg );
				} // end if

				// Draw avg
				else 
				{
					// Verify frame average range
					if ( pvi->frameavgweight < 1 ) pvi->frameavgweight = 1;
					else if ( pvi->frameavgweight > 99 ) pvi->frameavgweight = 99;

					pvi->os->TlDraw(	*pvi->tavg, *pvi->tavg, 
										10000 - ( pvi->frameavgweight * 100 ), FALSE );
				} // end else

			} // end if

			// Lose the averaging bitmap
			else if ( pvi->tavg->GetSafeHdc() != NULL ) pvi->tavg->Destroy();

		} // end if

	} // end if

	// Send refresh message
	if ( m_hRefreshWnd != NULL && ::IsWindow( m_hRefreshWnd ) )
		::SendMessage( m_hRefreshWnd, m_uRefreshMsg, pvi->index, (LPARAM)pvi );
	
	// Inform the dialog box if needed
	if (	m_simple.GetSafeHwnd() != NULL && 
			::IsWindow( m_simple.GetSafeHwnd() ) )
		::SendMessage( m_simple.GetSafeHwnd(), WM_USER + 201, wParam, lParam );
	
	LPPUBIMGINFO pcur = IMGLIST().GetSelImage();
	LPPUBIMGINFO ppii = NULL;
	while ( ( ppii = (LPPUBIMGINFO)IMGLIST().GetNext( ppii ) ) != NULL )
	{
		// Update the image if needed
		if (	ppii->imgview != NULL && 
				::IsWindow( ppii->imgview->GetSafeHwnd() ) &&
				IMGLIST().IsVideoInImage( ppii->cfg->GetDword( "Settings", "Driver" ), ppii ) )
		{	IMGLIST().Update( ppii ); if ( pcur == ppii ) pcur = NULL;
			ppii->imgview->UpdateImg( ppii->os );
		} // end if

	} // end while

	// Punt if no refresh
	if ( !m_bEnableRefresh ) return FALSE;

	// Is the video in the current image?
	if ( !IMGLIST().IsVideoInImage( driver, IMGLIST().GetSelImage() ) )
		return 0;
	
	if ( pcur != NULL ) IMGLIST().Update();
	EDIT().Update();

	// Add frame to avi
//	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
//	if ( ppii != NULL && ppii->os != NULL ) 
//		m_avi.AddFrame( ppii->os );	

	return 0;
}

void CMainFrame::RunSetupWizard()
{
	CWzFrame dlg( IDD_WZ_FRAME );

	// Set reg pointer
	CReg reg;
	{ // Get default data
		PUBINFO pi; ZeroMemory( &pi, sizeof( pi ) );
		CPubList::Default( &pi );
		CPubList::UpdateData( FALSE, &pi, reg.GetKey( "PUBINFO" ) );
	} // end get default data
	dlg.SetData( &reg );

	char helppath[ CWF_STRSIZE ];
	GetModuleFileName( NULL, helppath, sizeof( helppath ) );
	CWinFile::GetPathFromFileName( helppath, helppath );
	CWinFile::BuildPath( helppath, helppath, "Help" );
	dlg.SetHelpRoot( helppath );

	dlg.SetPage( 0, new CWzWelcome(), CWzWelcome::IDD, "Setup Wizard", "setup_wizard.htm", 1 );
	dlg.SetPage( 1, new CWzCam(), CWzCam::IDD, "Image Properties", "setupwizard_image.htm", 2, 0 );
	dlg.SetPage( 2, new CWzPubType(), CWzPubType::IDD, "Publish Method", "setupwizard_publishmethod.htm", 999, 1 );
	dlg.SetPage( 3, new CWzLocalServer(), CWzLocalServer::IDD, "Local Web Server", "setupwizard_webserver.htm", 999, 2 );
	dlg.SetPage( 4, new CDlgFtpList(), CDlgFtpList::IDD, "FTP Settings", "setupwizard_ftp.htm", 8, 2 );
	dlg.SetPage( 5, new CDlgEmailList(), CDlgEmailList::IDD, "E-mail Server", "setupwizard_email.htm", 6, 2 );
	dlg.SetPage( 6, new CWzEmailMsgInfo(), CWzEmailMsgInfo::IDD, "E-mail Message", "setupwizard_emailmsg.htm", 8, 5 );
	dlg.SetPage( 7, new CWzSave(), CWzSave::IDD, "Save To Disk", "setupwizard_save.htm", 8, 2 );

	dlg.SetPage( 8, new CPgPubWhen(), CPgPubWhen::IDD, "When To Publish", "setupwizard_whenpublish.htm", 9, 2 );
	dlg.SetPage( 9, new CDlgMotion(), CDlgMotion::IDD, "Motion Detection", "setupwizard_motion.htm" );
	dlg.SetPage( 10, new CPgPubHow(), CPgPubHow::IDD, "Filename", "setupwizard_filename.htm", 11, 8 );
	dlg.SetPage( 11, new CPgPubSounds(), CPgPubSounds::IDD, "Sounds", "setupwizard_sounds.htm" );

	// Run the wizard
	if ( dlg.DoModal() != IDOK ) return;
	
	// Add the publishing job
	AddPubJob( &reg );

	// Update the image view
	m_pubimglist.SetCurSel( m_pubimglist.Size() - 1 );
	m_pubimglist.Update();
	EDIT().SetEditFx( NULL );
	EDIT().Size( NULL );
	EDIT().Update();	
	m_imglist.Regen();

	// Save all settings to disk
	APP()->SaveSettings();

	SetUpdateTimer();
}

BOOL CMainFrame::AddPubJob(CReg *pReg)
{
	if ( pReg == NULL ) return FALSE;

	// Add a new image to the list
	LPPUBIMGINFO ppii = m_pubimglist.Add( pReg->GetSz( "CWzCam", "Name" ) );
	if ( ppii == NULL ) return FALSE;

	// Set default size
	long w = 320, h = 240;
	if ( pReg->GetDword( "CWzCam", "CustomSize", 0 ) != 0 )
	{	w = pReg->GetDword( "CWzCam", "Width", 320 );
		h = pReg->GetDword( "CWzCam", "Height", 240 );
	} // end if

	// Create offscreen buffer
	ppii->os->Create( NULL, 0, 0, w, h );
	SetRect( &ppii->rect, 0, 0, w, h );

	// Save bck type
	DWORD type = pReg->GetDword( "CWzCam", "BckType", 0 );
	ppii->cfg->SetValue( "Settings", "Type", type );

	if ( type == 1 )
		ppii->cfg->SetValue( "Settings", "Driver", pReg->GetDword( "CWzCam", "Driver" ) );

	else if ( type == 2 )
		ppii->cfg->SetValue( "Settings", "BckImage", pReg->GetSz( "CWzCam", "ImageFile" ) );

	// Set video driver

	DWORD pubtype = pReg->GetDword( "CWzPubType", "PubType", MAXDWORD );

	// Nothing
	if ( pubtype == 0 ) return TRUE;

	// Local server setup
	else if ( pubtype == 1 )
	{
		// Set the root folder
		CString rootdir = pReg->GetSz( "CWzLocalServer", "RootDir" );
		if ( !rootdir.IsEmpty() ) 
		{	
			// Make entry for root folder
			WEBSETTINGS().DeleteValue( "FolderMap", "/" );
			WEBSETTINGS().DeleteValue( "FolderMap", "\\" );
			WEBSETTINGS().Set( "FolderMap", "", rootdir );

			// Ensure folder exists
			CWinFile::CreateDirectory( rootdir );
		} // end if

		// Set server port
		CFG().SetValue( "Settings", "WebServerPort", 
						pReg->GetDword( "CWzLocalServer", "WebServerPort" ) );

		char password[ 256 ] = { 0 };
		if ( !pReg->Get( "CWzLocalServer", "WebServerHttpPassword", password, sizeof( password ) ) )
			*password = 0;

		// Enable/disable and set guest password
		if ( *password == 0 )
		{	CFG().SetValue( "Settings", "WebServerHttp", (DWORD)0 );
			CFG().SetValue( "Settings", "WebServerHttpPassword", "" );
		} // end if
		else
		{	CFG().SetValue( "Settings", "WebServerHttp", (DWORD)1 );
			CFG().SetValue( "Settings", "WebServerHttpPassword", password );
		} // end if

		// Set server password

		// Add file map
		WEBSETTINGS().Set(	"ImageMap", 
							pReg->GetSz( "CWzLocalServer", "Filename" ),
							pReg->GetSz( "CWzCam", "Name" ) );

		// Run the HTML builder if wanted?
		if ( ::MessageBox( GetSafeHwnd(),	"Do you want to create HTML pages\r\n"
											"and publish them now?", "Run HTHM Publisher?",
											MB_YESNO ) == IDYES )
			OnOptionsHtmlbuilder();

		// Start the local server
		StartServer();

		return TRUE;

	} // end if

	PUBINFO	pi, *ppi = &pi;
	ZeroMemory( &pi, sizeof( pi ) );
	pi.pemi = new EMAILMSGINFO;
	if ( pi.pemi == NULL ) return FALSE;

	// Save pub data
	CPubList::UpdateData( TRUE, ppi, pReg->GetKey( "PUBINFO" ) );
	pReg->Get( "CWzCam", "Name", pi.img, sizeof( pi.img ) );

	// FTP specific settings
	if ( pubtype == 2 )
	{
		// Save server type and string
		ppi->type = PUBTYPE_FTP;		
		pReg->Get( "CDlgFtpList", "Server", ppi->str, sizeof( ppi->str ) );

	} // end else if

	// E-mail specific settings
	else if ( pubtype == 3 )
	{
		// Save server type and string
		ppi->type = PUBTYPE_EMAIL;		
		pReg->Get( "CDlgEmailList", "Server", ppi->str, sizeof( ppi->str ) );
	} // end if

	// Disk file specific settings
	else if ( pubtype == 4 )
	{
		// Save server type and string
		ppi->type = PUBTYPE_DISK;

	} // end if

	// Add this publishing job
	PUBLIST().Add( &pi );	

	// Lose memory
	delete pi.pemi;

	return TRUE;
}


BOOL CMainFrame::ShowSettings(DWORD pg)
{
	// --- Run Check
//	if ( !APP()->IsRunOk() ) return FALSE;

	static BOOL once = FALSE;
	if ( once ) return FALSE; once = TRUE;

	CDlgTreeFrame	dlg;
	HTPAGE			htpage;

	char helppath[ CWF_STRSIZE ];
	GetModuleFileName( NULL, helppath, sizeof( helppath ) );
	CWinFile::GetPathFromFileName( helppath, helppath );
	CWinFile::BuildPath( helppath, helppath, "Help" );
	dlg.SetHelpRoot( helppath );
	
	// General settings
	dlg.AddPage(	NULL, APP()->LoadIcon( IDI_BARCODE ), 
					new CDlgGeneral(), CDlgGeneral::IDD, "General", "settings_general.htm" );

	dlg.AddPage(	NULL, APP()->LoadIcon( IDI_FILE ), 
					new CDlgPublishing(), CDlgPublishing::IDD, "Publishing", "settings_publishing.htm" );

	htpage = dlg.AddPage(	NULL, APP()->LoadIcon( IDI_WORLD ), 
							new CDlgServerLog, CDlgServerLog::IDD, "Web Server", "settings_webserver.htm" );
	dlg.AddPage(	htpage, APP()->LoadIcon( IDI_WRENCH	), 
							new CDlgServer, CDlgServer::IDD, "Setup", "settings_webserversetup.htm" );
	dlg.AddPage(	htpage, APP()->LoadIcon( IDI_FOLDER ), 
							new CDlgServerMap, CDlgServerMap::IDD, "File Mapping", "settings_filemapping.htm" );
	dlg.AddPage(	htpage, APP()->LoadIcon( IDI_CGI ), 
							new CDlgCgi, CDlgCgi::IDD, "CGI", "settings_cgi.htm" );
	dlg.AddPage(	htpage, APP()->LoadIcon( IDI_CHAT ), 
							new CDlgChat, CDlgChat::IDD, "Chat", "settings_chat.htm" );
					
	htpage = dlg.AddPage(	NULL, APP()->LoadIcon( IDI_SERVER ), 
					new CDlgLogFiles, CDlgLogFiles::IDD, "Servers", "settings_server.htm" );
	dlg.AddPage(	htpage, APP()->LoadIcon( IDI_FTP ), 
					new CDlgFtpList(), CDlgFtpList::IDD, "FTP", "settings_ftp.htm" );
	dlg.AddPage(	htpage, APP()->LoadIcon( IDI_SMALL_LETTER ), 
					new CDlgEmailList(), CDlgEmailList::IDD, "E-Mail", "settings_email.htm" );

	dlg.AddPage(	NULL, APP()->LoadIcon( IDI_MOTION ), 
					new CDlgMotion(), CDlgMotion::IDD, "Motion", "settings_motion.htm" );

	dlg.AddPage(	NULL, APP()->LoadIcon( IDI_DITHER ), 
					new CDlgVideoSettings(), CDlgVideoSettings::IDD, "Video Settings", "settings_video.htm" );
					
	dlg.AddPage(	NULL, APP()->LoadIcon( IDI_DILATE ), 
					new CDlgJpegSettings(), CDlgJpegSettings::IDD, "JPEG Settings", "settings_jpeg.htm" );

	// Run the box
	dlg.SetStartPage( pg );

	// Run the dialog box
	BOOL ok = ( dlg.DoModal() == IDOK );

	m_dwLastPage = dlg.GetLastPage();

	// Show server address
	GetAddressInfo();

	// Save all settings to disk
	APP()->SaveSettings();

	// Refresh the video
	APP()->ReGen();

	once = FALSE;

	return TRUE;
}

void CMainFrame::OnOptionsSettings() 
{
	ShowSettings( m_dwLastPage );
}

BOOL CMainFrame::StartServer()
{
	DWORD port = 80;
	CFG().GetValue( "Settings", "WebServerPort", &port );
	if ( port > 65535 ) port = 65535;

	DWORD max = 80;
	CFG().GetValue( "Settings", "WebServerMaxUsers", &max );
	if ( max > 65535 ) max = 65535;

	// Setup for session callbacks
	m_iserver.SetSessionMessageTarget( GetSafeHwnd(), WM_WEBSESSION );

	// Start the server
	if ( !m_iserver.Start( GetSafeHwnd(), WM_WEBSERVER, port, max ) )
		return FALSE;

	// Open the log file
	SERVERLOG().Destroy();
	SERVERLOG().Log( LOGTYPE_OOB | LOGTYPE_INFO, "Server Started" );

	// Ensure check button is correct
	m_webservertoolbar.RedrawWindow();

	GetAddressInfo();
	
	return TRUE;
}

void CMainFrame::StopServer()
{
	m_iserver.Destroy();
	SERVERLOG().Log( LOGTYPE_OOB | LOGTYPE_INFO, "Server Shutdown normally" );
	{	char log[ CWF_STRSIZE ];
		CWinFile::BuildPath( log, CWinFile::GetExePath( log ), "Weblog.txt" );
		SERVERLOG().Save( log );
	} // end void

	// Ensure check button is correct
	m_webservertoolbar.RedrawWindow();

	GetAddressInfo();

	m_dwHits = 0;
}

LRESULT CMainFrame::OnWebServer(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CMainFrame::OnWebSession(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == HTTPMSG_URL )
	{
		LPHTTPREQUEST phr = (LPHTTPREQUEST)lParam;
		if ( phr == NULL ) return 0;

		WEBUSERS().Add( phr->ip, 1 );

		// Count a hit
		m_dwHits++;

		// Update the user display
		UpdateUsers();

	} // end if

	return 0;
}

void CMainFrame::OnRun()
{
	// Toggle server state
	if ( m_iserver.IsRunning() ) StopServer();
	else StartServer();

/*	CButton *pRun = (CButton*)m_wndServerBar.GetDlgItem( IDC_RUN );
	if ( pRun == NULL ) return;

	// Start/Stop server
	if ( pRun->GetCheck() ) StartServer();
	else StopServer();
*/
}

BOOL CMainFrame::TrayIcon(LPCTSTR tip, DWORD flags, HICON hIcon)
{
	NOTIFYICONDATA	nid;

	nid.cbSize				= sizeof( NOTIFYICONDATA );
	nid.hWnd				= GetSafeHwnd();
	nid.uID					= 0;
	nid.uFlags				= NIF_ICON | NIF_MESSAGE;
	nid.uCallbackMessage	= WM_TRAYICON;

	// Are we setting the tip?
	if ( tip != NULL ) nid.uFlags |= NIF_TIP;
//	if ( hIcon != NULL ) nid.uFlags |= NIF_ICON;

	// Set Icon
	if ( hIcon != NULL ) nid.hIcon = hIcon;
	else nid.hIcon = AfxGetApp()->LoadIcon( IDI_TV_RED );

	// Set tool tip
	if ( tip != NULL ) strcpy( nid.szTip, tip );
	else *nid.szTip = 0x0;

	BOOL ret = Shell_NotifyIcon( flags, &nid );

	return ret;
}

LRESULT CMainFrame::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	UINT	uID = (UINT)wParam;
	UINT	uMouseMsg = (UINT)lParam;

	// Do they want to show the window?
	if ( uMouseMsg == WM_LBUTTONDBLCLK ||
		 uMouseMsg == WM_LBUTTONDOWN )
	{
		// Are we in simple mode?
		if ( ::IsWindow( m_simple.GetSafeHwnd() ) )
		{	m_simple.ShowWindow( SW_SHOWNORMAL );
			m_simple.SetForegroundWindow();
		} // end if

		else // Put us up front
		{	ShowWindow( SW_SHOWNORMAL );
			SetForegroundWindow();
		} // end else

	} // end if

	// Show the popup menu
	else if ( uMouseMsg == WM_RBUTTONDOWN )
	{
		POINT	pt = { 0, 0 };
		::GetCursorPos( &pt );
		ShowMenu( pt.x, pt.y );
	} // end else
	return 0;
}

LRESULT CMainFrame::OnTaskBarCreated( WPARAM wParam, LPARAM lParam )
{
	// Show icon
	TrayIcon( APPNAME );
	return 0;
}

void CMainFrame::ShowMenu(long x, long y)
{
	HMENU hMenu, hSub;
	BOOL bOnTop = FALSE;

	// Load the template
	hMenu = LoadMenu( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDM_TRAYMENU ) );
	if ( hMenu == NULL ) return;
	hSub = GetSubMenu( hMenu, 0 );
	if ( hSub == NULL ) return;

	if ( m_bEnablePublishing )
		CheckMenuItem( hSub, ID_TRAYICON_ENABLEPUBLISHING, MF_BYCOMMAND | MF_CHECKED );
	else CheckMenuItem( hSub, ID_TRAYICON_ENABLEPUBLISHING, MF_BYCOMMAND | MF_UNCHECKED );
	

	// We must be the foreground window for this to work correctly
	SetForegroundWindow();

	// Show the popup menu
	::TrackPopupMenu( hSub, TPM_RIGHTBUTTON, x, y, 0, GetSafeHwnd(), NULL );

	// Unload the menu
	DestroyMenu( hMenu );
}

void CMainFrame::OnTrayiconAbout() 
{
	APP()->OnAppAbout();
}

void CMainFrame::OnTrayiconEmailimage() 
{
	// Are we in capture mode?
	if ( ::IsWindow( m_simple.GetSafeHwnd() ) )
	{	m_simple.EmailImage();
		return;
	} // end if

	char from[ CWF_STRSIZE ] = { 0 };
	char fname[ CWF_STRSIZE ] = { 0 };
	CFG().GetValue( "Settings", "EmailFrom", from, sizeof( from ) );
	CFG().GetValue( "Settings", "EmailFilename", fname, sizeof( fname ) );

	CMime	mime;
	if ( !mime.Create( NULL, from, NULL ) ) return;
	
	// Use default file name if none specified
	if ( *fname == 0 ) strcpy( fname, "webcam.jpg" );

	// Attach file if needed
	if ( *fname != 0 )
	{
		CWinImg		img;
		LPBYTE		buf = NULL;
		DWORD		size = 0;

		// Get the image
		if (	IMGLIST().GetImage( MAXDWORD, &img ) && 
				SetQuality( &img ) &&
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
}

void CMainFrame::OnTrayiconEnablepublishing() 
{	OnEnablepublishing();
}

void CMainFrame::OnTrayiconExit() 
{
	m_bExit = TRUE;
	OnClose();

}

void CMainFrame::OnSaveimage() 
{
	CWinImg img;
	if ( !IMGLIST().GetImage( MAXDWORD, &img ) ) return;

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
	SetQuality( &img );
	if ( !img.Save( filename ) )
	{	_Log( MB_ICONERROR, "Error saving image", img.GetLastError() );
		return;
	} // end if
}

#define DEFJPEGQUALITY	52
BOOL CMainFrame::SetQuality(CWinImg *pImg)
{	
	return pImg->SetQuality( CFG().GetDword( "Settings", "JpegQuality", DEFJPEGQUALITY ) );
}

void CMainFrame::SetFrameRates()
{
	// Activate video in current image
//	IMGLIST().ActivateVideo();	
	IMGLIST().ConnectVideo();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

	if ( nType == SIZE_MINIMIZED )
	{
		DWORD dwval = 0;
		CFG().GetValue( "Settings", "NoMinimizeToTray", &dwval );
		if ( dwval == 0 ) ShowWindow( SW_HIDE );

		// Deactivate all the video
		IMGLIST().DeactivateUnusedVideo();

		// Mark the video as paused
		m_bPaused = TRUE;

	} // end if
	else if ( m_bPaused )
	{	m_bPaused = FALSE;
		SetFrameRates();
	} // end else if	

	// Position toolbar
	if ( ::IsWindow( m_wndMainToolbar.GetSafeHwnd() ) )
	{
		RECT rect;
		m_wndMainToolbar.GetClientRect( &rect );
		m_maintoolbar.MoveWindow( &rect );

	} // end if
	
	// Position toolbar
	if ( ::IsWindow( m_wndOverlayToolbar.GetSafeHwnd() ) )
	{
		RECT rect;
		m_wndOverlayToolbar.GetClientRect( &rect );
		m_overlaytoolbar.MoveWindow( &rect );

	} // end if
	
	// Position toolbar
	if ( ::IsWindow( m_wndWebServerToolbar.GetSafeHwnd() ) )
	{
		RECT rect;
		m_wndWebServerToolbar.GetClientRect( &rect );
		m_webservertoolbar.MoveWindow( &rect );

	} // end if
	
}

void CMainFrame::GetAddressInfo()
{
	DWORD	port = 80;
	char	strport[ 256 ] = { 0 };

	CString	addr, remote;

	if ( m_iserver.IsRunning() )
	{
		// Get port
		CFG().GetValue( "Settings", "WebServerPort", &port );
		if ( port > 65535 ) port = 65535;
		wsprintf( strport, "%lu", port );

		char name[ CWF_STRSIZE ] = { '~', 0x0 };
		if ( !gethostname( name, CWF_STRSIZE - 1 ) )
		{
			addr = "http://";
			addr += name;
			if ( port != 80 ) addr += ":", addr += strport;
			addr += "/";

			LPHOSTENT he = gethostbyname( name );
			if ( he != NULL )
			{
				remote = "http://";
				remote += inet_ntoa( *(LPIN_ADDR) *( he->h_addr_list ) );
				if ( port != 80 ) remote += ":", remote += strport;
				remote += "/";
			} // end if

		} // end if
	} // end if

	else { addr = ""; remote = ""; }

	m_sRemoteAddr = remote;

	CString str = "Server Address: " + remote;

	m_webservertoolbar.SetTextByCmd( 6, str );
	m_webservertoolbar.RedrawWindow();

/*
	// Update the window
	CWnd *pWnd = m_wndServerBar.GetDlgItem( IDC_LOCALADDRESS );
	if ( pWnd != NULL )
	{	pWnd->SetWindowText( addr );
		pWnd->RedrawWindow();
	} // end if
	pWnd = m_wndServerBar.GetDlgItem( IDC_REMOTEADDRESS );
	if ( pWnd != NULL )
	{	pWnd->SetWindowText( remote );
		pWnd->RedrawWindow();
	} // end if
*/
}

void CMainFrame::UpdateUsers()
{
	char num[ 256 ];
	wsprintf( num, "Users: %lu", WEBUSERS().Size() );
	m_webservertoolbar.SetTextByCmd( 4, num );

	wsprintf( num, "Hits: %lu", m_dwHits );
	m_webservertoolbar.SetTextByCmd( 5, num );

	m_webservertoolbar.RedrawWindow();

/*
	CWnd *pWnd = (CWnd*)m_wndServerBar.GetDlgItem( IDC_USERS );
	if ( pWnd != NULL )
	{
		char num[ 256 ];
		wsprintf( num, "%lu", WEBUSERS().Size() );
		pWnd->SetWindowText( num );
	} // end if

	pWnd = (CWnd*)m_wndServerBar.GetDlgItem( IDC_HITS );
	if ( pWnd != NULL )
	{	char num[ 256 ];
		wsprintf( num, "%lu", m_dwHits );
		pWnd->SetWindowText( num );
	} // end if
*/
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == TIMERID )
	{
		// Expire users if needed
		if ( WEBUSERS().Size() > 0 )
			if ( WEBUSERS().Expire( 3000 ) ) UpdateUsers();

		// Expire status if needed
		if ( m_dwStatusTimeout != 0 && m_dwStatusTimeout < GetTickCount() )
		{	SetStatus( NULL ); m_dwStatusTimeout = 0; }

	} // end if

	else if ( nIDEvent == GRAPHTIMER )
	{
		if ( ::IsWindow( m_graph.GetSafeHwnd() ) )
		{
			// Tick the graph
			m_graph.Tick();
			m_graph.Add( "CPU", 0, (DWORD)( CWin32::GetCpuLoad() * 10 ), RGB( 0, 255, 0 ), NULL );
			m_graph.RedrawWindow();

		} // end if

		// Update the refresh timer
		if ( m_dwUpdateTimer != 0 && m_dwUpdateTimer < GetTickCount() )
		{	m_dwUpdateTimer = 0;
			if ( m_bResize ) { m_bResize = FALSE; EDIT().Size( NULL ); }
			APP()->ReGen();
		} // end if

		// Clear expecting delays flag
		if ( APP()->m_bExpectDelays )
		{
			DWORD tickcount = GetTickCount();
			if ( m_dwExpectDelays == 0 || m_dwExpectDelays > ( tickcount + 3000 ) )
				m_dwExpectDelays = tickcount + 3000;
			else if ( m_dwExpectDelays < tickcount )
			{	m_dwExpectDelays = 0; APP()->ExpectDelays( FALSE ); }

		} // end if

		// Testing eyeball
		if ( ::IsWindow( m_maintoolbar.GetSafeHwnd() ) )
			m_maintoolbar.RedrawWindow();

	} // end else if
	
	CFrameWnd::OnTimer(nIDEvent);
}


void CMainFrame::OnOptionsCheckforupdate() 
{
	// Check for update
	m_productupdate.DoUpdateCheck( this, UPDATE_LINK, VER, FALSE );
}

void CMainFrame::OnEnablepublishing() 
{
	m_bEnablePublishing = !m_bEnablePublishing;

	// Enable publishing if needed
	CFG().SetValue( "Settings", "EnablePublishing", (DWORD)( m_bEnablePublishing ? 1 : 0 ) );

	if ( m_bEnablePublishing ) TrayIcon( NULL, NIM_MODIFY, APP()->LoadIcon( IDI_TV_GREEN ) );
	else TrayIcon( NULL, NIM_MODIFY, APP()->LoadIcon( IDI_TV_RED ) );

}

void CMainFrame::OnUpdateEnablepublishing(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bEnablePublishing );
}

void CMainFrame::OnRefresh() 
{
	m_bEnableRefresh = !m_bEnableRefresh;

	// Enable publishing if needed
	CFG().SetValue( "Settings", "EnableRefresh", (DWORD)( m_bEnableRefresh ? 1 : 0 ) );
}

void CMainFrame::OnUpdateRefresh(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bEnableRefresh );
}


void CMainFrame::OnProgress(WPARAM wParam, LPARAM lParam)
{
	char msg[ 256 ];

	if ( wParam == 0 )
	{
		if ( lParam < 60 )
		{
			wsprintf( msg, "Update in %lu seconds", lParam );
			m_wndStatusBar.SetPaneText( 1, msg );
		} // end if
		else if ( lParam < ( 60 * 60 ) )
		{
			DWORD sec = lParam % 60;
			DWORD min = lParam / 60;

			wsprintf( msg, "Update in %lu minute(s) %lu second(s)", min, sec );
			m_wndStatusBar.SetPaneText( 1, msg );

		} // end else
		else
		{
			DWORD sec = lParam % 60;
			DWORD min = lParam / 60;
			DWORD hour = min / 60;
			min %= 60;

			wsprintf( msg, "Update in %lu hour(s) %lu minute(s) %lu second(s)", hour, min, sec );
			m_wndStatusBar.SetPaneText( 1, msg );

		} // end else

	} // end if

	else if ( wParam == 1 )
	{	if ( lParam != 0 ) m_wndStatusBar.SetPaneText( 1, (char*)lParam );
		else m_wndStatusBar.SetPaneText( 1, "..." );
	} // end else
}

void CMainFrame::Exit()
{
	::PostMessage( FRAME()->GetSafeHwnd(), WM_EXITAPP, 0, 0L );
}

LRESULT CMainFrame::OnExitApp(WPARAM wParam, LPARAM lParam)
{
	m_bExit = TRUE; 
	OnClose(); 
	return 0;
}


void CMainFrame::OnHelpRegistration() 
{
	APP()->RegistrationDlg();
}

void CMainFrame::CreateVideoMenu()
{
	HMENU hMenu = ::GetMenu( GetSafeHwnd() );
	if ( hMenu == NULL ) return;

	HMENU hSub = ::CreateMenu();
	if ( hSub == NULL ) return;

	CCapture	cap;
	DWORD		drivers = cap.GetNumDrivers();

	// For each driver
	for ( DWORD i = 0; i < drivers; i++ )
	{
		char				show[ CWF_STRSIZE ];
		CAPTUREDRIVERINFO	ci;

		if ( cap.GetDriverInfo( i, &ci ) )
		{
			// Create driver string
			sprintf( show, "%s", ci.szName );

			// Build a sub menu
			HMENU hVid = ::CreateMenu();
			if ( hVid != NULL )
			{
				AppendMenu( hVid, MF_STRING, ID_VIDMENUBASE + ( i * 4 ), "Source" );
				AppendMenu( hVid, MF_STRING, ID_VIDMENUBASE + ( i * 4 ) + 1, "Format" );
				AppendMenu( hVid, MF_STRING, ID_VIDMENUBASE + ( i * 4 ) + 2, "Display" );
				AppendMenu( hVid, MF_STRING, ID_VIDMENUBASE + ( i * 4 ) + 3, "Compression" );

				// Modify sub menu
				::AppendMenu( hSub, MF_POPUP, (UINT)hVid, show ); 

			} // en dif

		} // end if

	} // end for

	InsertMenu( hMenu, 3, MF_BYPOSITION | MF_POPUP, (UINT)hSub, "Video" );
}

void CMainFrame::UpdateVideoMenu()
{
/*
	HMENU hMenu = ::GetMenu( GetSafeHwnd() );
	if ( hMenu == NULL ) return;

	HMENU hSub = ::GetSubMenu( hMenu, 3 );
	if ( hSub == NULL ) return;

	DWORD i = 0;

	// For each sub menu
	HMENU hVid = NULL;
	while ( ( hVid = ::GetSubMenu( hSub, i ) ) != NULL )
	{
		// Find index
		LPVIDEOINFO	pvi = VIDEOLIST().FindByIndex( i );
		if ( pvi != NULL )
		{
			::EnableMenuItem(	hVid, ID_VIDMENUBASE + ( i * 4 ), 
				MF_BYCOMMAND | ( ( pvi->cap->m_cdc.fHasDlgVideoSource ) ? MF_ENABLED : MF_GRAYED ) );
			::EnableMenuItem(	hVid, ID_VIDMENUBASE + ( i * 4 ) + 1, 
				MF_BYCOMMAND | ( ( pvi->cap->m_cdc.fHasDlgVideoFormat ) ? MF_ENABLED : MF_GRAYED ) );
			::EnableMenuItem(	hVid, ID_VIDMENUBASE + ( i * 4 ) + 2, 
				MF_BYCOMMAND | ( ( pvi->cap->m_cdc.fHasDlgVideoDisplay ) ? MF_ENABLED : MF_GRAYED ) );
			::EnableMenuItem(	hVid, ID_VIDMENUBASE + ( i * 4 ) + 3, 
				MF_BYCOMMAND | MF_ENABLED );

		} // end if

		else
		{
			::EnableMenuItem( hVid, ID_VIDMENUBASE + ( i * 4 ), MF_BYCOMMAND | MF_GRAYED );
			::EnableMenuItem( hVid, ID_VIDMENUBASE + ( i * 4 ) + 1, MF_BYCOMMAND | MF_GRAYED );
			::EnableMenuItem( hVid, ID_VIDMENUBASE + ( i * 4 ) + 2, MF_BYCOMMAND | MF_GRAYED );
			::EnableMenuItem( hVid, ID_VIDMENUBASE + ( i * 4 ) + 3, MF_BYCOMMAND | MF_GRAYED );
		} // end else

		// Next driver
		i++;

	} // end while
*/
}

void CMainFrame::OnVideoMenu(UINT uID)
{
	DWORD i = ( uID - ID_VIDMENUBASE ) / 4;
	DWORD f = uID % 4;

	LPVIDEOINFO	pvi = VIDEOLIST().FindByIndex( i );
	if ( pvi == NULL ) { UpdateVideoMenu(); return; }

	// Disable publishing
	CFG().SetValue( "Settings", "EnablePublishing", (DWORD)0 );
	APP()->ExpectDelays( TRUE );

	if ( f == 0 ) pvi->cap->ShowVideoSourceDlg();
	else if ( f == 1 ) pvi->cap->ShowVideoFormatDlg();
	else if ( f == 2 ) pvi->cap->ShowVideoDisplayDlg();
	else if ( f == 3 ) pvi->cap->ShowVideoCompressionDlg();

	// Inform user
	CDlgInform inform;
	inform.Run( "Reconnecting Video..." );

	// Force video reconnect
	inform.SetStatus( "Unloading driver" );
	pvi->cap->Destroy();

	inform.SetStatus( "Reloading driver" );
	VIDEOLIST().Connect( pvi );

	APP()->ExpectDelays( TRUE );

	// Regen everything
	inform.SetStatus( "Redrawing views" );
	APP()->ReGen();

	// Resize
	inform.SetStatus( "Verifying video sizes" );
	EDIT().Size( NULL );

	// Regen everything
	inform.SetStatus( "Redrawing views" );
	APP()->ReGen();

	// Enable publishing if needed
	CFG().SetValue( "Settings", "EnablePublishing", (DWORD)( m_bEnablePublishing ? 1 : 0 ) );

	// Refresh Everything
	SetUpdateTimer();
}

void CMainFrame::OnVideoMenuUpdate(CCmdUI *pCmdUI)
{
	UINT uID = pCmdUI->m_nID;
	DWORD i = ( uID - ID_VIDMENUBASE ) / 4;
	DWORD f = uID % 4;

	LPVIDEOINFO	pvi = VIDEOLIST().FindByIndex( i );
	if ( pvi == NULL ) { UpdateVideoMenu(); return; }

	if ( f == 0 ) pCmdUI->Enable( pvi != NULL && pvi->cap->m_cdc.fHasDlgVideoSource );
	else if ( f == 1 ) pCmdUI->Enable( pvi != NULL && pvi->cap->m_cdc.fHasDlgVideoFormat );
	else if ( f == 2 ) pCmdUI->Enable( pvi != NULL && pvi->cap->m_cdc.fHasDlgVideoDisplay );
	else if ( f == 3 ) pCmdUI->Enable( pvi != NULL );
	else return;	

}

void CMainFrame::OnEditProperties() 
{
	if ( m_propbar.IsWindowVisible() )
		APP()->ShowProperties( 1 );
	else ShowControlBar( &m_propbar, TRUE, FALSE );
}

void CMainFrame::OnViewOverlaybar() 
{
	if ( m_wndOverlayToolbar.IsWindowVisible() )
		ShowControlBar( &m_wndOverlayToolbar, FALSE, FALSE );
	else ShowControlBar( &m_wndOverlayToolbar, TRUE, FALSE );
}

void CMainFrame::OnUpdateViewOverlaybar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndOverlayToolbar.IsWindowVisible() );
}

void CMainFrame::OnViewToolbar() 
{
	if ( m_wndMainToolbar.IsWindowVisible() )
		ShowControlBar( &m_wndMainToolbar, FALSE, FALSE );
	else ShowControlBar( &m_wndMainToolbar, TRUE, FALSE );
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndMainToolbar.IsWindowVisible() );
}

void CMainFrame::OnViewGraphbar() 
{
	if ( m_graphbar.IsWindowVisible() )
		ShowControlBar( &m_graphbar, FALSE, FALSE );
	else ShowControlBar( &m_graphbar, TRUE, FALSE );
}

void CMainFrame::OnUpdateViewGraphbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_graphbar.IsWindowVisible() );
}


void CMainFrame::OnViewWebserver() 
{
	if ( m_wndWebServerToolbar.IsWindowVisible() )
		ShowControlBar( &m_wndWebServerToolbar, FALSE, FALSE );
	else ShowControlBar( &m_wndWebServerToolbar, TRUE, FALSE );

}

void CMainFrame::OnUpdateViewWebserver(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndWebServerToolbar.IsWindowVisible() );
}

void CMainFrame::OnViewProperties() 
{
	if ( m_propbar.IsWindowVisible() )
		ShowControlBar( &m_propbar, FALSE, FALSE );
	else ShowControlBar( &m_propbar, TRUE, FALSE );
}

void CMainFrame::OnUpdateViewProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_propbar.IsWindowVisible() );
}

void CMainFrame::OnOptionsHtmlbuilder() 
{
	CDlgHtmlBuild	dlg;
	dlg.DoModal();
}

void CMainFrame::OnViewPubimgview() 
{
	if ( m_pubimgbar.IsWindowVisible() )
		ShowControlBar( &m_pubimgbar, FALSE, FALSE );
	else ShowControlBar( &m_pubimgbar, TRUE, FALSE );
}

void CMainFrame::OnUpdateViewPubimgview(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_pubimgbar.IsWindowVisible() );
}

void CMainFrame::OnOptionsSetupwizard() 
{	RunSetupWizard();
}

void CMainFrame::OnFileLoadoverlay() 
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL || ppii->fx == NULL ) return;

	// Get the file name
	char fname[ CWF_STRSIZE ] = { 0 };
	if ( !CWinFile::GetOpenFileName(	fname, "Load Overlay",
										"Ovlerlay Files (*.ovl)\x0*.ovl\x0"
										"All Files (*.*)\x0*.*\x0",
										"ovl",
									 GetSafeHwnd() ) ) return;

	EDIT().SetEditFx( NULL );

	if ( !ppii->fx->Load( fname ) )
		AfxMessageBox( "Error Reading Overlay File" );

	// Add to recent docs
	else APP()->AddRecent( fname );

	m_pubimglist.Update();
	EDIT().Size( NULL );
	EDIT().Update();	
	m_imglist.Regen();
}

void CMainFrame::OnFileSaveoverlay() 
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL || ppii->fx == NULL ) return;

	// Get the file name
	char fname[ CWF_STRSIZE ] = { 0 };
	if ( !CWinFile::GetSaveFileName(	fname, "Save Overlay",
										"Ovlerlay Files (*.ovl)\x0*.ovl\x0"
										"All Files (*.*)\x0*.*\x0",
										"ovl",
									 GetSafeHwnd() ) ) return;

	if ( !ppii->fx->Save( fname ) )
		AfxMessageBox( "Error Saving Overlay File" );

	// Add to recent docs
	else APP()->AddRecent( fname );

}

void CMainFrame::OnFileMergeoverlay() 
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL || ppii->fx == NULL ) return;

	// Get the file name
	char fname[ CWF_STRSIZE ] = { 0 };
	if ( !CWinFile::GetOpenFileName(	fname, "Merge Overlay",
										"Ovlerlay Files (*.ovl)\x0*.ovl\x0"
										"All Files (*.*)\x0*.*\x0",
										"ovl",
									 GetSafeHwnd() ) ) return;
	
	if ( !ppii->fx->Load( fname, TRUE ) )
		AfxMessageBox( "Error Reading Overlay File" );

	// Add to recent docs
	else APP()->AddRecent( fname );

	m_pubimglist.Update();
	EDIT().Size( NULL );
	EDIT().Update();	
	m_imglist.Regen();

}

void CMainFrame::OnFileOpen() 
{
	OnFileLoadoverlay();
}

BOOL CMainFrame::LoadOverlay(LPCTSTR pFile)
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL || ppii->fx == NULL ) return FALSE;

	EDIT().SetEditFx( NULL );

	if ( !ppii->fx->Load( pFile ) )
	{	AfxMessageBox( "Error Reading Overlay File" );
		return FALSE;
	} // end if

	// Add to recent docs
	APP()->AddRecent( pFile );

	m_pubimglist.Update();
	EDIT().Size( NULL );
	EDIT().Update();	
	m_imglist.Regen();

	return TRUE;
}

BOOL CMainFrame::SaveOverlay(LPCTSTR pFile)
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL || ppii->fx == NULL ) return FALSE;

	if ( !ppii->fx->Save( pFile ) )
	{	AfxMessageBox( "Error Saving Overlay File" );
		return FALSE;
	} // end if

	// Add to recent docs
	APP()->AddRecent( pFile );

	return TRUE;
}


void CMainFrame::OnSimple() 
{
	try
	{
		// Lose the properties dialog
		APP()->KillProperties();

		// Hide main window
		ShowWindow( SW_HIDE );

		// Create capture mode dialog
		if ( m_simple.GetSafeHwnd() == NULL )
			m_simple.Create( CDlgSimple::IDD, GetDesktopWindow() );

		// Show capture mode dialog
		m_simple.ShowWindow( SW_SHOWNORMAL );
		m_simple.SetForegroundWindow();

	}
	catch( ... ) { ASSERT( 0 ); }
}


LRESULT CMainFrame::OnToolbar(WPARAM wParam, LPARAM lParam)
{
/*
	if ( wParam == 1 )
	{
		if ( !m_iserver.IsRunning() )
		{	MessageBox( "The server is not running", 
						"Server Not running", 
						MB_OK | MB_ICONEXCLAMATION );
			return 0;
		} // end if

		CWnd *pWnd = m_wndServerBar.GetDlgItem( IDC_REMOTEADDRESS );
		if ( pWnd == NULL ) return 0;

		char address[ CWF_STRSIZE ];
		pWnd->GetWindowText( address, sizeof( address ) );

		// Open page
		ShellExecute( NULL, "open", address, NULL, NULL, SW_SHOWNORMAL );
	} // end if

	else if ( wParam == 2 )
	{
		char folder[ CWF_STRSIZE ];
		if ( WEBSETTINGS().Get( "FolderMap", "", folder, sizeof( folder ) ) )

			// Open page
			ShellExecute( NULL, "open", folder, NULL, NULL, SW_SHOWNORMAL );
	} // end else if
*/
	return 1;
}

void CMainFrame::OnSetupsimple() 
{
	CDlgSimpleSettings dlg;
	dlg.DoModal();
}

LRESULT CMainFrame::OnSetStatus(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 ) wParam = 2000;
	m_dwStatusTimeout = GetTickCount() + wParam;	

	char msg[ 4096 ];
	char *txt = (char*)lParam;
	if ( txt != NULL && *txt != 0 )
	{	CWinTime wt;
		wt.GetString( msg, "%h:%m:%s %A : " );
		strcat( msg, (char*)lParam );
	} // end if
	else strcpy( msg, "Idle" );

	// Set status bar status
	m_wndStatusBar.SetPaneText( 1, msg );

	return 0;
}

BOOL CMainFrame::SetStatus(LPCTSTR pText)
{
	return SendMessage( WM_SETSTATUS, 0, (LPARAM)pText );
}


void CMainFrame::OnViewEventlog() 
{
	_ShowLog();	
}

void CMainFrame::OnHelpHelp() 
{
	char help[ CWF_STRSIZE ];

	CWinFile::GetExePath( help );
	CWinFile::BuildPath( help, help, "Help\\index.htm" );

	// Open default mailer
	ShellExecute( NULL, "open", help, NULL, NULL, SW_SHOWNORMAL );
}

LRESULT CMainFrame::OnEvent(WPARAM wParam, LPARAM lParam)
{

	switch( wParam )
	{
		case 1 :
			m_graph.Add( NULL, lParam, 0, 0, CGrDC::LoadIcon( IDI_SMALL_CAMERA ) );
			break;

		case 2 :
			m_graph.Add( NULL, lParam, 0, 0, CGrDC::LoadIcon( IDI_FTP ) );
			break;

		case 3 :
			m_graph.Add( NULL, lParam, 0, 0, CGrDC::LoadIcon( IDI_SMALL_LETTER ) );
			break;

		case 4 :
			m_graph.Add( NULL, lParam, 0, 0, CGrDC::LoadIcon( IDI_DISK ) );
			break;

		case 5 :
			m_graph.Add( NULL, lParam, 0, 0, CGrDC::LoadIcon( IDI_WORLD ) );
			break;

		case 6 :
			m_graph.Add( NULL, lParam, 0, 0, CGrDC::LoadIcon( IDI_CGI ) );
			break;

	} // end switch

	return 1;
}

void CMainFrame::SetEvent(DWORD event)
{	PostMessage( WM_SETEVENT, event, GetTickCount() );
}

LRESULT CMainFrame::OnMainToolbarCmd(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
		case 1 : OnNewimage(); break;
		case 2 : OnDeleteimage(); break;

		case 3 : OnEnablepublishing(); break;
		case 4 : OnRefresh(); break;

		case 5 : OnEditProperties(); break;

		case 6 : OnTrayiconEmailimage(); break;
		case 7 : OnSaveimage(); break;

		case 8 : OnOptionsSettings(); break;
		case 9 : OnOptionsHtmlbuilder(); break;

		case 10 : OnSimple(); break;
		case 11 : OnSetupsimple(); break;

		case 12 : OnTrayiconAbout(); break;

		case 13 : OnViewimage(); break;

	} // end switch


	return 0;
}

LRESULT CMainFrame::OnMainToolbarUpdate(WPARAM wParam, LPARAM lParam)
{
	LPSLANTTOOLBARINFO psti = (LPSLANTTOOLBARINFO)lParam;
	if ( psti == NULL ) return 0;

	if ( wParam == 3 ) 
	{	if ( m_bEnablePublishing ) psti->f1 |= STIF1_CHECKED;
		else psti->f1 &= ~STIF1_CHECKED;
	} // end if

	else if ( wParam == 4 ) 
	{	if ( m_bEnableRefresh ) psti->f1 |= STIF1_CHECKED;
		else psti->f1 &= ~STIF1_CHECKED;
	} // end if

	return 0;
}

/*
#define SETINSERT( flag ) 						\
	EDIT().ToggleInsert( flag );				\
	m_wndInsertBar.InvalidateRect( NULL );

#define SETCHECK( flag )						\
	pCmdUI->SetCheck( EDIT().GetInsertType() == flag );

// FXTYPE_TEXT
*/

#define SETOVINSERT( flag ) 					\
	EDIT().ToggleInsert( flag );				\

LRESULT CMainFrame::OnOverlayToolbarCmd(WPARAM wParam, LPARAM lParam)
{
	switch( wParam )
	{
		case 1 : SETOVINSERT( FXTYPE_TEXT ); break;
		case 2 : SETOVINSERT( FXTYPE_STEXT ); break;
		case 3 : SETOVINSERT( FXTYPE_RECT ); break;
		case 4 : SETOVINSERT( FXTYPE_TIME ); break;
		case 5 : SETOVINSERT( FXTYPE_DATE ); break;
		case 6 : SETOVINSERT( FXTYPE_IMAGE ); break;
		case 7 : SETOVINSERT( FXTYPE_WATERMARK ); break;
		case 8 : SETOVINSERT( FXTYPE_VIDEO ); break;
		case 9 : SETOVINSERT( FXTYPE_URL ); break;
		case 10 : SETOVINSERT( FXTYPE_SCREENSHOT ); break;

	} // end switch


	return 0;
}

#define SETOVCHECK( flag )												\
	if ( EDIT().GetInsertType() == flag ) psti->f1 |= STIF1_CHECKED;	\
	else psti->f1 &= ~STIF1_CHECKED;

LRESULT CMainFrame::OnOverlayToolbarUpdate(WPARAM wParam, LPARAM lParam)
{
	LPSLANTTOOLBARINFO psti = (LPSLANTTOOLBARINFO)lParam;
	if ( psti == NULL ) return 0;

	switch( wParam )
	{
		case 1 : SETOVCHECK( FXTYPE_TEXT ); break;
		case 2 : SETOVCHECK( FXTYPE_STEXT ); break;
		case 3 : SETOVCHECK( FXTYPE_RECT ); break;
		case 4 : SETOVCHECK( FXTYPE_TIME ); break;
		case 5 : SETOVCHECK( FXTYPE_DATE ); break;
		case 6 : SETOVCHECK( FXTYPE_IMAGE ); break;
		case 7 : SETOVCHECK( FXTYPE_WATERMARK ); break;
		case 8 : SETOVCHECK( FXTYPE_VIDEO ); break;
		case 9 : SETOVCHECK( FXTYPE_URL ); break;
		case 10 : SETOVCHECK( FXTYPE_SCREENSHOT ); break;

	} // end switch

	return 0;
}

void CMainFrame::RedrawOverlayBar()
{
	if ( ::IsWindow( m_overlaytoolbar.GetSafeHwnd() ) )
		m_overlaytoolbar.RedrawWindow();
}

LRESULT CMainFrame::OnWebServerToolbarCmd(WPARAM wParam, LPARAM lParam)
{
	switch( wParam )
	{
		case 1 : OnRun(); break;

		case 3 :
		{
			char folder[ CWF_STRSIZE ];
			if ( WEBSETTINGS().Get( "FolderMap", "", folder, sizeof( folder ) ) )

				// Open page
				ShellExecute( NULL, "open", folder, NULL, NULL, SW_SHOWNORMAL );

		} break;

		case 6 :
		{
			if ( !m_iserver.IsRunning() )
			{	MessageBox( "The server is not running", 
							"Server Not running", 
							MB_OK | MB_ICONEXCLAMATION );
				return 0;
			} // end if

			if ( !m_sRemoteAddr.IsEmpty() )

				// Open page
				ShellExecute( NULL, "open", m_sRemoteAddr, NULL, NULL, SW_SHOWNORMAL );

		} break;

	} // end switch

	return 0;
}

LRESULT CMainFrame::OnWebServerToolbarUpdate(WPARAM wParam, LPARAM lParam)
{
	LPSLANTTOOLBARINFO psti = (LPSLANTTOOLBARINFO)lParam;
	if ( psti == NULL ) return 0;

	switch( wParam )
	{
		case 1 :
			if ( m_iserver.IsRunning() ) psti->f1 |= STIF1_CHECKED;
			else psti->f1 &= ~STIF1_CHECKED;
			break;

	} // end switch

	return 0;
}

void CMainFrame::OnViewimage() 
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL || ppii->imgview == NULL ) return;
	OpenViewImgDialog( ppii );

}

BOOL CMainFrame::OpenViewImgDialog(LPPUBIMGINFO ppii)
{
	// Create the window
	ppii->imgview->SetImageSize( ppii->os->GetWidth(), ppii->os->GetHeight() );
	if ( !::IsWindow( ppii->imgview->GetSafeHwnd() ) )
		ppii->imgview->Create( CDlgImgView::IDD, GetDesktopWindow() );

	// Show the window
	ppii->imgview->SetTitle( ppii->name );
	ppii->imgview->RestorePosition();
	ppii->imgview->SetShowDc( ppii->os );
	ppii->imgview->ShowWindow( SW_SHOWNORMAL );
	ppii->imgview->SetForegroundWindow();

	return TRUE;
}

