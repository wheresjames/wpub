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
// PgDate.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgDate.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgDate dialog


CPgDate::CPgDate(CWnd* pParent /*=NULL*/)
	: CDialog(CPgDate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgDate)
	m_nGmt = 0;
	m_sSep = _T("");
	m_nStyle = 0;
	//}}AFX_DATA_INIT
}


void CPgDate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgDate)
	DDX_Control(pDX, IDC_FORMAT, m_comboFormat);
	DDX_Radio(pDX, IDC_GMT, m_nGmt);
	DDX_Text(pDX, IDC_SEP, m_sSep);
	DDX_Radio(pDX, IDC_STYLE2, m_nStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgDate, CDialog)
	//{{AFX_MSG_MAP(CPgDate)
	ON_CBN_SELCHANGE(IDC_FORMAT, OnSelchangeFormat)
	ON_BN_CLICKED(IDC_GMT, OnGmt)
	ON_BN_CLICKED(IDC_GMT2, OnGmt2)
	ON_EN_CHANGE(IDC_SEP, OnChangeSep)
	ON_BN_CLICKED(IDC_STYLE2, OnStyle2)
	ON_BN_CLICKED(IDC_STYLE3, OnStyle3)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

// %h = hour 12 hour fixed 2 digits
// %H = hour 12 hour
// %g = hour 24 hour fixed 2 digits
// %G = hour 24 hour
// %m = minute fixed 2 digits
// %G = minute
// %s = second fixed 2 digits
// %S = second
// %a = am/pm
// %A = AM/PM
// %c = Month [01-12] fixed 2 digits
// %C = Month [1-12]
// %d = Day [01-31] fixed 2 digits
// %D = Day [1-31]
// %i = Day of the week [0-6]
// %I = Day of the week [1-7]
// %y = 2 digit year
// %Y = 4 digit year
// %a = am/pm
// %A = AM/PM
// %w = Abbreviated day of week [Sun,Mon,Tue,Wed,Thu,Fri,Sat]
// %W = Day of week [Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday]
// %b = Abbreviated Month [Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec]
// %B = Month [January,February,March,April,May,June,July,August,September,October,November,December]
static char *g_dates[] =
{
	"Sat, Dec 25, 2002", "%w, %b %D, %Y",
	"Saturday, December 25, 2002", "%W, %B %D, %Y",
	"American (12-25-2002)", "%c-%d-%Y",
	"American (12-25-02)", "%c-%d-%y",
	"European (25-12-2002)", "%d-%c-%Y",
	"European (25-12-02)", "%d-%c-%y",
	"Internet (2002.12.25)", "%Y.%c.%d",
	NULL, NULL
};

/////////////////////////////////////////////////////////////////////////////
// CPgDate message handlers

BOOL CPgDate::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Add date tags
	int item = m_comboFormat.AddString( "< custom >" );
	if ( item != CB_ERR ) m_comboFormat.SetItemData( item, MAXDWORD );	
	for ( DWORD x = 0; g_dates[ x << 1 ] != NULL; x ++ )
	{	item = m_comboFormat.AddString( g_dates[ x << 1 ] );
		if ( item != CB_ERR ) m_comboFormat.SetItemData( item, x );
	} // end for

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgDate::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return -1; }

	if ( ( pfx->f1 & FXF1_GMT ) != 0 ) m_nGmt = 1;
	else m_nGmt = 0;

	if ( ( pfx->f1 & FXF1_ANALOGCAL ) != 0 ) m_nStyle = 1;
	else m_nStyle = 0;

	m_sSep = pfx->sep;

	// Set backward compatible date format
	if ( ( pfx->f1 & FXF1_DATEFORM1 ) != 0 ) CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), 0 );
	else if ( ( pfx->f1 & FXF1_DATEFORM2 ) != 0 ) CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), 1 );
	else if ( ( pfx->f1 & FXF1_DATEFORM3 ) != 0 ) CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), 2 );
	else if ( ( pfx->f1 & FXF1_DATEFORM4 ) != 0 ) CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), 3 );
	else if ( ( pfx->f1 & FXF1_DATEFORM5 ) != 0 ) CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), 4 );
	else if ( ( pfx->f1 & FXF1_DATEFORM6 ) != 0 ) CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), 5 );
	else if ( ( pfx->f1 & FXF1_DATEFORM7 ) != 0 ) CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), 6 );

	// Assume no backward compatible needed
	else
	{
		// See if user has selected a common template
		BOOL bSel = FALSE;
		for ( DWORD x = 0; !bSel && g_dates[ ( x << 1 ) + 1 ] != NULL; x ++ )
			if ( !strcmp( pfx->sep, g_dates[ ( x << 1 ) + 1 ] ) )
			{	bSel = TRUE; CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), x ); }

		// Select custom tag
		if ( !bSel ) CComCtrl::ComboSelectItemByData( m_comboFormat.GetSafeHwnd(), MAXDWORD );

	} // end else

	// Clear old date flags
	pfx->f1 &= ~(	FXF1_DATEFORM1 | FXF1_DATEFORM2 | FXF1_DATEFORM3 |
					FXF1_DATEFORM4 | FXF1_DATEFORM5 | FXF1_DATEFORM6 | 
					FXF1_DATEFORM7 );


	UpdateData( FALSE );

	return 0;
}

void CPgDate::OnSelchangeFormat() 
{
	int sel = m_comboFormat.GetCurSel();
	if ( sel == CB_ERR ) return;

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// For backward compatibility
	pfx->f1 &= ~(	 FXF1_DATEFORM1 | FXF1_DATEFORM2 | FXF1_DATEFORM3 |
					 FXF1_DATEFORM4 | FXF1_DATEFORM5 | FXF1_DATEFORM6 |
					 FXF1_DATEFORM7 );

	// Get users format
	DWORD data = m_comboFormat.GetItemData( sel );
	if ( data == MAXDWORD ) return;

	// Find the selection
	BOOL bSel = FALSE;
	for ( DWORD x = 0; !bSel && g_dates[ ( x << 1 ) + 1 ] != NULL; x ++ )
		if ( x == data )
		{	bSel = TRUE; strcpy_sz( pfx->sep, g_dates[ ( x << 1 ) + 1 ] ); }
	if ( !bSel ) return;

	// Update
	m_sSep = pfx->sep;
	UpdateData( FALSE );

	APP()->ReGen();
	CFx::AdjustRect( pfx );
	APP()->ReGen();
}

void CPgDate::OnGmt() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	pfx->f1 &= ~FXF1_GMT;
	APP()->ReGen();
}

void CPgDate::OnGmt2() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	pfx->f1 |= FXF1_GMT;
	APP()->ReGen();
}

void CPgDate::OnChangeSep() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;
	strcpy_sz( pfx->sep, m_sSep );
	APP()->ReGen();
	CFx::AdjustRect( pfx );
}

void CPgDate::OnOK() 
{
}
void CPgDate::OnCancel() 
{
}

void CPgDate::OnStyle2() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	pfx->f1 &= ~FXF1_ANALOGCAL;
	APP()->ReGen();
}

void CPgDate::OnStyle3() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	pfx->f1 |= FXF1_ANALOGCAL;
	APP()->ReGen();
}
