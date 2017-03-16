#pragma once
#ifndef _C_ALD_LIST_H_
#define _C_ALD_LIST_H_

#include <vector>
#include <string>

using namespace std;

typedef struct
{
	string			strDeviceName;
	int				iMajorVersion;
	int				iMinorVersion;
	unsigned int	uiSourceCount;
	vector<string>	*pvstrExtensions;
	bool			bSelected;
} ALDeviveInfo, *LpALDeviceInfo;

class CALDeviceList
{
private:
	//没有用到动态库
	//OPENALFNTABLE	ALFunction;
	vector<ALDeviveInfo> vDeviceInfo;
	int defaultDeviceIndex;
	int filterIndex;

public:
	CALDeviceList();
	~CALDeviceList();
	int GetNumDevices();
	char *GetDeviceName(int index);
	void GetDeviceVersion(int index, int *major, int *minor);
	unsigned int GetMaxNumSources(int index);
	bool IsExtensionSupported(int index, char *szExtName);
	int GetDefaultDevice();
	void FilterDevicesMinVer(int major, int minor);
	void FilterDevicesMaxVer(int major, int minor);
	void FilterDevicesExtension(char *szExtName);
	void ResetFilters();
	int GetFirstFilteredDevice();
	int GetNextFilteredDevice();

private:
	unsigned int GetMaxNumSources();
};

#endif	//_C_ALD_LIST_H_