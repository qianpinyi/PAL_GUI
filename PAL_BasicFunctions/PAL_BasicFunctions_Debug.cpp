#ifndef PAL_BASICFUNCTIONS_DEBUG_CPP
#define PAL_BASICFUNCTIONS_DEBUG_CPP 1

#include <iostream>
#include <fstream>
#include <string>
#include <map>

enum DebugOut_Channel
{
	DebugOut_OFF=0,
	DebugOut_COUT,
	DebugOut_CERR,
	DebugOut_LOG,
	DebugOut_CERR_LOG,
	DebugOut_PAL_DEBUG
};

class Debug_Out
{
	#define DefaultDebugOut_Channel DebugOut_CERR
	#define DefaultDebugOutLog "DebugOut_Log.txt"
	
	friend Debug_Out& endl(Debug_Out&);
	private:
		bool DebugOn=1,
			 CurrentDebugTypeOutput=1;
		DebugOut_Channel CurrentOutMode=DefaultDebugOut_Channel;
		std::ofstream fout;
		unsigned char DebugTypeOn=0xff;//Use bit to control which types of info to output.
		std::string DebugTypeName[8]={"Info","Warning","Error","Debug"};//0~3 are preset type,it can be modified.
		std::map <std::string,unsigned char> DebugTypeNameToID;
		/*
			Future plan:
				Multi thread safe(Used safe queue in multi thread to storage the debug info?)
				 
		*/
		
		void InitDebugTypeName()
		{
			for (unsigned char i=0;i<=3;++i)
				DebugTypeNameToID[DebugTypeName[i]]=i;
		}
		
	public:
		void SwitchToContrary()
		{DebugOn=!DebugOn;}
		
		void Switch(bool on)
		{DebugOn=on;}
		
		bool SetLOGFile(const std::string &path,const std::ios_base::openmode &_mode)
		{
			fout.open(path,_mode);
			return fout.is_open();
		}
		
		bool SetLOGFile(const std::string &path)
		{
			fout.open(path);
			return fout.is_open();
		}
		
		Debug_Out& operator % (DebugOut_Channel X)
		{
			if (X==DebugOut_LOG||X==DebugOut_CERR_LOG)
			{
				if (!fout.is_open())
					fout.open(DefaultDebugOutLog);
				if (!fout.is_open())
					return *this;
			}
			CurrentOutMode=X;
			return *this;
		}
		
		void SetDebuOutChannel(DebugOut_Channel X)
		{operator %(X);}
		
		void SetDebugTypeOn(unsigned int on)
		{DebugTypeOn=on;}
		
		void SetDebugTypePosOnOff(unsigned int pos,bool on)
		{
			if (pos>=8) return;
			if (on) DebugTypeOn|=1<<pos;
			else DebugTypeOn&=~(1<<pos);
		}
		
		inline void SetCurrentDebugTypeOutput()
		{CurrentDebugTypeOutput=1;}
		
		void SetDebugType(unsigned char id,const std::string &name)
		{
			if (id>=8) return;
			DebugTypeNameToID.erase(DebugTypeName[id]);
			DebugTypeName[id]=name;
			DebugTypeNameToID[name]=id;
		}
		
		Debug_Out& operator << (Debug_Out& (*func)(Debug_Out&))
		{
			return (*func)(*this);
		}
		
		template <typename T> Debug_Out& operator << (const T &X)//const??
		{
			if (DebugOn&&CurrentDebugTypeOutput)
				switch (CurrentOutMode)
				{
					case DebugOut_OFF: 			break;
					case DebugOut_COUT: 		std::cout<<X; break;
					case DebugOut_CERR: 		std::cerr<<X; break;
					case DebugOut_LOG: 			fout<<X; break;
					case DebugOut_CERR_LOG: 	std::cerr<<X; fout<<X; break;
					case DebugOut_PAL_DEBUG:	break; 
				}
			return *this;
		}
		
		Debug_Out& operator [] (unsigned char p)//Current Debug Info is type p. It will be clear when Endline is set!
		{
			if (p<=7&&DebugTypeName[p]!="")
			{
				CurrentDebugTypeOutput=bool(DebugTypeOn&1<<p);
				*this<<"["<<DebugTypeName[p]<<"] ";
			}
			return *this;
		}
		
		Debug_Out& operator [] (const std::string &type)
		{
			auto mp=DebugTypeNameToID.find(type);
			if (mp!=DebugTypeNameToID.end())
				CurrentDebugTypeOutput=bool(DebugTypeOn&1<<mp->second);
			else CurrentDebugTypeOutput=1;
			*this<<"["<<type<<"] ";
			return *this;
		}
		
	#ifdef PAL_BASICFUNCTIONS_POSIZE_CPP
		Debug_Out& operator << (const Point &pt)
		{return *this<<pt.x<<" "<<pt.y;}
	
		Debug_Out& operator << (const Posize &ps)
		{return *this<<ps.x<<" "<<ps.y<<" "<<ps.w<<" "<<ps.h;}
	#endif
	
		Debug_Out(DebugOut_Channel X,const std::string &logFile="")
		{
			if (logFile!="")
				SetLOGFile(logFile);
			operator %(X);
			InitDebugTypeName();
		}
		
		Debug_Out(DebugOut_Channel X,const std::string &logFile,const std::ios_base::openmode &_mode)
		{
			SetLOGFile(logFile,_mode);
			operator %(X);
			InitDebugTypeName();
		}
		
		Debug_Out() {InitDebugTypeName();}
		
	#undef DefaultDebugOut_Channel
	#undef DefaultDebugOutLog
}DD;//DD is Default Debug_Out

Debug_Out& endl(Debug_Out &dd)
{
	dd<<"\n";
	dd.CurrentDebugTypeOutput=1;
	return dd;
}

class Test_ConDeStructorClass
{
	public:
		static int IDcnt;
		int ID;
		std::string msg;

		Test_ConDeStructorClass& operator = (const Test_ConDeStructorClass &t)
		{
			DD<<"Test_ConDeStructorClass Assign "<<t.msg<<" to "<<msg<<endl;
			msg=t.msg;
			return *this;
		}

		Test_ConDeStructorClass(const Test_ConDeStructorClass &t)
		{
			ID=++IDcnt;
			msg=t.msg;
			DD<<"Test_ConDeStructorClass CopyCon "<<ID<<" "<<msg<<endl;
		}
		
		Test_ConDeStructorClass(const std::string &_msg=""):msg(_msg)
		{
			ID=++IDcnt;
			DD<<"Test_ConDeStructorClass Con "<<ID<<" "<<msg<<endl;
		}
		
		~Test_ConDeStructorClass()
		{
			DD<<"Test_ConDeStructorClass De "<<ID<<" "<<msg<<endl;
		}
};
int Test_ConDeStructorClass::IDcnt=0;

class Test_ConDeStructorClass_WithMove
{
	public:
		static int IDcnt;
		int ID;
		std::string msg;

		Test_ConDeStructorClass_WithMove& operator = (const Test_ConDeStructorClass_WithMove &t)
		{
			DD<<"Test_ConDeStructorClass_WithMove Assign "<<t.msg<<" to "<<msg<<endl;
			msg=t.msg;
			return *this;
		}
		
		Test_ConDeStructorClass_WithMove& operator = (const Test_ConDeStructorClass_WithMove &&t)
		{
			msg=std::move(t.msg);
			DD<<"Test_ConDeStructorClass_WithMove MoveAssign "<<ID<<" "<<msg<<endl;
			return *this;
		}
		
		Test_ConDeStructorClass_WithMove(const Test_ConDeStructorClass_WithMove &&t)
		{
			ID=++IDcnt;
			msg=move(t.msg);
			DD<<"Test_ConDeStructorClass_WithMove MoveCon "<<ID<<" "<<msg<<endl;
		}

		Test_ConDeStructorClass_WithMove(const Test_ConDeStructorClass_WithMove &t)
		{
			ID=++IDcnt;
			msg=t.msg;
			DD<<"Test_ConDeStructorClass_WithMove CopyCon "<<ID<<" "<<msg<<endl;
		}
		
		Test_ConDeStructorClass_WithMove(const std::string &_msg=""):msg(_msg)
		{
			ID=++IDcnt;
			DD<<"Test_ConDeStructorClass_WithMove Con "<<ID<<" "<<msg<<endl;
		}
		
		~Test_ConDeStructorClass_WithMove()
		{
			DD<<"Test_ConDeStructorClass_WithMove De "<<ID<<" "<<msg<<endl;
		}
};
int Test_ConDeStructorClass_WithMove::IDcnt=0;

void Debug_PrintStr(const std::string &str)
{
	DD<<"Debug_PrintStr: "<<str<<"\n";
	for (unsigned int i=0;i<str.length();++i)
		DD<<str[i]<<"|";
	DD<<"\n";
	for (unsigned int i=0;i<str.length();++i)
		DD<<(int)str[i]<<"|";
	DD<<"\n";
}

#endif
