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
// VideoList.cpp: implementation of the CVideoList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vp.h"
#include "VideoList.h"

#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoList::CVideoList()
{

}

CVideoList::~CVideoList()
{

}

void CVideoList::DeleteObject(void * node)
{
	LPVIDEOINFO pvi = (LPVIDEOINFO)node;
	if ( pvi == NULL ) return;

	// Release offscreen memory class
	if ( pvi->cap != NULL ) 
	{	pvi->cap->StopThread();
		delete pvi->cap;
		pvi->cap = NULL;
	} // end if

	// Release offscreen memory class
	if ( pvi->os != NULL ) 
	{	delete pvi->os;
		pvi->os = NULL;
	} // end if

	// Release offscreen memory class
	if ( pvi->tavg != NULL ) 
	{	delete pvi->tavg;
		pvi->tavg = NULL;
	} // end if

	// Release offscreen memory class
	if ( pvi->mos != NULL ) 
	{	delete pvi->mos;
		pvi->mos = NULL;
	} // end if

	// Release offscreen memory class
	if ( pvi->imgfx != NULL ) 
	{	delete pvi->imgfx;
		pvi->imgfx = NULL;
	} // end if

	// Release motion memory
	CWinDc::ReleaseMotion( &pvi->motion );

	CLList::DeleteObject( node );
}

LPVIDEOINFO CVideoList::Add( DWORD i )
{
	// See if it already exists
	LPVIDEOINFO node = FindByIndex( i );
	if ( node != NULL )
	{
		// Connect to driver if needed
		if ( !node->cap->IsConnected() ) Connect( node );

		// Ensure it is running
//		node->cap->SetFrameRate( node->framerate );

		return node;
	} // end if
	
	// Allocate new memory
	node = (LPVIDEOINFO)New();
	if ( node == NULL ) return NULL;

	// Set driver index
	node->index = i;
	node->active = 0;
	node->framerate = DEFFRAMERATE;

	// Allocate offscreen memory class
	node->os = new CWinDc();
	if ( node->os == NULL ) { Delete( node ); return NULL; }

	// Allocate offscreen memory class
	node->tavg = new CWinDc();
	if ( node->tavg == NULL ) { Delete( node ); return NULL; }

	// Allocate video capture class
	node->cap = new CCaptureThread();
	if ( node->cap == NULL ) { Delete( node ); return NULL; }

	// Allocate motion offscreen memory class
	node->mos = new CWinDc();
	if ( node->mos == NULL ) { Delete( node ); return NULL; }

	// Allocate memory for fx
	node->imgfx = new CImgEffects();
	if ( node->imgfx == NULL ) { Delete( node ); return NULL; }

	// Set default motion params
	CWinDc::DefaultMotionStruct( &node->motion );

	// Load settings
	LoadSettings( node );

	// Connect to driver
	Connect( node );

	// Ensure it is running
	node->cap->SetFrameRate( node->framerate );

	return node;
}


LPVIDEOINFO CVideoList::FindByIndex(DWORD i)
{
	if ( i >= Size() ) return NULL;

	LPVIDEOINFO pvi = NULL;

	// Search for driver index
	while( ( pvi = (LPVIDEOINFO)GetNext( pvi ) ) != NULL )
		if ( pvi->index == i ) return pvi;

	return NULL;
}

BOOL CVideoList::Connect(LPVIDEOINFO pVi)
{
	if ( pVi == NULL ) return FALSE;
	if ( pVi->cap == NULL ) return FALSE;
	if ( pVi->cap->IsConnected() ) return TRUE;

	char title[ 256 ];
	wsprintf( title, "Live Video %lu", pVi->index );

	// Inform user
//	CDlgInform inform;
//	inform.Run( "Connecting Video Driver..." );

	// Create live video
//	inform.SetStatus( "Creating video window" );
	if ( !pVi->cap->Create(	title, FRAME()->GetSafeHwnd(), 
								WS_CHILD, 1, 1, 1, 1 ) ) 
	{	_Log( MB_ICONERROR, pVi->name, "Unable to connect capture device" );
		return FALSE; }

	// Connect the capture driver
//	inform.SetStatus( "Loading video driver" );
	if ( !pVi->cap->Connect( pVi->index ) )
	{	_Log( MB_ICONERROR, pVi->name, "Unable to connect capture device" );
		return FALSE;
	} // end if

	// Get image properties
	LPBITMAPINFO pbmp = NULL;
//	inform.SetStatus( "Reading video format information" );
	if ( !pVi->cap->GetVideoFormat( &pbmp ) || pbmp == NULL )
	{	_Log( MB_ICONERROR, pVi->name, "Unable to get video information" );
		return FALSE; 
	}

	// Create memory for video frames
//	inform.SetStatus( "Creating video memory" );
	if ( !pVi->os->CreateDIBSection(	NULL,
										pbmp->bmiHeader.biWidth,
										pbmp->bmiHeader.biHeight ) )
//	if ( !pVi->os->Create(	NULL, 0, 0,
//							pbmp->bmiHeader.biWidth,
//							pbmp->bmiHeader.biHeight ) )
	{	_Log( MB_ICONERROR, pVi->name, "Unable to create video memory" );
		return FALSE; 
	}

	// Setup for drawing and callbacks
//	inform.SetStatus( "Setting up video driver" );
	pVi->cap->SetFrameMessageTarget(	*pVi->os, *pVi->os, 
										FRAME()->GetSafeHwnd(), WM_FRAMEMSG );

	// Enable callbacks
	pVi->cap->EnableCallbacks();

	pVi->cap->SetFrameRate( pVi->framerate );

	// Go ahead and update one frame
//	inform.SetStatus( "Testing video driver" );
	pVi->cap->GrabFrame();
	pVi->cap->WaitCapture();

	return TRUE;
}

BOOL CVideoList::SetFrameRate(DWORD i, DWORD rate)
{
	// Get video object
	LPVIDEOINFO pvi = FindByIndex( i );
	if ( pvi == NULL ) return FALSE;
	if ( pvi->cap == NULL ) return FALSE;

	// Reconnect if needed
	if ( !pvi->cap->IsConnected() ) Connect( pvi );

	// Set the frame rate
	pvi->cap->SetFrameRate( rate );

	return TRUE;
}

BOOL CVideoList::SetFrameRates( DWORD rate )
{
	LPVIDEOINFO pvi = NULL;

	// Search for driver index
	while( ( pvi = (LPVIDEOINFO)GetNext( pvi ) ) != NULL )
	{
		if ( pvi->active == 0 && pvi->cap->GetFrameRate() != 0 ) 

			// Set the frame rate
			pvi->cap->SetFrameRate( 0 );

		else if (	pvi->active > 0 && 
					( !pvi->cap->IsConnected() || pvi->cap->GetFrameRate() != rate ) )
		{
			// Reconnect if needed
			if ( !pvi->cap->IsConnected() ) Connect( pvi );

			if ( rate == MAXDWORD ) 

				// Set the frame rate
				pvi->cap->SetFrameRate( pvi->framerate );

			else pvi->cap->SetFrameRate( rate );

		} // end else

	} // end while

	return NULL;
}

BOOL CVideoList::LoadSettings(LPVIDEOINFO pVi)
{
	if ( pVi == NULL ) return FALSE;

	// Get settings key
	char name[ CWF_STRSIZE ];
	wsprintf( name, "Driver %lu", (DWORD)pVi->index );
	CRKey *pRk = VIDEOSETTINGS().FindKey( name );
	if ( pRk == NULL ) 
	{	ZeroMemory( &pVi->motion, sizeof( pVi->motion ) );
		pVi->motion.thmotion = 500;
		pVi->motion.thbright = 500;
		return FALSE;
	} // end if

	// Set default motion params
	CWinDc::DefaultMotionStruct( &pVi->motion );

	pVi->motion.thmotion = pRk->GetDword( "THMotion", 500 );
	pVi->motion.thbright = pRk->GetDword( "THBright", 500 );
	pVi->motion.scantype = pRk->GetDword( "ScanType", 0 );
	pVi->motion.bEnable = pRk->GetDword( "EnableMotionDetection", 0 );
	pVi->frameavgweight = pRk->GetDword( "FrameAvgWeight", 50 );
	pVi->framerate = pRk->GetDword( "FrameRate", 30 );
	pVi->f1 = pRk->GetDword( "Flags1", 0 );
	pRk->Get( "MotionSound", pVi->motionsnd, sizeof( pVi->motionsnd ) );
	if ( pVi->imgfx != NULL ) pVi->imgfx->Load( pRk, "ImageFx" );

	return TRUE;
}

BOOL CVideoList::SaveSettings(LPVIDEOINFO pVi)
{
	if ( pVi == NULL ) return FALSE;

	// Get settings key
	char name[ CWF_STRSIZE ];
	wsprintf( name, "Driver %lu", (DWORD)pVi->index );
	CRKey *pRk = VIDEOSETTINGS().GetKey( name );
	if ( pRk == NULL ) return FALSE;
	
	pRk->Set( "THMotion", (DWORD)pVi->motion.thmotion );
	pRk->Set( "THBright", (DWORD)pVi->motion.thbright );
	pRk->Set( "ScanType", (DWORD)pVi->motion.scantype );
	pRk->Set( "EnableMotionDetection", (DWORD)pVi->motion.bEnable );
	pRk->Set( "FrameAvgWeight", (DWORD)pVi->frameavgweight );
	pRk->Set( "FrameRate", (DWORD)pVi->framerate );
	pRk->Set( "Flags1", (DWORD)pVi->f1 );
	pRk->Set( "MotionSound", pVi->motionsnd );
	if ( pVi->imgfx != NULL ) pVi->imgfx->Save( pRk, "ImageFx" );

	return TRUE;
}

BOOL CVideoList::WriteSettings()
{
	// Write all settings
	LPVIDEOINFO pvi = NULL;
	while( ( pvi = (LPVIDEOINFO)GetNext( pvi ) ) != NULL )
		SaveSettings( pvi );

	return TRUE;
}

DWORD CVideoList::GetFrameRate(DWORD i)
{
	// Get video object
	LPVIDEOINFO pvi = FindByIndex( i );
	if ( pvi == NULL ) return DEFFRAMERATE;
	return pvi->framerate;
}
