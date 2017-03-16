#pragma once
#include "AL/al.h"
#include "AL/alc.h"

// Open AL 1.17.2 Function table definition

#ifndef _OPENAL_FUNC_TABLLE_H_
#define _OPENAL_FUNC_TABLLE_H_

//List part of functions available in AL 1.17.2 implementations

typedef void (ALAPIENTRY *LP_AL_ENABLE)(ALenum capability);

typedef struct
{
	LP_AL_ENABLE	alEnable;

} OPENALFNTABLE, *LPOPENALFNTABLE;

#endif	//_OPENAL_FUNC_TABLLE_H_

//Add dynamic library, not implementation now
ALboolean LoadOAL1172Library(ALchar* szOALFullPathName, LPOPENALFNTABLE lpOALFnTable);
ALvoid UnLoadOAL1172Library();