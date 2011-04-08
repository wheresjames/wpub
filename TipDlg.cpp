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
#include "stdafx.h"
#include "resource.h"
#include "TipDlg.h"
// CG: This file added by 'Tip of the Day' component.

//#include <winreg.h>
//#include <sys\stat.h>
//#include <sys\types.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog

#define MAX_BUFLEN 1024

static const TCHAR szSection[] = _T("Tip");
static const TCHAR szIntFilePos[] = _T("FilePos");
static const TCHAR szTimeStamp[] = _T("TimeStamp");
static const TCHAR szIntStartup[] = _T("StartUp");

CTipDlg::CTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TIP, pParent)
{
	//{{AFX_DATA_INIT(CTipDlg)
	m_bStartup = TRUE;
	//}}AFX_DATA_INIT

	// We need to find out what the startup and file position parameters are
	// If startup does not exist, we assume that the Tips on startup is checked TRUE.
	CWinApp* pApp = AfxGetApp();
	m_bStartup = !pApp->GetProfileInt(szSection, szIntStartup, 0);
	m_dwTipPos = (DWORD)pApp->GetProfileInt(szSection, szIntFilePos, 0);

	// Load the license
	if ( CWinFile::LoadResource( MAKEINTRESOURCE( IDR_TIPS ), 
								 NULL, &m_dwSize, "BUFFER", NULL ) && m_dwSize > 0 )
	{
		if ( m_sTips.allocate( m_dwSize + 1 ) )
			if ( !CWinFile::LoadResource(	MAKEINTRESOURCE( IDR_TIPS ), 
											(LPBYTE)m_sTips.ptr(), &m_dwSize, 
											"BUFFER", NULL ) )
			m_sTips.destroy();

	} // end if

	// Ensure we are not overflowing the tip buffer
	if ( m_dwTipPos >= m_dwSize ) m_dwTipPos = 0;

/*
	// Now try to open the tips file
	m_pStream = fopen("tips.txt", "r");
	if (m_pStream == NULL) 
	{
		m_strTip.LoadString(CG_IDS_FILE_ABSENT);
		return;
	} 

	// If the timestamp in the INI file is different from the timestamp of
	// the tips file, then we know that the tips file has been modified
	// Reset the file position to 0 and write the latest timestamp to the
	// ini file
	struct _stat buf;
	_fstat(_fileno(m_pStream), &buf);
	CString strCurrentTime = ctime(&buf.st_ctime);
	strCurrentTime.TrimRight();
	CString strStoredTime = 
		pApp->GetProfileString(szSection, szTimeStamp, NULL);
	if (strCurrentTime != strStoredTime) 
	{
		iFilePos = 0;
		pApp->WriteProfileString(szSection, szTimeStamp, strCurrentTime);
	}

	if (fseek(m_pStream, iFilePos, SEEK_SET) != 0) 
	{
		AfxMessageBox(CG_IDP_FILE_CORRUPT);
	}
	else 
	{
		GetNextTipString(m_strTip);
	}
*/
}

CTipDlg::~CTipDlg()
{
	// This destructor is executed whether the user had pressed the escape key
	// or clicked on the close button. If the user had pressed the escape key,
	// it is still required to update the filepos in the ini file with the 
	// latest position so that we don't repeat the tips! 
  
	// Save tip position
	AfxGetApp()->WriteProfileInt( szSection, szIntFilePos, (int)m_dwTipPos );

/*	// But make sure the tips file existed in the first place....
	if (m_pStream != NULL) 
	{
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(szSection, szIntFilePos, ftell(m_pStream));
		fclose(m_pStream);
	}
*/
}
        
void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTipDlg)
	DDX_Check(pDX, IDC_STARTUP, m_bStartup);
	DDX_Text(pDX, IDC_TIPSTRING, m_strTip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTipDlg, CDialog)
	//{{AFX_MSG_MAP(CTipDlg)
	ON_BN_CLICKED(IDC_NEXTTIP, OnNextTip)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipDlg message handlers

void CTipDlg::OnNextTip()
{
	GetNextTipString(m_strTip);
	UpdateData(FALSE);
}

void CTipDlg::GetNextTipString(CString& strNext)
{
	LPTSTR lpsz = strNext.GetBuffer(MAX_BUFLEN);

	DWORD i = 0;

	// Skip white space
	while ( m_dwTipPos < m_dwSize && m_sTips[ m_dwTipPos ] != 0 && 
			( m_sTips[ m_dwTipPos ] <= ' ' || m_sTips[ m_dwTipPos ] > '~' ) )
		m_dwTipPos++;

	// Reset tip position if needed
	if ( m_dwTipPos >= m_dwSize || m_sTips[ m_dwTipPos ] == 0 )
		m_dwTipPos = 0;

	// Read in next tip
	while(	m_dwTipPos < m_dwSize &&
			m_sTips[ m_dwTipPos ] != 0 && 
			m_sTips[ m_dwTipPos ] != '\r' &&
			m_sTips[ m_dwTipPos ] != '\n' )
		if ( m_sTips[ m_dwTipPos ] >= ' ' && m_sTips[ m_dwTipPos ] <= '~' )
			lpsz[ i++ ] = m_sTips[ m_dwTipPos++ ];
		else m_dwTipPos++;
	lpsz[ i ] = 0;

/*
	// This routine identifies the next string that needs to be
	// read from the tips file
	BOOL bStop = FALSE;
	while (!bStop) 
	{
		if (_fgetts(lpsz, MAX_BUFLEN, m_pStream) == NULL) 
		{
			// We have either reached EOF or enocuntered some problem
			// In both cases reset the pointer to the beginning of the file
			// This behavior is same as VC++ Tips file
			if (fseek(m_pStream, 0, SEEK_SET) != 0) 
				AfxMessageBox(CG_IDP_FILE_CORRUPT);
		} 
		else 
		{
			if (*lpsz != ' ' && *lpsz != '\t' && 
				*lpsz != '\n' && *lpsz != ';') 
			{
				// There should be no space at the beginning of the tip
				// This behavior is same as VC++ Tips file
				// Comment lines are ignored and they start with a semicolon
				bStop = TRUE;
			}
		}
	}

*/
	strNext.ReleaseBuffer();
}

HBRUSH CTipDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd->GetDlgCtrlID() == IDC_TIPSTRING)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CTipDlg::OnOK()
{
	CDialog::OnOK();
	
    // Update the startup information stored in the INI file
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(szSection, szIntStartup, !m_bStartup);
}

BOOL CTipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// If Tips file does not exist then disable NextTip
	if ( m_sTips == NULL )
		GetDlgItem(IDC_NEXTTIP)->EnableWindow(FALSE);
	else
	{
		GetNextTipString(m_strTip);
		UpdateData(FALSE);
	} // end else

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CTipDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Get paint area for the big static control
//	CWnd* pStatic = GetDlgItem(IDC_BULB);
	CWnd* pFrame = GetDlgItem( IDC_TIPFRAME );
	CRect rect;
//	pStatic->GetWindowRect(&rect);
	pFrame->GetWindowRect( &rect );
	ScreenToClient(&rect);

	// Paint the background white.
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	dc.FillRect(rect, &brush);

	// Load bitmap and get dimensions of the bitmap
	CBitmap bmp;
	bmp.LoadBitmap(IDB_LIGHTBULB);
	BITMAP bmpInfo;
	bmp.GetBitmap(&bmpInfo);

	// Draw the dark gray band on the side
	CRect left;
	CopyRect( left, rect );
	left.right = bmpInfo.bmWidth + 30;
	brush.CreateStockObject(LTGRAY_BRUSH);
	dc.FillRect(left, &brush);

	// Draw bitmap in top corner and validate only top portion of window
//	CDC dcTmp;
//	dcTmp.CreateCompatibleDC(&dc);
//	dcTmp.SelectObject(&bmp);
	rect.bottom = bmpInfo.bmHeight + rect.top;
	HICON hIcon = AfxGetApp()->LoadIcon( IDI_LIGHTBULB );
	if ( hIcon != NULL )
		DrawIconEx( dc.GetSafeHdc(), rect.left + 10, rect.top + 10, 
					hIcon, 0, 0, NULL, NULL, DI_NORMAL );
//	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), 
//		&dcTmp, 0, 0, SRCCOPY);

	int mode = dc.GetBkMode();
	dc.SetBkMode( TRANSPARENT );

	// Draw out "Did you know..." message next to the bitmap
	CString strMessage;
	strMessage.LoadString(CG_IDS_DIDYOUKNOW);
	rect.left += bmpInfo.bmWidth + 20;
	COLORREF old = dc.SetTextColor( RGB( 200, 200, 200 ) );
	OffsetRect( &rect, 2, 2 );
	dc.DrawText(strMessage, rect, DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor( RGB( 0, 0, 255 ) );
	OffsetRect( &rect, -2, -2 );
	dc.DrawText(strMessage, rect, DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor( old );

	dc.SetBkMode( mode );

	// Do not call CDialog::OnPaint() for painting messages
}

