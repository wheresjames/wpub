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
// DlgMotion.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgMotion.h"

#include "VideoList.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_REDRAWIMAGE		( WM_USER + 101 )

/////////////////////////////////////////////////////////////////////////////
// CDlgMotion dialog


CDlgMotion::CDlgMotion(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMotion::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMotion)
	m_bEnable = FALSE;
	m_bEnableSound = FALSE;
	m_sSoundFile = _T("");
	//}}AFX_DATA_INIT

	m_bright = 0;
	m_motion = 0;
}


void CDlgMotion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMotion)
	DDX_Control(pDX, IDC_CHOOSE_SOUND, m_btnChooseSound);
	DDX_Control(pDX, IDC_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_SLIDE_MOTION, m_slideMotion);
	DDX_Control(pDX, IDC_SLIDE_BRIGHT, m_slideBright);
	DDX_Control(pDX, IDC_SCANTYPE, m_comboScanType);
	DDX_Control(pDX, IDC_LEVEL_MOTION, m_progMotion);
	DDX_Control(pDX, IDC_LEVEL_BRIGHT, m_progBright);
	DDX_Control(pDX, IDC_IMAGE, m_image);
	DDX_Control(pDX, IDC_DRIVER, m_comboDriver);
	DDX_Check(pDX, IDC_ENABLE, m_bEnable);
	DDX_Check(pDX, IDC_ENABLESOUND, m_bEnableSound);
	DDX_Text(pDX, IDC_SOUNDFILE, m_sSoundFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMotion, CDialog)
	//{{AFX_MSG_MAP(CDlgMotion)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_CBN_SELCHANGE(IDC_DRIVER, OnSelchangeDriver)
	ON_CBN_SELCHANGE(IDC_SCANTYPE, OnSelchangeScantype)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_CHOOSE_SOUND, OnChooseSound)
	ON_BN_CLICKED(IDC_ENABLESOUND, OnEnablesound)
	ON_EN_CHANGE(IDC_SOUNDFILE, OnChangeSoundfile)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_MOTIONDETECT, OnMotion )
	ON_MESSAGE( WM_REDRAWIMAGE, OnRedrawImage )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMotion message handlers

BOOL CDlgMotion::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set button icons
	m_btnPlay.SetIcon( CGrDC::LoadIcon( IDI_DS_TICK_RIGHT ) );
	m_btnChooseSound.SetIcon( CGrDC::LoadIcon( IDI_FOLDER ) );

	m_slideBright.SetRange( 0, 1000 );
	m_slideMotion.SetRange( 0, 1000 );

	m_progBright.SetRange( 0, 1000 );
	m_progMotion.SetRange( 0, 1000 );

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
	
	m_comboScanType.SetItemData( m_comboScanType.AddString( "Grid" ), 0 );
	m_comboScanType.SetItemData( m_comboScanType.AddString( "Point" ), 1 );

	SetMotion( 0, 0 );

	// Are we in the wizard?
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg != NULL )
	{
		// Which driver does the user want?
		DWORD driver = pReg->GetDword( "PUBINFO", "Motion" );
		
		// Select the motion driver
		CComCtrl::ComboSelectItemByData( m_comboDriver.GetSafeHwnd(), driver );		

		// Enable the driver
		LPVIDEOINFO pvi = GetDriverInfo();
		if ( pvi != NULL ) pvi->motion.bEnable = TRUE;

	} // end if

	// Start with the first driver
	else m_comboDriver.SetCurSel( 0 );

	OnRefresh( 0, 0L );

	FRAME()->SetMotionWnd( GetSafeHwnd() );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgMotion::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );
	
	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return FALSE;

	// Set brightness threshold
	m_slideBright.SetPos( pvi->motion.thbright );

	// Set motion threshold
	m_slideMotion.SetPos( pvi->motion.thmotion );

	// Select the scan type
	CComCtrl::ComboSelectItemByData( m_comboScanType.GetSafeHwnd(), pvi->motion.scantype );

	// Set motion detection enable/disable
	m_bEnable = pvi->motion.bEnable;

	// Set the video image
	m_image.DrawMotion( TRUE );
	m_image.SetImage( pvi->os );
	m_image.SetVideoInfo( pvi );

	m_bEnableSound = ( pvi->f1 & VIF1_MOTIONSOUND ) != 0;
	m_sSoundFile = pvi->motionsnd;

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CDlgMotion::SetMotion(long bright, long motion)
{
	// Punt if no window
	if ( !IsWindow( GetSafeHwnd() ) ) return;

	if ( bright < 0 ) bright = 0; 
	else if ( bright > 1000 ) bright = 1000;

	if ( motion < 0 ) motion = 0; 
	else if ( motion > 1000 ) motion = 1000;

	if ( IsWindow( m_progBright.GetSafeHwnd() ) )
		m_progBright.SetPos( bright );

	if ( IsWindow( m_progMotion.GetSafeHwnd() ) )
		m_progMotion.SetPos( motion );
}


void CDlgMotion::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	long pos = pScrollBar->GetScrollPos();

	int min = 0, max = 100;
//	pScrollBar->GetScrollRange( &min, &max );

	switch( nSBCode )
	{
		case SB_LEFT : pos = min;							break;
		case SB_ENDSCROLL : 
			break;
		case SB_LINELEFT : pos -= 1;						break;
		case SB_LINERIGHT : pos += 1;						break;
		case SB_PAGELEFT : pos -= ( ( max - min ) / 10 );	break;
		case SB_PAGERIGHT : pos += ( ( max - min ) / 10 );	break;
		case SB_RIGHT : pos = max;							break;
		case SB_THUMBPOSITION : pos = long( nPos );			break;
		case SB_THUMBTRACK : pos = long( nPos );			break;
		default : return; break;

	} // end switch
	
	if ( (CSliderCtrl*)pScrollBar == &m_slideBright )
	{
		if ( nSBCode == SB_ENDSCROLL )
			pos = m_slideBright.GetPos();
		else m_slideBright.SetPos( pos );

//		char num[ 64 ];
//		wsprintf( num, "%lu", pos );
//		m_sBright = num;

		UpdateData( FALSE );

		if ( nSBCode == SB_ENDSCROLL )
		{
			LPVIDEOINFO pvi = GetDriverInfo();
			if ( pvi != NULL ) pvi->motion.thbright = pos;
		} // end if
	} // end if

	else if ( (CSliderCtrl*)pScrollBar == &m_slideMotion )
	{
		if ( nSBCode == SB_ENDSCROLL )
			pos = m_slideMotion.GetPos();
		else m_slideMotion.SetPos( pos );

//		char num[ 64 ];
//		wsprintf( num, "%lu", pos );
//		m_sMotion = num;

		UpdateData( FALSE );

		if ( nSBCode == SB_ENDSCROLL )
		{			
			LPVIDEOINFO pvi = GetDriverInfo();
			if ( pvi != NULL ) pvi->motion.thmotion = pos;
		} // end if
	} // end if
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

LPVIDEOINFO CDlgMotion::GetDriverInfo()
{
	DWORD driver = 0;
	int index = m_comboDriver.GetCurSel();
	if ( index != CB_ERR )
		driver = m_comboDriver.GetItemData( index );

	LPVIDEOINFO pvi = VIDEOLIST().Add( driver );
	if ( pvi == NULL ) return NULL;

	return pvi;
}

void CDlgMotion::OnEnable() 
{
	UpdateData( TRUE );

	// Clear motion data
	SetMotion( 0, 0 );

	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return;

	// Set motion detection enable/disable
	pvi->motion.bEnable = m_bEnable;

	DoEnable();
}

void CDlgMotion::OnSelchangeDriver() 
{
	OnRefresh( 0, 0L );
	
}

void CDlgMotion::OnSelchangeScantype() 
{
	int i = m_comboScanType.GetCurSel();
	if ( i == CB_ERR ) return;
	
	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return;

	// Set scantype
	pvi->motion.scantype = m_comboScanType.GetItemData( i );
}

void CDlgMotion::OnClose() 
{
	FRAME()->SetMotionWnd( NULL );
	
	CDialog::OnClose();
}

LRESULT CDlgMotion::OnMotion(WPARAM wParam, LPARAM lParam)
{
	// Update bright/motion
	LPVIDEOINFO	pvi = (LPVIDEOINFO)lParam;
	if ( pvi != NULL )
	{	m_bright = pvi->motion.sensebright;
		m_motion = pvi->motion.sensemotion;
	} // end if
		
	// Update motion
	SetMotion( m_bright, m_motion );

	// Redraw image
	if ( IsWindow( m_image.GetSafeHwnd() ) )
		m_image.RedrawWindow();

//	PostMessage( WM_REDRAWIMAGE, 0, 0L );

	return 0;
}

void CDlgMotion::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_TXT_SCANTYPE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );
	pWnd = GetDlgItem( IDC_SCANTYPE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );

	pWnd = GetDlgItem( IDC_TXT_MOTION );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );
	pWnd = GetDlgItem( IDC_SLIDE_MOTION );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );
	pWnd = GetDlgItem( IDC_LEVEL_MOTION );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );

	pWnd = GetDlgItem( IDC_TXT_BRIGHT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );
	pWnd = GetDlgItem( IDC_SLIDE_BRIGHT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );
	pWnd = GetDlgItem( IDC_LEVEL_BRIGHT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );

	pWnd = GetDlgItem( IDC_ENABLESOUND );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable );

	pWnd = GetDlgItem( IDC_SOUNDFILE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable && m_bEnableSound );
	pWnd = GetDlgItem( IDC_PLAY );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable && m_bEnableSound );
	pWnd = GetDlgItem( IDC_CHOOSE_SOUND );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnable && m_bEnableSound );
}

LRESULT CDlgMotion::OnRedrawImage(WPARAM wParam, LPARAM lParam)
{
	// Update motion
	SetMotion( m_bright, m_motion );

	// Redraw image
	if ( IsWindow( m_image.GetSafeHwnd() ) )
		m_image.RedrawWindow();

	return 0;
}

void CDlgMotion::OnPlay() 
{
	if ( m_sSoundFile.IsEmpty() ) PLAYSOUND( IDW_ALARM );
	else PLAYSOUND( (LPCTSTR)m_sSoundFile );
}

void CDlgMotion::OnChooseSound() 
{
	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return;

	// Get the file name
	char fname[ CWF_STRSIZE ] = { 0 };
	
	// Start with current sound file
	strcpy_sz( fname, pvi->motionsnd );

	// Get new sound file
	if ( !CWinFile::GetOpenFileName(	fname, "Choose Sound File",
										"Waveform Audio (*.wav)\x0*.wav\x0"
										"All Files (*.*)\x0*.*\x0",
										"wav", GetSafeHwnd() ) ) return;
	// Save the file
	m_sSoundFile = fname;

	// Copy sound file name
	strcpy_sz( pvi->motionsnd, fname );

	UpdateData( FALSE );
}

void CDlgMotion::OnEnablesound() 
{	UpdateData( TRUE ); DoEnable();

	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return;

	if ( m_bEnableSound ) pvi->f1 |= VIF1_MOTIONSOUND;
	else pvi->f1 &= ~VIF1_MOTIONSOUND;
}

void CDlgMotion::OnChangeSoundfile() 
{
	UpdateData( TRUE );

	LPVIDEOINFO pvi = GetDriverInfo();
	if ( pvi == NULL ) return;

	// Default sound?
	if ( m_sSoundFile.IsEmpty() ) *pvi->motionsnd = 0;

	// Save if valid file
	else if ( CWinFile::IsFile( m_sSoundFile ) )
	{	strcpy_sz( pvi->motionsnd, m_sSoundFile ); }

	// No file
	else *pvi->motionsnd = 0;
}
