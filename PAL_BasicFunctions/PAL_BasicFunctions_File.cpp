#ifndef PAL_BASICFUNCTIONS_FILE_CPP
#define PAL_BASICFUNCTIONS_FILE_CPP 1

#include <string>
#include <vector>
#include <fstream>
#include "io.h"

#include "PAL_BasicFunctions_Charset.cpp"
#include "PAL_BasicFunctions_StringEX.cpp"

template <typename T> inline void ReadDataBin(std::ifstream &fin,T &x)
{fin.read((char*)&x,sizeof(x));}

template <typename T> inline void WriteDataBin(std::ofstream &fout,const T &x)
{fout.write((char*)&x,sizeof(x));}

//class FileInfo
//{
//	public:
//		unsigned int type=0,//string hash of aftername
//					 size=0;
//		std::string path,showName;
//		int userCode=0;//user define
//		void *userData=NULL;
//		
//		
//		
//};

std::vector <std::string>* GetAllFile_UTF8(std::string str,bool fileFlag)
{
	using namespace std;
	vector <string> *ret=new vector <string>;
	wstring wstr=Charset::Utf8ToUnicode(str+"\\*");
	int hFiles=0;
	_wfinddata_t da;
	if ((hFiles=_wfindfirst(wstr.c_str(),&da))!=-1)
	{
		do
		{
			str=DeleteEndBlank(Charset::UnicodeToUtf8(da.name));
			//str.erase(str.length()-1);
			if (!(da.attrib&_A_HIDDEN))
				if (da.attrib&_A_SUBDIR)
				{
					if (!fileFlag)
						if (str!="."&&str!="..")
							ret->push_back(str);
				}
				else
				{
					if (fileFlag)
						ret->push_back(str);
				}
		}
		while (_wfindnext(hFiles,&da)==0);
	}
	_findclose(hFiles);
	return ret;
}
std::vector <std::pair<std::string,int> >* GetAllFile_UTF8(std::string str)
{
	using namespace std;
	vector <pair <string,int> > *ret=new vector <pair <string,int> >;
	wstring wstr=Charset::Utf8ToUnicode(str+"\\*");
	int hFiles=0;
	_wfinddata_t da;
	if ((hFiles=_wfindfirst(wstr.c_str(),&da))!=-1)
	{
		do
		{
			str=DeleteEndBlank(Charset::UnicodeToUtf8(da.name));
			if (!(da.attrib&_A_HIDDEN))
				if (da.attrib&_A_SUBDIR)
				{
					if (str!="."&&str!="..")
						ret->push_back({str,0});
				}
				else ret->push_back({str,1});
		}
		while (_wfindnext(hFiles,&da)==0);
	}
	_findclose(hFiles);
	return ret;
}

#endif
