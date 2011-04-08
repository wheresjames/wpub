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
#if !defined(AFX_DLGIMGVIEW_H__B709BF9F_784B_4C89_8308_D73EF6146D4B__INCLUDED_)
#define AFX_DLGIMGVIEW_H__B709BF9F_784B_4C89_8308_D73EF6146D4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImgView.h : header file
//

#include "ShowImg.h"
#include "SlantToolbar.h"
#include "WinMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgImgView dialog

class CDlgImgView : public CDialog
{
// Construction
public:
	LRESULT OnToolbarUpdate( WPARAM wParam, LPARAM lParam );
	LRESULT OnToolbarCmd( WPARAM wParam, LPARAM lParam );
	void Size();
	CDlgImgView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImgView)
	enum { IDD = IDD_IMGVIEW };
	CShowImg	m_img;
	CSlantToolbar	m_tb;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImgView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImgView)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEffectsFliphorizontally();
	afx_msg void OnEffectsFlipvertically();
	afx_msg void OnEffectsGrayscale();
	afx_msg void OnEffectsRotateleft();
	afx_msg void OnEffectsRotateright();
	afx_msg void OnEffectsNegative();
	afx_msg void OnEffectsLighten();
	afx_msg void OnEffectsDarken();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnOK(){ Shutdown(); }
	virtual void OnCancel(){ Shutdown(); }

public:
	BOOL SaveImg();
	BOOL RestorePosition();
	void Shutdown();
	void SizeDlg();
	BOOL EnableCapture( BOOL enable );
	BOOL ShowFxMenu( long x1, long y1, long x2, long y2 );

	BOOL LoadFile( LPCTSTR pFile ) { return m_img.LoadFile( pFile ); }
	void SetTitle( LPCTSTR pTitle ) 
	{	m_tb.SetTitle( pTitle ); 
		if ( ::IsWindow( GetSafeHwnd() ) )
		{	char fname[ CWF_STRSIZE ];
			CWinFile::GetFileNameFromPath( pTitle, fname );
			SetWindowText( fname );
		} // end if
	}

	void SetShowDc( CWinDc *pdc ) 
	{	m_img.SetShowDc( pdc ); Size(); }

	// Update the image
	void UpdateImg( CWinDc *pdc ) 
	{	if ( !::IsWindow( GetSafeHwnd() ) || !m_bCapture ) return;
		if ( pdc != NULL )
		{	m_img.SetShowDc( pdc );  
			RECT rect;
			m_img.GetClientRect( &rect );
			if ( RW( m_imgrect ) != pdc->GetWidth() || RH( m_imgrect ) != pdc->GetHeight() ) 
			{	CopyRect( &m_rect, *pdc ); SizeDlg(); RedrawWindow(); }
		} // end size dlg
		m_img.RedrawWindow(); 
	}

	void SetImageSize( long w, long h )
	{	SetRect( &m_rect, 0, 0, w, h ); }

private:

	CDlgMover		m_mover;

	CWinMenu		m_menu;

	CWinDc			m_bck;

	BOOL			m_bCapture;

	RECT			m_rect;

	RECT			m_imgrect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMGVIEW_H__B709BF9F_784B_4C89_8308_D73EF6146D4B__INCLUDED_)
