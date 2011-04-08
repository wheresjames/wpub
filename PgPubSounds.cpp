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
// PgPubSounds.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgPubSounds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgPubSounds dialog


CPgPubSounds::CPgPubSounds(CWnd* pParent /*=NULL*/)
	: CDialog(CPgPubSounds::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgPubSounds)
	m_sSoundFile = _T("");
	m_bEnableSound = FALSE;
	//}}AFX_DATA_INIT
}


void CPgPubSounds::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgPubSounds)
	DDX_Control(pDX, IDC_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_CHOOSE_SOUND, m_btnChooseSound);
	DDX_Text(pDX, IDC_SOUNDFILE, m_sSoundFile);
	DDX_Check(pDX, IDC_ENABLESOUND, m_bEnableSound);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgPubSounds, CDialog)
	//{{AFX_MSG_MAP(CPgPubSounds)
	ON_BN_CLICKED(IDC_CHOOSE_SOUND, OnChooseSound)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_EN_CHANGE(IDC_SOUNDFILE, OnChangeSoundfile)
	ON_BN_CLICKED(IDC_ENABLESOUND, OnEnablesound)
	//}}AFX_MSG_MAP
	
	ON_MESSAGE( WM_PG_SAVE, OnSave )
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgPubSounds message handlers

BOOL CPgPubSounds::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Set button icons
	m_btnPlay.SetIcon( CGrDC::LoadIcon( IDI_DS_TICK_RIGHT ) );
	m_btnChooseSound.SetIcon( CGrDC::LoadIcon( IDI_FOLDER ) );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgPubSounds::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	m_bEnableSound = ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_PLAYSOUND ) != 0;
	m_sSoundFile = pReg->GetSz( "PUBINFO", "Snd" );

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CPgPubSounds::OnChooseSound() 
{
	// Get the file name
	char fname[ CWF_STRSIZE ] = { 0 };
	
	// Start with current sound file
	strcpy_sz( fname, m_sSoundFile );

	// Get new sound file
	if ( !CWinFile::GetOpenFileName(	fname, "Choose Sound File",
										"Waveform Audio (*.wav)\x0*.wav\x0"
										"All Files (*.*)\x0*.*\x0",
										"wav", GetSafeHwnd() ) ) return;
	// Save the file
	m_sSoundFile = fname;

	UpdateData( FALSE );

	OnSave( 0, 0L );
}

void CPgPubSounds::OnPlay() 
{
	if ( m_sSoundFile.IsEmpty() ) PLAYSOUND( IDW_CAMERA );
	else PLAYSOUND( (LPCTSTR)m_sSoundFile );
}

LRESULT CPgPubSounds::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	// Save play sound status
	DWORD f1 = pReg->GetDword( "PUBINFO", "F1" );
	if ( m_bEnableSound ) f1 |= PUBF1_PLAYSOUND;
	else f1 &= ~PUBF1_PLAYSOUND;
	pReg->Set( "PUBINFO", "F1", f1 );

	pReg->Set( "PUBINFO", "Snd", m_sSoundFile );

	return 0;
}

void CPgPubSounds::DoEnable()
{
	CWnd *pWnd = GetDlgItem( IDC_SOUNDFILE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnableSound );
	pWnd = GetDlgItem( IDC_PLAY );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnableSound );
	pWnd = GetDlgItem( IDC_CHOOSE_SOUND );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bEnableSound );
}

void CPgPubSounds::OnChangeSoundfile() 
{
	UpdateData( TRUE );

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	// Default sound?
	if ( m_sSoundFile.IsEmpty() ) pReg->Set( "PUBINFO", "Snd", "" );

	// Save if valid file
	else if ( CWinFile::IsFile( m_sSoundFile ) )
		pReg->Set( "PUBINFO", "Snd", m_sSoundFile );

	// No file
	else pReg->Set( "PUBINFO", "Snd", "" );
}

void CPgPubSounds::OnEnablesound() 
{	UpdateData( TRUE ); OnSave( 0, 0L ); DoEnable();
}
