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
// LogFile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__C46F384F_CF58_4C87_B0C8_05654E56D0F8__INCLUDED_)
#define AFX_LOGFILE_H__C46F384F_CF58_4C87_B0C8_05654E56D0F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#define LOGTYPE_TYPEMASK	0x0000ffff
#define LOGTYPE_INFO		0x00000000
#define LOGTYPE_WARNING		0x00000001
#define LOGTYPE_ERROR		0x00000002
#define LOGTYPE_CRITICAL	0x00000003


#define LOGTYPE_FLAGMASK	0xffff0000
#define LOGTYPE_OOB			0x00010000




typedef struct tagLOGINFO : LLISTINFO
{
	DWORD		type;
	char		*text;

} LOGINFO, *LPLOGINFO; // end typedef struct

class CLogFile : public CLList  
{
public:
	DWORD Save( LPCTSTR pFile );

	BOOL vLog( DWORD type, LPCTSTR pFormat, va_list argptr );
	BOOL _cdecl Log( DWORD type, LPCTSTR pFormat, ... );
	
	virtual void DeleteObject( void *node );
	virtual DWORD GetObjSize() { return sizeof( LOGINFO ); }

	BOOL Add( DWORD type, LPCTSTR pText );
	CLogFile();
	virtual ~CLogFile();

};

#endif // !defined(AFX_LOGFILE_H__C46F384F_CF58_4C87_B0C8_05654E56D0F8__INCLUDED_)
