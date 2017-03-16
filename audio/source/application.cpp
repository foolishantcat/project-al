// /* Base On OpenAL 1.17.1 */

#include "framework.h"

ALboolean CreateAuxEffectSlot(ALuint *puiAuxEffectSlot)
{
	ALboolean bReturn = AL_FALSE;

	// Clear AL Error state
	alGetError();

	// Generate an Auxiliary Effect Slot
	alGenAuxiliaryEffectSlots(1, puiAuxEffectSlot);
	if (alGetError() == AL_NO_ERROR)
		bReturn = AL_TRUE;

	return bReturn;
}

ALboolean CreateEffect(ALuint *puiEffect, ALenum eEffectType)
{
	ALboolean bReturn = AL_FALSE;

	if (puiEffect)
	{
		// Clear AL Error State
		alGetError();

		// Generate an Effect
		alGenEffects(1, puiEffect);
		if (alGetError() == AL_NO_ERROR)
		{
			// Set the Effect Type
			alEffecti(*puiEffect, AL_EFFECT_TYPE, eEffectType);
			if (alGetError() == AL_NO_ERROR)
				bReturn = AL_TRUE;
			else
				alDeleteEffects(1, puiEffect);
		}
	}

	return bReturn;
}

#ifdef _WIN32
#include <windows.h>
#define sleep Sleep
#elif linux
#include <unistd.h>
#endif

ALvoid PlaySource(ALuint uiSource)
{
	ALint iState;

	// Play Source
	alSourcePlay(uiSource);
	do
	{
		alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);
		sleep(10);
	} while (iState == AL_PLAYING);
}

int main() 
{
	ALCdevice* pDevice = NULL;
	ALuint uiEffectSlots;
	ALuint uiEffects;
	ALuint uiFilters[1] = { 0 };
	ALuint uiSource, uiBuffer;
	ALboolean bEffectCreated = AL_FALSE;

	//³õÊ¼»¯¿ò¼Ü
	ALFWInit();

	ALFWprintf("Enumerate EFX Application\n");

	if (!ALFWInitOpenAL())
	{
		ALFWprintf("Failed to initialize OpenAL\n");
		ALFWShutdown();
		return -1;
	}

	//clear
	alGetError();

	alGenBuffers(1, &uiBuffer);

	//get the current al Device
	//pDevice = alcGetContextsDevice(alcGetCurrentContext());

	//Load wave file
	if (!ALFWLoadWaveToBuffer("./Footsteps.wav", uiBuffer))
	{
		ALFWprintf("Failed to load %s\n", "./Footsteps.wav");
		alDeleteBuffers(1, &uiBuffer);
		ALFWShutdownOpenAL();
		ALFWShutdown();
		return 0;
	}

	alGenSources(1, &uiSource);

	alSourcei(uiSource, AL_BUFFER, uiBuffer);

	//check for efx extension
	if (ALFWIsEFXSupported())
	{
		if (CreateAuxEffectSlot(&uiEffectSlots))
		{
			if (CreateEffect(&uiEffects, AL_EFFECT_EAXREVERB))
			{
				bEffectCreated = AL_TRUE;
			}
			else
			{
				ALFWprintf("Failed to Create an EAX Reverb Effect\n");
				return -1;
			}
		}
		else
		{
			ALFWprintf("Failed to generate an Auxilary Effect Slot\n");
		}

		ALFWprintf("Playing Source without Effects\n");
		PlaySource(uiSource);
		
		ALFWprintf("Playing Source with Send to 'Hangar' Reverb Effect\n");

		

		// Delete Filter
		//alDeleteFilters(1, &uiFilters);

		// Delete Effect
		alDeleteEffects(1, &uiEffects);

		// Delete Auxiliary Effect Slots
		alDeleteAuxiliaryEffectSlots(1, &uiEffectSlots);
	}
	else
	{
		ALFWprintf("EFX support not found\n");
	}

	ALFWShutdownOpenAL();

	ALFWShutdown();

	return 0;
}