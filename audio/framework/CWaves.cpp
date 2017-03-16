#include <memory.h>
#include "CWaves.h"

#pragma pack(push, 4)

typedef struct
{
	char szRIFF[4];
	unsigned long ulRIFFSize;
	char szWAVE[4];

}WaveFileHeader;

typedef struct  
{
	char szChunkName[4];
	unsigned long ulChunkSize;

} RiffChunk;

typedef struct
{
	unsigned short	usFormatTag;
	unsigned short	usChannels;
	unsigned long	ulSamplesPerSec;
	unsigned long	ulAvgBytesPerSec;
	unsigned short	usBlockAlign;
	unsigned short	usBitsPerSample;
	unsigned short	usSize;
	unsigned short  usReserved;
	unsigned long	ulChannelMask;
	GUID            guidSubFormat;
} WaveFmt;

#pragma pack(pop)

//copy from win API
#ifndef WAVE_FORMAT_PCM

/* OLD general waveform format structure (information common to all formats) */
typedef struct waveformat_tag {
	unsigned short    wFormatTag;        /* format type */
	unsigned short    nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
	unsigned long   nSamplesPerSec;    /* sample rate */
	unsigned long   nAvgBytesPerSec;   /* for buffer estimation */
	unsigned short    nBlockAlign;       /* block size of data */
} WaveFormat, *PWAVEFORMAT, NEAR *NPWAVEFORMAT, FAR *LPWAVEFORMAT;

/* flags for wFormatTag field of WAVEFORMAT */
#define WAVE_FORMAT_PCM     1

/* specific waveform format structure for PCM data */
typedef struct pcmwaveformat_tag {
	WaveFormat  wf;
	unsigned short        wBitsPerSample;
} PCMWAVEFORMAT, *PPCMWAVEFORMAT, NEAR *NPPCMWAVEFORMAT, FAR *LPPCMWAVEFORMAT;

#endif /* WAVE_FORMAT_PCM */

CWaves::CWaves()
{
	memset(&m_WaveIDs, 0, sizeof(m_WaveIDs));
}

CWaves::~CWaves()
{
	long lLoop;

	for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
	{
		if (m_WaveIDs[lLoop])
		{
			if (m_WaveIDs[lLoop]->pData)
				delete m_WaveIDs[lLoop]->pData;
			
			if (m_WaveIDs[lLoop]->pFile)
				fclose(m_WaveIDs[lLoop]->pFile);

			delete m_WaveIDs[lLoop];
			m_WaveIDs[lLoop] = 0;
		}
	}
}

WAVERESULT CWaves::LoadWaveFile(const char *szFilename, WAVEID *pWaveID)
{
	WAVERESULT wRet = WR_OUTOFMEMORY;
	LpWaveFileInfo pWaveInfo = NULL;

	do 
	{
		if (NULL == (pWaveInfo = new WaveFileInfo()))
			break;

		if ((wRet = ParseFile(szFilename, pWaveInfo)) < 0)
		{
			break;
		}

		pWaveInfo->pData = new char[pWaveInfo->ulDataSize];
		if (NULL == pWaveInfo->pData)
		{
			wRet = WR_OUTOFMEMORY;
			break;
		}

		fseek(pWaveInfo->pFile, pWaveInfo->ulDataOffset, SEEK_SET);

		int size = fread(pWaveInfo->pData, 1, pWaveInfo->ulDataSize, pWaveInfo->pFile);
		if (size != pWaveInfo->ulDataSize)
		{
			delete pWaveInfo->pData;
			wRet = WR_BADWAVEFILE;
		}

		long lLoop = 0;
		for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
		{
			if (!m_WaveIDs[lLoop])
			{
				m_WaveIDs[lLoop] = pWaveInfo;
				*pWaveID = lLoop;
				wRet = WR_OK;
				break;
			}
		}

		if (MAX_NUM_WAVEID == lLoop)
		{
			delete pWaveInfo->pData;
			wRet = WR_OUTOFMEMORY;
			break;
		}

		fclose(pWaveInfo->pFile);
		pWaveInfo->pFile = 0;

	} while (0);

	if (WR_OK != wRet)
		delete pWaveInfo;

	return wRet;
}

WAVERESULT CWaves::OpenWaveFile(const char *szFilename, WAVEID *pWaveID)
{
	WAVERESULT wRet = WR_OUTOFMEMORY;
	LpWaveFileInfo pWaveInfo;

	do 
	{
		pWaveInfo = new WaveFileInfo;
		if (NULL == pWaveInfo)
			break;
		if ((wRet = ParseFile(szFilename, pWaveInfo)) < 0)
			break;
		long lLoop = 0;
		for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
		{
			if (!m_WaveIDs[lLoop])
			{
				m_WaveIDs[lLoop] = pWaveInfo;
				*pWaveID = lLoop;
				wRet = WR_OK;
				break;
			}
		}

		if (MAX_NUM_WAVEID == lLoop)
			wRet = WR_OUTOFMEMORY;

	} while (0);

	if (WR_OK != wRet)
		delete pWaveInfo;
	
	return wRet;
}

WAVERESULT CWaves::ReadWaveData(WAVEID WaveID, void *pData, unsigned long ulDataSize, unsigned long *pulBytesWritten)
{
	LpWaveFileInfo pWaveInfo;
	WAVERESULT wRet = WR_BADWAVEFILE;

	do 
	{
		if (!pData || !pulBytesWritten || 0 == ulDataSize)
		{
			wRet = WR_INVALIDPARAM;
			break;
		}

		if (!IsWaveID(WaveID))
		{
			wRet = WR_INVALIDWAVEID;
			break;
		}

		pWaveInfo = m_WaveIDs[WaveID];
		if (pWaveInfo->pFile)
		{
			unsigned long ulOffset = ftell(pWaveInfo->pFile);
			if ((ulOffset - pWaveInfo->ulDataOffset + ulDataSize) > pWaveInfo->ulDataSize)
				ulDataSize = pWaveInfo->ulDataSize - (ulOffset - pWaveInfo->ulDataOffset);
			
			*pulBytesWritten = (unsigned long)fread(pData, 1, ulDataSize, pWaveInfo->pFile);

			wRet = WR_OK;
		}

	} while (0);

	return wRet;
}

WAVERESULT CWaves::SetWaveDataOffset(WAVEID WaveID, unsigned long ulOffset)
{
	LpWaveFileInfo pWaveInfo;
	WAVERESULT wRet = WR_INVALIDPARAM;

	do 
	{
		if (!IsWaveID(WaveID))
		{
			wRet = WR_INVALIDWAVEID;
			break;
		}
		
		pWaveInfo = m_WaveIDs[WaveID];
		if (pWaveInfo->pFile)
		{
			fseek(pWaveInfo->pFile, pWaveInfo->ulDataOffset + ulOffset, SEEK_SET);
			wRet = WR_OK;
		}

	} while (0);

	return wRet;
}

WAVERESULT CWaves::GetWaveDataOffset(WAVEID WaveID, unsigned long *pulOffset)
{
	LpWaveFileInfo pWaveInfo;
	WAVERESULT wRet = WR_INVALIDPARAM;

	do 
	{
		if (!IsWaveID(WaveID))
		{
			wRet = WR_INVALIDWAVEID;
			break;
		}

		pWaveInfo = m_WaveIDs[WaveID];
		if ((pWaveInfo->pFile) && pulOffset)
		{
			*pulOffset = ftell(pWaveInfo->pFile);
			*pulOffset -= pWaveInfo->ulDataOffset;
			wRet = WR_OK;
		}

	} while (0);

	return wRet;
}

#ifdef _WIN32
#define strnicmp _strnicmp
#define strncpy strncpy_s
#endif

#if !defined(WAVE_FORMAT_EXTENSIBLE)
#define  WAVE_FORMAT_EXTENSIBLE                 0xFFFE
#endif // !defined(WAVE_FORMAT_EXTENSIBLE)

WAVERESULT CWaves::ParseFile(const char *szFilename, LpWaveFileInfo pWaveInfo)
{
	WaveFileHeader waveFileHeader;
	RiffChunk riffChunk;
	WaveFmt waveFmt;
	WAVERESULT wRet = WR_BADWAVEFILE;

	do 
	{
		if (!szFilename || !pWaveInfo)
		{
			wRet = WR_INVALIDPARAM;
			break;
		}

		memset(pWaveInfo, 0, sizeof(WaveFileInfo));

#ifdef _WIN32
		fopen_s(&pWaveInfo->pFile, szFilename, "rb");
#elif linux
		pWaveInfo->pFile = fopen(szFilename, "rb");
#endif   //_win32

		if (!pWaveInfo->pFile)
		{
			wRet = WR_INVALIDFILENAME;
			break;
		}

		fread(&waveFileHeader, 1, sizeof(WaveFileHeader), pWaveInfo->pFile);
		if (!strnicmp(waveFileHeader.szRIFF, "RIFF", 4) && !strnicmp(waveFileHeader.szWAVE, "WAVE", 4))
		{
			while (fread(&riffChunk, 1, sizeof(RiffChunk), pWaveInfo->pFile) == sizeof(RiffChunk))
			{
				if (!strnicmp(riffChunk.szChunkName, "fmt ", 4))
				{
					if (riffChunk.ulChunkSize <= sizeof(WaveFmt))
					{
						fread(&waveFmt, 1, riffChunk.ulChunkSize, pWaveInfo->pFile);

						if (waveFmt.usFormatTag == WAVE_FORMAT_PCM)
						{
							pWaveInfo->wfType = WF_EX;
							memcpy(&pWaveInfo->wfEXT.Format, &waveFmt, sizeof(PCMWAVEFORMAT));
						}
						else if (waveFmt.usFormatTag == WAVE_FORMAT_EXTENSIBLE)
						{
							pWaveInfo->wfType = WF_EXT;
							memcpy(&pWaveInfo->wfEXT, &waveFmt, sizeof(WaveFormatExtensible));
						}
					}
					else
					{
						fseek(pWaveInfo->pFile, riffChunk.ulChunkSize, SEEK_CUR);;
					}
				}
				else if (!strnicmp(riffChunk.szChunkName, "data", 4))
				{
					pWaveInfo->ulDataSize = riffChunk.ulChunkSize;
					pWaveInfo->ulDataOffset = ftell(pWaveInfo->pFile);
					fseek(pWaveInfo->pFile, riffChunk.ulChunkSize, SEEK_CUR);
				}
				else
				{
					fseek(pWaveInfo->pFile, riffChunk.ulChunkSize, SEEK_CUR);
				}

				if (riffChunk.ulChunkSize & 1)
				{
					fseek(pWaveInfo->pFile, riffChunk.ulChunkSize, SEEK_CUR);
				}
			}

			if (pWaveInfo->ulDataSize && pWaveInfo->ulDataOffset && ((pWaveInfo->wfType == WF_EX) || (pWaveInfo->wfType == WF_EXT)))
				wRet = WR_OK;
			else
				fclose(pWaveInfo->pFile);
		}

	} while (0);

	return wRet;
}

WAVERESULT CWaves::DeleteWaveFile(WAVEID WaveID)
{
	WAVERESULT wRet = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (m_WaveIDs[WaveID]->pData)
			delete m_WaveIDs[WaveID]->pData;

		if (m_WaveIDs[WaveID]->pFile)
			fclose(m_WaveIDs[WaveID]->pFile);

		delete m_WaveIDs[WaveID];
		m_WaveIDs[WaveID] = 0;
	}
	else
	{
		wRet = WR_INVALIDWAVEID;
	}

	return wRet;
}

WAVERESULT CWaves::GetWaveType(WAVEID WaveID, WAVEFILETYPE *pwfType)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!pwfType)
		return WR_INVALIDPARAM;

	*pwfType = m_WaveIDs[WaveID]->wfType;

	return WR_OK;
}

WAVERESULT CWaves::GetWaveFormatExHeader(WAVEID WaveID, WaveFormatEx *pWFEX)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!pWFEX)
		return WR_INVALIDPARAM;

	memcpy(pWFEX, &(m_WaveIDs[WaveID]->wfEXT.Format), sizeof(WaveFormatEx));

	return WR_OK;
}

WAVERESULT CWaves::GetWaveFormatExtensibleHeader(WAVEID WaveID, WaveFormatExtensible *pWFEXT)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (m_WaveIDs[WaveID]->wfType != WF_EXT)
		return WR_NOTWAVEFORMATEXTENSIBLEFORMAT;

	if (!pWFEXT)
		return WR_INVALIDPARAM;

	memcpy(pWFEXT, &(m_WaveIDs[WaveID]->wfEXT), sizeof(WaveFormatExtensible));

	return WR_OK;
}

WAVERESULT CWaves::GetWaveData(WAVEID WaveID, void **ppAudioData)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!ppAudioData)
		return WR_INVALIDPARAM;

	*ppAudioData = m_WaveIDs[WaveID]->pData;

	return WR_OK;
}

WAVERESULT CWaves::GetWaveSize(WAVEID WaveID, unsigned long *pulDataSize)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!pulDataSize)
		return WR_INVALIDPARAM;

	*pulDataSize = m_WaveIDs[WaveID]->ulDataSize;

	return WR_OK;
}

WAVERESULT CWaves::GetWaveFrequency(WAVEID WaveID, unsigned long *pulFrequency)
{
	WAVERESULT wRet = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (pulFrequency)
			*pulFrequency = m_WaveIDs[WaveID]->wfEXT.Format.nSamplesPerSec;
		else
			wRet = WR_INVALIDPARAM;
	}
	else
	{
		wRet = WR_INVALIDWAVEID;
	}

	return WR_OK;
}

// Speaker Positions:
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000

WAVERESULT CWaves::GetWaveALBufferFormat(WAVEID WaveID, PFNALGETENUMVALUE pfnGetEnumValue, unsigned long *pulFormat)
{
	WAVERESULT wRet = WR_OK;

	do 
	{
		if (!IsWaveID(WaveID))
		{
			wRet = WR_INVALIDWAVEID;
			break;
		}

		if (!(pfnGetEnumValue && pulFormat))
		{
			wRet = WR_INVALIDPARAM;
			break;
		}

		*pulFormat = 0;

		if (m_WaveIDs[WaveID]->wfType == WF_EX)
		{
			if (1 == m_WaveIDs[WaveID]->wfEXT.Format.nChannels)
			{
				switch (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample)
				{
				case 4:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO_IMA4");
					break;
				case 8:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO8");
					break;
				case 16:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO16");
					break;
				}
			}
			else if (2 == m_WaveIDs[WaveID]->wfEXT.Format.nChannels)
			{
				switch (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample)
				{
				case 4:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO_IMA4");
					break;
				case 8:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO8");
					break;
				case 16:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO16");
					break;
				}
			}
			else if ((4 == m_WaveIDs[WaveID]->wfEXT.Format.nChannels) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16))
			{
				*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
			}
		}
		else if (m_WaveIDs[WaveID]->wfType == WF_EXT)
		{
			if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 1) &&
				((m_WaveIDs[WaveID]->wfEXT.dwChannelMask == SPEAKER_FRONT_CENTER) ||
				(m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)) ||
				(m_WaveIDs[WaveID]->wfEXT.dwChannelMask == 0)))
			{
				switch (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample)
				{
				case 4:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO_IMA4");
					break;
				case 8:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO8");
					break;
				case 16:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO16");
					break;
				}
			}
			else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 2) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)))
			{
				switch (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample)
				{
				case 4:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO_IMA4");
					break;
				case 8:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO8");
					break;
				case 16:
					*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO16");
					break;
				}
			}
			else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 2) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_REAR16");
			else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 4) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
			else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 6) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_51CHN16");
			else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 7) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_BACK_CENTER)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_61CHN16");
			else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 8) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_71CHN16");
		}

		if (0 == *pulFormat)
			wRet = WR_INVALIDWAVEFILETYPE;

	} while (0);


	return wRet;
}

bool CWaves::IsWaveID(WAVEID WaveID)
{
	bool bRet = false;

	if ((WaveID >= 0) && (WaveID < MAX_NUM_WAVEID))
	{
		if (m_WaveIDs[WaveID])
			bRet = true;
	}

	return bRet;
}

char *CWaves::GetErrorString(WAVERESULT wr, char *szErrorString, unsigned long nSizeOfErrorString)
{
	switch (wr)
	{
	case WR_OK:
		strncpy(szErrorString, nSizeOfErrorString, "Success\n", nSizeOfErrorString - 1);
		break;

	case WR_INVALIDFILENAME:
		strncpy(szErrorString, nSizeOfErrorString, "Invalid file name or file does not exist\n", nSizeOfErrorString - 1);
		break;

	case WR_BADWAVEFILE:
		strncpy(szErrorString, nSizeOfErrorString, "Invalid Wave file\n", nSizeOfErrorString - 1);
		break;

	case WR_INVALIDPARAM:
		strncpy(szErrorString, nSizeOfErrorString, "Invalid parameter passed to function\n", nSizeOfErrorString - 1);
		break;

	case WR_FILEERROR:
		strncpy(szErrorString, nSizeOfErrorString, "File I/O error\n", nSizeOfErrorString - 1);
		break;

	case WR_INVALIDWAVEID:
		strncpy(szErrorString, nSizeOfErrorString, "Invalid WAVEID\n", nSizeOfErrorString - 1);
		break;

	case WR_NOTSUPPORTEDYET:
		strncpy(szErrorString, nSizeOfErrorString, "Function not supported yet\n", nSizeOfErrorString - 1);
		break;

	case WR_WAVEMUSTBEMONO:
		strncpy(szErrorString, nSizeOfErrorString, "Input wave files must be mono\n", nSizeOfErrorString - 1);
		break;

	case WR_WAVEMUSTBEWAVEFORMATPCM:
		strncpy(szErrorString, nSizeOfErrorString, "Input wave files must be in Wave Format PCM\n", nSizeOfErrorString - 1);
		break;

	case WR_WAVESMUSTHAVESAMEBITRESOLUTION:
		strncpy(szErrorString, nSizeOfErrorString, "Input wave files must have the same Bit Resolution\n", nSizeOfErrorString - 1);
		break;

	case WR_WAVESMUSTHAVESAMEFREQUENCY:
		strncpy(szErrorString, nSizeOfErrorString, "Input wave files must have the same Frequency\n", nSizeOfErrorString - 1);
		break;

	case WR_WAVESMUSTHAVESAMEBITRATE:
		strncpy(szErrorString, nSizeOfErrorString, "Input wave files must have the same Bit Rate\n", nSizeOfErrorString - 1);
		break;

	case WR_WAVESMUSTHAVESAMEBLOCKALIGNMENT:
		strncpy(szErrorString, nSizeOfErrorString, "Input wave files must have the same Block Alignment\n", nSizeOfErrorString - 1);
		break;

	case WR_OFFSETOUTOFDATARANGE:
		strncpy(szErrorString, nSizeOfErrorString, "Wave files Offset is not within audio data\n", nSizeOfErrorString - 1);
		break;

	case WR_INVALIDSPEAKERPOS:
		strncpy(szErrorString, nSizeOfErrorString, "Invalid Speaker Destinations\n", nSizeOfErrorString - 1);
		break;

	case WR_OUTOFMEMORY:
		strncpy(szErrorString, nSizeOfErrorString, "Out of memory\n", nSizeOfErrorString - 1);
		break;

	case WR_INVALIDWAVEFILETYPE:
		strncpy(szErrorString, nSizeOfErrorString, "Invalid Wave File Type\n", nSizeOfErrorString - 1);
		break;

	case WR_NOTWAVEFORMATEXTENSIBLEFORMAT:
		strncpy(szErrorString, nSizeOfErrorString, "Wave file is not in WAVEFORMATEXTENSIBLE format\n", nSizeOfErrorString - 1);
		break;

	default:
		strncpy(szErrorString, nSizeOfErrorString, "Undefined error\n", nSizeOfErrorString - 1);
	}
	szErrorString[nSizeOfErrorString - 1] = '\0';
	return szErrorString;
}