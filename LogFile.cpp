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
// LogFile.cpp: implementation of the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFile::CLogFile()
{

}

CLogFile::~CLogFile()
{

}

BOOL CLogFile::Add(DWORD type, LPCTSTR pText)
{
	if ( pText == NULL ) return FALSE;

	// Allocate mem for node
	LPLOGINFO node = (LPLOGINFO)New();
	if ( node == NULL ) return FALSE;

	// Allocate memory for text
	node->text = new char[ strlen( pText ) + 1 ];
	if ( node->text == NULL ) { Delete( node ); return FALSE; }
	strcpy( node->text, pText );

	// Save type
	node->type = type;

	return TRUE;
}

void CLogFile::DeleteObject(void *node)
{
	LPLOGINFO pli = (LPLOGINFO)node;
	if ( pli == NULL ) return;

	if ( pli->text != NULL )
	{	delete [] pli->text;
		pli->text = NULL;
	} // end if

	CLList::DeleteObject( node );
}

BOOL _cdecl CLogFile::Log( DWORD type, LPCTSTR pFormat, ... )
{
	return vLog( type, pFormat, (char*) ( &pFormat + 1 ) );
} 

BOOL CLogFile::vLog( DWORD type, LPCTSTR pFormat, va_list argptr )
{
	char buf[ 4096 ] = { 0 };

	try
	{
		// Sanity Check
		if ( pFormat == NULL || pFormat[ 0 ] == 0x0 ) return FALSE;

		// Create the string
		vsprintf( buf, pFormat, argptr );

		// Add CRLF
//		strcat( buf, "\r\n" );

		// Add the buffer
		Add( type, buf );

	} // end try
	catch( ... )
	{
		ASSERT( 0 );
		return FALSE;
	} // end catch

	return TRUE;
}


DWORD CLogFile::Save(LPCTSTR pFile)
{
	if ( pFile == NULL ) return FALSE;

	CWinFile f;
	if ( !f.OpenNew( pFile ) ) return FALSE;

	// Write out each line
	LPLOGINFO pli = NULL;
	while ( ( pli = (LPLOGINFO)GetNext( pli ) ) != NULL )
	{	
		// Write if not out of band data
		if ( ( pli->type & LOGTYPE_FLAGMASK & LOGTYPE_OOB ) == 0 )
			f.Write( pli->text ); f.Write( "\r\n" ); 
	}

	return TRUE;
}
