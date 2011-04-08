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
// DlgMapImage.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgMapImage.h"

#include "PubImgList.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMapImage dialog


CDlgMapImage::CDlgMapImage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapImage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMapImage)
	m_sMapTo = _T("");
	//}}AFX_DATA_INIT

	m_sImage = "";
}


void CDlgMapImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMapImage)
	DDX_Control(pDX, IDC_IMAGES, m_comboImages);
	DDX_Text(pDX, IDC_MAPTO, m_sMapTo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMapImage, CDialog)
	//{{AFX_MSG_MAP(CDlgMapImage)
	ON_CBN_SELCHANGE(IDC_IMAGES, OnSelchangeImages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMapImage message handlers

BOOL CDlgMapImage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillCombo();

	// Select image name
	if ( !m_sImage.IsEmpty() ) 
		m_comboImages.SelectString( -1, m_sImage );

	// Fix edit name if needed
	if ( m_sMapTo.IsEmpty() ) FixEdit();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMapImage::FillCombo()
{
	m_comboImages.ResetContent();

	DWORD index = 0;
	LPPUBIMGINFO ppii = NULL;
	while ( ( ppii = (LPPUBIMGINFO)IMGLIST().GetNext( ppii ) ) != NULL )
	{
		int i = m_comboImages.AddString( ppii->name );
		if ( i != CB_ERR ) m_comboImages.SetItemData( i, index++ );
	} // end while

	// Select the first image
	m_comboImages.SetCurSel( 0 );
}

void CDlgMapImage::OnOK() 
{
	UpdateData( TRUE );

	// Get the current selection
	int sel = m_comboImages.GetCurSel();
	if ( sel == CB_ERR ) return;

	// Get the item index
	m_comboImages.GetLBText( sel, m_sImage );
	
	CDialog::OnOK();
}

void CDlgMapImage::FixEdit()
{
	UpdateData( TRUE );

	// Get the current selection
	int sel = m_comboImages.GetCurSel();
	if ( sel == CB_ERR ) return;

	char text[ CWF_STRSIZE ];
	m_comboImages.GetLBText( sel, text );

	char fname[ CWF_STRSIZE ] = { 0 };

	DWORD x = 0;
	for ( DWORD i = 0; text[ i ] != 0 && i < sizeof( fname ); i++ )
		if ( text[ i ] > ' ' && text[ i ] <= '~' )
			fname[ x++ ] = text[ i ];
	fname[ x ] = 0;
	strcat( fname, ".jpg" );
	CWinFile::CleanChars( fname );

	// Set the new name
	m_sMapTo = fname;

	UpdateData( FALSE );
}

void CDlgMapImage::OnSelchangeImages() 
{
	FixEdit();
}
