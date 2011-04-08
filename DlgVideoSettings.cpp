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
// DlgVideoSettings.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgVideoSettings.h"

#include "DlgTreeFrame.h"

#include "VpDoc.h"
#include "VpView.h"

#include "VideoList.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_REDRAWIMAGE		( WM_USER + 101 )

/////////////////////////////////////////////////////////////////////////////
// CDlgVideoSettings dialog


CDlgVideoSettings::CDlgVideoSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVideoSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgVideoSettings)
	m_bFrameAvg = FALSE;
	m_sFrameRate = _T("");
	//}}AFX_DATA_INIT
}


void CDlgVideoSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVideoSettings)
	DDX_Control(pDX, IDC_SLIDE_FRAMEAVG, m_slideFrameAvg);
	DDX_Control(pDX, IDC_SETTINGS, m_comboSettings);
	DDX_Control(pDX, IDC_DRIVER, m_comboDriver);
	DDX_Control(pDX, IDC_IMAGE, m_image);
	DDX_Check(pDX, IDC_FRAMEAVG, m_bFrameAvg);
	DDX_Text(pDX, IDC_FRAMERATE, m_sFrameRate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgVideoSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgVideoSettings)
	ON_BN_CLICKED(IDC_EFFECTS, OnEffects)
	ON_BN_CLICKED(IDC_FRAMEAVG, OnFrameavg)
	ON_CBN_SELCHANGE(IDC_DRIVER, OnSelchangeDriver)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_FRAMERATE, OnChangeFramerate)
	ON_CBN_SELCHANGE(IDC_SETTINGS, OnSelchangeSettings)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_REDRAWIMAGE, OnRedrawImage )
	ON_MESSAGE( WM_PG_SAVE, OnSave )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVideoSettings message handlers

BOOL CDlgVideoSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Load capture drier names
	CCapture	cap;
	DWORD		drivers = cap.GetNumDrivers();
	for ( DWORD i = 0; i < drivers; i++ )
	{
		CAPTUREDRIVERINFO	ci;

		// For each driver
		if ( cap.GetDriverInfo( i, &ci ) )
		{
			int index = CB_ERR;
			char show[ CWF_STRSIZE ];
			char str[ 256 ];

			// Driver string
			wsprintf( str, "Driver %lu", i );		

			// Driver name
			sprintf( show, "%s (%s)", ci.szName, ci.szVer );

			if ( *show ) index = m_comboDriver.AddString( show );
			else  index = m_comboDriver.AddString( str );
			if ( index != CB_ERR ) m_comboDriver.SetItemData( index, i );
		} // end if

	} // end for
	
	m_comboDriver.SetCurSel( 0 );

	m_slideFrameAvg.SetRange( 1, 99 );
	m_slideFrameAvg.SetPos( 10 );

	OnRefresh( 0, 0L );
	
	FRAME()->SetRefreshWnd( GetSafeHwnd(), WM_REDRAWIMAGE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgVideoSettings::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return FALSE;

	// Set the video image
	m_image.DrawMotion( FALSE );

	// Used averaged frame if needed
	if ( ( pvi->f1 & VIF1_FRAMEAVG ) != 0 )
	{	m_bFrameAvg = TRUE; m_image.SetImage( pvi->tavg ); }
	else { m_bFrameAvg = FALSE; m_image.SetImage( pvi->os ); }
	
	m_image.SetVideoInfo( pvi );

	m_slideFrameAvg.SetPos( RANGE( pvi->frameavgweight, 1, 99 ) );

	char num[ 32 ];
	wsprintf( num, "%lu", pvi->framerate );
	m_sFrameRate = num;

	m_comboSettings.ResetContent();
	if ( pvi->cap != NULL )
	{
		if ( pvi->cap->m_cdc.fHasDlgVideoSource )
			m_comboSettings.SetItemData( m_comboSettings.AddString( "Source" ), 1 );
		if ( pvi->cap->m_cdc.fHasDlgVideoFormat )
			m_comboSettings.SetItemData( m_comboSettings.AddString( "Format" ), 2 );
		if ( pvi->cap->m_cdc.fHasDlgVideoDisplay )
			m_comboSettings.SetItemData( m_comboSettings.AddString( "Display" ), 3 );
		m_comboSettings.SetItemData( m_comboSettings.AddString( "Compression" ), 4 );

	} // end if

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

LRESULT CDlgVideoSettings::OnRedrawImage(WPARAM wParam, LPARAM lParam)
{
	// Redraw image
	if ( IsWindow( m_image.GetSafeHwnd() ) )
		m_image.RedrawWindow();

	return 0;
}

LPVIDEOINFO CDlgVideoSettings::GetDriverInfo()
{
	DWORD driver = 0;
	int index = m_comboDriver.GetCurSel();
	if ( index != CB_ERR )
		driver = m_comboDriver.GetItemData( index );

	LPVIDEOINFO pvi = VIDEOLIST().Add( driver );
	if ( pvi == NULL ) return NULL;

	return pvi;
}

void CDlgVideoSettings::OnEffects() 
{
	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return;

	// Set image data
	pvi->imgfx->SetFx( pvi->name, pvi->os->GetWidth(), pvi->os->GetHeight() );
	pvi->imgfx->Edit( this );
}

void CDlgVideoSettings::OnFrameavg() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() ); 
	UpdateData( TRUE ); DoEnable();
}

void CDlgVideoSettings::OnSelchangeDriver() 
{
	UpdateData( TRUE );
	OnRefresh( 0, 0L );	
}

void CDlgVideoSettings::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ( pScrollBar == (CScrollBar*)&m_slideFrameAvg )
		CDlgTreeFrame::EnableApply( GetSafeHwnd() );
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgVideoSettings::OnChangeFramerate() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

LRESULT CDlgVideoSettings::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return 0;

	if ( m_bFrameAvg ) pvi->f1 |= VIF1_FRAMEAVG;
	else pvi->f1 &= ~VIF1_FRAMEAVG;

	if ( ( pvi->f1 & VIF1_FRAMEAVG ) != 0 ) m_image.SetImage( pvi->tavg );
	else m_image.SetImage( pvi->os );

	// Assign new value
	long pos = m_slideFrameAvg.GetPos();
	pvi->frameavgweight = RANGE( pos, 1, 99 );		

	pvi->framerate = strtoul( m_sFrameRate, NULL, 10 );
	pvi->framerate = RANGE( pvi->framerate, 0, 60 );

	if ( pvi->cap != NULL ) pvi->cap->SetFrameRate( pvi->framerate );

	return 1;
}

void CDlgVideoSettings::OnSelchangeSettings() 
{
	int sel = m_comboSettings.GetCurSel();
	if ( sel == CB_ERR ) return;

	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return;

	DWORD dialog = m_comboSettings.GetItemData( sel );
	
	// Enable publishing if needed
	CFG().SetValue( "Settings", "EnablePublishing", (DWORD)0 );
	APP()->ExpectDelays( TRUE );
	
	if ( dialog == 1 ) pvi->cap->ShowVideoSourceDlg();
	else if ( dialog == 2 ) pvi->cap->ShowVideoFormatDlg();
	else if ( dialog == 3 ) pvi->cap->ShowVideoDisplayDlg();
	else if ( dialog == 4 ) pvi->cap->ShowVideoCompressionDlg();	

	m_comboSettings.SetCurSel( -1 );

	// Inform user
	CDlgInform inform;
	inform.Run( "Reconnecting Video..." );

	// Force video reconnect
	inform.SetStatus( "Closing capture driver" );
	pvi->cap->Destroy();

	inform.SetStatus( "Connecting video driver" );
	VIDEOLIST().Connect( pvi );

	APP()->ExpectDelays( TRUE );
		 
	// Regen everything
	inform.SetStatus( "Updating views" );
	APP()->ReGen();

	// Resize
	inform.SetStatus( "Verifying image size" );
	EDIT().Size( NULL );

	// Regen everything
	inform.SetStatus( "Updating views" );
	APP()->ReGen();

	CFG().SetValue( "Settings", "EnablePublishing", (DWORD)( FRAME()->m_bEnablePublishing ? 1 : 0 ) );

	FRAME()->SetUpdateTimer();
}

void CDlgVideoSettings::DoEnable()
{
	CWnd *pCtrl = GetDlgItem( IDC_SLIDE_FRAMEAVG );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bFrameAvg );
}
