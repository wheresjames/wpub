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
// PgPubThumbnail.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgPubThumbnail.h"

#include "PubThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgPubThumbnail dialog


CPgPubThumbnail::CPgPubThumbnail(CWnd* pParent /*=NULL*/)
	: CDialog(CPgPubThumbnail::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgPubThumbnail)
	m_sThmFile = _T("");
	m_sHeight = _T("");
	m_sPHeight = _T("");
	m_sPWidth = _T("");
	m_sWidth = _T("");
	m_bThumbnail = FALSE;
	m_bRatio = FALSE;
	m_nTnSizing = -1;
	m_bRotate = FALSE;
	m_sFinal = _T("");
	m_bTimestamp = FALSE;
	//}}AFX_DATA_INIT
}


void CPgPubThumbnail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgPubThumbnail)
	DDX_Text(pDX, IDC_TFNAME, m_sThmFile);
	DDX_Text(pDX, IDC_HEIGHT, m_sHeight);
	DDX_Text(pDX, IDC_PHEIGHT, m_sPHeight);
	DDX_Text(pDX, IDC_PWIDTH, m_sPWidth);
	DDX_Text(pDX, IDC_WIDTH, m_sWidth);
	DDX_Check(pDX, IDC_THUMBNAIL, m_bThumbnail);
	DDX_Check(pDX, IDC_RATIO, m_bRatio);
	DDX_Radio(pDX, IDC_TNSIZING, m_nTnSizing);
	DDX_Check(pDX, IDC_ROTATE, m_bRotate);
	DDX_Text(pDX, IDC_FINAL, m_sFinal);
	DDX_Check(pDX, IDC_TIMESTAMP, m_bTimestamp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgPubThumbnail, CDialog)
	//{{AFX_MSG_MAP(CPgPubThumbnail)
	ON_BN_CLICKED(IDC_THUMBNAIL, OnThumbnail)
	ON_BN_CLICKED(IDC_RATIO, OnRatio)
	ON_BN_CLICKED(IDC_TNSIZING, OnTnsizing)
	ON_BN_CLICKED(IDC_TNSIZING2, OnTnsizing2)
	ON_EN_CHANGE(IDC_TFNAME, OnChangeTfname)
	ON_BN_CLICKED(IDC_ROTATE, OnRotate)
	ON_BN_CLICKED(IDC_TIMESTAMP, OnTimestamp)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_PG_SAVE, OnSave )
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgPubThumbnail message handlers

BOOL CPgPubThumbnail::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgPubThumbnail::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	m_sThmFile = "thm_webcam.jpg";

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	// Get the file name
	m_sThmFile = pReg->GetSz( "PUBINFO", "TnFname", "thm_webcam.jpg" );

	DWORD f1 = pReg->GetDword( "PUBINFO", "F1" );

	m_bThumbnail = ( ( f1 & PUBF1_THUMBNAIL ) != 0 );
	m_bRatio = ( ( f1 & PUBF1_FIXEDRATIO ) != 0 );
	m_nTnSizing = ( ( f1 & PUBF1_TNSIZING ) != 0 ) ? 1 : 0;

	m_bRotate = ( ( f1 & PUBF1_TNROTATE ) != 0 );
	m_bTimestamp = ( ( f1 & PUBF1_TNTIMESTAMP ) != 0 );

	m_sWidth = pReg->GetSz( "PUBINFO", "TnWidth", "64" );
	m_sHeight = pReg->GetSz( "PUBINFO", "TnHeight", "48" );
	m_sPWidth = pReg->GetSz( "PUBINFO", "TnPWidth", "25" );
	m_sPHeight = pReg->GetSz( "PUBINFO", "TnPHeight", "25" );

	PreviewFilename();

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CPgPubThumbnail::DoEnable()
{
	UpdateData( TRUE );

	BOOL bRotate = TRUE, bTimestamp = TRUE;

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg != NULL )
	{	bRotate = ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_ROTATEFNAME ) != 0;
		bTimestamp = ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_TIMESTAMP ) != 0;
	} // end if

	CWnd *pWnd = GetDlgItem( IDC_TFNAME );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail );
	pWnd = GetDlgItem( IDC_TFNAME_TEXT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail );

	pWnd = GetDlgItem( IDC_FINAL );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail );

	pWnd = GetDlgItem( IDC_ROTATE );
	if ( pWnd != NULL ) pWnd->EnableWindow( bRotate && m_bThumbnail );
	pWnd = GetDlgItem( IDC_TIMESTAMP );
	if ( pWnd != NULL ) pWnd->EnableWindow( bTimestamp && m_bThumbnail );

	pWnd = GetDlgItem( IDC_RATIO );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail );

	pWnd = GetDlgItem( IDC_TNSIZING );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail );
	pWnd = GetDlgItem( IDC_PWIDTH );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail && m_nTnSizing == 0 );
	pWnd = GetDlgItem( IDC_PHEIGHT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail && m_nTnSizing == 0 && !m_bRatio );
	pWnd = GetDlgItem( IDC_PSIZETEXT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail && m_nTnSizing == 0 && !m_bRatio );
	
	pWnd = GetDlgItem( IDC_TNSIZING2 );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail );
	pWnd = GetDlgItem( IDC_WIDTH );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail && m_nTnSizing == 1 );
	pWnd = GetDlgItem( IDC_HEIGHT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail && m_nTnSizing == 1 && !m_bRatio );
	pWnd = GetDlgItem( IDC_SIZETEXT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bThumbnail && m_nTnSizing == 1 && !m_bRatio );
}

LRESULT CPgPubThumbnail::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	DWORD f1 = pReg->GetDword( "PUBINFO", "F1" );

	if ( m_bThumbnail ) f1 |= PUBF1_THUMBNAIL;
	else f1 &= ~PUBF1_THUMBNAIL;
	if ( m_bRatio ) f1 |= PUBF1_FIXEDRATIO;
	else f1 &= ~PUBF1_FIXEDRATIO;
	if ( m_nTnSizing ) f1 |= PUBF1_TNSIZING;
	else f1 &= ~PUBF1_TNSIZING;

	if ( m_bRotate ) f1 |= PUBF1_TNROTATE;
	else f1 &= ~PUBF1_TNROTATE;
	if ( m_bTimestamp ) f1 |= PUBF1_TNTIMESTAMP;
	else f1 &= ~PUBF1_TNTIMESTAMP;

	pReg->Set( "PUBINFO", "F1", f1 );

	DWORD tnwidth = strtoul( m_sWidth, NULL, 10 );
	if ( tnwidth > 1200 ) tnwidth = 1200;
	DWORD tnheight = strtoul( m_sHeight, NULL, 10 );
	if ( tnheight > 1200 ) tnheight = 1200;
	DWORD tnpwidth = strtoul( m_sPWidth, NULL, 10 );
	if ( tnpwidth > 100 ) tnpwidth = 100;
	DWORD tnpheight = strtoul( m_sPHeight, NULL, 10 );
	if ( tnpheight > 100 ) tnpheight = 100;

	// Fix values
	if ( tnwidth == 0 ) tnwidth = 64;
	if ( tnheight == 0 ) tnheight = 48;
	if ( tnpwidth == 0 ) tnpwidth = 25;
	if ( tnpheight == 0 ) tnpheight = 25;

	pReg->Set( "PUBINFO", "TnWidth", tnwidth );
	pReg->Set( "PUBINFO", "TnHeight", tnheight );
	pReg->Set( "PUBINFO", "TnPWidth", tnpwidth );
	pReg->Set( "PUBINFO", "TnPHeight", tnpheight );

	PreviewFilename();

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CPgPubThumbnail::OnThumbnail() 
{	OnSave(	0, 0L );
}

void CPgPubThumbnail::OnRatio() 
{	OnSave(	0, 0L );
}

void CPgPubThumbnail::OnTnsizing() 
{	OnSave(	0, 0L );
}

void CPgPubThumbnail::OnTnsizing2() 
{	OnSave(	0, 0L );
}

void CPgPubThumbnail::OnChangeTfname() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	UpdateData( TRUE );
	pReg->Set( "PUBINFO", "TnFname", m_sThmFile );

	PreviewFilename();

	UpdateData( FALSE );
}


void CPgPubThumbnail::PreviewFilename()
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	PUBINFO pi;
	ZeroMemory( &pi, sizeof( pi ) );

	// Init data
	pi.f1 = pReg->GetDword( "PUBINFO", "F1" );
	pReg->Get( "PUBINFO", "TnFname", pi.tfname, sizeof( pi.tfname ) );
	pReg->Get( "PUBINFO", "Template", pi.tmpl, sizeof( pi.tmpl ) );
	pi.startindex = pReg->GetDword( "PUBINFO", "StartIndex" );
	pi.stopindex = pReg->GetDword( "PUBINFO", "StopIndex" );
	pi.curindex = pReg->GetDword( "PUBINFO", "CurrentIndex" );

	// Preview the filename
	CPubThread::GetFileName( &pi, TRUE ); 

	// Load the result
	m_sFinal = pi.pub_tfname;
}

void CPgPubThumbnail::OnRotate() 
{
	OnSave( 0, 0L );
}

void CPgPubThumbnail::OnTimestamp() 
{
	OnSave( 0, 0L );
}
