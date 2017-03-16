#pragma once
//********************************************************************
//    audio-CWaves.h
//    文件名  :    CWAVES.H
//    文件路径:    E:\IMAYCODE\AUDIO\AUDIO\SOURCE/
//    作者    :    Ethan
//    创建时间:    2017/3/15 9:54
//    文件描述:    
//*********************************************************************
#ifndef _C_WAVES_H_
#define _C_WAVES_H_

#include <stdio.h>

#define MAX_NUM_WAVEID			1024

enum WAVEFILETYPE
{
	WF_EX = 1,
	WF_EXT = 2
};

enum WAVERESULT
{
	WR_OK = 0,
	WR_INVALIDFILENAME = -1,
	WR_BADWAVEFILE = -2,
	WR_INVALIDPARAM = -3,
	WR_INVALIDWAVEID = -4,
	WR_NOTSUPPORTEDYET = -5,
	WR_WAVEMUSTBEMONO = -6,
	WR_WAVEMUSTBEWAVEFORMATPCM = -7,
	WR_WAVESMUSTHAVESAMEBITRESOLUTION = -8,
	WR_WAVESMUSTHAVESAMEFREQUENCY = -9,
	WR_WAVESMUSTHAVESAMEBITRATE = -10,
	WR_WAVESMUSTHAVESAMEBLOCKALIGNMENT = -11,
	WR_OFFSETOUTOFDATARANGE = -12,
	WR_FILEERROR = -13,
	WR_OUTOFMEMORY = -14,
	WR_INVALIDSPEAKERPOS = -15,
	WR_INVALIDWAVEFILETYPE = -16,
	WR_NOTWAVEFORMATEXTENSIBLEFORMAT = -17
};

typedef struct tWaveFormatEx
{
	unsigned short	wFormatTag;
	unsigned short    nChannels;
	unsigned long   nSamplesPerSec;
	unsigned long   nAvgBytesPerSec;
	unsigned short    nBlockAlign;
	unsigned short    wBitsPerSample;
	unsigned short    cbSize;
} WaveFormatEx;

#ifndef _WIN32
typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID;
#else
#include <windows.h>
#endif //WIN32

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
typedef struct {
	WaveFormatEx    Format;
	union {
		unsigned short wValidBitsPerSample;       /* bits of precision  */
		unsigned short wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
		unsigned short wReserved;                 /* If neither applies, set to zero. */
	} Samples;
	unsigned long           dwChannelMask;      /* which channels are */
										/* present in stream  */
	GUID            SubFormat;
} WaveFormatExtensible, *PWaveFormatExtensible;
#endif // !_WAVEFORMATEXTENSIBLE_

typedef struct
{
	WAVEFILETYPE	wfType;
	WaveFormatExtensible wfEXT;		// For non-WAVEFORMATEXTENSIBLE wavefiles, the header is stored in the Format member of wfEXT
	char			*pData;
	unsigned long	ulDataSize;
	FILE			*pFile;
	unsigned long	ulDataOffset;
} WaveFileInfo, *LpWaveFileInfo;

typedef int(__cdecl *PFNALGETENUMVALUE)(const char *szEnumName);
typedef int	WAVEID;

class CWaves
{
public:
	CWaves();
	virtual ~CWaves();

	WAVERESULT LoadWaveFile(const char *szFilename, WAVEID *WaveID);
	WAVERESULT OpenWaveFile(const char *szFilename, WAVEID *WaveID);
	WAVERESULT ReadWaveData(WAVEID WaveID, void *pData, unsigned long ulDataSize, unsigned long *pulBytesWritten);
	WAVERESULT SetWaveDataOffset(WAVEID WaveID, unsigned long ulOffset);
	WAVERESULT GetWaveDataOffset(WAVEID WaveID, unsigned long *pulOffset);
	WAVERESULT GetWaveType(WAVEID WaveID, WAVEFILETYPE *pwfType);
	WAVERESULT GetWaveFormatExHeader(WAVEID WaveID, WaveFormatEx *pWFEX);
	WAVERESULT GetWaveFormatExtensibleHeader(WAVEID WaveID, WaveFormatExtensible *pWFEXT);
	WAVERESULT GetWaveData(WAVEID WaveID, void **ppAudioData);
	WAVERESULT GetWaveSize(WAVEID WaveID, unsigned long *pulDataSize);
	WAVERESULT GetWaveFrequency(WAVEID WaveID, unsigned long *pulFrequency);
	WAVERESULT GetWaveALBufferFormat(WAVEID WaveID, PFNALGETENUMVALUE pfnGetEnumValue, unsigned long *pulFormat);
	WAVERESULT DeleteWaveFile(WAVEID WaveID);

	char *GetErrorString(WAVERESULT wr, char *szErrorString, unsigned long nSizeOfErrorString);
	bool IsWaveID(WAVEID WaveID);

private:
	WAVERESULT ParseFile(const char *szFilename, LpWaveFileInfo pWaveInfo);
	WAVEID InsertWaveID(LpWaveFileInfo pWaveFileInfo);

	LpWaveFileInfo	m_WaveIDs[MAX_NUM_WAVEID];
};

#endif //_C_WAVES_H_