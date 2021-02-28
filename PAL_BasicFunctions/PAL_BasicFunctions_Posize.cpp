#ifndef PAL_BASICFUNCTIONS_POSIZE_CPP
#define PAL_BASICFUNCTIONS_POSIZE_CPP 1

#include <algorithm>
//#include "PAL_BasicFunctions_Debug.cpp"

struct Range
{
	int l,r;
	
	inline bool Len0() const
	{return l==r;}
	
	inline bool IsEmpty() const
	{return l>r;}
	
	inline int Length() const
	{return r-l;}
	
	template <typename T> bool InRange(const T &x) const
	{return l<=x&&x<=r;}
	
	template <typename T> T EnsureInRange(const T &x) const
	{
		if (x<l) return l;
		else if (x>r) return r;
		else return x;
	}
	
	inline void Shift(const int &x)
	{l+=x;r+=x;}
	
	inline Range operator & (const Range &rg) const
	{return {std::max(l,rg.l),std::min(r,rg.r)};}
	
	Range() {}
	Range(const int &_l,const int &_r):l(_l),r(_r) {}
};

struct Point
{
	int x,y;
	
	inline Point operator + (const Point &_pt) const
	{return {x+_pt.x,y+_pt.y};}

	inline Point operator - (const Point &_pt) const
	{return {x-_pt.x,y-_pt.y};}
	
	inline bool operator == (const Point &_pt) const
	{return x==_pt.x&&y==_pt.y;}
	
	inline int operator * (const Point &_pt) const
	{return x*_pt.x+y*_pt.y;}
	
	inline int operator % (const Point &_pt) const
	{return x*_pt.y-y*_pt.x;}
	
	Point() {}
	Point(const int &_x,const int &_y):x(_x),y(_y) {}
};

struct Posize
{
	int x,y,w,h;
	
	inline int x2() const
	{return x+w-1;}
	
	inline int y2() const
	{return y+h-1;}
	
	inline int midX() const
	{return (x+x2())>>1;}
	
	inline int midY() const
	{return (y+y2())>>1;}
	
	inline Point GetCentre() const
	{return {midX(),midY()};}
	
	inline void SetCentre(const Point &pt)
	{
		x=pt.x-((w+1)>>1)+1;
		y=pt.y-((h+1)>>1)+1;
	}
	
	inline Point GetLU() const
	{return {x,y};}
	
	inline Point GetLD() const
	{return {x,y2()};}
	
	inline Point GetRU() const
	{return {x2(),y};}
	
	inline Point GetRD() const
	{return {x2(),y2()};}
	
	inline Posize ToOrigin() const
	{return {0,0,w,h};}
	
	inline int Size() const
	{return w*h;}
	
	inline double Slope() const
	{return w==0?1e100:h*1.0/w;}
	
	inline double InverseSlope() const
	{return h==0?1e100:w*1.0/h;}
	
	inline bool In(const int &xx,const int &yy) const
	{return x<=xx&&xx<x+w&&y<=yy&&yy<y+h;}
	
	inline bool In(const Point &pt) const
	{return x<=pt.x&&pt.x<x+w&&y<=pt.y&&pt.y<y+h;}
	
	inline void SetX_ChangeW(const int &_x)
	{w=x2()-_x+1;x=_x;}
	
	inline void SetY_ChangeH(const int &_y)
	{h=y2()-_y+1;y=_y;}
	
	inline void SetX2(const int &_x2)
	{w=_x2-x+1;}
	
	inline void SetY2(const int &_y2)
	{h=_y2-y+1;}
	
	inline void SetX2_ChangeX(const int &_x2)
	{x=_x2-w+1;}
	
	inline void SetY2_ChangeY(const int &_y2)
	{y=_y2-h+1;}
	
	inline void SetW_ChangeX(const int &_w)
	{x=x2()-_w+1;w=_w;}
	
	inline void SetH_ChangeY(const int &_h)
	{y=y2()-_h+1;h=_h;}
	
	inline Point EnsureIn(const Point &_pt) const
	{
		using namespace std;
		return {max(min(_pt.x,x2()),x),max(min(_pt.y,y2()),y)};
	}
		
	inline Posize EnsureIn(Posize _ps)
	{
		if (_ps.w>w) _ps.w=w;
		if (_ps.h>h) _ps.h=h;
		if (_ps.x2()>x2()) _ps.SetX2_ChangeX(x2());
		if (_ps.y2()>y2()) _ps.SetY2_ChangeY(y2());
		if (_ps.x<x) _ps.x=x;
		if (_ps.y<y) _ps.y=y;
		return _ps;	
	}
	
	inline Posize operator & (const Posize &_ps) const
	{
		using namespace std;
		Posize re;
		re.x=max(x,_ps.x);
		re.y=max(y,_ps.y);
		re.w=max(0,min(x2(),_ps.x2())-re.x+1);
		re.h=max(0,min(y2(),_ps.y2())-re.y+1);
		return re;
	}
	
	inline Posize operator | (const Posize &_ps) const
	{
		using namespace std;
		if (_ps.w*_ps.h==0)
			return *this;
		if (w*h==0)
			return _ps;
		Posize re;
		re.x=min(x,_ps.x);
		re.y=min(y,_ps.y);
		re.w=max(0,max(x2(),_ps.x2())-re.x+1);
		re.h=max(0,max(y2(),_ps.y2())-re.y+1);
		return re;
	}
	
	inline Posize& operator |= (const Posize &_ps)
	{
		using namespace std;
		if (_ps.w*_ps.h==0)
			return *this;
		Posize re;
		if (w*h==0)
			re=_ps;
		else
		{
			re.x=min(x,_ps.x);
			re.y=min(y,_ps.y);
			re.w=max(0,max(x2(),_ps.x2())-re.x+1);
			re.h=max(0,max(y2(),_ps.y2())-re.y+1);
		}
		x=re.x;y=re.y;w=re.w;h=re.h;
//		DD<<"#1:"<<_ps.x<<" "<<_ps.y<<" "<<_ps.w<<" "<<_ps.h<<endl;
//		DD<<"#2:"<<x<<" "<<y<<" "<<w<<" "<<h<<endl;
		return *this;
	}
	
	inline Posize operator + (const Posize &_ps) const
	{return {x+_ps.x,y+_ps.y,w,h};}
	
	inline Posize operator + (const Point &_pt) const
	{return {x+_pt.x,y+_pt.y,w,h};}
	
	inline Posize operator - (const Posize &_ps) const
	{return {x-_ps.x,y-_ps.y,w,h};}
	
	inline Posize operator - (const Point &_pt) const
	{return {x-_pt.x,y-_pt.y,w,h};}
	
	inline bool operator == (const Posize &_ps) const
	{return x==_ps.x&&y==_ps.y&&w==_ps.w&&h==_ps.h;}
	
	inline Posize Expand(const int &d) const
	{return {x-d,y-d,w+2*d,h+2*d};}
	
	inline Posize Shrink(const int &d) const
	{return {x+d,y+d,w-2*d,h-2*d};}
	
	inline Posize Flexible(const double &px,const double &py) const
	{return Posize(x*px+0.5,y*py+0.5,w*px+0.5,h*py+0.5);}
	
	Posize() {}
	Posize(const int &_x,const int &_y,const int &_w,const int &_h):x(_x),y(_y),w(_w),h(_h) {}
	Posize(const Point &pt,int _w,int _h):x(pt.x),y(pt.y),w(_w),h(_h) {}
};

const Range ZERO_RANGE={0,0};
const Point ZERO_POINT={0,0};
const Posize ZERO_POSIZE={0,0,0,0};

Point MakePoint(int x,int y)//NotRecommended
{return {x,y};}

Posize MakePosize(int x,int y,int w,int h)//NotRecommended
{return {x,y,w,h};}
#endif

#ifdef USE_SDL
inline void PosizeToSDLRect(const Posize &src,SDL_Rect &tar)
{
	tar.x=src.x;
	tar.y=src.y;
	tar.w=src.w;
	tar.h=src.h;
}

inline const SDL_Rect PosizeToSDLRect(const Posize &src)
{return {src.x,src.y,src.w,src.h};}

inline void SDLRectToPosize(const SDL_Rect &src,Posize &tar)
{
	tar.x=src.x;
	tar.y=src.y;
	tar.w=src.w;
	tar.h=src.h;
}

inline Posize SDLRectToPosize(const SDL_Rect &src)
{return {src.x,src.y,src.w,src.h};}
#endif
