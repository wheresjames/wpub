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
// PropList.cpp: implementation of the CPropList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vp.h"
#include "PropList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPropList::CPropList()
{

}

CPropList::~CPropList()
{

}

void CPropList::DeleteObject(void *node)
{

	CLList::DeleteObject( node );
}

LPPROPINFO CPropList::Add(	DWORD type, DWORD edit, 
							DWORD user, LPCTSTR name, 
							LPVOID data, DWORD size,
							long min, long max,
							DWORD mask, LPCTSTR on, LPCTSTR off,
							BOOL fullredraw )
{
	LPPROPINFO ppi = (LPPROPINFO)New();
	if ( ppi == NULL ) return NULL;

	// Save property information
	ppi->type = type;
	ppi->edit = edit;
	ppi->user = user;

	if ( name != NULL ) strcpy_sz( ppi->name, name );

	ppi->data = data;
	ppi->size = size;

	ppi->min = min;
	ppi->max = max;
	
	ppi->mask = mask;
	if ( on != NULL ) strcpy_sz( ppi->on, on );
	if ( off != NULL ) strcpy_sz( ppi->off, off );

	ppi->fullredraw = fullredraw;

	return ppi;
}


double CPropList::GetNum(LPPROPINFO ppi)
{
	switch( ppi->type )
	{

		case PROPTYPE_BOOL :

			if ( *( (LPBOOL)ppi->data ) != 0 ) return TRUE;
			else return FALSE;

			break;

		case PROPTYPE_RGBSTR :
		{
			return CGrDC::HtmlToRgb( (char*)ppi->data );

		} break;

		case PROPTYPE_BIN :
		case PROPTYPE_DWORD :
		case PROPTYPE_RGB :

			return *( (LPDWORD)ppi->data );

			break;

		case PROPTYPE_LONG :

			return *( (LPLONG)ppi->data );

			break;

		case PROPTYPE_FLAG :

			if ( ( *( (LPDWORD)ppi->data ) & ppi->mask ) != 0 )
				return 1; else return 0;

			break;

		case PROPTYPE_DOUBLE :

			return *( (double*)ppi->data );

			break;

		case PROPTYPE_DIR : 
		case PROPTYPE_FILE : 
		case PROPTYPE_STR :

			return strtol( (LPCTSTR)ppi->data, NULL, 10 );

			break;


	} // end switch

	return 0;
}

BOOL CPropList::GetStr(LPPROPINFO ppi, LPSTR str, DWORD size)
{
	switch( ppi->type )
	{
		case PROPTYPE_BOOL :
			
			if ( *( (LPBOOL)ppi->data ) != FALSE ) strcpy( str, ppi->on );
			else strcpy( str, ppi->off );

			break;

		case PROPTYPE_DWORD : 
			wsprintf( str, "%lu", (DWORD)*( (LPDWORD)ppi->data ) );
			break;

		case PROPTYPE_LONG : 
			wsprintf( str, "%li", (LONG)*( (LPLONG)ppi->data ) );
			break;

		case PROPTYPE_DOUBLE : 
			if ( ppi->data == NULL ) strcpy( str, "0" );
			else sprintf( str, "%g", (double)*( (double*)ppi->data ) );
			break;

		case PROPTYPE_RGBSTR :
		case PROPTYPE_DIR : 
		case PROPTYPE_FILE : 
		case PROPTYPE_STR :
			if ( ppi->data == NULL ) *str = 0;
			else strncpy( str, (char*)ppi->data, size );
			break;

		case PROPTYPE_RGB : 
		{
			COLORREF rgb = *( (LPCOLORREF)ppi->data );

			CGrDC::RgbToHtml( rgb, str );

		} break;

		case PROPTYPE_FLAG : 
		{
			if ( GetNum( ppi ) != 0 ) strcpy( str, ppi->on );
			else strcpy( str, ppi->off );

		} break;

		case PROPTYPE_FONT : 
			if ( ppi->data == NULL ) strcpy( str, "AaBbCc" ); 
			else { strncpy( str, ( (LPLOGFONT)ppi->data )->lfFaceName, 31 ); str[ 31 ] = 0; }
			break;

		default : return FALSE; break;

	} // end switch

	return TRUE;
}

void CPropList::SetNum(LPPROPINFO ppi, double num)
{
	switch( ppi->type )
	{
		case PROPTYPE_BOOL :

			if ( num != FALSE ) *( (LPBOOL)ppi->data ) = TRUE;
			else *( (LPBOOL)ppi->data ) = FALSE;

			break;
		
		case PROPTYPE_RGBSTR :
		{
			COLORREF rgb = (COLORREF)(DWORD)num;

			CGrDC::RgbToHtml( rgb, (char*)ppi->data );

		} break;

		case PROPTYPE_BIN :
		case PROPTYPE_DWORD :
		case PROPTYPE_RGB :

			*( (LPDWORD)ppi->data ) = (DWORD)num;

			break;

		case PROPTYPE_LONG :

			*( (LPLONG)ppi->data ) = (DWORD)num;

			break;

		case PROPTYPE_FLAG :

			if ( num ) *( (LPDWORD)ppi->data ) |= ppi->mask;
			else *( (LPDWORD)ppi->data ) &= ~ppi->mask;

			break;

		case PROPTYPE_DOUBLE :

			*( (double*)ppi->data ) = num;

			break;

		case PROPTYPE_DIR : 
		case PROPTYPE_FILE : 
		case PROPTYPE_STR :

			sprintf( (LPSTR)ppi->data, "%g", (double)num );

			break;


	} // end switch

}

void CPropList::SetStr(LPPROPINFO ppi, LPCTSTR str)
{
	switch( ppi->type )
	{
		case PROPTYPE_BOOL :
		{
			long num = 0; 

			// Try to determine state
			if ( !strcmpi( str, ppi->on ) ) num = 1;
			else if ( !strcmpi( str, ppi->off ) ) num = 1;
			else num = strtol( str, NULL, 10 );

			if ( num != FALSE ) *( (LPBOOL)ppi->data ) = TRUE;
			else *( (LPBOOL)ppi->data ) = FALSE;

		} break;
		
		case PROPTYPE_BIN :
		case PROPTYPE_DWORD :

			*( (LPDWORD)ppi->data ) = strtoul( str, NULL, 10 );

			break;

		case PROPTYPE_LONG :

			*( (LPLONG)ppi->data ) = strtol( str, NULL, 10 );

			break;

		case PROPTYPE_RGB :
		{
			COLORREF rgb = CGrDC::HtmlToRgb( str );			
			*( (LPCOLORREF)ppi->data ) = rgb;	

		}	break;

		case PROPTYPE_FLAG :
		{
			long num = 0; 

			// Try to determine state
			if ( !strcmpi( str, ppi->on ) ) num = 1;
			else if ( !strcmpi( str, ppi->off ) ) num = 1;
			else num = strtol( str, NULL, 10 );

			if ( num != 0 ) *( (LPDWORD)ppi->data ) |= ppi->mask;
			else *( (LPDWORD)ppi->data ) &= ~ppi->mask;

		} break;

		case PROPTYPE_DOUBLE :

			*( (double*)ppi->data ) = strtod( str, NULL );

			break;

		case PROPTYPE_RGBSTR :
		case PROPTYPE_DIR : 
		case PROPTYPE_FILE : 
		case PROPTYPE_STR :
			
			if ( ppi->data != NULL )
			{	
				if ( ppi->size > 0 )
				{	strncpy( (LPSTR)ppi->data, str, ppi->size );
					( (LPSTR)ppi->data )[ ppi->size - 1 ] = 0;
				} // end if
				else *( (LPBYTE)ppi->data ) = 0;
			} // end if

			break;


	} // end switch

}
