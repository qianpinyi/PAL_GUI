#ifndef PAL_BASICFUNCTIONS_COLOR_CPP
#define PAL_BASICFUNCTIONS_COLOR_CPP 1
struct RGBA
{
	#define Uint8 unsigned char
	Uint8 r,g,b,a=255;
	
	inline float Opacity() const
	{return a/255.0;}
	
	inline bool TransParent() const
	{return a!=255;}
	
	inline bool HaveColor() const
	{return a!=0/*r!=0||g!=0||b!=0||a!=0*/;}
	
	inline bool operator ! () const
	{return r==0&&g==0&&b==0&&a==0;}
	
	inline explicit operator bool () const 
	{return r!=0||g!=0||b!=0||a!=0;}
	
	inline void SetColor(const Uint8 &_r,const Uint8 &_g,const Uint8 &_b)
	{r=_r;g=_g;b=_b;}
	
	inline void SetColor(const Uint8 &_r,const Uint8 &_g,const Uint8 &_b,const Uint8 &_a)
	{r=_r;g=_g;b=_b;a=_a;}
	
//	inline RGBA operator + (const RGBA &co) const
//	{
//		//...
//	}
//	
//	inline RGBA operator * (const RGBA &co) const
//	{
//		//...
//	}
//	
//	inline RGBA operator % (const RGBA &co) const
//	{
//		//...
//	}
	
	inline bool operator == (const RGBA &co) const
	{return {r==co.r&&g==co.g&&b==co.b&&a==co.a};}
	
	Uint8 ToGray()
	{return (r*0.299+g*0.587+b*0.114)*a/255.0;}
	
	RGBA() {}
	RGBA(const Uint8 &_r,const Uint8 &_g,const Uint8 &_b):r(_r),g(_g),b(_b) {} 
	RGBA(const Uint8 &_r,const Uint8 &_g,const Uint8 &_b,const Uint8 &_a):r(_r),g(_g),b(_b),a(_a) {} 
	#undef Uint8
};

const RGBA RGBA_WHITE={255,255,255,255},
	RGBA_BLACK={0,0,0,255},
	RGBA_NONE={0,0,0,0},
	RGBA_TRANSPARENT={255,255,255,0},
	RGBA_BLUE_8A[8]={{0,100,255,31},{0,100,255,63},{0,100,255,95},{0,100,255,127},{0,100,255,159},{0,100,255,191},{0,100,255,223},{0,100,255,255}},
	RGBA_GRAY_B8A[8]={{0,0,0,31},{0,0,0,63},{0,0,0,95},{0,0,0,127},{0,0,0,159},{0,0,0,191},{0,0,0,223},{0,0,0,255}},
	RGBA_GRAY_W8A[8]={{255,255,255,31},{255,255,255,63},{255,255,255,95},{255,255,255,127},{255,255,255,159},{255,255,255,191},{255,255,255,223},{255,255,255,255}},
	RGBA_BLUE_8[8]={{224,236,255,255},{192,217,255,255},{160,197,255,255},{128,178,255,255},{96,158,255,255},{64,139,255,255},{32,119,255,255},{0,100,255,255}},
	RGBA_GRAY_8[8]={{224,224,224,255},{192,192,192,255},{160,160,160,255},{128,128,128,255},{96,96,96,255},{64,64,64,255},{32,32,32,255},{0,0,0,255}},
	RGBA_GREEN_8[8]={{224,255,224,255},{192,255,192,255},{160,255,160,255},{128,255,128,255},{96,255,96,255},{64,255,64,255},{32,255,32,255},{0,255,0,255}};
/*
RGBA ConvertToOpaque(const RGBA &co,const RGBA &bg)
{
	
}

RGBA ConvertToOpaque(const RGBA &co)
{
	
}
*/
#endif

#ifdef USE_SDL
#ifndef PAL_BASICFUNCTIONS_COLOR_CPP_COLORWITHSDL
#define PAL_BASICFUNCTIONS_COLOR_CPP_COLORWITHSDL 1
inline void SDLColorToRGBA(const SDL_Color &src,RGBA &tar)
{
	tar.r=src.r;
	tar.g=src.g;
	tar.b=src.b;
	tar.a=src.a;
}

inline RGBA SDLColorToRGBA(const SDL_Color &src)
{return {src.r,src.g,src.b,src.a};}

inline void RGBAToSDLColor(const RGBA &src,SDL_Color &tar)
{
	tar.r=src.r;
	tar.g=src.g;
	tar.b=src.b;
	tar.a=src.a;
}

inline SDL_Color RGBAToSDLColor(const RGBA &src)
{return {src.r,src.g,src.b,src.a};}
#endif
#endif
