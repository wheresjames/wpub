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
// WavFile.cpp: implementation of the CWavFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "WavFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWavFile::CWavFile()
{

}

CWavFile::~CWavFile()
{

}

BOOL CWavFile::Load(LPCTSTR pFile)
{
	if ( pFile == NULL ) return FALSE;

	// Open file
	HMMIO hMmio = mmioOpen( (LPSTR)pFile, NULL, MMIO_ALLOCBUF | MMIO_READ );
	if ( hMmio == NULL ) return FALSE;

	// Load the file
	BOOL ret = Load( hMmio );

	// Close the handle
	mmioClose( hMmio, 0 );

	return ret;
}

BOOL CWavFile::Load(LPVOID buf, DWORD size)
{
	// Sanity check
	if ( buf == NULL || size == 0 ) return FALSE;

	// Save buffer information
	MMIOINFO	mmi;
	ZeroMemory( &mmi, sizeof( mmi ) );
	mmi.fccIOProc = FOURCC_MEM;
	mmi.cchBuffer = size;
	mmi.pchBuffer = (char*)buf;
	
	// Open mm handle
	HMMIO hMmio = mmioOpen( NULL, &mmi, 0 );
	if ( hMmio == NULL ) return FALSE;

	// Load the file
	BOOL ret = Load( hMmio );

	// Close the handle
	mmioClose( hMmio, 0 );

	return ret;
}

BOOL CWavFile::Load( HMMIO hMmio )
{
	if ( hMmio == NULL ) return FALSE;

	try
	{
		MMCKINFO		mmi;
		MMCKINFO		in;
		PCMWAVEFORMAT	pwf;

		// Get chunk
		ZeroMemory( &mmi, sizeof( mmi ) );
		if( mmioDescend( hMmio, &mmi, NULL, 0 ) != MMSYSERR_NOERROR )
			return FALSE;

		// Validate file type
		if( mmi.ckid != FOURCC_RIFF ||
			mmi.fccType != mmioFOURCC( 'W', 'A', 'V', 'E' ) )
			return FALSE;

		// Get format chunk
		ZeroMemory( &in, sizeof( in ) );
		in.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
		if( mmioDescend( hMmio, &in, &mmi, MMIO_FINDCHUNK ) != MMSYSERR_NOERROR ||
			in.cksize < sizeof( PCMWAVEFORMAT ) ) return FALSE;

		// Read format information
		if ( mmioRead( hMmio, (HPSTR)&pwf, sizeof( pwf ) ) 
				!= sizeof( pwf ) ) return FALSE;

		// How many extra bytes
		WORD dwExtraBytes = 0;
		if( pwf.wf.wFormatTag != WAVE_FORMAT_PCM )
			if( mmioRead( hMmio, (HPSTR)&dwExtraBytes, sizeof( dwExtraBytes ) ) 
					!= sizeof( dwExtraBytes ) ) return FALSE;

		// Allocate memory
		if ( !m_apWf.allocate( sizeof( WAVEFORMATEX ) + dwExtraBytes ) )
			return FALSE;

		// Copy data
		memcpy( m_apWf.ptr(), &pwf, sizeof( pwf ) );
		m_apWf->cbSize = dwExtraBytes;

		// Read in the extra data
		if ( mmioRead( hMmio, (HPSTR)m_apWf.ptr( sizeof( WAVEFORMATEX ) ), dwExtraBytes )
				!= dwExtraBytes ) return FALSE;

	    // Ascend the input file out of the 'fmt ' chunk.                                                            
	    if( mmioAscend( hMmio, &in, 0 ) != MMSYSERR_NOERROR )
			return FALSE;

		// Seek to start of data
		if( mmioSeek( hMmio, mmi.dwDataOffset + sizeof( FOURCC ), SEEK_SET ) == -1 )
			return FALSE;

		// Get data chunk
		ZeroMemory( &in, sizeof( in ) );
		in.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );
		if ( mmioDescend( hMmio, &in, &mmi, MMIO_FINDCHUNK ) != MMSYSERR_NOERROR )
			return FALSE;

		// Allocate memory
		if ( !m_mem.allocate( in.cksize ) ) return FALSE;

		MMIOINFO mmio;
		if ( mmioGetInfo( hMmio, &mmio, 0 ) != MMSYSERR_NOERROR )
			return FALSE;

		DWORD ptr = 0;
		DWORD total = in.cksize;
		do
		{
			// How many bytes to copy
			DWORD copy = mmio.pchEndRead - mmio.pchNext;

			// Do copy if any
			if ( copy > 0 ) 
			{	
				// Don't overflow the buffer
				if ( copy > ( total - ptr ) ) copy = ( total - ptr );

				// Copy the data
				memcpy( m_mem.ptr( ptr ), mmio.pchNext, copy );

				// Update data pointer
				ptr += copy;

			} // end if

			// Point to the end of the buffer
			mmio.pchNext = mmio.pchEndRead;

		// Get more data
		} while ( ptr < total && mmioAdvance( hMmio, &mmio, MMIO_READ ) == 0 );

	} // end try

	catch( ... ) { ASSERT( 0 ); }	

	return TRUE;
}

BOOL CWavFile::LoadFromResource(LPCTSTR pRes, LPCTSTR pResType)
{
	// Sanity checks
	if ( pRes == NULL || pResType == NULL ) return FALSE;

	DWORD	size = 0;
	
	// Get resource size
	if ( !CWinFile::LoadResource( pRes, NULL, &size, pResType, NULL ) || size == 0 )
		return FALSE;

	// Allocate memory
	TMem< BYTE > buf;
	if ( !buf.allocate( size ) ) return FALSE;

	// Load resource
	if ( !CWinFile::LoadResource( pRes, buf, &size, pResType, NULL ) )
		return FALSE;

	// Decode the image
	return Load( buf, size );
}
