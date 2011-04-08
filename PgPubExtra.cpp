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
// PgPubExtra.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgPubExtra.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgPubExtra dialog


CPgPubExtra::CPgPubExtra(CWnd* pParent /*=NULL*/)
	: CDialog(CPgPubExtra::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgPubExtra)
	m_sCapFrames = _T("");
	m_sCapSeconds = _T("");
	m_sPbFrames = _T("");
	m_sPbSeconds = _T("");
	m_bCompress = FALSE;
	m_bCapMotion = FALSE;
	//}}AFX_DATA_INIT
}


void CPgPubExtra::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgPubExtra)
	DDX_Control(pDX, IDC_CODEC, m_comboCodec);
	DDX_Control(pDX, IDC_SLIDE_PB_SCALE, m_slidePbScale);
	DDX_Control(pDX, IDC_SLIDE_CAP_SCALE, m_slideCapScale);
	DDX_Text(pDX, IDC_CAP_FRAMES, m_sCapFrames);
	DDX_Text(pDX, IDC_CAP_SECONDS, m_sCapSeconds);
	DDX_Text(pDX, IDC_PB_FRAMES, m_sPbFrames);
	DDX_Text(pDX, IDC_PB_SECONDS, m_sPbSeconds);
	DDX_Check(pDX, IDC_COMPRESS, m_bCompress);
	DDX_Check(pDX, IDC_CAPMOTION, m_bCapMotion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgPubExtra, CDialog)
	
	//{{AFX_MSG_MAP(CPgPubExtra)
	ON_EN_CHANGE(IDC_CAP_FRAMES, OnChangeCapFrames)
	ON_EN_CHANGE(IDC_CAP_SECONDS, OnChangeCapSeconds)
	ON_EN_CHANGE(IDC_PB_FRAMES, OnChangePbFrames)
	ON_EN_CHANGE(IDC_PB_SECONDS, OnChangePbSeconds)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_CBN_SELCHANGE(IDC_CODEC, OnSelchangeCodec)
	ON_BN_CLICKED(IDC_COMPRESS, OnCompress)
	ON_BN_CLICKED(IDC_CAPMOTION, OnCapmotion)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgPubExtra message handlers

BOOL CPgPubExtra::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_slideCapScale.SetRange( -60, 60 );
	m_slidePbScale.SetRange( -60, 60 );

	FillCodecs();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgPubExtra::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg != NULL ) 
	{
		m_sCapFrames = pReg->GetSz( "PUBINFO", "AviCapFrames", "1" );
		m_sCapSeconds = pReg->GetSz( "PUBINFO", "AviCapSeconds", "1" );
		m_sPbFrames = pReg->GetSz( "PUBINFO", "AviPbFrames", "8" );
		m_sPbSeconds = pReg->GetSz( "PUBINFO", "AviPbSeconds", "1" );

		m_bCompress = ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_COMPRESSVIDEO ) != 0;
		m_bCapMotion = ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_AVICAPMOTION ) != 0;
		SelectCodec( pReg->GetDword( "PUBINFO", "fourCC" ) );

	} 
	else
	{
		m_sCapFrames = "1";
		m_sCapSeconds = "1";
		m_sPbFrames = "8";
		m_sPbSeconds = "1";
		m_bCompress = TRUE;
		SelectCodec( mmioFOURCC( 'm', 'p', 'g', '4' ) );

	} // end else

	UpdateData( FALSE );

	// Set capture slider position
	m_slideCapScale.SetPos( GetSlidePosition( strtol( m_sCapFrames, NULL, 10 ),
											  strtol( m_sCapSeconds, NULL, 10 ) ) );

	// Set playback slider position
	m_slidePbScale.SetPos( GetSlidePosition( strtol( m_sPbFrames, NULL, 10 ),
											 strtol( m_sPbSeconds, NULL, 10 ) ) );

	// Select new codec
	OnSelchangeCodec();

	DoEnable();

	// Redraw sliders
	m_slideCapScale.RedrawWindow();
	m_slidePbScale.RedrawWindow();

	return 0;
}

LRESULT CPgPubExtra::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	pReg->Set( "PUBINFO", "AviCapFrames", strtoul( m_sCapFrames, NULL, 10 ) );
	pReg->Set( "PUBINFO", "AviCapSeconds", strtoul( m_sCapSeconds, NULL, 10 ) );
	pReg->Set( "PUBINFO", "AviPbFrames", strtoul( m_sPbFrames, NULL, 10 ) );
	pReg->Set( "PUBINFO", "AviPbSeconds", strtoul( m_sPbSeconds, NULL, 10 ) );

	// Save play sound status
	DWORD f1 = pReg->GetDword( "PUBINFO", "F1" );
	if ( m_bCompress ) f1 |= PUBF1_COMPRESSVIDEO;
	else f1 &= ~PUBF1_COMPRESSVIDEO;
	if ( m_bCapMotion ) f1 |= PUBF1_AVICAPMOTION;
	else f1 &= ~PUBF1_AVICAPMOTION;
	pReg->Set( "PUBINFO", "F1", f1 );

	return 0;
}

void CPgPubExtra::DoEnable()
{		  
	CWnd *pCtrl = GetDlgItem( IDC_CODEC );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bCompress );
	pCtrl = GetDlgItem( IDC_SETTINGS );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bCompress );

	pCtrl = GetDlgItem( IDC_SLIDE_CAP_SCALE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( !m_bCapMotion );
	pCtrl = GetDlgItem( IDC_TXT_CAPSLOWER );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( !m_bCapMotion );
	pCtrl = GetDlgItem( IDC_TXT_CAPFASTER );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( !m_bCapMotion );
	pCtrl = GetDlgItem( IDC_TXT_CAPFRAMES );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( !m_bCapMotion );
	pCtrl = GetDlgItem( IDC_TXT_CAPSECONDS );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( !m_bCapMotion );
	pCtrl = GetDlgItem( IDC_CAP_FRAMES );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( !m_bCapMotion );
	pCtrl = GetDlgItem( IDC_TXT_CAPPER );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( !m_bCapMotion );
	pCtrl = GetDlgItem( IDC_CAP_SECONDS );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( !m_bCapMotion );

}

void CPgPubExtra::OnChangeCapFrames() 
{	OnSave( 0, 0L );
}

void CPgPubExtra::OnChangeCapSeconds() 
{	OnSave( 0, 0L );
}

void CPgPubExtra::OnChangePbFrames() 
{	OnSave( 0, 0L );
}

void CPgPubExtra::OnChangePbSeconds() 
{	OnSave( 0, 0L );
}

void CPgPubExtra::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ( pScrollBar == (CScrollBar*)&m_slideCapScale )
	{
		char num[ 64 ];
		long pos = m_slideCapScale.GetPos();
		if ( pos == 0 ) pos = 1;
		if ( pos < 0 )
		{	wsprintf( num, "%lu", (DWORD)( -pos ) );
			m_sCapFrames = "1";
			m_sCapSeconds = num;
		} // end if
		else
		{	wsprintf( num, "%lu", (DWORD)pos );
			m_sCapFrames = num;
			m_sCapSeconds = "1";
		} // end if
		
		UpdateData( FALSE );
		OnSave( 0, 0L );

	} // end if

	else if ( pScrollBar == (CScrollBar*)&m_slidePbScale )
	{
		char num[ 64 ];
		long pos = m_slidePbScale.GetPos();
		if ( pos == 0 ) pos = 1;
		if ( pos < 0 )
		{	wsprintf( num, "%lu", (DWORD)( -pos ) );
			m_sPbFrames = "1";
			m_sPbSeconds = num;
		} // end if
		else
		{	wsprintf( num, "%lu", (DWORD)pos );
			m_sPbFrames = num;
			m_sPbSeconds = "1";
		} // end if

		UpdateData( FALSE );
		OnSave( 0, 0L );

	} // end if
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

double CPgPubExtra::GetFrameRate(long frames, long seconds)
{
	if ( frames == 0 ) frames = 1;
	if ( seconds == 1 ) seconds = 1;
	return (double)frames / (double)seconds;
}

long CPgPubExtra::GetSlidePosition(long frames, long seconds)
{
	double fr = GetFrameRate( frames, seconds );

	// Convert to negative slide if less than one
	if ( fr < 1 ) fr = -( (double)1 / fr );

	return (long)fr;
}

void CPgPubExtra::FillCodecs()
{
	m_comboCodec.ResetContent();
	m_codecs.Destroy();

	BITMAPINFOHEADER bih; 
	ZeroMemory( &bih, sizeof( bih ) );
	bih.biSize = sizeof( bih );
	bih.biPlanes = 1; 
	bih.biCompression = BI_RGB;
	bih.biBitCount = 24;
	
	ICINFO ii;
	ZeroMemory( &ii, sizeof( ii ) );
	ii.dwSize = sizeof( ii );
	
	for ( DWORD i = 0; ICInfo( ICTYPE_VIDEO, i, &ii ); i++ )
	{
		// This one is valid
		DWORD fourCC = ii.fccHandler;

		HIC hIc = ICOpen( ii.fccType, ii.fccHandler, ICMODE_QUERY );
		if ( hIc != NULL )
		{
			// Ensure it supports our video type
			if (	ii.fccType == ICTYPE_VIDEO && 
//					ICCompressQuery( hIc, &bih, NULL ) == ICERR_OK &&
					ICGetInfo( hIc, &ii, sizeof( ii ) ) )
			{
				char str[ 256 ] = { 0 };
				wcstombs( str, ii.szDescription, sizeof( str ) );

				// Save codec info
				LPCODECINFO pci = (LPCODECINFO)m_codecs.NewObj( sizeof( CODECINFO ), NULL, i, str );
				if ( pci != NULL )
				{
					// Save data
					pci->bConfig = ICQueryConfigure( hIc ) != ICERR_OK; // Backwards???
					pci->fourCC = fourCC;

					// Get settings
					int item = m_comboCodec.AddString( str );
					if ( item != CB_ERR ) m_comboCodec.SetItemData( item, (DWORD)pci );

				} // end if
					
			} // end if

			ICClose( hIc ); 

		} // end if

		// Next codec
		ZeroMemory( &ii, sizeof( ii ) );
		ii.dwSize = sizeof( ii );
		i++;

	} // end while

}

void CPgPubExtra::OnSettings() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;
	CRKey *pRk = pReg->FindKey( "PUBINFO" );
	if ( pRk == NULL ) return;

	int sel = m_comboCodec.GetCurSel();
	if ( sel == CB_ERR ) return;

	// Get data pointer
	LPCODECINFO pci = (LPCODECINFO)m_comboCodec.GetItemData( sel );
	if ( pci == NULL || !m_codecs.VerifyPointer( pci ) ) return;

	// Attempt to open settings dialog
	HIC hIc = ICOpen( ICTYPE_VIDEO, pci->fourCC, ICMODE_QUERY );
	if ( hIc != NULL )
	{
		DWORD size = pRk->GetValueSize( "CodecData" );
		LPBYTE buf = (LPBYTE)pRk->GetValuePtr( "CodecData" );

		// Restore settings
		if ( size > 0 && buf != NULL )
			ICSetState( hIc, buf, size );

		// Configure the compressor
		if ( ICConfigure( hIc, GetSafeHwnd() ) != ICERR_OK )
			return;

		size = ICGetStateSize( hIc );
		if ( size > 0 )
		{
			TMem< BYTE > mem;
			if ( mem.allocate( size ) )
				if ( ICGetState( hIc, mem.ptr(), size ) == ICERR_OK )
					pRk->Set( "CodecData", mem.ptr(), size );

		} // end if

		ICClose( hIc ); 

	} // end if
	
}

void CPgPubExtra::OnSelchangeCodec() 
{
	CWnd *pSettings = GetDlgItem( IDC_SETTINGS );

	int sel = m_comboCodec.GetCurSel();
	if ( sel == CB_ERR ) { pSettings->EnableWindow( FALSE ); return; }

	// Get data pointer
	LPCODECINFO pci = (LPCODECINFO)m_comboCodec.GetItemData( sel );
	if ( pci == NULL || !m_codecs.VerifyPointer( pci ) ) 
	{	pSettings->EnableWindow( FALSE ); return; }

	// Do we have a configuration ability
	pSettings->EnableWindow( pci->bConfig );	

	// Save codec code
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg != NULL )
	{	pReg->Set( "PUBINFO", "fourCC", pci->fourCC );
		pReg->DeleteValue( "PUBINFO", "CodecData" );
	} // end if
}

void CPgPubExtra::SelectCodec(DWORD fourCC)
{
	int count = m_comboCodec.GetCount();
	if ( count == 0 ) return;

	for ( int i = 0; i < count; i++ )
	{
		// Get object pointer
		LPCODECINFO pci = (LPCODECINFO)m_comboCodec.GetItemData( i );
		if ( pci != NULL && m_codecs.VerifyPointer( pci ) ) 

			// Is this our
			if ( pci->fourCC == fourCC ) 
			{	m_comboCodec.SetCurSel( i ); return; }

	} // end for

	// Couldn't find codec
	m_comboCodec.SetCurSel( -1 );
}

void CPgPubExtra::OnCompress() 
{	UpdateData( TRUE ); DoEnable(); OnSave( 0, 0L );
}

void CPgPubExtra::OnCapmotion() 
{	UpdateData( TRUE ); DoEnable(); OnSave( 0, 0L );
}
