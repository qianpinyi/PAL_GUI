#ifndef PAL_BASICFUNCTIONS_0_CPP
#define PAL_BASICFUNCTIONS_0_CPP 1

#define null NULL

#include <string>
#include <ctime>

void * const CONST_THIS=new int(0);
void * const CONST_TRUE=new int(0);
void * const CONST_FALSE=new int(0);
void * const CONST_Ptr_0=new int(0);
void * const CONST_Ptr_1=new int(1);
void * const CONST_Ptr_2=new int(2);
void * const CONST_Ptr_3=new int(3);

template <typename T1,typename T2,typename T3> T1 EnsureInRange(const T1 &x,const T2 &L,const T3 &R) 
{
	if (x<L) return L;
	else if (x>R) return R;
	else return x;
}

template <typename T1,typename T2,typename T3>  bool InRange(const T1 &x,const T2 &L,const T3 &R)
{return L<=x&&x<=R;}

template <typename T> void DeleteToNULL(T *&ptr)
{
	delete ptr;
	ptr=NULL;
}

template <typename T> void DELETEtoNULL(T *&ptr)
{
	delete[] ptr;
	ptr=NULL;
}

char *GetTime1()
{
	time_t rawtime;
	time(&rawtime);
	return ctime(&rawtime);
}

bool IsLeapYear(int y)
{return (y%4==0&&y%100!=0)||y%400==0;}

int GetMonthDaysCount(int m,bool isLeapYear=0)
{
	const static int mon[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	if (m==2) return mon[1]+isLeapYear;
	else return mon[m-1];
}

#endif
