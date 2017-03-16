#include "CALDeviceList.h"
#include "AL/alc.h"
#include "AL/al.h"

CALDeviceList::CALDeviceList()
{
	ALDeviveInfo	alDeviceInfo;
	char* devices;
	int index;
	const char* defaultDeviceName;
	const char* actualDeviceName;

	vDeviceInfo.empty();
	vDeviceInfo.reserve(10);

	defaultDeviceIndex = 0;

	if (AL_TRUE == alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
	{
		devices = (char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		defaultDeviceName = (char*)alcGetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
		index = 0;
		// go through device list (each device terminated with a single NULL, list terminated with double NULL)
		while (NULL != *devices)
		{
			if (0 == strcmp(defaultDeviceName, devices))
			{
				defaultDeviceIndex = index;
			}
			ALCdevice* device = alcOpenDevice(devices);
			if (device)
			{
				ALCcontext* context = alcCreateContext(device, NULL);
				if (context)
				{
					alcMakeContextCurrent(context);
					actualDeviceName = alcGetString(device, ALC_DEVICE_SPECIFIER);
					bool bNewName = true;
					for (int i = 0; i < GetNumDevices(); i++)
					{
						if (0 == strcmp(GetDeviceName(i), actualDeviceName))
						{
							bNewName = false;
							break;
						}
					}
					if ((bNewName) && (NULL != actualDeviceName) * (strlen(actualDeviceName) > 0))
					{
						memset(&alDeviceInfo, 0, sizeof(ALDeviveInfo));
						alDeviceInfo.bSelected = true;
						alDeviceInfo.strDeviceName = actualDeviceName;
						alcGetIntegerv(device, ALC_MAJOR_VERSION, sizeof(int), &alDeviceInfo.iMajorVersion);
						alcGetIntegerv(device, ALC_MINOR_VERSION, sizeof(int), &alDeviceInfo.iMinorVersion);

						alDeviceInfo.pvstrExtensions = new vector<string>;

						//check for alc extensions
						if (AL_TRUE == alcIsExtensionPresent(device, "ALC_EXT_CAPTURE"))
							alDeviceInfo.pvstrExtensions->push_back("ALC_EXT_CAPTURE");
						if (AL_TRUE == alcIsExtensionPresent(device, "ALC_EXT_EFX"))
							alDeviceInfo.pvstrExtensions->push_back("ALC_EXT_EFX");
						//check for extensions
						if (AL_TRUE == alIsExtensionPresent("AL_EXT_OFFSET"))
							alDeviceInfo.pvstrExtensions->push_back("AL_EXT_OFFSET");
						if (AL_TRUE == alIsExtensionPresent("AL_EXT_LINEAR_DISTANCE"))
							alDeviceInfo.pvstrExtensions->push_back("AL_EXT_LINEAR_DISTANCE");

						if (AL_TRUE == alIsExtensionPresent("EAX2.0"))
							alDeviceInfo.pvstrExtensions->push_back("EAX2.0");
						if (AL_TRUE == alIsExtensionPresent("EAX3.0"))
							alDeviceInfo.pvstrExtensions->push_back("EAX3.0");
						if (AL_TRUE == alIsExtensionPresent("EAX4.0"))
							alDeviceInfo.pvstrExtensions->push_back("EAX4.0");
						if (AL_TRUE == alIsExtensionPresent("EAX5.0"))
							alDeviceInfo.pvstrExtensions->push_back("EAX5.0");

						if (AL_TRUE == alIsExtensionPresent("EAX-RAM"))
							alDeviceInfo.pvstrExtensions->push_back("EAX-RAM");

						//get source count
						alDeviceInfo.uiSourceCount = GetMaxNumSources();
						vDeviceInfo.push_back(alDeviceInfo);
					}
					alcMakeContextCurrent(NULL);
					alcDestroyContext(context);
				}
				alcCloseDevice(device);
			}
			devices += strlen(devices) + 1;
			index += 1;
		}
	}

	ResetFilters();
}

CALDeviceList::~CALDeviceList()
{
	for (unsigned int i = 0; i < vDeviceInfo.size(); i++)
	{
		if (vDeviceInfo[i].pvstrExtensions)
		{
			vDeviceInfo[i].pvstrExtensions->empty();
			delete vDeviceInfo[i].pvstrExtensions;
		}
	}

	vDeviceInfo.empty();
}

int CALDeviceList::GetNumDevices()
{
	return (int)vDeviceInfo.size();
}

char* CALDeviceList::GetDeviceName(int index)
{
	if (index < GetNumDevices())
		return (char*)vDeviceInfo[index].strDeviceName.c_str();
	else
		return NULL;
}

void CALDeviceList::ResetFilters()
{
	for (int i = 0; i < GetNumDevices(); i++)
	{
		vDeviceInfo[i].bSelected = true;
	}
	filterIndex = 0;
}

unsigned int CALDeviceList::GetMaxNumSources(int index)
{
	if (index < GetNumDevices())
		return vDeviceInfo[index].uiSourceCount;
	else
		return 0;
}

void CALDeviceList::GetDeviceVersion(int index, int *major, int *minor)
{
	if (index < GetNumDevices())
	{
		if (major)
			*major = vDeviceInfo[index].iMajorVersion;
		if (minor)
			*minor = vDeviceInfo[index].iMinorVersion;
	}
	return;
}

#ifdef linux
#include <strings.h>  
int stricmp(const char * s1, const char *s2)
{
	return strcasecmp(s1, s2);
}

#elif _WIN32
#include <windows.h>
#define stricmp _stricmp

#endif  

bool CALDeviceList::IsExtensionSupported(int index, char *szExtName)
{
	bool bReturn = false;
	if (index < GetNumDevices())
	{
		for (unsigned int i = 0; i < vDeviceInfo[index].pvstrExtensions->size(); i++)
		{
			if (!stricmp(vDeviceInfo[index].pvstrExtensions->at(i).c_str(), szExtName))
			{
				bReturn = true;
				break;
			}
		}
	}

	return bReturn;
}

int CALDeviceList::GetDefaultDevice()
{
	return defaultDeviceIndex;
}

void CALDeviceList::FilterDevicesMinVer(int major, int minor)
{
	int dMajor, dMinor;
	for (unsigned int i = 0; i < vDeviceInfo.size(); i++)
	{
		GetDeviceVersion(i, &dMajor, &dMinor);
		if ((dMajor < major) || ((dMajor == major) && (dMinor < minor)))
		{
			vDeviceInfo[i].bSelected = false;
		}
	}
}

void CALDeviceList::FilterDevicesMaxVer(int major, int minor)
{
	int dMajor, dMinor;
	for (unsigned int i = 0; i < vDeviceInfo.size(); i++) {
		GetDeviceVersion(i, &dMajor, &dMinor);
		if ((dMajor > major) || ((dMajor == major) && (dMinor > minor))) {
			vDeviceInfo[i].bSelected = false;
		}
	}
}

void CALDeviceList::FilterDevicesExtension(char *szExtName)
{
	bool bFound;
	for (unsigned int i = 0; i < vDeviceInfo.size(); i++)
	{
		bFound = false;
		for (unsigned int j = 0; j < vDeviceInfo[i].pvstrExtensions->size(); j++)
		{
			if (!stricmp(vDeviceInfo[i].pvstrExtensions->at(j).c_str(), szExtName))
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
			vDeviceInfo[i].bSelected = false;
	}
}

int CALDeviceList::GetFirstFilteredDevice()
{
	int i;

	for (i = 0; i < GetNumDevices(); i++) 
	{
		if (vDeviceInfo[i].bSelected == true) 
		{
			break;
		}
	}
	filterIndex = i + 1;
	return i;
}

int CALDeviceList::GetNextFilteredDevice()
{
	int i;

	for (i = filterIndex; i < GetNumDevices(); i++) 
	{
		if (vDeviceInfo[i].bSelected == true) 
		{
			break;
		}
	}
	filterIndex = i + 1;
	return i;
}

unsigned int CALDeviceList::GetMaxNumSources()
{
	ALuint uiSources[256];
	unsigned int iSourceCount = 0;

	// Clear AL Error Code
	alGetError();

	// Generate up to 256 Sources, checking for any errors
	for (iSourceCount = 0; iSourceCount < 256; iSourceCount++)
	{
		alGenSources(1, &uiSources[iSourceCount]);
		if (alGetError() != AL_NO_ERROR)
			break;
	}

	// Release the Sources
	alDeleteSources(iSourceCount, uiSources);
	if (alGetError() != AL_NO_ERROR)
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			alDeleteSources(1, &uiSources[i]);
		}
	}

	return iSourceCount;
}