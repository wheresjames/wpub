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
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__BF5E335E_91AF_4F73_B386_D917135CEE19__INCLUDED_)
#define AFX_MAINFRM_H__BF5E335E_91AF_4F73_B386_D917135CEE19__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PubImgList.h"
#include "VideoList.h"

#include "IServer.h"

#include "PubThread.h"

#include "BarProp.h"
#include "DlgProp.h"

#include "opc/scbarg.h"

#include "DlgSimple.h"

#include "ToolbarWindow.h"

#include "BarGraph.h"

#include "DlgProductUpdate.h"

#include "SlantToolbar.h"

#include "ImgList.h"

#include "DlgInform.h"


#define WM_FRAMEMSG			( WM_USER + 801 )
#define WM_MOTIONDETECT		( WM_USER + 802 )
#define WM_SETEVENT			( WM_USER + 803 )
#define WM_INFORM			( WM_USER + 804 )

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;

//	CToolBar    m_wndToolBar;
//	CToolBar    m_wndInsertBar;
//	CDialogBar	m_wndServerBar;

public:
	LRESULT OnImgList( WPARAM wParam, LPARAM lParam );

	CPubImgList			m_pubimglist;
	CVideoList			m_videolist;

	CDlgProp			m_dlgprop;
	CSizingControlBarG	m_propbar;

	CImgList			m_imglist;
//	CPubImgView			m_dlgpubimg;
	CSizingControlBarG	m_pubimgbar;
	
	CBarGraph			m_graph;
	CSizingControlBarG	m_graphbar;

	CIServer			m_iserver;
	CISession			m_isession;

	CSlantToolbar		m_maintoolbar;
	CDialogBar			m_wndMainToolbar;

	CSlantToolbar		m_overlaytoolbar;
	CDialogBar			m_wndOverlayToolbar;

	CSlantToolbar		m_webservertoolbar;
	CDialogBar			m_wndWebServerToolbar;

	HWND				m_hMotionWnd;

	HWND				m_hRefreshWnd;
	UINT				m_uRefreshMsg;

	BOOL				m_bResize;

	DWORD				m_dwExpectDelays;

	BOOL				m_bEnablePublishing;

// Generated message map functions
protected:
	
	CDialogBar m_wndImage;
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnPopupCopy();
	afx_msg void OnPopupCut();
	afx_msg void OnPopupDelete();
	afx_msg void OnPopupDuplicate();
	afx_msg void OnPopupPaste();
	afx_msg void OnPopupProperties();
	afx_msg void OnOptionsSettings();
	afx_msg void OnTrayiconAbout();
	afx_msg void OnTrayiconEmailimage();
	afx_msg void OnTrayiconEnablepublishing();
	afx_msg void OnTrayiconExit();
	afx_msg void OnSaveimage();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnOptionsCheckforupdate();
	afx_msg void OnEnablepublishing();
	afx_msg void OnUpdateEnablepublishing(CCmdUI* pCmdUI);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateRefresh(CCmdUI* pCmdUI);
	afx_msg void OnHelpRegistration();
	afx_msg void OnEditProperties();
	afx_msg void OnViewOverlaybar();
	afx_msg void OnUpdateViewOverlaybar(CCmdUI* pCmdUI);
	afx_msg void OnViewWebserver();
	afx_msg void OnUpdateViewWebserver(CCmdUI* pCmdUI);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnOptionsHtmlbuilder();
	afx_msg void OnViewPubimgview();
	afx_msg void OnUpdateViewPubimgview(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSetupwizard();
	afx_msg void OnFileLoadoverlay();
	afx_msg void OnFileSaveoverlay();
	afx_msg void OnFileMergeoverlay();
	afx_msg void OnFileOpen();
	afx_msg void OnSimple();
	afx_msg void OnSetupsimple();
	afx_msg void OnViewEventlog();
	afx_msg void OnHelpHelp();
	afx_msg void OnViewGraphbar();
	afx_msg void OnUpdateViewGraphbar(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	static UINT			m_uTaskBarCreated;

	BOOL				m_bExit;
	BOOL				m_bPaused;

	DWORD				m_dwHits;

	UINT				m_uTimer;
	UINT				m_uGraphTimer;

	CPubThread			m_pubthread;

	BOOL				m_bEnableRefresh;

	DWORD				m_dwLastPage;

//	CToolbarWindow		m_servertoolbar;

	DWORD				m_dwStatusTimeout;

	CDlgProductUpdate	m_productupdate;

	HICON				m_hHtml;
	HICON				m_hChecked;
	HICON				m_hUnchecked;

	CString				m_sRemoteAddr;

	DWORD				m_dwUpdateTimer;

	CWinAvi				m_avi;

public:
	BOOL OpenViewImgDialog( LPPUBIMGINFO ppii );
	LRESULT OnWebServerToolbarUpdate( WPARAM wParam, LPARAM lParam );
	LRESULT OnWebServerToolbarCmd( WPARAM wParam, LPARAM lParam );
	void RedrawOverlayBar();
	LRESULT OnOverlayToolbarUpdate( WPARAM wParam, LPARAM lParam );
	LRESULT OnOverlayToolbarCmd( WPARAM wParam, LPARAM lParam );
	LRESULT OnMainToolbarUpdate( WPARAM wParam, LPARAM lParam );
	LRESULT OnMainToolbarCmd( WPARAM wParam, LPARAM lParam );
	void SetEvent( DWORD event );
	LRESULT OnEvent( WPARAM wParam, LPARAM lParam );
	BOOL AddPubJob( CReg *pReg );
	BOOL SetStatus( LPCTSTR pText );
	LRESULT OnSetStatus( WPARAM wParam, LPARAM lParam );
	LRESULT OnToolbar( WPARAM wParam, LPARAM lParam );
	BOOL SaveOverlay( LPCTSTR pFile );
	BOOL LoadOverlay( LPCTSTR pFile );
	void OnVideoMenuUpdate( CCmdUI *pCmdUI );
	void OnVideoMenu( UINT uID );
	void UpdateVideoMenu();
	void CreateVideoMenu();
	LRESULT OnExitApp( WPARAM wParam, LPARAM lParam );
	void OnProgress( WPARAM wParam, LPARAM lParam );
	LRESULT OnFrameRefresh( WPARAM wParam, LPARAM lParam );
	void UpdateUsers();
	LRESULT OnWebSession( WPARAM wParam, LPARAM lParam );
	void GetAddressInfo();
	void SetFrameRates();
	BOOL SetQuality( CWinImg *pImg );
	void ShowMenu( long x, long y );
	LRESULT OnTaskBarCreated( WPARAM wParam, LPARAM lParam );
	LRESULT OnTrayIcon( WPARAM wParam, LPARAM lParam );
	BOOL TrayIcon( LPCTSTR tip, DWORD flags = NIM_ADD, HICON hIcon = NULL );
	void OnRun();
	LRESULT OnWebServer( WPARAM wParam, LPARAM lParam );
	void StopServer();
	BOOL StartServer();
	BOOL VerifyBarState( LPCTSTR pProfileName );
	BOOL ShowSettings( DWORD pg );
	void RunSetupWizard();
	void Shutdown();
	LRESULT OnFrameMsg( WPARAM wParam, LPARAM lParam );
	BOOL RestoreSettings();

	afx_msg void OnNewimage();
	afx_msg void OnDeleteimage();
	afx_msg void OnViewimage();

	DWORD GetTotalHits() { return m_dwHits; }

	void SetMotionWnd( HWND hWnd ) { m_hMotionWnd = hWnd; }

	void SetRefreshWnd( HWND hWnd, UINT uMsg ) 
	{	m_hRefreshWnd = hWnd; m_uRefreshMsg = uMsg; }

	void SetUpdateTimer( DWORD dwTimeout = 1000 )
	{	m_dwUpdateTimer = GetTickCount() + dwTimeout; }

	void Exit();

	CDlgSimple			m_simple;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BF5E335E_91AF_4F73_B386_D917135CEE19__INCLUDED_)
