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
// ChooseDriver.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "ChooseDriverDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseDriverDlg dialog


CChooseDriverDlg::CChooseDriverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseDriverDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseDriverDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChooseDriverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseDriverDlg)
	DDX_Control(pDX, IDC_DRIVERS, m_listDrivers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseDriverDlg, CDialog)
	//{{AFX_MSG_MAP(CChooseDriverDlg)
	ON_LBN_DBLCLK(IDC_DRIVERS, OnDblclkDrivers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseDriverDlg message handlers

BOOL CChooseDriverDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillInDrivers();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseDriverDlg::FillInDrivers()
{
	m_listDrivers.ResetContent();

	CCapture	cap;
	DWORD		drivers = cap.GetNumDrivers();

	for ( DWORD i = 0; i < drivers; i++ )
	{
		char				show[ CWF_STRSIZE ];
		CAPTUREDRIVERINFO	ci;

		if ( cap.GetDriverInfo( i, &ci ) )
		{
			sprintf( show, "%s (%s)", ci.szName, ci.szVer );

			int index = m_listDrivers.AddString( show );
			if ( index != LB_ERR ) m_listDrivers.SetItemData( index, ci.index );
		} // end if

	} // end for
}

void CChooseDriverDlg::OnOK() 
{	  
	BOOL	selected = FALSE;
	DWORD	num = (DWORD)m_listDrivers.GetCount();
	if ( num == 0 ) return;

	for ( DWORD i = 0; i < num; i++ )
		if ( m_listDrivers.GetSel( i ) != 0 )
		{
			m_dwDriverIndex = m_listDrivers.GetItemData( i );
			if ( m_dwDriverIndex != LB_ERR ) 
			{
				selected = TRUE;
				i = num;
			} // end if	
		} // end if

	if ( !selected ) return;
	
	CDialog::OnOK();
}

void CChooseDriverDlg::OnDblclkDrivers() 
{
	UpdateData( TRUE );

	OnOK();	
}
