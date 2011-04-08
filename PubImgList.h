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
// PubImgList.h: interface for the CPubImgList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBIMGLIST_H__9853FB46_E2AF_4F08_9EA2_D04D7B98B344__INCLUDED_)
#define AFX_PUBIMGLIST_H__9853FB46_E2AF_4F08_9EA2_D04D7B98B344__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Fx.h"
#include "DlgImgView.h"

#define PIIF1_CUSTOMSIZE		0x00000001
#define PIIF1_CHROMAKEY			0x00000002
#define PIIF1_KEYIMG			0x00000004
#define PIIF1_KEYDYNAMIC		0x00000008
#define PIIF1_KEYVIDEO			0x00000010
#define PIIF1_NOSMOOTHING		0x00000020
#define PIIF1_MASK				0x00000040
#define PIIF1_MASKIMG			0x00000080
#define PIIF1_IMGDLG			0x00000100

typedef struct tagPUBIMGINFO : LLISTINFO
{
	char			name[ 256 ];
	char			bckimgname[ CWF_STRSIZE ];
	RECT			rect;
	COLORREF		rgbbck;
	DWORD			f1, fc1;

	long			cpos;
	long			cneg;
	COLORREF		rgbkey;
	char			keyimg[ CWF_STRSIZE ];
	char			keyimgloaded[ CWF_STRSIZE ];
	char			mask[ CWF_STRSIZE ];
	char			maskloaded[ CWF_STRSIZE ];

	DWORD			keydriver;

	COLORREF		rgbmask;

//	DWORD			index;

	CWinDc			*os;
	CCfgFile		*cfg;
	CFx				*fx;
	CWinDc			*bck;
	CWinDc			*keybck;
	CWinDc			*maskdc;
	CDlgImgView		*imgview;
	CTlLock			*lock;


} PUBIMGINFO, *LPPUBIMGINFO; // end typedef struct


class CPubImgList : public CLList  
{
public:
	BOOL IsDlg( DWORD driver );
	BOOL IsVideoInImage( DWORD driver, LPPUBIMGINFO ppii );
	BOOL DrawChromaBck( CWinDc *pDC, LPPUBIMGINFO pPii );
	BOOL Update( LPCTSTR pImg, BOOL bAll = FALSE );
	BOOL GetImage( LPCTSTR pImgName, CWinImg *pImg, long w = 0, long h = 0 );
	BOOL GetImage( DWORD i, CWinImg *pImg );
	BOOL GetImageName( DWORD i, LPSTR pName );
	BOOL ConnectVideo( DWORD i = MAXDWORD );
	void ConnectVideo( LPPUBIMGINFO pPii );
	virtual void Destroy();
	virtual DWORD GetObjSize() { return sizeof( PUBIMGINFO ); }

//	BOOL ActivateVideo( LPPUBIMGINFO pPii );
//	BOOL ActivateVideo( DWORD i = MAXDWORD );
	BOOL DeactivateUnusedVideo();
	virtual void Delete( void *ptr, BOOL bDelete = TRUE );
	BOOL Update( LPPUBIMGINFO pPii, BOOL bAll = FALSE );
	LPPUBIMGINFO FindByName( LPCTSTR pName );
	BOOL Restore();
	BOOL Save();
	BOOL Scale( DWORD i, LPRECT pRect );
	BOOL Update( DWORD i = MAXDWORD, BOOL bAll = FALSE );
	LPPUBIMGINFO GetSelImage();
	LPPUBIMGINFO GetImage( DWORD i );
	BOOL Draw( HDC hDC, LPRECT pRect, DWORD i );
	LPPUBIMGINFO Add( LPCTSTR pName );
	virtual void DeleteObject( void *node );
	CPubImgList();
	virtual ~CPubImgList();

	void SetCurSel( DWORD sel ) { m_dwSel = sel; Update(); }
	DWORD GetCurSel() { return m_dwSel; }


private:

	DWORD				m_dwSel;

};

#endif // !defined(AFX_PUBIMGLIST_H__9853FB46_E2AF_4F08_9EA2_D04D7B98B344__INCLUDED_)
