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
// VideoList.h: interface for the CVideoList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOLIST_H__2CD38B1F_CD48_4756_861C_6DB2E4060936__INCLUDED_)
#define AFX_VIDEOLIST_H__2CD38B1F_CD48_4756_861C_6DB2E4060936__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ImgEffects.h"
#include "CaptureThread.h"

#define VIF1_MOTIONOVERLAY		0x00000001
#define VIF1_MOTIONSOUND		0x00000002
#define VIF1_FRAMEAVG			0x00000004

typedef struct tagVIDEOINFO : LLISTINFO
{
	char			name[ 256 ];
	DWORD			index;
	DWORD			f1;

	DWORD			active;
	DWORD			frameavgweight;

	DWORD			framerate;

	CWinDc			*os;
	CCaptureThread	*cap;

	CWinDc			*tavg;

	CWinDc			*mos;
	MOTION			motion;

	CImgEffects		*imgfx;

	char			motionsnd[ CWF_STRSIZE ];

} VIDEOINFO, *LPVIDEOINFO; // end typdef struct

class CVideoList : public CLList  
{
public:
	DWORD GetFrameRate( DWORD i );
	BOOL WriteSettings();
	BOOL SaveSettings( LPVIDEOINFO pVi );
	BOOL LoadSettings( LPVIDEOINFO pVi );
	BOOL SetFrameRates( DWORD rate );
	BOOL SetFrameRate( DWORD i, DWORD rate );
	BOOL Connect( LPVIDEOINFO pVi );
	LPVIDEOINFO FindByIndex( DWORD i  );
	LPVIDEOINFO Add( DWORD i );

	virtual void DeleteObject( void *node );
	virtual DWORD GetObjSize() { return sizeof( VIDEOINFO ); }

	CVideoList();
	virtual ~CVideoList();

};

#endif // !defined(AFX_VIDEOLIST_H__2CD38B1F_CD48_4756_861C_6DB2E4060936__INCLUDED_)
