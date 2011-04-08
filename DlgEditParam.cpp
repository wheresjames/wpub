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
// DlgEditParam.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "DlgEditParam.h"

#include "ColorBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_CTRL1		101
#define IDC_CTRL2		102
#define IDC_CTRL3		103
#define IDC_CTRL4		104

/////////////////////////////////////////////////////////////////////////////
// CDlgEditParam dialog


CDlgEditParam::CDlgEditParam(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditParam::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditParam)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_prop = NULL;
	m_type = 0;
	ZeroMemory( &m_ctrl, sizeof( m_ctrl ) );

	m_hWndCallback = NULL;
	m_uMsgCombo = 0;
	m_uMsgRefresh = 0;

	*m_szPath = 0;
}

CDlgEditParam::~CDlgEditParam()
{
	DeleteControls();
}

void CDlgEditParam::DeleteControls( BOOL bRefresh )
{
	// Refresh if we're dying
	if ( bRefresh && ::IsWindow( GetSafeHwnd() ) )
		Refresh( TRUE, 0 );

	for ( DWORD i = 0; i < 4; i++ )
		if ( m_ctrl[ i ] != NULL )
		{	delete m_ctrl[ i ];
			m_ctrl[ i ] = NULL;
		} // end if

	*m_szPath = 0;
}

void CDlgEditParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditParam)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditParam, CDialog)
	//{{AFX_MSG_MAP(CDlgEditParam)
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP

	ON_EN_CHANGE( IDC_CTRL1, OnCtrl1Changed )
	ON_BN_CLICKED( IDC_CTRL1, OnCtrl1Clicked )
	ON_CBN_SELCHANGE(IDC_CTRL1, OnSelchangeCombo)
	ON_BN_CLICKED( IDC_CTRL2, OnCtrl2Clicked )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditParam message handlers

BOOL CDlgEditParam::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditParam::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	

}

void CDlgEditParam::Size()
{

}

void CDlgEditParam::SetEditProp(LPPROPINFO ppi)
{
	// Save values
	m_prop = ppi;
	m_type = ppi->edit;

	*m_szPath = 0;

	// Must have window
	if ( !::IsWindow( GetSafeHwnd() ) ) return;

	RECT rect;
	GetClientRect( &rect );

	rect.left += 2;
	rect.right -= 2;

	// Lose old controls
	DeleteControls( FALSE );

	switch( m_type )
	{
		case PROPEDIT_BUTTON :
		{
			CButton *pButton = new CButton;
			m_ctrl[ 0 ] = pButton;

			if ( pButton != NULL )
			{
				// Create slider control
				pButton->Create(	CPropList::GetNum( m_prop ) != 0 ? m_prop->on : m_prop->off, 
									WS_VISIBLE, rect, this, IDC_CTRL1 );

				pButton->SetCheck( CPropList::GetNum( m_prop ) != 0 );

			} // end if		
			
		}	break;

		case PROPEDIT_SLIDER :
		{
			RECT ctrl;
			CopyRect( &ctrl, &rect );

			CSliderCtrl *pSlider = new CSliderCtrl;
			m_ctrl[ 0 ] = pSlider;

			if ( pSlider != NULL )
			{
				ctrl.right -= 32;

				// Create slider control
				pSlider->Create( WS_VISIBLE, ctrl, this, IDC_CTRL1 );

				// Set slider range
				pSlider->SetRange( m_prop->min, m_prop->max );

				pSlider->SetPos( (int)CPropList::GetNum( ppi ) );


			} // end if		

			CStatic *pStatic = new CStatic;
			m_ctrl[ 1 ] = pStatic;

			if ( pStatic != NULL )
			{
				ctrl.left = ctrl.right + 1;
				ctrl.right = rect.right;

				char str[ 256 ];
				CPropList::GetStr( ppi, str, sizeof( str ) );

				pStatic->Create( str, WS_VISIBLE, ctrl, this, IDC_CTRL2 );

			} // end if
			
		}	break;

		case PROPEDIT_CHECK :
		{
			CButton *pButton = new CButton;
			m_ctrl[ 0 ] = pButton;

			if ( pButton != NULL )
			{
				// Create slider control
				pButton->Create(	CPropList::GetNum( m_prop ) != 0 ? m_prop->on : m_prop->off, 
									WS_VISIBLE | BS_AUTOCHECKBOX, 
									rect, this, IDC_CTRL1 );

				pButton->SetCheck( CPropList::GetNum( m_prop ) != 0 );

			} // end if		
			
		}	break;

		case PROPEDIT_COMBO :
		case PROPEDIT_COMBOEDIT :
		{
			CComboBox *pCombo = new CComboBox;
			m_ctrl[ 0 ] = pCombo;

			if ( pCombo != NULL )
			{
				RECT combo;
				CopyRect( &combo, &rect );

				combo.bottom += RH( combo ) * 8;

				DWORD style = WS_VISIBLE;

				if ( m_type == PROPEDIT_COMBOEDIT )
					style |= CBS_DROPDOWN | CBS_AUTOHSCROLL;
				else style |= CBS_DROPDOWNLIST;

				// Create slider control
				pCombo->Create(	WS_VSCROLL | style, combo, this, IDC_CTRL1 );

				// Center combo in window
				RECT center;
				CopyRect( &center, &rect );
				long h = pCombo->GetItemHeight( -1 ) + 6;
				if ( h != CB_ERR )
				{	center.bottom = center.top + h;
					CGrDC::Center( &center, &rect );
					pCombo->SetWindowPos( NULL, center.left, center.top, 0, 0,												
												SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
				} // end if				

				// Fill in the combo box
				FillCombo();

				// Set list box width
				SetComboWidth();

				// Select initial item
				if (	m_prop->type == PROPTYPE_DWORD ||
						m_prop->type == PROPTYPE_LONG ||
						m_prop->type == PROPTYPE_RGB ||
						m_prop->type == PROPTYPE_BOOL ||
						m_prop->type == PROPTYPE_BIN ||
						m_prop->type == PROPTYPE_FLAG )
				{
					// Select the correct item
					CComCtrl::ComboSelectItemByData(	pCombo->GetSafeHwnd(), 
														(DWORD)CPropList::GetNum( ppi ) );

				} // end if

				else
				{
					char str[ 256 ];
					CPropList::GetStr( ppi, str, sizeof( str ) );

					// Attempt to select this string
					pCombo->SetWindowText( str );

					// Attempt to set the current selection
					int n = pCombo->FindString( -1, str );
					if ( n != CB_ERR ) pCombo->SetCurSel( n );
				} // end else

			} // end if		

		}	break;
		
		case PROPEDIT_FILE :
		{
			CComboBox *pCombo = new CComboBox;
			m_ctrl[ 0 ] = pCombo;

			RECT ctrl;
			CopyRect( &ctrl, &rect );

			if ( pCombo != NULL )
			{
				ctrl.bottom += RH( ctrl ) * 8;
				ctrl.right -= 26;

				DWORD style = WS_VISIBLE;
				style |= CBS_DROPDOWNLIST;

				// Create slider control
				pCombo->Create(	WS_VSCROLL | style, ctrl, this, IDC_CTRL1 );
								
				// Center combo in window
				RECT center;
				CopyRect( &center, &rect );
				long h = pCombo->GetItemHeight( -1 ) + 6;
				if ( h != CB_ERR )
				{	center.bottom = center.top + h;
					CGrDC::Center( &center, &rect );
					pCombo->SetWindowPos( NULL, center.left, center.top, 0, 0,												
												SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
				} // end if				

				char str[ 256 ];
				CPropList::GetStr( ppi, str, sizeof( str ) );

				// Get starting path
				CWinFile::GetPathFromFileName( str, m_szPath );

				// Fill in file names
				FillFileCombo();

				// Attempt to select this string
				pCombo->SetWindowText( str );

				// Attempt to set the current selection
				int n = pCombo->FindString( -1, str );
				if ( n != CB_ERR ) pCombo->SetCurSel( n );

			} // end if	
			
			CIconButton *pButton = new CIconButton;
			m_ctrl[ 1 ] = pButton;

			if ( pButton != NULL )
			{
				ctrl.left = ctrl.right + 1;
				ctrl.right = rect.right;
				ctrl.bottom = ctrl.top + RW( ctrl );

				rect.left = ctrl.left;
				rect.right = ctrl.right;
				CGrDC::Center( &ctrl, &rect );
				
				// Create slider control
				pButton->Create(	"", WS_VISIBLE, 
									ctrl, this, IDC_CTRL2 );

				pButton->SetIcon( CGrDC::LoadIcon( IDI_DIR_CLOSED ) );

			} // end if


		}	break;

		case PROPEDIT_EDIT :
		{
			CEdit *pEdit = new CEdit;
			m_ctrl[ 0 ] = pEdit;

			if ( pEdit != NULL )
			{
				// Create slider control
				pEdit->Create( WS_VISIBLE | ES_AUTOHSCROLL, rect, this, IDC_CTRL1 );

				char str[ 256 ];
				CPropList::GetStr( ppi, str, sizeof( str ) );

				pEdit->SetWindowText( str );

			} // end if		

		}	break;

		case PROPEDIT_SCROLLEDIT :
			break;

		case PROPEDIT_RGB :
		{
			RECT ctrl;
			CopyRect( &ctrl, &rect );

			CIconButton *pButton = new CIconButton;
			m_ctrl[ 0 ] = pButton;

			if ( pButton != NULL )
			{
				ctrl.left = ctrl.right - 28;

				// Create button control
				pButton->Create(	"", WS_VISIBLE, 
									ctrl, this, IDC_CTRL1 );

				pButton->SetCheck( CPropList::GetNum( m_prop ) != 0 );

				pButton->SetIcon( CGrDC::LoadIcon( IDI_COLOR ) );

			} // end if		

			CColorBox *pColor = new CColorBox;
			m_ctrl[ 1 ] = pColor;

			if ( pColor != NULL )
			{
				ctrl.right = ctrl.left - 1;
				ctrl.left = rect.left + 1;
				InflateRect( &ctrl, -1, -1 );

				// Create slider control
				pColor->Create(	"", WS_VISIBLE | WS_BORDER, ctrl, this, IDC_CTRL2 );

				// Set color
				pColor->SetColor( (COLORREF)CPropList::GetNum( m_prop ) );

			} // end if

		}	break;

		case PROPEDIT_FONT :
		{
			CStatic *pStatic = new CStatic;
			m_ctrl[ 0 ] = pStatic;

			RECT ctrl;
			CopyRect( &ctrl, &rect );

			if ( pStatic != NULL )
			{
				ctrl.top += 2;
				ctrl.right -= 28;

				char str[ 256 ];
				CPropList::GetStr( ppi, str, sizeof( str ) );

				pStatic->Create( str, WS_VISIBLE, ctrl, this, IDC_CTRL1 );

			} // end if	
			
			CIconButton *pButton = new CIconButton;
			m_ctrl[ 1 ] = pButton;

			if ( pButton != NULL )
			{
				ctrl.top = rect.top;
				ctrl.left = ctrl.right + 1;
				ctrl.right = rect.right;
				InflateRect( &ctrl, -1, -1 );
				
				// Create slider control
				pButton->Create( NULL, WS_VISIBLE, ctrl, this, IDC_CTRL2 );

				pButton->SetIcon( CGrDC::LoadIcon( IDI_FONT ) );

			} // end if	

		}	break;

	} // end switch	

	// Set focus to control if needed
	if ( m_ctrl[ 0 ] != NULL ) m_ctrl[ 0 ]->SetFocus();
	
}

BOOL CDlgEditParam::Edit(CWnd *pWnd, LPRECT pRect, LPPROPINFO ppi)
{
	DWORD styles = 0;

	// Lose window if any
	if ( GetSafeHwnd() != NULL ) DestroyWindow();

	// Sanity checks
	if ( pWnd == NULL || pRect == NULL || ppi == NULL ) 
		return FALSE;

	// Adjust rect for combo box
	if (	ppi->edit == PROPEDIT_COMBO || ppi->edit == PROPEDIT_COMBOEDIT ||
			ppi->edit == PROPEDIT_FILE )
	{
		// Get minimum vertical size
//		long miny = GetSystemMetrics( SM_CYVSCROLL ) + 4;
		long miny = 32;

		// Ensure minimum height
		if ( PRH( pRect ) < miny )
		{
			RECT temp;
			CopyRect( &temp, pRect );

			// Adjust height
			pRect->bottom = pRect->top + miny;

			// Center new rect
			CGrDC::Center( pRect, &temp );

			// Add a border
			styles = WS_BORDER;

		} // end if

	} // end if

	if ( !Create( IDD, pWnd ) ) return FALSE;

	// Modify window styles
	ModifyStyle( 0, styles, SWP_NOZORDER | SWP_NOACTIVATE );

	// Set window position
	SetWindowPos( NULL, pRect->left, pRect->top,
						PRW( pRect ), PRH( pRect ),
						SWP_NOZORDER | SWP_NOACTIVATE );

	SetFocus();

	SetEditProp( ppi );

	// Show normal
	ShowWindow( SW_SHOWNORMAL );

	return TRUE;
}

void CDlgEditParam::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);

}

void CDlgEditParam::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
}

void CDlgEditParam::OnDestroy() 
{
	DeleteControls();

	CDialog::OnDestroy();	
}

BOOL CDlgEditParam::DestroyWindow() 
{
	if ( IsWindow( GetSafeHwnd() ) ) 
		ShowWindow( SW_HIDE );

	DeleteControls();

	// Punt if no window
	if ( !IsWindow( GetSafeHwnd() ) ) return FALSE;

	BOOL ret = CDialog::DestroyWindow();

	Refresh( 0, NULL );

	return ret;
}

void CDlgEditParam::OnCtrl1Clicked()
{
	if ( m_prop != NULL && m_type == PROPEDIT_BUTTON )
	{
		CButton *pButton = (CButton*)GetDlgItem( IDC_CTRL1 );
		if ( pButton != NULL ) 
		{
			BOOL bChecked = ( CPropList::GetNum( m_prop ) != 0 ) ? FALSE : TRUE;
			CPropList::SetNum( m_prop, bChecked  );
			pButton->SetWindowText( bChecked ? m_prop->on : m_prop->off );
	
		} // end if

		Refresh( m_prop->fullredraw, m_prop );

	} // end if

	if ( m_prop != NULL && m_type == PROPEDIT_CHECK )
	{
		CButton *pButton = (CButton*)GetDlgItem( IDC_CTRL1 );
		if ( pButton != NULL ) 
		{
			CPropList::SetNum( m_prop, pButton->GetCheck() ? 1 : 0 );
			pButton->SetWindowText( CPropList::GetNum( m_prop ) != 0 ? m_prop->on : m_prop->off );
	
		} // end if

		Refresh( m_prop->fullredraw, m_prop );

	} // end if

	else if ( m_prop != NULL && m_type == PROPEDIT_RGB )
	{
		COLORREF rgb = (COLORREF)CPropList::GetNum( m_prop );

		if ( CWinFile::ChooseColor( &rgb ) )
		{
			CColorBox *pColor = (CColorBox*)GetDlgItem( IDC_CTRL2 );
			if ( pColor != NULL ) pColor->SetColor( rgb );
			CPropList::SetNum( m_prop, (double)rgb );

			Refresh( m_prop->fullredraw, m_prop );
		} // end if

	} // end if
}

void CDlgEditParam::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Verify scroll bar
	if ( pScrollBar == NULL || !IsWindow( pScrollBar->GetSafeHwnd() ) )
		return;

	if ( m_prop != NULL && m_type == PROPEDIT_SLIDER )
	{
		long pos = pScrollBar->GetScrollPos();

		int min = m_prop->min, max = m_prop->max;
//	pScrollBar->GetScrollRange( &min, &max );

		switch( nSBCode )
		{
			case SB_LEFT : pos = min;							break;
			case SB_ENDSCROLL : 
				break;
			case SB_LINELEFT : pos -= 1;						break;
			case SB_LINERIGHT : pos += 1;						break;
			case SB_PAGELEFT : pos -= ( max - min ) / 10;		break;
			case SB_PAGERIGHT : pos += ( max - min ) / 10;		break;
			case SB_RIGHT : pos = max;							break;
			case SB_THUMBPOSITION : pos = long( nPos );			break;
			case SB_THUMBTRACK : pos = long( nPos );			break;
			default : return; break;

		} // end switch
		
		if ( (CSliderCtrl*)pScrollBar == m_ctrl[ 0 ] )
		{
			CSliderCtrl *pSlider = (CSliderCtrl*)pScrollBar;

			if ( nSBCode == SB_ENDSCROLL ) pos = pSlider->GetPos();
			else pSlider->SetPos( pos );

			char num[ 64 ];
			wsprintf( num, "%li", (long)pos );
			m_ctrl[ 1 ]->SetWindowText( num );

			// Set number if end
			if ( nSBCode == SB_ENDSCROLL )
			{
				CPropList::SetNum( m_prop, pos );
				Refresh( m_prop->fullredraw, m_prop );
			} // end if

		} // end if
	} // end if
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgEditParam::OnOK() 
{
	CDialog::OnOK();
	Refresh( 0, NULL );
}

void CDlgEditParam::OnCancel() 
{
	CDialog::OnCancel();
	Refresh( 0, NULL );
}

void CDlgEditParam::OnCtrl1Changed()
{
	if ( m_prop != NULL && m_type == PROPEDIT_EDIT )
	{
		CEdit *pEdit = (CEdit*)m_ctrl[ 0 ];
		if ( pEdit == NULL ) return;

		char str[ CWF_STRSIZE ];
		pEdit->GetWindowText( str, sizeof( str ) );

		CPropList::SetStr( m_prop, str );

//		Refresh( m_prop->fullredraw, m_prop );

	} // end if
}

void CDlgEditParam::SetComboWidth()
{
	if ( m_prop == NULL || 
			(	m_type != PROPEDIT_COMBO && 
				m_type != PROPEDIT_COMBOEDIT &&
				m_type != PROPEDIT_FILE ) )
		return;
	
	CComboBox *pCombo = (CComboBox*)m_ctrl[ 0 ];
	if ( pCombo == NULL ) return;

	CDC* pDC = pCombo->GetDC();

	CSize sz;
	long newWidth = 0;
	long nWidth = pCombo->GetDroppedWidth(); 
	long maxWidth = nWidth * 3;


	CString str;
	for( long i = 0; newWidth < maxWidth && i < pCombo->GetCount();  i++ )
	{
		pCombo->GetLBText( i, str );
		sz = pDC->GetTextExtent( str );

		if( sz.cx > newWidth ) newWidth = sz.cx;

		if ( newWidth > maxWidth ) newWidth = maxWidth;
	}

	// Add allowance for vertical scroll bar and edges
	newWidth += ( GetSystemMetrics( SM_CXVSCROLL ) + 4 + 2 * GetSystemMetrics( SM_CXEDGE ) );

	nWidth = max( nWidth, newWidth );
	pCombo->SetDroppedWidth( nWidth );
	pCombo->ReleaseDC( pDC );

}

BOOL CDlgEditParam::FillCombo()
{
	if ( m_prop == NULL || ( m_type != PROPEDIT_COMBO && m_type != PROPEDIT_COMBOEDIT ) )
		return FALSE;

	// Do we have a combo fillin' window?
	if ( m_hWndCallback == NULL || !IsWindow( m_hWndCallback ) ) return FALSE;

	// Let someone else do the work
	::SendMessage( m_hWndCallback, m_uMsgCombo, (WPARAM)m_prop->user, (LPARAM)m_ctrl[ 0 ] );

	return TRUE;
}

BOOL CDlgEditParam::Refresh( DWORD dwAll, LPPROPINFO ppi )
{
	// Do we have a window? 
	if ( m_hWndCallback == NULL || !IsWindow( m_hWndCallback ) ) return FALSE;

	// Let someone else do the work
	return (BOOL)::SendMessage( m_hWndCallback, m_uMsgRefresh, (WPARAM)dwAll, (LPARAM)ppi );
}


void CDlgEditParam::OnSelchangeCombo()
{
	if ( m_prop == NULL ) return;
	
	CComboBox *pCombo = (CComboBox*)m_ctrl[ 0 ];
	if ( pCombo == NULL ) return;
	
	int n = pCombo->GetCurSel();
	if ( n == CB_ERR ) return;

	if ( m_type == PROPEDIT_COMBO || m_type == PROPEDIT_COMBOEDIT )
	{
		if ( m_prop->type == PROPTYPE_CUSTOM )
		{
			// Do we have a combo fillin' window?
			if ( m_hWndCallback == NULL || !IsWindow( m_hWndCallback ) ) return;

			PROPCALLBACKINFO pci;
			pci.dwUser = m_prop->user;
			pci.pCombo = pCombo;
			pci.pPi = m_prop;
			pci.nSel = n;
			pci.dwData = pCombo->GetItemData( n );

			// Let someone else do the work
			::SendMessage( m_hWndCallback, m_uMsgSetCombo, (WPARAM)m_prop->user, (LPARAM)&pci );

		} // end if


		// Get the data
		else if (	m_prop->type == PROPTYPE_DWORD ||
					m_prop->type == PROPTYPE_LONG ||
					m_prop->type == PROPTYPE_RGB ||
					m_prop->type == PROPTYPE_BOOL ||
					m_prop->type == PROPTYPE_BIN ||
					m_prop->type == PROPTYPE_FLAG )
		{
			DWORD dw = pCombo->GetItemData( n );
			CPropList::SetNum( m_prop, dw );
		} // end if

		else
		{
			CString str;
			pCombo->GetLBText( n, str );

			CPropList::SetStr( m_prop, str );
		} // end else			

	} // end if

	else if ( m_type == PROPEDIT_FILE )
	{
		// Get root
//		char path[ CWF_STRSIZE ];
//		CPropList::GetStr( m_prop, path, sizeof( path ) );
//		CWinFile::GetPathFromFileName( path, path );

		char path[ CWF_STRSIZE ];

		// Get file name
		CString str;
		pCombo->GetLBText( n, str );

		// Check for NULL string
		if ( *str == 0 || !strcmp( str, "< none >" ) ) *path = 0;
		
		// Build full path
		else CWinFile::BuildPath( path, m_szPath, str );

		// Set new file name
		CPropList::SetStr( m_prop, path );

	} // end else if

	Refresh( m_prop->fullredraw, m_prop );
}

void CDlgEditParam::FillFileCombo()
{
	// Sanity check
	if ( m_prop == NULL || m_type != PROPEDIT_FILE )
		return;

	CComboBox *pCombo = (CComboBox*)m_ctrl[ 0 ];
	if ( pCombo == NULL ) return;

	// Empty the combo box
	pCombo->ResetContent();

	// Add NULL string
	pCombo->AddString( "< none >" );

	// Any path?
	if ( *m_szPath == 0 ) return;

	// Get the file name
	char fname[ CWF_STRSIZE ];
	CPropList::GetStr( m_prop, fname, sizeof( fname ) );
	CWinFile::GetFileNameFromPath( fname, fname );
	
	WIN32_FIND_DATA		wfd;
	char				search[ CWF_STRSIZE ];
	HANDLE				hFind;

	// Get first file
	CWinFile::BuildPath( search, m_szPath, "*.*" );

	// Punt if empty
	hFind = FindFirstFile( search, &wfd );
	if ( hFind == INVALID_HANDLE_VALUE ) return;
	
	do
	{
		// Do we need to copy this file?
		if (	*wfd.cFileName != NULL &&
				strcmp( wfd.cFileName, "." ) &&
				strcmp( wfd.cFileName, ".." ) && 
				( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
		{
			DWORD type = CWinImg::GetFileType( wfd.cFileName );

			// Add this string if recognized image type
			if ( type != MAXDWORD )
				pCombo->AddString( wfd.cFileName );

		} // end if

	// Get the next file or directory
	} while ( FindNextFile( hFind, &wfd ) );
	
	// Close the find handle
	FindClose( hFind );

	// Select current file
	pCombo->SelectString( -1, fname );	

	// Set list box width
	SetComboWidth();

}

void CDlgEditParam::OnCtrl2Clicked()
{
	if ( m_prop != NULL && m_type == PROPEDIT_FILE )
	{
		char fname[ CWF_STRSIZE ];

		CPropList::GetStr( m_prop, fname, sizeof( fname ) );

		if ( !CWinFile::GetOpenFileName( fname,	"New File", 
												"All Files (*.*)\x0*.*\x0",
												NULL, GetSafeHwnd() ) ) return;

		// Get the file path
		CWinFile::GetPathFromFileName( fname, m_szPath );

		CPropList::SetStr( m_prop, fname );

		FillFileCombo();
		Refresh( m_prop->fullredraw, m_prop );

	} // end if

	else if ( m_prop != NULL && m_type == PROPEDIT_FONT )
	{
		// Select a new font
		if ( CText::ChooseFont( (LPLOGFONT)m_prop->data, GetSafeHwnd(), TRUE ) )
		{
			if ( m_ctrl[ 0 ] != NULL && ::IsWindow( m_ctrl[ 0 ]->GetSafeHwnd() ) )
				m_ctrl[ 0 ]->SetWindowText( ( (LPLOGFONT)m_prop->data )->lfFaceName );

			Refresh( m_prop->fullredraw, m_prop );
			
		} // end if

	} // end else if
}
