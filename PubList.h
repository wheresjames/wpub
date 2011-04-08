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
// PubList.h: interface for the CPubList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLIST_H__02401BDD_3EFB_4BE8_8338_54C204AF81F1__INCLUDED_)
#define AFX_PUBLIST_H__02401BDD_3EFB_4BE8_8338_54C204AF81F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ftp.h"
#include "EmailThread.h"

#define PUBTYPE_NONE			0
#define PUBTYPE_FTP				1
#define PUBTYPE_EMAIL			2
#define PUBTYPE_DISK			3

#define PUBF1_THUMBNAIL			0x00000001
#define PUBF1_FIXEDRATIO		0x00000002
#define PUBF1_ROTATEFNAME		0x00000004
#define PUBF1_TNSIZING			0x00000008
#define PUBF1_SYNCTOCLOCK		0x00000010
#define PUBF1_MOTION			0x00000020
#define PUBF1_TIMESTAMP			0x00000040
#define PUBF1_TNROTATE			0x00000080
#define PUBF1_TNTIMESTAMP		0x00000100
#define PUBF1_MOTIONALL			0x00000200
#define PUBF1_PLAYSOUND			0x00000400
#define PUBF1_W32FTP			0x00000800
#define PUBF1_COMPRESSVIDEO		0x00001000
#define PUBF1_AVI				0x00002000
#define PUBF1_THMAVI			0x00004000
#define PUBF1_AVICAPMOTION		0x00008000

typedef struct tagEMAILMSGINFO
{
	CStr			to;
	CStr			from;
	CStr			subject;
	CStr			body;

} EMAILMSGINFO, *LPEMAILMSGINFO; // end typedef struct


typedef struct tagPUBINFO : LLISTINFO
{
	DWORD			type;
	DWORD			interval;
	DWORD			lasttime;
	DWORD			timeout;
	DWORD			motion;
	DWORD			lastmotionframe;
	DWORD			motioninterval;
	DWORD			nextmotion;

	DWORD			f1;

	DWORD			startindex;
	DWORD			stopindex;
	DWORD			curindex;

	DWORD			tnwidth;
	DWORD			tnheight;
	DWORD			tnpwidth;
	DWORD			tnpheight;

	DWORD			avitimeout;
	DWORD			thmavitimeout;
	DWORD			capframes;
	DWORD			capseconds;
	DWORD			pbframes;
	DWORD			pbseconds;
	DWORD			fourCC;		

	BOOL			bHold;

	LPEMAILMSGINFO	pemi;

	CFtp			*pftp;
	CFtp			*pftpthm;
	CNetFile		*pw32ftp;
	CNetFile		*pw32ftpthm;

	CWinAvi			*avi;
	CWinAvi			*thmavi;

	CWinDc			*avgdc;

	CEmailThread	*pemail;

	void			*pthm;
	TMem< BYTE >	*mem;
	TMem< BYTE >	*tnmem;
	TMem< BYTE >	*codecdata;

	char			img[ CWF_STRSIZE ];
	char			path[ CWF_STRSIZE ];
	char			fname[ CWF_STRSIZE ];
	char			tfname[ CWF_STRSIZE ];
	char			pub_fname[ CWF_STRSIZE ];
	char			pub_tfname[ CWF_STRSIZE ];
	char			str[ CWF_STRSIZE ];
	char			tmpl[ CWF_STRSIZE ];
	char			snd[ CWF_STRSIZE ];
	char			avicachefile[ CWF_STRSIZE ];
	char			thmavicachefile[ CWF_STRSIZE ];

} PUBINFO, *LPPUBINFO; // end typedef struct

class CPubList : public CLList  
{
public:

	static BOOL UpdateData( BOOL bRead, LPPUBINFO ppi, CRKey *pRk );
	static void ReleaseObject( LPPUBINFO ppi );
	static void Default( LPPUBINFO ppi );
	
	virtual void DeleteObject( void *ptr );
	virtual DWORD GetObjSize() { return sizeof( PUBINFO ); }

	virtual DWORD Load( LPCTSTR pFile );
	virtual DWORD Save( LPCTSTR pFile );
	LPPUBINFO Add( LPPUBINFO ppi );
	CPubList();
	virtual ~CPubList();

};

#endif // !defined(AFX_PUBLIST_H__02401BDD_3EFB_4BE8_8338_54C204AF81F1__INCLUDED_)
