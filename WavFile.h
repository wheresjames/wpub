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
// WavFile.h: interface for the CWavFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVFILE_H__3F22A56D_FC22_4DC1_BED3_F528ED43B869__INCLUDED_)
#define AFX_WAVFILE_H__3F22A56D_FC22_4DC1_BED3_F528ED43B869__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWavFile  
{
public:
	BOOL LoadFromResource( LPCTSTR pRes, LPCTSTR pResType );
	BOOL Load( HMMIO hMmio );
	BOOL Load( LPVOID buf, DWORD size );
	BOOL Load( LPCTSTR pFile );
	CWavFile();
	virtual ~CWavFile();

	LPBYTE GetBuffer() { return m_mem.ptr(); }
	DWORD GetBufferSize() { return m_mem.size(); }

	LPWAVEFORMATEX GetFormat() { return (LPWAVEFORMATEX)m_apWf.ptr(); }

private:

	TPodAutoPtr< WAVEFORMATEX >		m_apWf;
	TMem< BYTE >					m_mem;

};

#endif // !defined(AFX_WAVFILE_H__3F22A56D_FC22_4DC1_BED3_F528ED43B869__INCLUDED_)
