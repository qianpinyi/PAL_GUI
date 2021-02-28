#ifndef PAL_BASICFUNCTIONS_SYSTEM_CPP
#define PAL_BASICFUNCTIONS_SYSTEM_CPP 1

#include "PAL_BasicFunctions_Charset.cpp"
#include "PAL_BasicFunctions_Color.cpp"
#include "PAL_BasicFunctions_StringEX.cpp"

#include <cstdlib>
#include <string>
#include <vector>
#include "windows.h"

void systemUTF8(const std::string &str)
{_wsystem(Charset::Utf8ToUnicode(str).c_str());}

void SelectInWinExplorer(const std::string &path)
{systemUTF8("explorer /select, \""+path+"\"");}

int ShowPropertiesOfExplorer(const std::string &pathUTF8)
{
	std::wstring path=Charset::Utf8ToUnicode("\""+pathUTF8+"\"");
	SHELLEXECUTEINFOW ShExecInfo={0};
	ShExecInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask=SEE_MASK_INVOKEIDLIST ;
	ShExecInfo.hwnd=NULL;
	ShExecInfo.lpVerb=L"properties";
	ShExecInfo.lpFile=path.c_str();
	ShExecInfo.lpParameters=L"";
	ShExecInfo.lpDirectory=NULL;
	ShExecInfo.nShow=SW_SHOW;
	ShExecInfo.hInstApp=NULL;
	return ShellExecuteExW(&ShExecInfo);
}

int GetLogicalDriveCount()
{
	unsigned int x=GetLogicalDrives();
	int re=0;
	while (x)
	{
		if (x&1)
			++re;
		x>>=1;
	}
	return re;
}

std::vector <std::string> GetAllLogicalDrive()
{
	unsigned int bufferSize=GetLogicalDriveStrings(0,NULL);
	char *buffer=new char[bufferSize];
	memset(buffer,0,bufferSize*sizeof(char));
	GetLogicalDriveStrings(bufferSize,buffer);
	std::string str;
	std::vector <std::string> re;
	for (int i=0;i<bufferSize;++i)
		if (buffer[i]==0)
		{
			if (!str.empty())
			{
				str.erase(str.length()-1,1);
				re.push_back(str);
			}
			str.clear();
		}
		else str+=buffer[i];
	delete buffer;
	return re;
}

struct LogicalDriveInfo
{
	std::string path,
				name,
				fileSystem;
	unsigned long long FreeBytesAvailable=0,
					   TotalNumberOfBytes=0,
					   TotalNumberOfFreeBytes=0;
	long unsigned int VolumeSerialNumber=0,
				 	  MaxComponentLength=0,
				 	  FSflags=0;
	bool stat=0;
};

LogicalDriveInfo GetLogicalDriveInfo(const std::string &rootPath)
{
	std::wstring rootPathW=Charset::Utf8ToUnicode(rootPath+"\\");
	LogicalDriveInfo re;
	wchar_t nameBuffer[32]={0},
			FSnameBuffer[16]={0};
	ULARGE_INTEGER FreeBytesAvailable,
				   TotalNumberOfBytes,
				   TotalNumberOfFreeBytes;
	unsigned int x=GetVolumeInformationW(rootPathW.c_str(),nameBuffer,32,&re.VolumeSerialNumber,&re.MaxComponentLength,&re.FSflags,FSnameBuffer,16),
				 y=GetDiskFreeSpaceExW(rootPathW.c_str(),&FreeBytesAvailable,&TotalNumberOfBytes,&TotalNumberOfFreeBytes);
//	std::cout<<x<<" "<<y<<std::endl;
	re.path=rootPath;
	re.name=Charset::UnicodeToUtf8(nameBuffer);
	re.fileSystem=Charset::UnicodeToUtf8(FSnameBuffer);
	re.TotalNumberOfBytes=TotalNumberOfBytes.QuadPart;
	re.TotalNumberOfFreeBytes=TotalNumberOfFreeBytes.QuadPart;
	re.FreeBytesAvailable=FreeBytesAvailable.QuadPart;
	return re;
}

struct WinThemeColorFromRegistry
{
	RGBA accentColorMenu,
		 accentColorInactive,
		 accentPalette[8];
	int stat=0;
};

int GetWinThemeColor(WinThemeColorFromRegistry &re)
{
	memset(&re,0,sizeof(re));
	HKEY hKey;
	DWORD type=4,size=4;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\DWM",0,KEY_READ,&hKey)==ERROR_SUCCESS)
		if (RegQueryValueEx(hKey,"AccentColorInactive",NULL,&type,(BYTE*)&re.accentColorInactive,&size)==ERROR_SUCCESS)
			re.stat|=0x0200;
	RegCloseKey(hKey);
	
	if (RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",0,KEY_READ,&hKey)==ERROR_SUCCESS)
	{
		type=4,size=4;
		if (RegQueryValueEx(hKey,"AccentColorMenu",NULL,&type,(BYTE*)&re.accentColorMenu,&size)==ERROR_SUCCESS)
			re.stat|=0x0100;
		type=3,size=32;
		if (RegQueryValueEx(hKey,"AccentPalette",NULL,&type,(BYTE*)&re.accentPalette,&size)==ERROR_SUCCESS)
			re.stat|=0xFF;
	}
	RegCloseKey(hKey);
	return re.stat;
}

int SetWinThemeColor(WinThemeColorFromRegistry &co,bool delayFlag)
{
	
}

#endif
