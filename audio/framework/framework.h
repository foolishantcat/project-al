#pragma once
#ifndef _FRAMEWORK_H_
#define _FRAMEWORK_H_

#include <stdio.h>
#include <io.h>
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/efx.h"
#include "AL/efx-creative.h"

//Initialization and shutdown
ALvoid ALFWInit();
ALvoid ALFWShutdown();

ALboolean ALFWInitOpenAL();
ALboolean ALFWShutdownOpenAL();

ALboolean ALFWLoadWaveToBuffer(const ALchar* szWaveFile, ALuint uiBufferID, ALenum eXRAMBufferMode = 0);

ALboolean ALFWIsXRAMSupported();
ALboolean ALFWIsEFXSupported();

ALvoid ALFWprintf(const ALchar* x, ...);
ALchar* ALFWaddMediaPath(const ALchar* filename);

// EFX Extension function pointer variables

// Effect objects
extern LPALGENEFFECTS alGenEffects;
extern LPALDELETEEFFECTS alDeleteEffects;
extern LPALISEFFECT alIsEffect;
extern LPALEFFECTI alEffecti;
extern LPALEFFECTIV alEffectiv;
extern LPALEFFECTF alEffectf;
extern LPALEFFECTFV alEffectfv;
extern LPALGETEFFECTI alGetEffecti;
extern LPALGETEFFECTIV alGetEffectiv;
extern LPALGETEFFECTF alGetEffectf;
extern LPALGETEFFECTFV alGetEffectfv;

// Filter objects
extern LPALGENFILTERS alGenFilters;
extern LPALDELETEFILTERS alDeleteFilters;
extern LPALISFILTER alIsFilter;
extern LPALFILTERI alFilteri;
extern LPALFILTERIV alFilteriv;
extern LPALFILTERF alFilterf;
extern LPALFILTERFV alFilterfv;
extern LPALGETFILTERI alGetFilteri;
extern LPALGETFILTERIV alGetFilteriv;
extern LPALGETFILTERF alGetFilterf;
extern LPALGETFILTERFV alGetFilterfv;

// Auxiliary slot object
extern LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
extern LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
extern LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
extern LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
extern LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
extern LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
extern LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
extern LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
extern LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
extern LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
extern LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

// XRAM Extension function pointer variables and enum values

typedef ALboolean(__cdecl *LPEAXSETBUFFERMODE)(ALsizei n, ALuint *buffers, ALint value);
typedef ALenum(__cdecl *LPEAXGETBUFFERMODE)(ALuint buffer, ALint *value);

extern LPEAXSETBUFFERMODE eaxSetBufferMode;
extern LPEAXGETBUFFERMODE eaxGetBufferMode;


#endif	//_FRAMEWORK_H_