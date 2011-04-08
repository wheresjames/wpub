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
// PubList.cpp: implementation of the CPubList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "PubList.h"
#include "Ftp.h"
#include "EmailThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPubList::CPubList()
{

}

CPubList::~CPubList()
{
	Destroy();
}

void CPubList::DeleteObject(void *ptr)
{
	LPPUBINFO ppi = (LPPUBINFO)ptr;
	if ( ppi == NULL ) return;

	ReleaseObject( ppi );

	CLList::DeleteObject( ptr );
}

void CPubList::ReleaseObject(LPPUBINFO ppi)
{
	if ( ppi == NULL ) return;

	// Lose ftp objects
	if ( ppi->pftp != NULL )
	{	delete ppi->pftp;
		ppi->pftp = NULL;
	} // end if
	if ( ppi->pftpthm != NULL )
	{	delete ppi->pftpthm;
		ppi->pftpthm = NULL;
	} // end if

	// Lose netfile objects
	if ( ppi->pw32ftp != NULL )
	{	delete ppi->pw32ftp;
		ppi->pw32ftp = NULL;
	} // end if
	if ( ppi->pw32ftpthm != NULL )
	{	delete ppi->pw32ftpthm;
		ppi->pw32ftpthm = NULL;
	} // end if

	// Lose email object
	if ( ppi->pemail != NULL )
	{	delete ppi->pemail;
		ppi->pemail = NULL;
	} // end if

	if ( ppi->pemi != NULL )
	{	delete ppi->pemi;
		ppi->pemi = NULL;
	} // end if

	if ( ppi->avi != NULL )
	{	delete ppi->avi;
		ppi->avi = NULL;
	} // end if

	if ( ppi->thmavi != NULL )
	{	delete ppi->thmavi;
		ppi->thmavi = NULL;
	} // end if

	if ( ppi->avgdc != NULL )
	{	delete ppi->avgdc;
		ppi->avgdc = NULL;
	} // end if

	// Lose the memory
	if ( ppi->codecdata != NULL )
	{	delete ppi->codecdata;
		ppi->codecdata = NULL;
	} // end if

	if ( ppi->mem != NULL )
	{	delete ppi->mem;
		ppi->mem = NULL;
	} // end if
	if ( ppi->tnmem != NULL )
	{	delete ppi->tnmem;
		ppi->tnmem = NULL;
	} // end if
}


LPPUBINFO CPubList::Add(LPPUBINFO ppi)
{
	if ( ppi == NULL ) return NULL;

	LPPUBINFO node = (LPPUBINFO)New( ppi );
	if ( node == NULL ) return NULL;

	// Create email object
	node->pemi = new EMAILMSGINFO;
	
	// Create ftp object
//	if ( node->type == PUBTYPE_FTP )
	{
//		if ( ( node->f1 & PUBF1_W32FTP ) != 0 )
		{	node->pw32ftp = new CNetFile();
			node->pw32ftpthm = new CNetFile();
		} // end if
//		else
		{	node->pftp = new CFtp();
			node->pftpthm = new CFtp();
		} // end else

	} // end if

//	else if ( node->type == PUBTYPE_EMAIL )
	{	node->pemail = new CEmailThread();
		if ( ppi->pemi != NULL )
		{	node->pemi->to.copy( ppi->pemi->to );
			node->pemi->from.copy( ppi->pemi->from );
			node->pemi->subject.copy( ppi->pemi->subject );
			node->pemi->body.copy( ppi->pemi->body );
		} // end if
	} // end else if

	// Allocate new memory
	node->mem = new TMem< BYTE >;
	node->tnmem = new TMem< BYTE >;
	node->codecdata = new TMem< BYTE >;

	// Copy codec data if needed
	if ( ppi->codecdata != NULL ) 
		node->codecdata->copy( *ppi->codecdata );

	// Allocate more memory
	node->avi = new CWinAvi();
	node->thmavi = new CWinAvi();

	// Allocate avg frame memory
	node->avgdc = new CWinDc();

	// Set default values if needed
	if ( ppi == NULL ) Default( node );

	return node;
}

DWORD CPubList::Save(LPCTSTR pFile)
{
	CCfgFile	cfg;

	DWORD i = 1;
	char msg[ 256 ];

	LPPUBINFO ppi = NULL;
	while ( ( ppi = (LPPUBINFO)GetNext( ppi ) ) != NULL )
	{
		wsprintf( msg, "Publish %lu", i++ );
		HGROUP hGroup = cfg.AddGroup( msg );
		if ( hGroup != NULL )
		{
			cfg.SetValue( hGroup, "Type", ppi->type );
			cfg.SetValue( hGroup, "Interval", ppi->interval );
			cfg.SetValue( hGroup, "Image", ppi->img );
			cfg.SetValue( hGroup, "Str", ppi->str );
			cfg.SetValue( hGroup, "Filename", ppi->fname );
			cfg.SetValue( hGroup, "ThumbnailFilename", ppi->tfname );
			cfg.SetValue( hGroup, "Path", ppi->path );

			cfg.SetValue( hGroup, "TimestampTemplate", ppi->tmpl );
			cfg.SetValue( hGroup, "Motion", ppi->motion );
			cfg.SetValue( hGroup, "MotionInterval", ppi->motioninterval );

			cfg.SetValue( hGroup, "F1", ppi->f1 );
			cfg.SetValue( hGroup, "StartIndex", ppi->startindex );
			cfg.SetValue( hGroup, "StopIndex", ppi->stopindex );
			cfg.SetValue( hGroup, "CurrentIndex", ppi->curindex );

			cfg.SetValue( hGroup, "TnWidth", ppi->tnwidth );
			cfg.SetValue( hGroup, "TnHeight", ppi->tnheight );
			cfg.SetValue( hGroup, "TnPWidth", ppi->tnpwidth );
			cfg.SetValue( hGroup, "TnPHeight", ppi->tnpheight );

			cfg.SetValue( hGroup, "AviCapFrames", ppi->capframes );
			cfg.SetValue( hGroup, "AviCapSeconds", ppi->capseconds );
			cfg.SetValue( hGroup, "AviPbFrames", ppi->pbframes );
			cfg.SetValue( hGroup, "AviPbSeconds", ppi->pbseconds );
			cfg.SetValue( hGroup, "fourCC", ppi->fourCC );

			if ( ppi->codecdata != NULL && ppi->codecdata->size() != 0 )
				cfg.SetValue( hGroup, "CodecData", ppi->codecdata->ptr(), 
												 ppi->codecdata->size() );

			// Save e-mail information
			if ( ppi->type == PUBTYPE_EMAIL )
			{	cfg.SetValue( hGroup, "EmailTo", ppi->pemi->to );
				cfg.SetValue( hGroup, "EmailFrom", ppi->pemi->from );
				cfg.SetValue( hGroup, "EmailSubject", ppi->pemi->subject );
				cfg.SetValue( hGroup, "EmailBody", ppi->pemi->body );				
			} // end if

		} // end if

	} // end while

	return cfg.Save( pFile );
}

DWORD CPubList::Load(LPCTSTR pFile)
{
	CCfgFile cfg;
	if ( !cfg.Load( pFile ) ) return FALSE;

	PUBINFO	pi;
	HGROUP hGroup = NULL;
	while ( ( hGroup = cfg.GetNext( hGroup ) ) != NULL )
	{
		// Initialize memory
		ZeroMemory( &pi, sizeof( pi ) );

		// Get values
		cfg.GetValue( hGroup, "Type", &pi.type );
		cfg.GetValue( hGroup, "Interval", &pi.interval );
		cfg.GetValue( hGroup, "Image", pi.img, sizeof( pi.img ) );
		cfg.GetValue( hGroup, "Str", pi.str, sizeof( pi.str ) );
		cfg.GetValue( hGroup, "Filename", pi.fname, sizeof( pi.fname ) );
		cfg.GetValue( hGroup, "ThumbnailFilename", pi.tfname, sizeof( pi.tfname ) );
		cfg.GetValue( hGroup, "Path", pi.path, sizeof( pi.path ) );
		cfg.GetValue( hGroup, "TimestampTemplate", pi.tmpl, sizeof( pi.tmpl ) );
		cfg.GetValue( hGroup, "Motion", &pi.motion );
		cfg.GetValue( hGroup, "MotionInterval", &pi.motioninterval );

		cfg.GetValue( hGroup, "F1", &pi.f1 );
		cfg.GetValue( hGroup, "StartIndex", &pi.startindex );
		cfg.GetValue( hGroup, "StopIndex", &pi.stopindex );
		cfg.GetValue( hGroup, "CurrentIndex", &pi.curindex );

		cfg.GetValue( hGroup, "TnWidth", &pi.tnwidth );
		cfg.GetValue( hGroup, "TnHeight", &pi.tnheight );
		cfg.GetValue( hGroup, "TnPWidth", &pi.tnpwidth );
		cfg.GetValue( hGroup, "TnPHeight", &pi.tnpheight );

		cfg.GetValue( hGroup, "AviCapFrames", &pi.capframes );
		cfg.GetValue( hGroup, "AviCapSeconds", &pi.capseconds );
		cfg.GetValue( hGroup, "AviPbFrames", &pi.pbframes );
		cfg.GetValue( hGroup, "AviPbSeconds", &pi.pbseconds );
		cfg.GetValue( hGroup, "fourCC", &pi.fourCC );

		if ( pi.codecdata != NULL )
			pi.codecdata->put( cfg.GetValuePtr( hGroup, "CodecData" ),
		    				   cfg.GetValueSize( hGroup, "CodecData" ) );

		// Save e-mail information
		if ( pi.type == PUBTYPE_EMAIL )
		{	pi.pemi = new EMAILMSGINFO;
			if ( pi.pemi != NULL )
			{
				pi.pemi->to.copy( (LPCTSTR)cfg.GetValuePtr( hGroup, "EmailTo" ) );
				pi.pemi->from.copy( (LPCTSTR)cfg.GetValuePtr( hGroup, "EmailFrom" ) );
				pi.pemi->subject.copy( (LPCTSTR)cfg.GetValuePtr( hGroup, "EmailSubject"  ) );
				pi.pemi->body.copy( (LPCTSTR)cfg.GetValuePtr( hGroup, "EmailBody" ) );
			} // end if
		} // end if

		// Set AVI flags
		LPCTSTR pExt = CWinFile::GetExtension( pi.fname );
		if ( pExt != NULL && !strcmp( pExt, "avi" ) ) pi.f1 |= PUBF1_AVI;
		else pi.f1 &= ~PUBF1_AVI;

		// Set Thumbnail AVI flag
		pExt = CWinFile::GetExtension( pi.tfname );
		if ( pExt != NULL && !strcmp( pExt, "avi" ) ) pi.f1 |= PUBF1_THMAVI;
		else pi.f1 &= ~PUBF1_THMAVI;

		// Add to the list
		Add( &pi );
		if ( pi.pemi != NULL ) delete pi.pemi;

	} // end while

	return TRUE;
}


void CPubList::Default(LPPUBINFO ppi)
{
	if ( ppi == NULL ) return;

	// Default to 30 seconds
	ppi->interval = 30;
	ppi->motioninterval = 30;

	ppi->startindex = 1;
	ppi->stopindex = 10;
	ppi->curindex = 1;

	ppi->tnwidth = 64;
	ppi->tnwidth = 48;
	ppi->tnpwidth = 25;
	ppi->tnpheight = 25;

	ppi->capframes = 1;
	ppi->capseconds = 2;
	ppi->pbframes = 8;
	ppi->pbseconds = 1;

	// Default to Microsoft MPG4 compressor
	ppi->f1 |= PUBF1_COMPRESSVIDEO;
	ppi->fourCC = mmioFOURCC( 'm', 'p', 'g', '4' );
}


BOOL CPubList::UpdateData( BOOL bRead, LPPUBINFO ppi, CRKey *pRk)
{
	// Sanity check
	if ( ppi == NULL || pRk == NULL ) return FALSE;

	// Exchange variables
	pRk->UpdateData( bRead, "Type", &ppi->type );
	pRk->UpdateData( bRead, "Interval", &ppi->interval, 30 );
	pRk->UpdateData( bRead, "MotionInterval", &ppi->motioninterval, 30 );
	pRk->UpdateData( bRead, "Motion", &ppi->motion );
	pRk->UpdateData( bRead, "F1", &ppi->f1, PUBF1_COMPRESSVIDEO );
	pRk->UpdateData( bRead, "StartIndex", &ppi->startindex, 1 );
	pRk->UpdateData( bRead, "StopIndex", &ppi->stopindex, 10 );
	pRk->UpdateData( bRead, "CurrentIndex", &ppi->curindex, 1 );
	pRk->UpdateData( bRead, "TnWidth", &ppi->tnwidth, 64 );
	pRk->UpdateData( bRead, "TnHeight", &ppi->tnheight, 48 );
	pRk->UpdateData( bRead, "TnPWidth", &ppi->tnpwidth, 25 );
	pRk->UpdateData( bRead, "TnPHeight", &ppi->tnpheight, 25 );

	pRk->UpdateData( bRead, "Image", ppi->img, sizeof( ppi->img ) );
	pRk->UpdateData( bRead, "Path", ppi->path, sizeof( ppi->path ) );
	pRk->UpdateData( bRead, "Fname", ppi->fname, sizeof( ppi->fname ) );
	pRk->UpdateData( bRead, "TnFname", ppi->tfname, sizeof( ppi->tfname ) );
	pRk->UpdateData( bRead, "Str", ppi->str, sizeof( ppi->str ) );
	pRk->UpdateData( bRead, "Template", ppi->tmpl, sizeof( ppi->tmpl ) );
	pRk->UpdateData( bRead, "Snd", ppi->snd, sizeof( ppi->snd ) );

	pRk->UpdateData( bRead, "AviCapFrames", &ppi->capframes, 1 );
	pRk->UpdateData( bRead, "AviCapSeconds", &ppi->capseconds, 2 );
	pRk->UpdateData( bRead, "AviPbFrames", &ppi->pbframes, 8 );
	pRk->UpdateData( bRead, "AviPbSeconds", &ppi->pbseconds, 1 );
	pRk->UpdateData( bRead, "fourCC", &ppi->fourCC, mmioFOURCC( 'm', 'p', 'g', '4' ) );
	
	if ( bRead )
	{
		// Email message data
		if ( ppi->pemi != NULL )
		{	ppi->pemi->to.copy( pRk->GetSz( "To" ) );
			ppi->pemi->from.copy( pRk->GetSz( "From" ) );
			ppi->pemi->subject.copy( pRk->GetSz( "Subject" ) );
			ppi->pemi->body.copy( pRk->GetSz( "Msg" ) );
		} // end if
		
		// Codec data
		if ( ppi->codecdata != NULL )
			ppi->codecdata->put( pRk->GetValuePtr( "CodecData" ),
		   						 pRk->GetValueSize( "CodecData" ) );

		// Set AVI flags
		LPCTSTR pExt = CWinFile::GetExtension( ppi->fname );
		if ( pExt != NULL && !strcmp( pExt, "avi" ) ) ppi->f1 |= PUBF1_AVI;
		else ppi->f1 &= ~PUBF1_AVI;

		// Set Thumbnail AVI flag
		pExt = CWinFile::GetExtension( ppi->tfname );
		if ( pExt != NULL && !strcmp( pExt, "avi" ) ) ppi->f1 |= PUBF1_THMAVI;
		else ppi->f1 &= ~PUBF1_THMAVI;
	
	} // end else

	else
	{
		// Email message data
		if ( ppi->pemi != NULL )
		{	pRk->Set( "To", ppi->pemi->to );
			pRk->Set( "From", ppi->pemi->from );
			pRk->Set( "Subject", ppi->pemi->subject );
			pRk->Set( "Msg", ppi->pemi->body );
		} // end if

		// Codec data
		if ( ppi->codecdata != NULL && ppi->codecdata->size() != 0 )
			pRk->Set( "CodecData", ppi->codecdata->ptr(), 
					   			   ppi->codecdata->size() );

	} // end if

	return TRUE;	
}

