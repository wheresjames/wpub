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
#if !defined(AFX_DLGSIMPLE_H__E666243A_F735_4F36_BAED_FE22904504C8__INCLUDED_)
#define AFX_DLGSIMPLE_H__E666243A_F735_4F36_BAED_FE22904504C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSimple.h : header file
//

#include "WndButton.h"
#include "ImageView.h"
#include "ToolbarWindow.h"
#include "ColorBox.h"
#include "ImgThread.h"
#include "IconButton.h"

#include "Ftp.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSimple dialog

class CDlgSimple : public CDialog
{
// Construction
public:
	BOOL EmailImage();
	LRESULT OnFrameMsg( WPARAM wParam, LPARAM lParam );
	BOOL CreateImage( LPVIDEOINFO pvi = NULL );
	void FillBackgroundList();
	void FillOverlayList();
	void DoEnable();
	LRESULT OnToolbar( WPARAM wParam, LPARAM lParam );
	void Size();
	CDlgSimple(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSimple)
	enum { IDD = IDD_SIMPLE };
	CColorBox	m_colorMask;
	CIconButton	m_btnChooseMaskColor;
	CIconButton	m_btnChooseColor;
	CSliderCtrl	m_slideErr;
	CColorBox	m_colorbox;
	CListCtrl	m_listBackgrounds;
	CListCtrl	m_listOverlays;
	CToolbarWindow	m_toolbar;
	CImageView	m_image;
	BOOL	m_bEnableBackground;
	BOOL	m_bEnableOverlay;
	BOOL	m_bMaskImg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSimple)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSimple)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnableBackground();
	afx_msg void OnEnableOverlay();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysColorChange();
	afx_msg void OnClickBackgrounds(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickOverlays(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChooseColor();
	afx_msg void OnMaskimg();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChooseMaskcolor();
	//}}AFX_MSG

	virtual void OnClose(); 
	virtual void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:

	CWinDc			m_bckfill;

	CImageList		m_thm_overlays;
	CImageList		m_thm_backgrounds;

	char			m_szBackgroundPath[ CWF_STRSIZE ];
	char			m_szOverlayPath[ CWF_STRSIZE ];

	char			m_szBackgroundImg[ CWF_STRSIZE ];
	char			m_szOverlayImg[ CWF_STRSIZE ];

	CWinDc			m_capture;
	CWinDc			m_overlayimg;
	CWinDc			m_backgroundimg;

	BOOL			m_bCapture;

	HICON			m_hExit;

	BOOL			m_bClose;

	CFtp			m_ftp;
	TMem< BYTE >	m_mem;

	CImgThread		m_ovthread;
	CImgThread		m_bckthread;

public:
	LRESULT OnLoadNextBck( WPARAM wParam, LPARAM lParam );
	LRESULT OnLoadNextOverlay( WPARAM wParam, LPARAM lParam );
	void LoadToolbar();
	BOOL FtpImage( LPCTSTR pServer, CFtp *pFtp, LPBYTE buf, DWORD size, LPCTSTR pPath );
	BOOL Ftp();
	void SaveToFile();
	void Shutdown();

	CWinDc			m_img;
	BOOL			m_bMaskLoaded;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSIMPLE_H__E666243A_F735_4F36_BAED_FE22904504C8__INCLUDED_)
