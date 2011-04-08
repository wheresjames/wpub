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
// Vp.cpp : Defines the class behaviors for the application.
//

// Ref
// http://www.microsoft.com/whdc/hwdev/archive/devdes/fourcc.mspx


#include "stdafx.h"
#include "Vp.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"
#include "About.h"

#include "PgDrawingOrder.h"
#include "PgText.h"
#include "PgTextFlags.h"
#include "PgTime.h"
#include "PgDate.h"
#include "PgRect.h"
#include "PgImage.h"
#include "PgTrans.h"
#include "PgVideo.h"
#include "PgStext.h"
#include "PgScreenShot.h"
#include "PgBckImage.h"
#include "PgImageProp.h"
#include "PgChromaKey.h"
#include "PgChromaBck.h"
#include "PgMask.h"
#include "PgUrl.h"

//#include "DlgPurchase.h"
//#include "DlgRegBrowser.h"
//#include "Registration.h"

#ifdef _DEBUG
#undef _VALIDATE_FILE_
#else
#define _VALIDATE_FILE_
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVpApp

BEGIN_MESSAGE_MAP(CVpApp, CWinApp)
	//{{AFX_MSG_MAP(CVpApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVpApp construction

CVpApp::CVpApp()
{
	m_bRestart = FALSE;
	m_fxProperties = NULL;
	m_hMutex = NULL;
	m_bExpectDelays = FALSE;
}

CVpApp::~CVpApp()
{

}

void CVpApp::AddRecent( LPCTSTR pFile )
{ m_pRecentFileList->Add( pFile ); }


/////////////////////////////////////////////////////////////////////////////
// The one and only CVpApp object

CVpApp theApp;
//WTL::CAppModule _Module;

/////////////////////////////////////////////////////////////////////////////
// CVpApp initialization

BOOL CVpApp::InitInstance()
{
	// Init ATL
//	_Module.Init(NULL, AfxGetInstanceHandle());

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	CWinApp::InitInstance();

// File validation
#ifdef _VALIDATE_FILE_

	if ( !CWinFile::ValidateModuleCRC32() )
	{
		char msg[ MAX_PATH * 2 ];
		char exe[ MAX_PATH ];

		// Get module filename
		if ( !GetModuleFileName( NULL, exe, MAX_PATH - 1 ) )
			return FALSE;
			
		strcpy( msg,	"This program has been currupted or tampered with.\r\n\r\n"
						"This may occur during a file transfer.\r\n"
						"You may also consider scanning this file for a virus.\r\n\r\n"
						"This program is available at http://www.wheresjames.com\r\n\r\n" );
		strcat( msg, exe );
					
		CWinFile::ShowLastError( "Self validation error", msg, MB_OK | MB_ICONSTOP );

		return FALSE;

	} // end if

#endif


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Only one instance at a time please
	m_hMutex = CreateMutex( NULL, TRUE, "WheresJames Webcam Publisher Pro (Single Instance)" );
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{	ReleaseMutex( m_hMutex );
		m_hMutex = NULL;
		AfxMessageBox( "WheresJames Webcam Publisher is already running." );
		return FALSE;
	} // end if

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey( _T(MFCKEY_SETTINGS) );

	// Initialize XP themes
	CXpTheme::Init();

	// Don't let anyone timeout during initialization
	ExpectDelays( TRUE );
	
	// Delete the cache folder
	char cache[ CWF_STRSIZE ];
	CWinFile::GetExePath( cache );
	CWinFile::BuildPath( cache, cache, "Cache" );
	CWinFile::DeleteDirectory( cache, TRUE );

	// Load settings from files
	LoadSettings();

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Do registration stuff
	if ( !Register() ) return FALSE;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CVpDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CVpView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Set window text
	FRAME()->SetWindowText( APPNAME );

	// Restore simple mode if needed
	DWORD dw = 1;
	if ( !CFG().GetValue( "Settings", "CaptureMode", &dw ) || dw == 0 )
	{
		// Restore Window Position
		if ( !CFG().RestoreWindowPos( FRAME()->GetSafeHwnd(), "Positions", "MainWindow" ) )
			m_pMainWnd->ShowWindow( SW_SHOWNORMAL );
	} // end if
	else m_pMainWnd->ShowWindow( SW_HIDE );

	m_pMainWnd->UpdateWindow();

	// Get run times
	DWORD runtimes = 0;
	if ( !CFG().GetValue( "Settings", "RunTimes", &runtimes ) )
		runtimes = 0;

	// Run setup if first time
	if ( !runtimes ) 
	{
		// Set default values
/*		CFG().SetValue( "Settings", "RootDirectory", "webcam" );	
		CFG().SetValue( "Settings", "HtmlRootDirectory", "webcam" );	
		CFG().SetValue( "Settings", "Interval", (DWORD)30 );
		CFG().SetValue( "Settings", "PublishScreenShot", (DWORD)0 );
		CFG().SetValue( "Settings", "PublishCamera", (DWORD)1 );
		CFG().SetValue( "Settings", "ScreenShotFileName", "sshot.jpg" );	
		CFG().SetValue( "Settings", "ImageFileName", "webcam.jpg" );	
		CFG().SetValue( "Settings", "SShotWidth", (DWORD)320 );	
		CFG().SetValue( "Settings", "SShotHeight", (DWORD)240 );	
		CFG().SetValue( "Settings", "SenseMotion", (DWORD)300 );
		CFG().SetValue( "Settings", "SenseBright", (DWORD)1000 );
		CFG().SetValue( "Settings", "EmailSubject", "WheresJames Webcam Publisher" );
		CFG().SetValue( "Settings", "EmailFilename", "webcam.jpg" );
		CFG().SetValue( "Settings", "EmailBody", "Motion Detected!" );

		// Go ahead and save the settings
		m_cfg.Save( path );
*/
	} // end if

	runtimes++;
	CFG().SetValue( "Settings", "RunTimes", runtimes );

	// Restore the settings
	FRAME()->RestoreSettings();

	_Log( MB_ICONINFORMATION, "Init()", "Application Started" );	

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVpApp commands

void CVpApp::LoadSettings()
{
	char cfg[ CWF_STRSIZE ];
	char path[ CWF_STRSIZE ];

	CWinFile::GetExePath( path );

	// Load configuration
	CWinFile::BuildPath( cfg, path, "Default.wjp" );
	BOOL loaded = m_cfg.Load( cfg );

	// Protect against file damage
	DWORD runs = 0;
	if ( !loaded || !m_cfg.GetValue( "Settings", "RunTimes", &runs ) )
	{	CWinFile::BuildPath( cfg, path, "Default.bak" );
		m_cfg.Load( cfg );
	} // end if

	// Load web server settings
	CWinFile::BuildPath( cfg, path, "Webserver.cfg" );
	WEBSETTINGS().LoadRegFile( cfg );
	
	// Load web server settings
	CWinFile::BuildPath( cfg, path, "HtmlVars.cfg" );
	HTMLVARS().LoadRegFile( cfg );
	
	// Load FTP server settings
	CWinFile::BuildPath( cfg, path, "Ftp.cfg" );
	FTPSERVERS().Load( cfg );

	// Load SMTP server settings
	CWinFile::BuildPath( cfg, path, "Smtp.cfg" );
	EMAILSERVERS().Load( cfg );

	// Load Publishing settings
	CWinFile::BuildPath( cfg, path, "Publishing.cfg" );
	PUBLIST().Load( cfg );

	// Load Publishing settings
	CWinFile::BuildPath( cfg, path, "CGI.cfg" );
	CGI().LoadRegFile( cfg );

	// Load video settings
	CWinFile::BuildPath( cfg, path, "Video.cfg" );
	VIDEOSETTINGS().LoadRegFile( cfg );

	// Load chat settings
	CWinFile::BuildPath( cfg, path, "Chat.cfg" );
	CHAT().LoadRegFile( cfg );

	// Open log file
	CWinFile::BuildPath( cfg, path, "Log.txt" );
	_SetLogFile( cfg );

}

void CVpApp::SaveSettings()
{
	char cfg[ CWF_STRSIZE ];
	char path[ CWF_STRSIZE ];

	CWinFile::GetExePath( path );

	// Save configuration
	CWinFile::BuildPath( cfg, path, "Default.bkp" );
	CFG().Save( cfg );
	CWinFile::BuildPath( cfg, path, "Default.wjp" );
	CFG().Save( cfg );

	// Save web server settings
	CWinFile::BuildPath( cfg, path, "Webserver.cfg" );
	WEBSETTINGS().SaveRegFile( cfg );

	// Save web server settings
	CWinFile::BuildPath( cfg, path, "HtmlVars.cfg" );
	HTMLVARS().SaveRegFile( cfg );

	// Save FTP server settings
	CWinFile::BuildPath( cfg, path, "Ftp.cfg" );
	FTPSERVERS().Save( cfg );

	// Save SMTP server settings
	CWinFile::BuildPath( cfg, path, "Smtp.cfg" );
	EMAILSERVERS().Save( cfg );
	
	// Save Publishing settings
	CWinFile::BuildPath( cfg, path, "Publishing.cfg" );
	PUBLIST().Save( cfg );

	// Save Publishing settings
	CWinFile::BuildPath( cfg, path, "CGI.cfg" );
	CGI().SaveRegFile( cfg );

	// Save video settings
	VIDEOLIST().WriteSettings();
	CWinFile::BuildPath( cfg, path, "Video.cfg" );
	VIDEOSETTINGS().SaveRegFile( cfg );

	// Save Publishing settings
	CWinFile::BuildPath( cfg, path, "Chat.cfg" );
	CHAT().SaveRegFile( cfg );

	// Save the image list
	IMGLIST().Save();
}

int CVpApp::ExitInstance() 
{
	// Don't do anything if we failed to init
	if ( m_hMutex == NULL ) return CWinApp::ExitInstance();

	try
	{
		CXpTheme::Uninit();

		m_sndthread.StopThread();
		m_dlgimgfxeditor.DestroyWindow();
		m_dlgproperties.Destroy();
		m_cfg.Destroy();
		m_serverlog.Destroy();

		m_websettings.Destroy();
		m_htmlvars.Destroy();
		m_ftpservers.Destroy();
		m_emailservers.Destroy();
		m_publist.Destroy();
		m_cgi.Destroy();
		m_chat.Destroy();

		m_screenshot.Destroy();
		m_webusers.Destroy();
		m_authusers.Destroy();
		m_authadmin.Destroy();

	} catch( ... ) { ASSERT( 0 ); }

	// Release run-once mutex
	if ( m_hMutex != NULL )
	{	ReleaseMutex( m_hMutex );
		CloseHandle( m_hMutex );
		m_hMutex = NULL;
	} // end if

	// Want to restart the application
	if ( m_bRestart ) 
	{
		// Wait one second for good measure
		Sleep( 1000 );

		// Restart the app
		CWin32::StartNewApp();

	} // end if

	// Kill the ATL
//	_Module.Term();	

	return CWinApp::ExitInstance();
}

void CVpApp::ShowProperties(DWORD i)
{
	// Lose current pages
//	if ( ::IsWindow( m_dlgproperties.GetSafeHwnd() ) )
//		m_dlgproperties.DestroyWindow();

	// Get edit type
	DWORD	type = 0;
	LPFX	pfx = EDIT().GetEditFx();
	if ( pfx != NULL ) type = pfx->type;

	// Show properties
	FRAME()->m_dlgprop.Show( pfx );

	// Do we need to bother the properties dialog?
	if ( ::IsWindowVisible( m_dlgproperties.GetSafeHwnd() ) )
	{	if ( m_fxProperties == pfx ) 
		{	RefreshProperties(); 
			return; 
		} // end if
	} // end if

	// Punt if not creating
	else if ( i == 0 ) return;

	// save property page
	m_fxProperties = pfx;

	// Lose current pages
	m_dlgproperties.Destroy();

	switch( type )
	{
		case FXTYPE_NONE :
			m_dlgproperties.SetPage( 0, new CPgImageProp(), CPgImageProp::IDD, "Image" );
			m_dlgproperties.SetPage( 1, new CPgBckImage(), CPgBckImage::IDD, "Background" );
			m_dlgproperties.SetPage( 2, new CPgMask(), CPgMask::IDD, "Frame" );
			m_dlgproperties.SetPage( 3, new CPgChromaKey(), CPgChromaKey::IDD, "Chroma-Key" );
			m_dlgproperties.SetPage( 4, new CPgChromaBck(), CPgChromaBck::IDD, "Key Image" );
			m_dlgproperties.SetPage( 5, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;

		case FXTYPE_TEXT :
			m_dlgproperties.SetPage( 0, new CPgText(), CPgText::IDD, "Text" );
			m_dlgproperties.SetPage( 1, new CPgTextFlags(), CPgTextFlags::IDD, "Flags" );
			m_dlgproperties.SetPage( 2, new CPgRect(), CPgRect::IDD, "Background" );
			m_dlgproperties.SetPage( 3, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;

		case FXTYPE_TIME :
			m_dlgproperties.SetPage( 0, new CPgTime(), CPgTime::IDD, "Time" );
			m_dlgproperties.SetPage( 1, new CPgText(), CPgText::IDD, "Text" );
			m_dlgproperties.SetPage( 2, new CPgTextFlags(), CPgTextFlags::IDD, "Flags" );
			m_dlgproperties.SetPage( 3, new CPgRect(), CPgRect::IDD, "Background" );
			m_dlgproperties.SetPage( 4, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;

		case FXTYPE_DATE :
			m_dlgproperties.SetPage( 0, new CPgDate(), CPgDate::IDD, "Date" );
			m_dlgproperties.SetPage( 1, new CPgText(), CPgText::IDD, "Text" );
			m_dlgproperties.SetPage( 2, new CPgTextFlags(), CPgTextFlags::IDD, "Flags" );
			m_dlgproperties.SetPage( 3, new CPgRect(), CPgRect::IDD, "Background" );
			m_dlgproperties.SetPage( 4, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;
	
		case FXTYPE_RECT :
			m_dlgproperties.SetPage( 0, new CPgRect(), CPgRect::IDD, "Rectangle" );
			m_dlgproperties.SetPage( 1, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;

		case FXTYPE_IMAGE :
		case FXTYPE_WATERMARK :
			m_dlgproperties.SetPage( 0, new CPgImage(), CPgImage::IDD, "Image" );
			m_dlgproperties.SetPage( 1, new CPgTrans(), CPgTrans::IDD, "Transparency" );
			m_dlgproperties.SetPage( 2, new CPgRect(), CPgRect::IDD, "Background" );
			m_dlgproperties.SetPage( 3, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;

		case FXTYPE_VIDEO :
			m_dlgproperties.SetPage( 0, new CPgVideo(), CPgVideo::IDD, "Video" );
			m_dlgproperties.SetPage( 1, new CPgTrans(), CPgTrans::IDD, "Chorma-Keying" );
			m_dlgproperties.SetPage( 2, new CPgRect(), CPgRect::IDD, "Background" );
			m_dlgproperties.SetPage( 3, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;

		case FXTYPE_SCREENSHOT :
			m_dlgproperties.SetPage( 0, new CPgScreenShot(), CPgScreenShot::IDD, "Window Capture" );
			m_dlgproperties.SetPage( 1, new CPgTrans(), CPgTrans::IDD, "Transparency" );
			m_dlgproperties.SetPage( 2, new CPgRect(), CPgRect::IDD, "Background" );
			m_dlgproperties.SetPage( 3, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;

		case FXTYPE_STEXT :
			m_dlgproperties.SetPage( 0, new CPgStext(), CPgStext::IDD, "Special Text" );
			m_dlgproperties.SetPage( 1, new CPgText(), CPgText::IDD, "Text" );
			m_dlgproperties.SetPage( 2, new CPgTextFlags(), CPgTextFlags::IDD, "Flags" );
			m_dlgproperties.SetPage( 3, new CPgRect(), CPgRect::IDD, "Background" );
			m_dlgproperties.SetPage( 4, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;

		case FXTYPE_URL :
			m_dlgproperties.SetPage( 0, new CPgUrl(), CPgUrl::IDD, "Internet Image" );
			m_dlgproperties.SetPage( 1, new CPgTrans(), CPgTrans::IDD, "Transparency" );
			m_dlgproperties.SetPage( 2, new CPgRect(), CPgRect::IDD, "Background" );
			m_dlgproperties.SetPage( 3, new CPgDrawingOrder(), CPgDrawingOrder::IDD, "Drawing Order" );
			break;
	} // end switch

	if ( !::IsWindow( m_dlgproperties.GetSafeHwnd() ) )
	{
		m_dlgproperties.SizeDialog( TRUE );
		m_dlgproperties.SetRestore( "Properties" );
		m_dlgproperties.Create( IDD_PROPERTIES, FRAME() );
		m_dlgproperties.ShowWindow( SW_SHOWNA );
	} // end if
	else
	{
		m_dlgproperties.SetPage( 0 );
		m_dlgproperties.ShowWindow( SW_SHOWNA );
	} // end else

	VIEW()->SetFocus();
}

void CVpApp::RefreshProperties()
{
	// Refresh property pages
	if ( ::IsWindow( m_dlgproperties.GetSafeHwnd() ) )
		m_dlgproperties.RefreshPages();

	// Show properties
	FRAME()->m_dlgprop.Refresh();

}

void CVpApp::ReGen( BOOL bAll )
{
	IMGLIST().Update( MAXDWORD, bAll );
	FRAME()->m_imglist.SetUpdateTimer();
//	FRAME()->m_dlgpubimg.Regen();
//	FRAME()->m_dlgpubimg.Update();
//	FRAME()->m_dlgpubimg.SetUpdateTimer();
	EDIT().Update();	
	FRAME()->SetFrameRates();

	// Refresh video menu
//	FRAME()->UpdateVideoMenu();

	// Refresh properties
	RefreshProperties();
}

CFx* CVpApp::GetActiveFx()
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return NULL;
	return ppii->fx;
}

BOOL CVpApp::RegistrationDlg()
{
/*	// Beg user for money
	CDlgTabFrame	dlg;
	CDlgPurchase	*purchase = new CDlgPurchase;

	dlg.SetPage( 0, purchase, CDlgPurchase::IDD, "Register" );
	dlg.SetPage( 1, new CDlgRegBrowser(), CDlgRegBrowser::IDD, "Purchase" );

	dlg.SizeDialog( FALSE );
	dlg.SetRestore( "Properties" );
	dlg.Create( IDD_REGISTER );
	dlg.CenterWindow();
	dlg.ShowWindow( SW_SHOWNORMAL );    
	dlg.SetWindowText( APPNAME );

	// Run modal loop
	dlg.RunModalLoop();
*/
	return TRUE;
}

BOOL CVpApp::Register()
{
	return TRUE;
/*
	char decode[ 256 ] = { 0 };
	char val[ 256 ] = { 0 };

	if ( !CFG().GetValue( "Settings", "RegistrationCode", val, sizeof( val ) ) )
		*val = 0;

	// See if user is registered
	if (	*val != 0 &&
			CRegistration::Decode( (LPBYTE)decode, ( sizeof( decode ) - 2 ) * 8, val ) == 0 &&
			strtoul( decode, NULL, 10 ) == PRODUCTID ) return TRUE;

	// Beg user for money
	CDlgTabFrame	dlg;
	CDlgPurchase	*purchase = new CDlgPurchase;

	dlg.SetPage( 0, purchase, CDlgPurchase::IDD, "Register" );
	dlg.SetPage( 1, new CDlgRegBrowser(), CDlgRegBrowser::IDD, "Purchase" );

	dlg.SizeDialog( FALSE );
	dlg.SetRestore( "Properties" );
	dlg.Create( IDD_REGISTER );
	dlg.CenterWindow();
	dlg.ShowWindow( SW_SHOWNORMAL );    
	dlg.SetWindowText( APPNAME );

	// Run modal loop
	dlg.RunModalLoop();

	// Did they buy?
	if (	CFG().GetValue( "Settings", "RegistrationCode", val, sizeof( val ) ) &&
			CRegistration::Decode( (LPBYTE)decode, ( sizeof( decode ) - 2 ) * 8, val ) == 0 &&
			strtoul( decode, NULL, 10 ) == PRODUCTID ) return TRUE;

	// Quit if no days left on trial
	if ( purchase->m_dwDaysLeft == 0 ) 
	{	AfxMessageBox( "Your Trial Period has expired" );
		return FALSE;
	} // end if

	return TRUE;
*/
}

BOOL CVpApp::Restart()
{	m_bRestart = TRUE;
	FRAME()->Exit();
	return TRUE;
}


void CVpApp::Log(CWinFile *f, BOOL bRx, LPCTSTR pPre, LPCTSTR pData, DWORD len)
{
	if ( f == NULL || pData == NULL ) return;

	DWORD s = 0, e = 0;

	if ( len == 0 ) len = strlen( pData );

	while ( pData[ s ] != 0 && s < len )
	{
		// Find the end of line or message
		while ( e < len && pData[ e ] != 0 && 
				pData[ e ] != '\r' && pData[ e ] != '\n' ) e++;

		if ( bRx ) f->Write( "\r\nRx: " );
		else f->Write( "\r\nTx: " );

		if ( pPre != NULL )
		{	f->Write( (LPVOID)pPre );
			f->Write( ": " );
		} // end if

		// Write data if any
		if ( e > s ) f->Write( (LPVOID)&pData[ s ], e - s );

		// Skip CRLF
		if ( e < len && pData[ e ] == '\r' ) e++;
		if ( e < len && pData[ e ] == '\n' ) e++;

		// Next
		s = e;

	} // end whiel

}

void CVpApp::FtpLog(BOOL bRx, LPCTSTR pServer, LPCTSTR pData, DWORD len)
{
	DWORD dw = 0;
	CFG().GetValue( "Settings", "EnableFtpLog", &dw );
	if ( !dw ) return;
	
	if ( !m_ftplog.IsOpen() )
	{
		char str[ CWF_STRSIZE ];
		char path[ CWF_STRSIZE ];
		strcpy( str, "FtpLog.txt" );
		CFG().GetValue( "Settings", "FtpLogFile", str, sizeof( str ) );

		CWinFile::GetExePath( path, sizeof( path ) );
		CWinFile::BuildPath( path, path, str );

		if ( !m_ftplog.OpenNew( path ) ) return;

	} // end if

	Log( &m_ftplog, bRx, pServer, pData, len );
}


void CVpApp::SmtpLog(BOOL bRx, LPCTSTR pServer, LPCTSTR pData, DWORD len)
{
	DWORD dw = 0;
	CFG().GetValue( "Settings", "EnableSmtpLog", &dw );
	if ( !dw ) return;
	
	if ( !m_smtplog.IsOpen() )
	{
		char str[ CWF_STRSIZE ];
		char path[ CWF_STRSIZE ];
		strcpy( str, "FtpLog.txt" );
		CFG().GetValue( "Settings", "SmtpLogFile", str, sizeof( str ) );

		CWinFile::GetExePath( path, sizeof( path ) );
		CWinFile::BuildPath( path, path, str );

		if ( !m_smtplog.OpenNew( path ) ) return;

	} // end if

	Log( &m_smtplog, bRx, pServer, pData, len );
}

void CVpApp::KillProperties()
{
	// Lose current pages
	m_dlgproperties.Destroy();
	m_dlgproperties.DestroyWindow();
}

