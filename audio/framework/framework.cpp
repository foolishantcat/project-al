#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include "framework.h"
#include "CALDeviceList.h"
#include "CWaves.h"

static CWaves *g_pWaveLoader = NULL;

// Imported EFX functions

// Effect objects
LPALGENEFFECTS alGenEffects = NULL;
LPALDELETEEFFECTS alDeleteEffects = NULL;
LPALISEFFECT alIsEffect = NULL;
LPALEFFECTI alEffecti = NULL;
LPALEFFECTIV alEffectiv = NULL;
LPALEFFECTF alEffectf = NULL;
LPALEFFECTFV alEffectfv = NULL;
LPALGETEFFECTI alGetEffecti = NULL;
LPALGETEFFECTIV alGetEffectiv = NULL;
LPALGETEFFECTF alGetEffectf = NULL;
LPALGETEFFECTFV alGetEffectfv = NULL;

//Filter objects
LPALGENFILTERS alGenFilters = NULL;
LPALDELETEFILTERS alDeleteFilters = NULL;
LPALISFILTER alIsFilter = NULL;
LPALFILTERI alFilteri = NULL;
LPALFILTERIV alFilteriv = NULL;
LPALFILTERF alFilterf = NULL;
LPALFILTERFV alFilterfv = NULL;
LPALGETFILTERI alGetFilteri = NULL;
LPALGETFILTERIV alGetFilteriv = NULL;
LPALGETFILTERF alGetFilterf = NULL;
LPALGETFILTERFV alGetFilterfv = NULL;

// Auxiliary slot object
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = NULL;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = NULL;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = NULL;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = NULL;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = NULL;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = NULL;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = NULL;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = NULL;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = NULL;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = NULL;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv = NULL;

// XRAM functions and enum values

LPEAXSETBUFFERMODE eaxSetBufferMode = NULL;
LPEAXGETBUFFERMODE eaxGetBufferMode = NULL;


ALvoid ALFWInit()
{
	g_pWaveLoader = new CWaves();
}

#ifdef _WIN32
#define getch _getch
#endif

ALvoid ALFWShutdown()
{
	if (g_pWaveLoader)
	{
		delete g_pWaveLoader;
		g_pWaveLoader = NULL;
	}

	ALFWprintf("\nPress a key to quit\n");
	ALchar ch = getch();
}

ALboolean ALFWInitOpenAL()
{
	CALDeviceList* pDeviceList = NULL;
	ALCcontext* pContext = NULL;
	ALCdevice* pDevice = NULL;
	ALint i;
	ALboolean bReturn = AL_FALSE;

	pDeviceList = new CALDeviceList();
	if ((pDeviceList) && (pDeviceList->GetNumDevices()))
	{
		ALFWprintf("\nSelect OpenAL Device:\n");
		for (i = 0; i < pDeviceList->GetNumDevices(); i++)
		{
			ALFWprintf("%d. %s%s\n", i + 1, pDeviceList->GetDeviceName(i), i == pDeviceList->GetDefaultDevice() ? "(DEFAULT)" : "");
		}
		do 
		{
			ALchar ch = getch();
			i = atoi(&ch);
		} while ((i < 1) || (i > pDeviceList->GetNumDevices()));

		pDevice = alcOpenDevice(pDeviceList->GetDeviceName(i - 1));
		if (pDevice)
		{
			pContext = alcCreateContext(pDevice, NULL);
			if (pContext)
			{
				ALFWprintf("\nOpened %s Device\n", alcGetString(pDevice, ALC_DEVICE_SPECIFIER));
				alcMakeContextCurrent(pContext);
				bReturn = AL_TRUE;
			}
			else
			{
				alcCloseDevice(pDevice);
			}
		}
		delete pDeviceList;
	}
	return bReturn;
}

ALboolean ALFWShutdownOpenAL()
{
	ALCcontext* pContext = NULL;
	ALCdevice* pDevice = NULL;

	pContext = alcGetCurrentContext();
	pDevice = alcGetContextsDevice(pContext);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(pContext);
	alcCloseDevice(pDevice);

	return AL_TRUE;
}

ALboolean ALFWIsEFXSupported()
{
	ALCdevice *pDevice = NULL;
	ALCcontext *pContext = NULL;
	ALboolean bEFXSupport = AL_FALSE;

	pContext = alcGetCurrentContext();
	pDevice = alcGetContextsDevice(pContext);

	if (alcIsExtensionPresent(pDevice, (ALCchar*)ALC_EXT_EFX_NAME))
	{
		// Get function pointers
		alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
		alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
		alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
		alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
		alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
		alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
		alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
		alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
		alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
		alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
		alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
		alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
		alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
		alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
		alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
		alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
		alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
		alGetFilteriv = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
		alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
		alGetFilterfv = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
		alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
		alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
		alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
		alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
		alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
		alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
		alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
		alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
		alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
		alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
		alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");

		if (alGenEffects &&	alDeleteEffects && alIsEffect && alEffecti && alEffectiv &&	alEffectf &&
			alEffectfv && alGetEffecti && alGetEffectiv && alGetEffectf && alGetEffectfv &&	alGenFilters &&
			alDeleteFilters && alIsFilter && alFilteri && alFilteriv &&	alFilterf && alFilterfv &&
			alGetFilteri &&	alGetFilteriv && alGetFilterf && alGetFilterfv && alGenAuxiliaryEffectSlots &&
			alDeleteAuxiliaryEffectSlots &&	alIsAuxiliaryEffectSlot && alAuxiliaryEffectSloti &&
			alAuxiliaryEffectSlotiv && alAuxiliaryEffectSlotf && alAuxiliaryEffectSlotfv &&
			alGetAuxiliaryEffectSloti && alGetAuxiliaryEffectSlotiv && alGetAuxiliaryEffectSlotf &&
			alGetAuxiliaryEffectSlotfv)
			bEFXSupport = AL_TRUE;
	}

	return bEFXSupport;
}

ALvoid ALFWprintf(const ALchar* x, ...)
{
	va_list args;
	va_start(args, x);
	vprintf(x, args);
	va_end(args);
}

#pragma warning(disable:4996)
ALchar fullPath[_MAX_PATH];
ALchar* ALFWaddMediaPath(const ALchar* filename)
{
	sprintf(fullPath, "%s%s", "./media/", filename);
	return fullPath;
}

ALboolean ALFWLoadWaveToBuffer(const ALchar* szWaveFile, ALuint uiBufferID, ALenum eXRAMBufferMode /* = 0 */)
{
	WAVEID WaveID;
	ALint iDataSize, iFrequency;
	ALenum eBufferFormat;
	ALchar* pData;
	ALboolean bReturn;

	bReturn = AL_FALSE;
	if (g_pWaveLoader)
	{
		if (SUCCEEDED(g_pWaveLoader->LoadWaveFile(szWaveFile, &WaveID)))
		{
			if (
				(g_pWaveLoader->GetWaveSize(WaveID, (unsigned long*)&iDataSize) >= 0) &&
				(g_pWaveLoader->GetWaveData(WaveID, (void**)&pData) >= 0) &&
				(g_pWaveLoader->GetWaveFrequency(WaveID, (unsigned long*)&iFrequency) >= 0) &&
				(g_pWaveLoader->GetWaveALBufferFormat(WaveID, &alGetEnumValue, (unsigned long*)&eBufferFormat) >= 0)
				)
			{
				//set xram mode, if need
				if (eaxSetBufferMode && eXRAMBufferMode)
					eaxSetBufferMode(1, &uiBufferID, eXRAMBufferMode);

				alGetError();
				alBufferData(uiBufferID, eBufferFormat, pData, iDataSize, iFrequency);
				if (alGetError() == AL_NO_ERROR)
					bReturn = AL_TRUE;



				g_pWaveLoader->DeleteWaveFile(WaveID);
			}
		}
	}

	return bReturn;
}