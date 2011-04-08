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
// WzEmailMsgInfo.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzEmailMsgInfo.h"

#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzEmailMsgInfo dialog


CWzEmailMsgInfo::CWzEmailMsgInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CWzEmailMsgInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzEmailMsgInfo)
	m_sFrom = _T("");
	m_sMsg = _T("");
	m_sSubject = _T("");
	m_sTo = _T("");
	//}}AFX_DATA_INIT
}


void CWzEmailMsgInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzEmailMsgInfo)
	DDX_CBString(pDX, IDC_FROM, m_sFrom);
	DDX_Text(pDX, IDC_MSG, m_sMsg);
	DDX_Text(pDX, IDC_SUBJECT, m_sSubject);
	DDX_CBString(pDX, IDC_TO, m_sTo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzEmailMsgInfo, CDialog)
	//{{AFX_MSG_MAP(CWzEmailMsgInfo)
	//}}AFX_MSG_MAP
	ON_MESSAGE( CWZ_PAGECHG, OnPageChange )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzEmailMsgInfo message handlers

BOOL CWzEmailMsgInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzEmailMsgInfo::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get pointer to reg
	CReg *pReg = (CReg*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	m_sTo = pReg->GetSz( "PUBINFO", "To" );
	m_sFrom = pReg->GetSz( "PUBINFO", "From" );
	m_sSubject = pReg->GetSz( "PUBINFO", "Subject" );
	m_sMsg = pReg->GetSz( "PUBINFO", "Msg" );

	if ( m_sSubject.IsEmpty() ) m_sSubject = "Webcam Image";

	UpdateData( FALSE );

	return 1;
}

LRESULT CWzEmailMsgInfo::OnPageChange(WPARAM wParam, LPARAM lParam)
{
	// Get pointer to reg
	CReg *pReg = (CReg*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return 0;

	UpdateData( TRUE );

	pReg->Set( "PUBINFO", "To", m_sTo );
	pReg->Set( "PUBINFO", "From", m_sFrom );
	pReg->Set( "PUBINFO", "Subject", m_sSubject );
	pReg->Set( "PUBINFO", "Msg", m_sMsg );

	if ( wParam != 1 ) return 0;

	if ( m_sTo.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "To field cannot be blank",
		"Blank Field", MB_OK | MB_ICONSTOP ); return 1; }

	if ( m_sFrom.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "To field cannot be blank",
		"Blank Field", MB_OK | MB_ICONSTOP ); return 1; }
	
	if ( m_sSubject.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "To field cannot be blank",
		"Blank Field", MB_OK | MB_ICONSTOP ); return 1; }

	return 0;
}
