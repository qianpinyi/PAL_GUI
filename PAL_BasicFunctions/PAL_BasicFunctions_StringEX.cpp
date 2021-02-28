#ifndef PAL_BASICFUNCTIONS_STRINGEX_CPP
#define PAL_BASICFUNCTIONS_STRINGEX_CPP 1

#include <string>
#include <stack>
#include <vector>
#include <fstream>
#include <ctime>

int strTOint(const std::string &str)
{
	int x=0;
	for (size_t i=0;i<str.length();++i)
		x*=10,x+=str[i]-'0';
	return x;
}

long long strTOll(const std::string &str)
{
	long long x=0;
	for (size_t i=0;i<str.length();++i)
		x*=10,x+=str[i]-'0';
	return x;
}

double strTOdb(const std::string &str)
{
	double x=0;
	for (size_t i=0,flag=0;i<str.length();++i)
		if (str[i]=='.') flag=1;
		else if (flag==0) x*=10,x+=str[i]-'0';
		else x+=(double(str[i]-'0'))/(flag*=10);
	return x;
}

std::string llTOstr(long long x,unsigned int w=1)
{
//	if (x==0) return "0";
	std::string re;
	if (x<0) re+="-",x=-x;
	std::stack <char> sta;
	while (x)
		sta.push(x%10+'0'),x/=10;
	if (w!=0)
		while (sta.size()<w)
			sta.push('0');
	while (!sta.empty())
		re+=sta.top(),sta.pop();
	return re;
}

std::string ullTOstr(unsigned long long x,unsigned int w=1)
{
	std::string re;
	std::stack <char> sta;
	while (x)
		sta.push(x%10+'0'),x/=10;
	if (w!=0)
		while (sta.size()<w)
			sta.push('0');
	while (!sta.empty())
		re+=sta.top(),sta.pop();
	return re;
}

bool strISll(const std::string &str)
{
	size_t p=0;
	if (str[0]=='-')
		++p;
	while (p<str.length())
		if (str[p]<'0'||str[p]>'9')
			return 0;
		else ++p;
	return 1;
}

bool strISull(const std::string &str)
{
	for (auto sp:str)
		if (sp<'0'||sp>'9')
			return 0;
	return 1;
}

std::string ReplaceCharInStr(std::string str,const size_t &L,const size_t &R,const char &ch1,const char &ch2)
{
	for (size_t i=L;i<=R;++i)
		if (str[i]==ch1)
			str[i]=ch2;
	return str;
}

std::string ReplaceCharInStr(const std::string &str,const char &ch1,const char &ch2)
{
	if (str.empty()) return "";
	return ReplaceCharInStr(str,0,str.length()-1,ch1,ch2);
}

std::string DeletePreBlank(const std::string &str)
{
	for (size_t i=0;i<str.length();++i)
		switch (str[i])
		{
			case '\n':break;//??
			case '\r':break;//??
			case '\t':break;
			case ' ' :break;
			case '\0':break;
			default:
				return str.substr(i,str.length()-i);
		}
	return "";
}

std::string DeleteEndBlank(const std::string &str)
{
	if (str.empty()) return "";
	for (size_t i=str.length()-1;i>=0;--i)
		switch (str[i])
		{
			case '\n':break;//??
			case '\r':break;//??
			case '\t':break;
			case ' ' :break;
			case '\0':break;
			default:
				return str.substr(0,i+1);
		}
	return "";
}

std::wstring DeleteEndBlank(const std::wstring &wstr)
{
	if (wstr.empty()) return L"";
	for (size_t i=wstr.length()-1;i>=0;--i)
		switch (wstr[i])
		{
			case L'\n':break;//??
			case L'\r':break;//??
			case L'\t':break;
			case L' ' :break;
			case L'\0':break;
			default:
				return wstr.substr(0,i+1);
		}
	return L"";
}

std::string CutFirstCharInvolveSubStr(const std::string &str,const char &ch)
{
	size_t pos1=0,pos2=0;
	pos1=str.find(ch);
	if (pos1+1<str.length())
		pos2=str.find(pos1+1,ch);
	if (pos1<pos2&&pos1!=str.npos&&pos2!=str.npos) return str.substr(pos1+1,pos2-pos1-1);
	else return "";
}

std::string CutFirstSubStringBetweenChar(const std::string &str,char chL,char chR)//chL,chR !=0
{
	std::string re;
	for (auto sp:str)
		if (chL!=0)
			sp==chL?chL=0:0;
		else if (sp==chR)
			return re;
		else re+=sp;
	return "";
}

std::string GetAftername(const std::string &str)
{
	if (str.empty()) return "";
	size_t p=str.rfind(".",str.length()-1);
	if (p==str.npos) return "";
	return str.substr(p,str.length()-p);
}

std::string GetWithOutAftername(const std::string &str)
{
	if (str.empty()) return "";
	size_t p=str.rfind(".",str.length()-1);
	if (p==str.npos) return str;
	return str.substr(0,p);
}

std::string GetLastAfterBackSlash(const std::string &str)
{
	if (str.empty()) return "";
	size_t p=str.rfind("\\",str.length()-1);
	if (p==str.npos||p==str.length()-1) return "";
	return str.substr(p+1,str.length()-p-1);
}

std::string GetPreviousBeforeBackSlash(const std::string &str)
{
	if (str.empty()) return "";
	size_t p=str.rfind("\\",str.length()-1);
	if (p==str.npos||p==0) return "";
	return str.substr(0,p);
}

std::string Atoa(std::string str)
{
	for (size_t i=0;i<str.length();++i)
		if ('A'<=str[i]&&str[i]<='Z')
			str[i]+='a'-'A';
	return str;
}

char Atoa(char ch)
{
	if ('A'<=ch&&ch<='Z')
		return ch-'A'+'a';
	else return ch;
}

std::string atoA(std::string str)
{
	for (size_t i=0;i<str.length();++i)	
		if ('a'<=str[i]&&str[i]<='z')
			str[i]+='A'-'a';
	return str;
}

std::string GetRandStr(int w,const std::vector <char> &charset)
{
	std::string re;
	for (int i=1;i<=w;++i)
		re+=charset[rand()%charset.size()];
	return re;
}

void ReplaceXMLescapecharWithReal(std::string &str)
{
	while (1)
	{
		size_t c=str.find("&amp;");
		if (c==str.npos) break;
		str.replace(c,5,"&");
	}
	while (1)
	{
		size_t c=str.find("&lt;");
		if (c==str.npos) break;
		str.replace(c,4,"<");
	}
	while (1)
	{
		size_t c=str.find("&gt;");
		if (c==str.npos) break;
		str.replace(c,4,">");
	}
	while (1)
	{
		size_t c=str.find("&apos;");
		if (c==str.npos) break;
		str.replace(c,6,"'");
	}
	while (1)
	{
		size_t c=str.find("&quot;");
		if (c==str.npos) break;
		str.replace(c,6,"\"");
	}
}

void GetRidOfEndChar0(std::string &str)
{
	while (!str.empty()&&(*str.rbegin())==0)
		str.erase(str.end()-1);
}

void GetRidOfEndChar0(std::wstring &wstr)
{
	while (!wstr.empty()&&(*wstr.rbegin())==0)
		wstr.erase(wstr.end()-1);
}

std::wstring GetRidOfEndChar0_Re(std::wstring wstr)
{
	while (!wstr.empty()&&(*wstr.rbegin())==0)
		wstr.erase(wstr.end()-1);
	return wstr;
}

std::string GetRidOfEndChar0_Re(std::string str)
{
	while (!str.empty()&&(*str.rbegin())==0)
		str.erase(str.end()-1);
	return str;
}

std::string GetSamePrefix(const std::string &str1,const std::string &str2)
{
	size_t pos=0;
	while (pos<str1.length()&&pos<str2.length()&&str1[pos]==str2[pos])
		++pos;
	if (pos==0) return "";
	else return str1.substr(0,pos);
}

std::string GetSubStrAfterCntChar(const std::string &str,char ch,size_t cnt=1)
{
	size_t pos=0,t=0;
	while (t<cnt&&pos<str.length())
	{
		pos=str.find(ch,pos);
		if (pos==str.npos)
			break;
		else ++pos;
		++t;
	}
	if (pos!=str.npos&&pos<str.length())
		return str.substr(pos,str.length()-pos);
	else return "";
}

std::vector <std::string> DivideStringByLine(const std::string &str)
{
	using namespace std;
	vector <string> re;
	bool flag=0;
	for (auto sp:str)
	{
		if (flag==0)
			re.push_back(""),flag=1;
		if (sp=='\n')
			flag=0;
		else if (sp!='\r')
			re.back()+=sp;
	}
	return re;
}

std::vector <unsigned long long> GetULLsInStr(const std::string &str)
{
	std::vector <unsigned long long> re;
	unsigned long long x=0;
	bool flag=0;
	for (auto sp:str)
		if ('0'<=sp&&sp<='9')
			if (flag) x=x*10+sp-'0';
			else x=sp-'0',flag=1;
		else if (flag)
			re.push_back(x),flag=0;
	if (flag)
		re.push_back(x);
	return re;
}

std::string GetFileSizeString(unsigned long long size)
{
	static char BKBMBGBTBPB[6][3]={"B","KB","MB","GB","TB","PB"};
	size*=100;
	for (int i=0;i<=5;size/=1024,++i)
		if (size<102400)
			return llTOstr(size/100)+"."+llTOstr(size%100,2)+" "+BKBMBGBTBPB[i];
	return "";
}

int FileWriteString(std::ofstream &fout,const std::string &str)
{
	unsigned int len=str.length();
	fout.write((char*)&len,4);
	if (len>0)
		fout.write(str.c_str(),len);
	return 0;
}

std::string FileReadString(std::ifstream &fin)
{
	unsigned int len;
	fin.read((char*)&len,4);
	if (len==0) return "";
	char *str=new char[len+1];
	fin.read(str,len);
	str[len]=0;
	std::string re(str);
	delete[] str;
	return re;
}

bool SortComp_WithNum(const std::string &a,const std::string &b)
{
	size_t p=0,q=0,x,y;
	while (p<a.length()&&q<b.length())
	{
		if ('0'<=a[p]&&a[p]<='9'&&'0'<=b[q]&&b[q]<='9')
		{
			x=y=0;
			do x=x*10+a[p]-'0';
			while (++p<a.length()&&'0'<=a[p]&&a[p]<='9');
			do y=y*10+b[q]-'0';
			while (++q<b.length()&&'0'<=b[q]&&b[q]<='9');
			if (x!=y) return x<y;
		}
		else
		{
			char cha=a[p],chb=b[q];
			if (cha==chb)
				++p,++q;
			else if (cha>0&&chb>0)
				return Atoa(cha)<Atoa(chb);
			else if (cha<0&&chb<0)
				return cha>chb;
			else return cha>0;
		}
	}
	return p==a.length();
}

//Fixme:stringUTF8 need replace int or long long to size_t
template <class T> class stringUTF8_WithData;

class stringUTF8
{
	template <class T> friend class stringUTF8_WithData;
	protected:
		std::vector <std::string> strUTF8;
		
	public:
		inline bool empty() const
		{return strUTF8.size()==0;}
		
		void clear()
		{strUTF8.clear();}
		
		inline int length() const
		{return strUTF8.size();}
		
		void append(const stringUTF8 &str,int srcPos,int srcLen)
		{
			for (int i=srcPos;i<srcPos+srcLen;++i)
				strUTF8.push_back(str[i]);
		}
		
		void append(const stringUTF8 &str)
		{
			for (auto vp:str.strUTF8)
				strUTF8.push_back(vp);
		}
		
		void append(const std::string &str)
		{
			unsigned p=0,q=0,stat=0;
			while (p<str.length())
			{
				if ((str[p]&0x80)==0)//0XXXXXXX
				{
					stat=0;
					strUTF8.push_back(str.substr(p,1));
					q=p;
				}
				else if ((str[p]&0xC0)==0x80)//10XXXXXX
				{
					if (p-q+1==stat)
					{
						strUTF8.push_back(str.substr(q,p-q+1));
						stat=0;
					}
				}
				else 
				{
					if ((str[p]&0xE0)==0xC0)	stat=2;//110XXXXX
					else if ((str[p]&0xF0)==0xE0)	stat=3;//1110XXXX
					else if ((str[p]&0xF8)==0xF0)	stat=4;//11110XXX
					else if ((str[p]&0xFC)==0xF8)	stat=5;//111110XX
					else if ((str[p]&0xFE)==0xFC)	stat=6;//1111110X
					else ;//Error
					q=p;
				}
				++p;
			}
		}
		
		void append(const char *str)
		{append(std::string(str));}
		
		std::string operator () (int l,int r,char mode[3]) const
		{
			if (mode[0]=='(') ++l;
			if (mode[1]==')') --r;
			std::string re;
			for (int i=l;i<=r;++i)
				re.append(strUTF8[i]);
			return re;	
		}
		
		std::string operator () (int l,int r) const
		{
			std::string re;
			for (int i=l;i<=r;++i)
				re.append(strUTF8[i]);
			return re;
		}
		
		stringUTF8 substrUTF8(int pos,int len) const
		{
			stringUTF8 re;
			for (int i=pos;i<pos+len;++i)
				re.strUTF8.push_back(strUTF8[i]);
			return re;
		}
		
		std::string substr(int pos,int len)
		{
			std::string re;
			for (int i=pos;i<pos+len;++i)
				re.append(strUTF8[i]);
			return re;
		}
		
		std::string cppString() const
		{
			std::string re;
			for (auto vp:strUTF8)
				re.append(vp);
			return re;
		}
		
		int find(const stringUTF8 &str)
		{
			for (unsigned int i=0;i<strUTF8.size();++i)
			{
				bool flag=1;
				for (unsigned int j=0;j<str.strUTF8.size();++j)
					if (strUTF8[i]!=str.strUTF8[j])
					{
						flag=0;
						break;
					}
				if (flag)
					return i;
			}
		}
		
		int find(const std::string &str)
		{return find(stringUTF8(str));}
		
		int find(const char *str)
		{return find(stringUTF8(std::string(str)));}
		
		void erase(int pos,int len)
		{strUTF8.erase(strUTF8.begin()+pos,strUTF8.begin()+pos+len);}
		
		stringUTF8& insert(int pos,const stringUTF8 &str,int srcPos,int srcLen)
		{
			if (pos>length()||pos<0)
			{
//				std::cerr<<"stringUTF8 Error: insert out of range!"<<std::endl;
				return *this;
			}
			if (pos==length())
				append(str,srcPos,srcLen);
			else strUTF8.insert(strUTF8.begin()+pos,str.strUTF8.begin()+srcPos,str.strUTF8.begin()+srcPos+srcLen);
			return *this;
		}
		
		stringUTF8& insert(int pos,const stringUTF8 &str)
		{
			if (pos>length()||pos<0)
			{
//				std::cerr<<"stringUTF8 Error: insert out of range!"<<std::endl;
				return *this;
			}
			if (pos==length())
				append(str);
			else strUTF8.insert(strUTF8.begin()+pos,str.strUTF8.begin(),str.strUTF8.end());
			return *this;
		}
		
		stringUTF8& insert(int pos,const std::string &str)
		{return insert(pos,stringUTF8(str));}
		
		stringUTF8& insert(int pos,const char *str)
		{return insert(pos,stringUTF8(str));}
		
		stringUTF8& operator += (const stringUTF8 &str)
		{
			append(str);
			return *this;
		}
		
		stringUTF8& operator += (const std::string &str)
		{
			append(str);
			return *this;
		}
		
		stringUTF8& operator += (const char *str)
		{
			append(str);
			return *this;
		}
		
		stringUTF8 operator + (const stringUTF8 &str) const
		{
			stringUTF8 re(*this);
			re.append(str);
			return re;
		}
		
		stringUTF8 operator + (const std::string &str) const
		{
			stringUTF8 re(*this);
			re.append(str);
			return re;
		}
		
		stringUTF8 operator + (const char *str) const
		{
			stringUTF8 re(*this);
			re.append(str);
			return re;
		}
		
		stringUTF8& operator = (const stringUTF8 &str)
		{
			if (&str!=this)
				strUTF8=str.strUTF8;
			return *this;
		}
		
		stringUTF8& operator = (const std::string &str)
		{
			strUTF8.clear();
			append(str);
			return *this;
		}
		
		stringUTF8& operator = (const char *str)
		{
			strUTF8.clear();
			append(str);
			return *this;
		}
		
		std::string& operator [] (int x)
		{return strUTF8[x];}
		
		const std::string& operator [] (int x) const
		{return strUTF8[x];}
		
		stringUTF8(const stringUTF8 &str):strUTF8(str.strUTF8) {}
		
		stringUTF8(const std::string &str)
		{append(str);}
		
		stringUTF8(const char *str)
		{append(str);}
		
		stringUTF8() {}
};

template <class T> class stringUTF8_WithData
{
	protected:
		struct EachCharData
		{
			std::string ch;
			T Data;
			
			EachCharData(const std::string str,const T &da)
			:ch(str),Data(da) {}
			
			EachCharData(const std::string str)
			:ch(str) {}
		};
		
		std::vector <EachCharData> strUTF8;
		
	public:
		inline bool empty() const
		{return strUTF8.size()==0;}
		
		void clear()
		{strUTF8.clear();}
		
		inline int length() const
		{return strUTF8.size();}
		
		void append(const stringUTF8_WithData <T> &str,int srcPos,int srcLen)
		{
			for (int i=srcPos;i<srcPos+srcLen;++i)
				strUTF8.push_back(str[i]);
		}
		
		void append(const stringUTF8 &str,const T &defaultData,int srcPos,int srcLen)
		{
			for (int i=srcPos;i<srcPos+srcLen;++i)
				strUTF8.push_back(EachCharData(str[i],defaultData));
		}
		
		void append(const stringUTF8 &str,int srcPos,int srcLen)
		{
			for (int i=srcPos;i<srcPos+srcLen;++i)
				strUTF8.push_back(EachCharData(str[i]));
		}
		
		void append(const stringUTF8_WithData <T> &str)
		{
			for (auto vp:str.strUTF8)
				strUTF8.push_back(vp);
		}
		
		void append(const stringUTF8 &str,const T &defaultData)
		{
			for (auto vp:str.strUTF8)
				strUTF8.push_back(EachCharData(str,defaultData));
		}
		
		void append(const stringUTF8 &str)
		{
			for (auto vp:str.strUTF8)
				strUTF8.push_back(EachCharData(vp));
		}
		
		void append(const std::string &str,const T &defaultData)
		{
			int p=0,q=0,stat=0;
			while (p<str.length())
			{
				if ((str[p]&0x80)==0)//0XXXXXXX
				{
					stat=0;
					strUTF8.push_back(EachLineData(str.substr(p,1),defaultData));
					q=p;
				}
				else if ((str[p]&0xC0)==0x80)//10XXXXXX
				{
					if (p-q+1==stat)
					{
						strUTF8.push_back(EachCharData(str.substr(q,p-q+1),defaultData));
						stat=0;
					}
				}
				else 
				{
					if ((str[p]&0xE0)==0xC0)	stat=2;//110XXXXX
					else if ((str[p]&0xF0)==0xE0)	stat=3;//1110XXXX
					else if ((str[p]&0xF8)==0xF0)	stat=4;//11110XXX
					else if ((str[p]&0xFC)==0xF8)	stat=5;//111110XX
					else if ((str[p]&0xFE)==0xFC)	stat=6;//1111110X
					else ;//Error
					q=p;
				}
				++p;
			}
		}
		
		void append(const std::string &str)
		{
			unsigned int p=0,q=0,stat=0;
			while (p<str.length())
			{
				if ((str[p]&0x80)==0)//0XXXXXXX
				{
					stat=0;
					strUTF8.push_back(EachCharData(str.substr(p,1)));
					q=p;
				}
				else if ((str[p]&0xC0)==0x80)//10XXXXXX
				{
					if (p-q+1==stat)
					{
						strUTF8.push_back(EachCharData(str.substr(q,p-q+1)));
						stat=0;
					}
				}
				else 
				{
					if ((str[p]&0xE0)==0xC0)	stat=2;//110XXXXX
					else if ((str[p]&0xF0)==0xE0)	stat=3;//1110XXXX
					else if ((str[p]&0xF8)==0xF0)	stat=4;//11110XXX
					else if ((str[p]&0xFC)==0xF8)	stat=5;//111110XX
					else if ((str[p]&0xFE)==0xFC)	stat=6;//1111110X
					else ;//Error
					q=p;
				}
				++p;
			}
		}
		
		void append(const char *str,const T &defaultData)
		{append(std::string(str),defaultData);}
		
		void append(const char *str)
		{append(std::string(str));}
		
		std::string operator () (int l,int r,char mode[3]) const
		{
			if (mode[0]=='(') ++l;
			if (mode[1]==')') --r;
			std::string re;
			for (int i=l;i<=r;++i)
				re.append(strUTF8[i].ch);
			return re;
		}
		
		std::string operator () (int l,int r) const
		{
			std::string re;
			for (int i=l;i<=r;++i)
				re.append(strUTF8[i].ch);
			return re;
		}
		
		stringUTF8_WithData <T> substrUTF8_WithData(int pos,int len) const
		{
			stringUTF8_WithData <T> re;
			for (int i=pos;i<pos+len;++i)
				re.strUTF8.push_back(strUTF8[i]);
			return re;
		}
		
		stringUTF8 substrUTF8(int pos,int len) const
		{
			stringUTF8 re;
			for (int i=pos;i<pos+len;++i)
				re.strUTF8.push_back(strUTF8[i].ch);
			return re;
		}
		
		std::string substr(int pos,int len)
		{
			std::string re;
			for (int i=pos;i<pos+len;++i)
				re.append(strUTF8[i].ch);
			return re;
		}
		
		stringUTF8 StringUTF8() const
		{
			stringUTF8 re;
			for (auto vp:strUTF8)
				re.strUTF8.push_back(vp.ch);
			return re;
		}
		
		std::string cppString() const
		{
			std::string re;
			for (auto vp:strUTF8)
				re.append(vp.ch);
			return re;
		}
		
		int find(const stringUTF8 &str)
		{
			for (int i=0;i<strUTF8.size();++i)
			{
				bool flag=1;
				for (int j=0;j<str.strUTF8.size();++j)
					if (strUTF8[i]!=str.strUTF8[j])
					{
						flag=0;
						break;
					}
				if (flag)
					return i;
			}
		}
		
		int find(const std::string &str)
		{return find(stringUTF8(str));}
		
		int find(const char *str)
		{return find(stringUTF8(std::string(str)));}
		
		void erase(int pos,int len)
		{strUTF8.erase(strUTF8.begin()+pos,strUTF8.begin()+pos+len);}
		
		stringUTF8_WithData <T>& insert(int pos,const stringUTF8_WithData <T> &str,int srcPos,int srcLen)
		{
			if (pos>length()||pos<0)
				return *this;
			if (pos==length())
				append(str,srcPos,srcLen);
			else strUTF8.insert(strUTF8.begin()+pos,str.strUTF8.begin()+srcPos,str.strUTF8.begin()+srcPos+srcLen);
			return *this;
		}
		
		stringUTF8_WithData <T>& insert(int pos,const stringUTF8_WithData <T> &str)
		{
			if (pos>length()||pos<0)
				return *this;
			if (pos==length())
				append(str);
			else strUTF8.insert(strUTF8.begin()+pos,str.strUTF8.begin(),str.strUTF8.end());
			return *this;
		}
		
		stringUTF8_WithData <T>& insert(int pos,const stringUTF8 &str,const T &defaultData)
		{return insert(stringUTF8_WithData <T> (str,defaultData));}
		
		stringUTF8_WithData <T>& insert(int pos,const stringUTF8 &str)
		{return insert(stringUTF8_WithData <T> (str));}
		
		stringUTF8_WithData <T>& insert(int pos,const std::string &str,const T &defaultData)
		{return insert(pos,stringUTF8_WithData <T> (str,defaultData));}
		
		stringUTF8_WithData <T>& insert(int pos,const std::string &str)
		{return insert(pos,stringUTF8_WithData <T> (str));}
		
		stringUTF8_WithData <T>& insert(int pos,const char *str,const T &defaultData)
		{return insert(pos,stringUTF8_WithData <T> (str,defaultData));}
		
		stringUTF8_WithData <T>& insert(int pos,const char *str)
		{return insert(pos,stringUTF8_WithData <T> (str));}
		
		stringUTF8_WithData <T>& operator += (const stringUTF8_WithData <T> &str)
		{
			append(str);
			return *this;
		}
		
		stringUTF8_WithData <T>& operator += (const stringUTF8 &str)
		{
			append(str);
			return *this;
		}
		
		stringUTF8_WithData <T>& operator += (const std::string &str)
		{
			append(str);
			return *this;
		}
		
		stringUTF8_WithData <T>& operator += (const char *str)
		{
			append(str);
			return *this;
		}
		
		stringUTF8_WithData <T> operator + (const stringUTF8_WithData <T> &str) const
		{
			stringUTF8_WithData <T> re(*this);
			re.append(str);
			return re;
		}
		
		stringUTF8_WithData <T> operator + (const stringUTF8 &str) const
		{
			stringUTF8_WithData <T> re(*this);
			re.append(str);
			return re;
		}
		
		stringUTF8_WithData <T> operator + (const std::string &str) const
		{
			stringUTF8_WithData <T> re(*this);
			re.append(str);
			return re;
		}
		
		stringUTF8_WithData <T> operator + (const char *str) const
		{
			stringUTF8_WithData <T> re(*this);
			re.append(str);
			return re;
		}
		
		stringUTF8_WithData <T>& operator = (const stringUTF8_WithData <T> &str)
		{
			if (&str!=this)
				strUTF8=str.strUTF8;
			return *this;
		}
		
		stringUTF8_WithData <T>& operator = (const stringUTF8 &str)
		{
			strUTF8.clear();
			append(str);
			return *this;
		}
		
		stringUTF8_WithData <T>& operator = (const std::string &str)
		{
			strUTF8.clear();
			append(str);
			return *this;
		}
		
		stringUTF8_WithData <T>& operator = (const char *str)
		{
			strUTF8.clear();
			append(str);
			return *this;
		}
		
		std::string& operator [] (int x)
		{return strUTF8[x].ch;}
		
		const std::string& operator [] (int x) const
		{return strUTF8[x].ch;}
		
		T& operator () (int x)
		{return strUTF8[x].Data;}
		
		const T& operator () (int x) const
		{return strUTF8[x].Data;}
		
		stringUTF8_WithData(const stringUTF8_WithData <T> &str)
		:strUTF8(str.strUTF8) {}
		
		stringUTF8_WithData(const stringUTF8 &str,const T &defaultData)
		{append(str,defaultData);}
		
		stringUTF8_WithData(const stringUTF8 &str)
		{append(str);}
		
		stringUTF8_WithData(const std::string &str,const T &defaultData)
		{append(str,defaultData);}
		
		stringUTF8_WithData(const std::string &str)
		{append(str);}
		
		stringUTF8_WithData(const char *str,const T &defaultData)
		{append(str,defaultData);}
		
		stringUTF8_WithData(const char *str)
		{append(str);}
		
		stringUTF8_WithData() {}
};
#endif
