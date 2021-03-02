#ifndef PAL_GUI_0_CPP
#define PAL_GUI_0_CPP 1
/*
	Refactoring of PAL_GUI_Alpha
	By:qianpinyi
	
//	0:Core
//	1:Basic widgets
//	2.Advanced widgets
//	//3.Widgets transferred from PUIA 
//	4.Others
	
	Plan:
		
		
	Memo:
		
		
	Bugs:
		
*/
#include <string>
#include <set>
#include <map>
#include <cstring>
#include <atomic>
#include <queue>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_syswm.h>

#undef main

#define USE_SDL

#include "../PAL_BasicFunctions/PAL_BasicFunctions_0.cpp"
#include "../PAL_BasicFunctions/PAL_BasicFunctions_Posize.cpp"
#include "../PAL_BasicFunctions/PAL_BasicFunctions_StringEX.cpp"
#include "../PAL_BasicFunctions/PAL_BasicFunctions_Color.cpp"
#include "../PAL_BasicFunctions/PAL_BasicFunctions_Debug.cpp"
#include "../PAL_DataStructure/PAL_SplayTree.cpp"

//#define USE_WINDOWS_IME

#if defined(USE_WINDOWS_IME)
	#include <Windows.h>
#endif

namespace PAL_GUI
{
	using namespace std;
	
	//Global option:
	//...
	int WheelSensibility=40;
	bool PUISkipEventFlag=0,
		 PUISkipFrameFlag=0;
	
	//Debug option:
	//...
	Debug_Out PUI_DD;
	
	bool DEBUG_DisplayBorderFlag=0,
		 DEBUG_EnableWidgetsShowInTurn=0,
		 DEBUG_DisplayPresentLimitFlag=0;
	
	int DEBUG_WidgetsShowInTurnDelayTime=100;
	
	//Data
	//...
	struct SharedTexturePtr
	{
		protected:
			int *count=NULL;
			SDL_Texture *tex=NULL;
			
		public:
			inline bool operator ! ()
			{return tex==NULL;}
			
			inline SDL_Texture* operator () ()
			{return tex;}
			
			inline SDL_Texture* GetPic()
			{return tex;}
			
			SharedTexturePtr& operator = (const SharedTexturePtr &tar)
			{
				if (&tar==this)
					return *this;
				if (tex!=NULL)
				{
					--*count;
					if (*count==0)
					{
						SDL_DestroyTexture(tex);
						delete count;
					}
					tex=NULL;
					count=NULL;
				}
				tex=tar.tex;
				count=tar.count;
				if (tex!=NULL)
					++*count;
				return *this;
			}
			
			~SharedTexturePtr()
			{
				if (tex!=NULL)
				{
					--*count;
					if (*count==0)
					{
						SDL_DestroyTexture(tex);
						delete count;
					}
				}
			}
			
			SharedTexturePtr(const SharedTexturePtr &tar)
			{
				tex=tar.tex;
				count=tar.count;
				if (tex!=NULL)
					++*count;
			}
			
			explicit SharedTexturePtr(SDL_Texture *tarTex)
			{
				tex=tarTex;
				if (tex!=NULL)
					count=new int(1);
			}
			
			explicit SharedTexturePtr() {}
	};
	
	struct PUI_ThemeColor
	{
		enum
		{
			PUI_ThemeColor_Blue,
			PUI_ThemeColor_Green,
			PUI_ThemeColor_Red,
			PUI_ThemeColor_Pink,
			PUI_ThemeColor_Orange,
//			PUI_ThemeColor_DarkBlue,
			PUI_ThemeColor_Violet,
//			PUI_ThemeColor_Silvery,
//			PUI_ThemeColor_White,
//			PUI_ThemeColor_Gray,
//			PUI_ThemeColor_Black,
			PUI_ThemeColor_DeepGreen,
//			PUI_ThemeColor_Colorful,
			PUI_ThemeColor_UserDefined
		};
		
		RGBA MainColor[8],
			 BackgroundColor[8],
			 MainTextColor[2];
		int CurrentThemeColor=0;
			 
		inline RGBA& operator [] (int x)
		{
			if (InRange(x,0,7))
				return MainColor[x];
			else return PUI_DD[1]<<"ThemeColor[x],x is not in Range[0,7],MainColor[7] is used as needed."<<endl,MainColor[7];
		}
		
		PUI_ThemeColor(int theme=PUI_ThemeColor_Blue)
		{
			switch (theme)
			{
				default:
					theme=PUI_ThemeColor_UserDefined;
				case PUI_ThemeColor_Blue:
					MainColor[0]={224,236,255,255};
					MainColor[1]={192,217,255,255};
					MainColor[2]={160,197,255,255};
					MainColor[3]={128,178,255,255};
					MainColor[4]={96,158,255,255};
					MainColor[5]={64,139,255,255};
					MainColor[6]={32,119,255,255};
					MainColor[7]={0,100,255,255};
					break;
				case PUI_ThemeColor_Green:
					MainColor[0]={224,255,224,255};
					MainColor[1]={192,255,192,255};
					MainColor[2]={160,255,160,255};
					MainColor[3]={128,255,128,255};
					MainColor[4]={96,255,96,255};
					MainColor[5]={64,255,64,255};
					MainColor[6]={32,255,32,255};
					MainColor[7]={0,255,0,255};
					break;
				case PUI_ThemeColor_Red:
					MainColor[0]={255,140,140,255};
					MainColor[1]={255,120,120,255};
					MainColor[2]={255,100,100,255};
					MainColor[3]={255,80,80,255};
					MainColor[4]={255,60,60,255};
					MainColor[5]={255,40,40,255};
					MainColor[6]={255,20,20,255};
					MainColor[7]={255,0,0,255};
					break;
				case PUI_ThemeColor_Pink:
					MainColor[0]={255,198,222,255};
					MainColor[1]={254,188,200,255};
					MainColor[2]={255,178,191,255};
					MainColor[3]={250,158,173,255};
					MainColor[4]={255,140,153,255};
					MainColor[5]={255,120,131,255};
					MainColor[6]={253,108,118,255};
					MainColor[7]={253,88,99,255};
					break;
				case PUI_ThemeColor_Orange:
					MainColor[0]={255,223,182,255};
					MainColor[1]={255,210,170,255};
					MainColor[2]={255,202,168,255};
					MainColor[3]={255,192,124,255};
					MainColor[4]={255,174,108,255};
					MainColor[5]={255,161,93,255};
					MainColor[6]={255,140,55,255};
					MainColor[7]={255,81,0,255};
					break;
//				case PUI_ThemeColor_DarkBlue:
//					
//					break;
				case PUI_ThemeColor_Violet:
					MainColor[0]={167,171,248,255};
					MainColor[1]={145,155,229,255};
					MainColor[2]={125,127,232,255};
					MainColor[3]={94,108,235,255};
					MainColor[4]={83,97,225,255};
					MainColor[5]={75,84,221,255};
					MainColor[6]={70,73,217,255};
					MainColor[7]={50,64,163,255};
					break;
//				case PUI_ThemeColor_Silvery:
//					
//					break;
//				case PUI_ThemeColor_White:
//					
//					break;
//				case PUI_ThemeColor_Gray:
//					
//					break;
//				case PUI_ThemeColor_Black:
//					
//					break;
				case PUI_ThemeColor_DeepGreen:
					MainColor[0]={134,215,161,255};
					MainColor[1]={122,201,154,255};
					MainColor[2]={96,195,130,255};
					MainColor[3]={64,191,100,255};
					MainColor[4]={47,179,102,255};
					MainColor[5]={51,158,81,255};
					MainColor[6]={39,144,98,255};
					MainColor[7]={11,116,27,255};
					break;
//				case PUI_ThemeColor_Colorful:
//					break;
			}
			
			switch (theme)
			{
				default:
					BackgroundColor[0]={250,250,250,255};
					BackgroundColor[1]={224,224,224,255};
					BackgroundColor[2]={192,192,192,255};
					BackgroundColor[3]={160,160,160,255};
					BackgroundColor[4]={128,128,128,255};
					BackgroundColor[5]={96,96,96,255};
					BackgroundColor[6]={64,64,64,255};
					BackgroundColor[7]={32,32,32,255};
					MainTextColor[0]=RGBA_BLACK;
					MainTextColor[1]=RGBA_WHITE;
					break;
			}
			CurrentThemeColor=theme;
		}
	}ThemeColor;
	
	//Font manage
	//...
	
	struct PUI_Font_Struct
	{
		private:
			TTF_Font *font[128];
			string fontFile;
			int defaultSize=-1;
			
			void Deconstruct()
			{
				for (int i=1;i<=127;++i)
					if (font[i]!=NULL)
						TTF_CloseFont(font[i]),
						font[i]=NULL;
			}

		public:
			inline bool operator ! ()
			{return fontFile.empty();}
			
			inline TTF_Font* operator () ()
			{return font[defaultSize];}
			
			inline TTF_Font* operator [] (int x)
			{
				if (x==0)
					return font[defaultSize];
				if (InRange(x,1,127))
					if (font[x]==NULL)
						return font[x]=TTF_OpenFont(fontFile.c_str(),x);
					else return font[x];
				else return PUI_DD[2]<<"PUI_Font[x] x is not in range[1,127]"<<endl,font[defaultSize];
			}
			
			~PUI_Font_Struct()
			{Deconstruct();}
			
			void SetDefaultSize(int _defaultsize)
			{defaultSize=_defaultsize;}
			
			void SetFontFile(const string &_fontfile)
			{
				Deconstruct();
				fontFile=_fontfile;
				static bool Inited=0;
				if (!Inited)
				{
					if (TTF_Init()!=0)
						PUI_DD[2]<<"Failed to TTF_Init()"<<endl;
					Inited=1;
				}	
				font[defaultSize]=TTF_OpenFont(fontFile.c_str(),defaultSize);
			}
			
			PUI_Font_Struct(const string &_fontfile,const int _defaultsize)
			{
				memset(font,0,sizeof font);
				SetDefaultSize(_defaultsize);
				SetFontFile(_fontfile);
			}
			
			PUI_Font_Struct()
			{memset(font,0,sizeof font);}
	}PUI_DefaultFonts;//???
	
	struct PUI_Font
	{
		PUI_Font_Struct *fonts=NULL;
		int Size=0;
		
		inline TTF_Font* operator () () const
		{return (*fonts)[Size];}

		PUI_Font(PUI_Font_Struct *_fonts=&PUI_DefaultFonts,int _size=0):fonts(_fonts),Size(_size) {}
		PUI_Font(PUI_Font_Struct &_fonts,int _size=0):fonts(&_fonts),Size(_size) {}
		PUI_Font(int _size):fonts(&PUI_DefaultFonts),Size(_size) {}
	};
	
	//Surface/Texture/Drawing/... Functions:
	
//	void RenderDrawPointWithLmt(const Point &pt,const RGBA &co,const Posize &lmt,const PUI_Window *win=CurrentWindow)
//	{
//		if (lmt.In(pt)&&co.a!=0)//Is it slow?
//		{
//			SDL_SetRenderDrawColor(win->ren,co.r,co.g,co.b,co.a);
//			SDL_RenderDrawPoint(win->ren,pt.x,pt.y);
//		}
//	}
	
//	void RenderDrawShadow(/*...*/)
//	{
//		//...
//	}

	Posize GetTexturePosize(SDL_Texture *tex)
	{
		if (tex==NULL) return ZERO_POSIZE;
		Posize re;
		re.x=re.y=re.w=re.h=0;
		SDL_QueryTexture(tex,NULL,NULL,&re.w,&re.h);
		return re;
	}
	
	SDL_Surface *CreateRGBATextSurface(const char *text,const RGBA &co,const PUI_Font &font=PUI_Font())
	{
		if (text==NULL) return NULL;
		SDL_Surface *sur=TTF_RenderUTF8_Blended(font(),text,RGBAToSDLColor(co));
		SDL_SetSurfaceBlendMode(sur,SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceAlphaMod(sur,co.a);
		return sur;
	}
	
	SDL_Surface *CreateRingSurface(int d1,int d2,const RGBA &co)//return a ring whose radius is d1/2 and d2/2 
	{
		if (d2<=0) return NULL;
		SDL_Surface *sur=SDL_CreateRGBSurfaceWithFormat(0,d2,d2,32,SDL_PIXELFORMAT_RGBA32);
		SDL_SetSurfaceBlendMode(sur,SDL_BLENDMODE_BLEND);
		Uint32 col=SDL_MapRGBA(sur->format,co.r,co.g,co.b,co.a),col0=SDL_MapRGBA(sur->format,0,0,0,0);
		
		int r1=d1/2,r2=d2/2;
		for (int i=0;i<d2;++i)
			for (int j=0;j<d2;++j)
				*((Uint32*)sur->pixels+i*sur->pitch/4+j)=InRange((i-r2)*(i-r2)+(j-r2)*(j-r2),r1*r1,r2*r2)?col:col0;
		return sur;
	}
	
	SDL_Surface* CreateTriangleSurface(int w,int h,const Point &pt1,const Point &pt2,const Point &pt3,const RGBA &co)
	{
		SDL_Surface *sur=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_RGBA32);
		SDL_SetSurfaceBlendMode(sur,SDL_BLENDMODE_BLEND);
		SDL_Rect rct;
		Uint32 col=SDL_MapRGBA(sur->format,co.r,co.g,co.b,co.a),col0=SDL_MapRGBA(sur->format,0,0,0,0);
		for (int i=0;i<h;++i)
		{
			int j,k;
			for (j=0;j<w;++j)
			{
				Point v0(j,i),
					  v1=pt1-v0,
					  v2=pt2-v0,
					  v3=pt3-v0;
				if (abs(v1%v2)+abs(v2%v3)+abs(v3%v1)==abs((pt2-pt1)%(pt3-pt1)))
					break;
			}
			for (k=w-1;k>=j;--k)
			{
				Point v0(k,i),
					  v1=pt1-v0,
					  v2=pt2-v0,
					  v3=pt3-v0;
				if (abs(v1%v2)+abs(v2%v3)+abs(v3%v1)==abs((pt2-pt1)%(pt3-pt1)))
					break;
			}
			if (j>0)
			{
				rct={0,i,j,1};
				SDL_FillRect(sur,&rct,col0);
			}
			if (k>j)
			{
				rct={j,i,k-j+1,1};
				SDL_FillRect(sur,&rct,col);
			}
			if (k<w-1)
			{
				rct={k+1,i,w-k-1,1};
				SDL_FillRect(sur,&rct,col0);
			}
		}
		return sur;
	}
	
	SDL_Surface* CreateSpotLightSurface(int r,const RGBA &co={255,255,255,100})//return a round whose radius is r,alpha color decrease from center to edge 
	{
		if (r<=0) return NULL;
		PUI_DD[3]<<"CreateSpotLightSurface"<<endl;
		SDL_Surface *sur=SDL_CreateRGBSurfaceWithFormat(0,r<<1,r<<1,32,SDL_PIXELFORMAT_RGBA32);
		SDL_SetSurfaceBlendMode(sur,SDL_BLENDMODE_BLEND);
		
		for (int i=0;i<=r<<1;++i)
			for (int j=0;j<=r<<1;++j)
				*((Uint32*)sur->pixels+i*sur->pitch/4+j)=SDL_MapRGBA(sur->format,co.r,co.g,co.b,co.a*max(0.0,1-((i-r)*(i-r)+(j-r)*(j-r))*1.0/(r*r)));
		return sur;
	}
	
	RGBA GetSDLSurfacePixel(SDL_Surface *sur,const Point &pt)//maybe format has some problem
	{
		if (sur==NULL||!Posize(0,0,sur->w-1,sur->h-1).In(pt))
			return RGBA_NONE;
		RGBA re=RGBA_NONE;
		SDL_LockSurface(sur);
		Uint32 col=((Uint32*)sur->pixels)[(pt.y*sur->w+pt.x)];
		SDL_UnlockSurface(sur);
		SDL_GetRGBA(col,sur->format,&re.r,&re.g,&re.b,&re.a);
		return re;
	}
	
	void SetSDLSurfacePixel(SDL_Surface *sur,const Point &pt,const RGBA &co)
	{
		if (sur==NULL||!Posize(0,0,sur->w-1,sur->h-1).In(pt))
			return;
		SDL_Rect rct={pt.x,pt.y,1,1};
		Uint32 col=SDL_MapRGBA(sur->format,co.r,co.g,co.b,co.a);
		SDL_FillRect(sur,&rct,col);
	}
	
	class Widgets;
	class Layer;
	class PUI_Window//A window(to support multi windows)
	{
		friend bool PresentWidgets();
		friend void PresentWidgets(PUI_Window *win,Posize lmt);
		friend void PresentWidgets(Widgets *tar);
		friend void UpdateWidgetsPosize(Widgets *wg);
		friend void UpdateWidgetsPosize(PUI_Window *win);
		friend int SolveEvent(SDL_Event &event);
		friend class Widgets;
		
		public:
			static const int PUI_WINPS_CENTER,
					  	 	 PUI_WINPS_UNDEFINE;
		
		protected:
			static set <PUI_Window*> AllWindow;
			static map <unsigned int,PUI_Window*> WinOfSDLWinID;
			static int WindowCnt;
			static bool NeedFreshScreenAll;
			
			string WindowTitle;
			SDL_Window *win=NULL;
			SDL_Renderer *ren=NULL;
			unsigned int SDLWinID;
			
			SDL_Event *NowSolvingEvent=NULL;
		
			bool NeedFreshScreen=1,
				 NeedUpdatePosize=1,
				 NeedSolveEvent=0,
				 NeedSolvePosEvent=0,
				
				 PosFocused=1,
				 KeyboardFocused=1,
				 Hidden=0;
			int NowSolvingPosEventMode=0;//0:common 1:from LoseFocus 2:from OccupyPos 3:virtual PosEvent
			
			Posize WinPS,
				   PresentLimit;
		
			Point _NowPos;
			
			Layer *_BackGroundLayer=NULL,
				  *_MenuLayer=NULL;
			
			struct LoseFocusLinkTable
			{
				LoseFocusLinkTable *nxt=NULL;
				Widgets *wg=NULL;
				
				LoseFocusLinkTable(Widgets *tar):wg(tar) {}
			};
			LoseFocusLinkTable *LoseFocusWgHead=NULL;//The original OccupyPos
			int LoseFocusState=0;
			
			Widgets *OccupyPosWg=NULL;
			Widgets *KeyboardInputWg=NULL;
			
			BaseTypeFuncAndData *CloseFunc=NULL;//The function to call when closed button was click
			
		public:
			static set <PUI_Window*>& GetAllWindowSet()
			{return AllWindow;}
			
			static PUI_Window* GetWindowBySDLID(int _ID)
			{
				auto mp=WinOfSDLWinID.find(_ID);
				if (mp==WinOfSDLWinID.end())
					return NULL;
				else return mp->second;
			}
			
			inline unsigned int GetSDLWinID()
			{return SDLWinID;}
			
			inline SDL_Window* GetSDLWindows()
			{return win;}
			
			inline SDL_Renderer* GetSDLRenderer()
			{return ren;}
			
			inline void SetWindowGrab(bool enable)
			{SDL_SetWindowGrab(win,enable?SDL_TRUE:SDL_FALSE);}
			
			inline const Point& NowPos()
			{return _NowPos;}
			
			inline Layer* BackGroundLayer()
			{return _BackGroundLayer;}
			
			inline Layer* MenuLayer()
			{return _MenuLayer;}
			
			inline void SetWindowTitle(const string &title)
			{
				WindowTitle=title;
				SDL_SetWindowTitle(win,title.c_str());
			}
			
			inline string GetWindowTitle()
			{return WindowTitle;}

			inline Posize GetWinPS()
			{return WinPS;}
			
			inline Uint32 GetSDLWindowFlags()
			{return	SDL_GetWindowFlags(win);}
			
			inline void SetMinWinSize(int _w,int _h)
			{SDL_SetWindowMinimumSize(win,_w,_h);}

			inline void SetMaxWinSize(int _w,int _h)
			{SDL_SetWindowMaximumSize(win,_w,_h);}
			
			inline void SetWindowPos(const Point &pos)
			{
				WinPS.x=pos.x;
				WinPS.y=pos.y;
				SDL_SetWindowPosition(win,pos.x,pos.y);
			}
			
			inline void SetWindowSize(int w,int h)
			{
				WinPS.w=w;
				WinPS.h=h;
				SDL_SetWindowSize(win,w,h);
			}
			
			inline void SetWinPS(const Posize &winps)
			{
				SetWindowPos(winps.GetLU());
				SetWindowSize(winps.w,winps.h);
			}
			
			inline void SetMaxWindowSize(int w,int h)
			{SDL_SetWindowMaximumSize(win,w,h);}
			
			inline void SetMinWindowSize(int w,int h)
			{SDL_SetWindowMinimumSize(win,w,h);}
			
			inline void SetWindowsResizable(bool enable)
			{SDL_SetWindowResizable(win,enable?SDL_TRUE:SDL_FALSE);}
			
			inline void SetWindowIcon(SDL_Surface *icon)
			{SDL_SetWindowIcon(win,icon);}
			
			inline void MaximizeWindow()
			{SDL_MaximizeWindow(win);}
			
			inline void MinimizeWindow()
			{SDL_MinimizeWindow(win);}
			
			inline void RestoreWindow()
			{SDL_RestoreWindow(win);}
			
			inline void RaiseWindow()
			{SDL_RaiseWindow(win);}
			
			inline void HideWindow()
			{
				Hidden=1;
				SDL_HideWindow(win);
			}
			
			inline void ShowWindow()
			{
				Hidden=0;
				SDL_ShowWindow(win);
				SetNeedFreshScreen();
				SetNeedUpdatePosize();
				SetPresentArea(WinPS.ToOrigin());
			}
			
			inline void SetFullScreen()
			{SDL_SetWindowFullscreen(win,SDL_WINDOW_FULLSCREEN);}
			
			inline void SetWindowFullScreen()
			{SDL_SetWindowFullscreen(win,SDL_WINDOW_FULLSCREEN_DESKTOP);}
			
			inline void CancelFullScreen()
			{SDL_SetWindowFullscreen(win,0);}
				
			inline void SetWindowBordered(bool bordered)
			{SDL_SetWindowBordered(win,bordered?SDL_TRUE:SDL_FALSE);}
			
			inline void SetWindowOpacity(float opa)
			{SDL_SetWindowOpacity(win,opa);}
			
			inline float GetWindowOpacity()
			{
				float re=1;
				SDL_GetWindowOpacity(win,&re);
				return re;
			}
			
			inline void SetOccupyPosWg(Widgets *tar)
			{OccupyPosWg=tar;}
			
			inline Widgets* GetOccupyPosWg()
			{return OccupyPosWg;}
			
			inline void SetKeyboardInputWg(Widgets *tar)
			{KeyboardInputWg=tar;}
			
			inline Widgets* GetKeyboardInputWg()
			{return KeyboardInputWg;}
			
			inline SDL_Event* GetNowSolvingEvent()
			{return NowSolvingEvent;}
			
			inline void SetNowSolvingEvent(SDL_Event &tar)
			{NowSolvingEvent=&tar;}
			
			static void SetNeedFreshScreenAll()
			{NeedFreshScreenAll=1;}

			inline void SetNeedFreshScreen()
			{NeedFreshScreen=1;}
			
			inline bool IsNeedFreshScreen()
			{return NeedFreshScreen;}
			
			inline void SetNeedUpdatePosize()
			{NeedUpdatePosize=1;}
			
			inline bool IsNeedUpdatePosize()
			{return NeedUpdatePosize;}
			
			inline void StopSolveEvent()
			{NeedSolveEvent=0;}
			
			inline bool IsNeedSolveEvent()
			{return NeedSolveEvent;}
			
			inline void StopSolvePosEvent()
			{NeedSolvePosEvent=0;}
			
			inline bool IsNeedSolvePosEvent()
			{return NeedSolvePosEvent;}
			
			inline bool IsPosFocused()
			{return PosFocused;}
			
			inline bool IsKeyboardFocused()
			{return KeyboardFocused;}
			
			inline void SetMousePoint(const Point &pt)
			{SDL_WarpMouseInWindow(win,pt.x,pt.y);}
			
			inline void SetPresentArea(const Posize &ps)
			{
				if (ps.Size()==0)
					return;
				NeedFreshScreen=1;
				PresentLimit|=ps;
			}
			
			inline int GetNowSolvingPosEventMode()
			{return NowSolvingPosEventMode;}
			
			inline void SetNowSolvingPosEventMode(int mode)
			{NowSolvingPosEventMode=mode;}
			
			void SetBackgroundColor(const RGBA &co);
			
			void SetCloseFunc(BaseTypeFuncAndData *func)
			{
				if (CloseFunc!=NULL)
					delete CloseFunc;
				CloseFunc=func;
			}
			
			void SetRenderColor(const RGBA &co)
			{SDL_SetRenderDrawColor(ren,co.r,co.g,co.b,co.a);}
	
			void RenderDrawLine(const Point &pt1,const Point &pt2)
			{SDL_RenderDrawLine(ren,pt1.x,pt1.y,pt2.x,pt2.y);}
			
			void RenderFillRect(const Posize &ps,const RGBA &co)
			{
				if (!co.HaveColor()||ps.Size()==0) return;
				SDL_SetRenderDrawColor(ren,co.r,co.g,co.b,co.a);
				SDL_Rect rct=PosizeToSDLRect(ps);
				SDL_RenderFillRect(ren,&rct);
			}
			
			void RenderDrawRectWithLimit(const Posize &ps,const RGBA &co,Posize lmt)
			{
				SDL_SetRenderDrawColor(ren,co.r,co.g,co.b,co.a);
				lmt=lmt&ps;
				
				if (lmt.Size()==0)
					return;
				
				if (lmt.x==ps.x)
					RenderDrawLine(lmt.GetLU(),lmt.GetLD());
				if (lmt.x2()==ps.x2())
					RenderDrawLine(lmt.GetRU(),lmt.GetRD());
				if (lmt.y==ps.y)
					RenderDrawLine(lmt.GetLU(),lmt.GetRU());
				if (lmt.y2()==ps.y2())
					RenderDrawLine(lmt.GetLD(),lmt.GetRD());
			}
			
			void RenderCopy(SDL_Texture *tex,const Point &targetPt)
			{
				if (tex==NULL) return;
				Posize tex_PS=GetTexturePosize(tex);
				SDL_Rect srct=PosizeToSDLRect(tex_PS),
					 	 drct=PosizeToSDLRect(tex_PS+targetPt);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopy(SDL_Texture *tex,const Posize &targetPS)
			{
				if (tex==NULL||targetPS.Size()==0) return;
				Posize tex_PS=GetTexturePosize(tex);
				SDL_Rect srct=PosizeToSDLRect(tex_PS),
					 	 drct=PosizeToSDLRect(targetPS);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopy(SDL_Texture *tex,Posize tex_PS,const Posize &targetPS)
			{
				if (targetPS.Size()==0||tex_PS.Size()==0||tex==NULL) return;
				if (tex_PS==ZERO_POSIZE) tex_PS=GetTexturePosize(tex);
				SDL_Rect srct=PosizeToSDLRect(tex_PS),
					 	 drct=PosizeToSDLRect(targetPS);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopy(SDL_Texture *tex,Posize tex_PS,const Point &pt)
			{
				if (tex_PS.Size()==0||tex==NULL) return;
				if (tex_PS==ZERO_POSIZE) tex_PS=GetTexturePosize(tex);
				SDL_Rect srct=PosizeToSDLRect(tex_PS),
					 	 drct={pt.x,pt.y,tex_PS.w,tex_PS.h};
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopyWithLmt(SDL_Texture *tex,Posize tex_PS,const Posize &targetPS,const Posize &lmt)
			{
				if (lmt.Size()==0||targetPS.Size()==0||tex_PS.Size()==0||tex==NULL) return;
				if (tex_PS==ZERO_POSIZE) tex_PS=GetTexturePosize(tex);
				SDL_Rect srct=PosizeToSDLRect((((lmt&targetPS)-targetPS).Flexible(tex_PS.w*1.0/targetPS.w,tex_PS.h*1.0/targetPS.h)+tex_PS)&tex_PS),
						 drct=PosizeToSDLRect(lmt&targetPS);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopyWithLmt(SDL_Texture *tex,const Posize &targetPS,const Posize &lmt)
			{
				if (lmt.Size()==0||targetPS.Size()==0||tex==NULL) return;
				Posize texPs=GetTexturePosize(tex);
				SDL_Rect srct=PosizeToSDLRect(((lmt&targetPS)-targetPS).Flexible(texPs.w*1.0/targetPS.w,texPs.h*1.0/targetPS.h)&texPs),
						 drct=PosizeToSDLRect(lmt&targetPS);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopyWithLmt(SDL_Texture *tex,Posize tex_PS,const Point &pt,const Posize &lmt)
			{
				if (lmt.Size()==0||tex_PS.Size()==0||tex==NULL) return;
				Posize texPs=GetTexturePosize(tex);
				if (tex_PS==ZERO_POSIZE) tex_PS=texPs;
				Posize targetPS={pt.x,pt.y,tex_PS.w,tex_PS.h};
				SDL_Rect srct=PosizeToSDLRect(((lmt&targetPS)-targetPS)&tex_PS),
						 drct=PosizeToSDLRect(lmt&targetPS);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopyWithLmt(SDL_Texture *tex,const Point &pt,const Posize &lmt)
			{
				if (lmt.Size()==0||tex==NULL) return;
				Posize texPs=GetTexturePosize(tex);
				Posize targetPS={pt.x,pt.y,texPs.w,texPs.h};
				SDL_Rect srct=PosizeToSDLRect(((lmt&targetPS)-targetPS)&texPs),
						 drct=PosizeToSDLRect(lmt&targetPS);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopyWithLmt_Centre(SDL_Texture *tex,Posize targetPS,const Posize &lmt)
			{
				if (lmt.Size()==0||tex==NULL) return;
				Posize texPs=GetTexturePosize(tex);
				targetPS.x+=targetPS.w-texPs.w>>1;
				targetPS.y+=targetPS.h-texPs.h>>1;
				targetPS.w=texPs.w;
				targetPS.h=texPs.h;
				SDL_Rect srct=PosizeToSDLRect(((lmt&targetPS)-targetPS)&texPs),
						 drct=PosizeToSDLRect(lmt&targetPS);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			void RenderCopyWithLmt_Centre(SDL_Texture *tex,const Point &pt,const Posize &lmt)
			{
				if (lmt.Size()==0||tex==NULL) return;
				Posize texPs=GetTexturePosize(tex);
				Posize targetPS={pt.x-texPs.w/2,pt.y-texPs.h/2,texPs.w,texPs.h};
				SDL_Rect srct=PosizeToSDLRect(((lmt&targetPS)-targetPS)&texPs),
						 drct=PosizeToSDLRect(lmt&targetPS);
				SDL_RenderCopy(ren,tex,&srct,&drct);
			}
			
			SDL_Texture *CreateTextureFromSurface(SDL_Surface *sur)
			{return SDL_CreateTextureFromSurface(ren,sur);}
			
			SDL_Texture *CreateTextureFromSurfaceAndDelete(SDL_Surface *sur)
			{
				SDL_Texture *tex=SDL_CreateTextureFromSurface(ren,sur);
				SDL_FreeSurface(sur);
				return tex;
			}
			
			SDL_Texture *CreateRGBATextTexture(const char *text,const RGBA &co,const PUI_Font &font=PUI_Font())
			{return CreateTextureFromSurfaceAndDelete(CreateRGBATextSurface(text,co,font));}
			
			void RenderDrawText(const string &str,const Posize &tarPS,const Posize &lmt,const int mode=0,const RGBA &co=RGBA_NONE,const PUI_Font &font=PUI_Font())
			{
				if (lmt.Size()==0||tarPS.Size()==0) return;
				SDL_Texture *tex=CreateRGBATextTexture(str.c_str(),!co?ThemeColor.MainTextColor[0]:co,font);
				Posize texPs=GetTexturePosize(tex),
					   DBGborderPS;
				
				switch (mode)//0:mid -1:Left 1:Right
				{
					default:
						PUI_DD[1]<<"RenderDrawText wrong mode :"<<mode<<" and the default mode 0 will be used."<<endl;
					case 0:
						RenderCopy(tex,((lmt&tarPS)-tarPS-Point(tarPS.w-texPs.w>>1,tarPS.h-texPs.h>>1))&texPs,
								lmt&tarPS&(DBGborderPS=Posize((tarPS.w-texPs.w>>1)+tarPS.x,(tarPS.h-texPs.h>>1)+tarPS.y,texPs.w,texPs.h)));
						break;
					case 1:
						RenderCopy(tex,((lmt&tarPS)-tarPS-Point(tarPS.w-texPs.w,tarPS.h-texPs.h>>1))&texPs,
								lmt&tarPS&(DBGborderPS=Posize(tarPS.w-texPs.w+tarPS.x,(tarPS.h-texPs.h>>1)+tarPS.y,texPs.w,texPs.h)));
						break;
					case -1:
						RenderCopy(tex,((lmt&tarPS)-tarPS)&texPs,lmt&tarPS&(DBGborderPS=Posize(tarPS.x,(tarPS.h-texPs.h>>1)+tarPS.y,texPs.w,texPs.h)));
						break;
				}
				SDL_DestroyTexture(tex);
		
				if (DEBUG_DisplayBorderFlag)
					Debug_DisplayBorder(DBGborderPS,RGBA(255,178,178,255));
			}
			
			void Debug_DisplayBorder(const Posize &ps,const RGBA &co={255,0,0,200})
			{
				if (DEBUG_DisplayBorderFlag)
				{
					SDL_SetRenderDrawColor(ren,co.r,co.g,co.b,co.a);
					SDL_RenderDrawLine(ren,ps.x+5,ps.y,ps.x2()-5,ps.y);
					SDL_RenderDrawLine(ren,ps.x+5,ps.y2(),ps.x2()-5,ps.y2());
					SDL_RenderDrawLine(ren,ps.x,ps.y+5,ps.x,ps.y2()-5);
					SDL_RenderDrawLine(ren,ps.x2(),ps.y+5,ps.x2(),ps.y2()-5);
					SDL_RenderDrawLine(ren,ps.x+5,ps.y,ps.x,ps.y+5);
					SDL_RenderDrawLine(ren,ps.x2()-5,ps.y,ps.x2(),ps.y+5);
					SDL_RenderDrawLine(ren,ps.x+5,ps.y2(),ps.x,ps.y2()-5);
					SDL_RenderDrawLine(ren,ps.x2()-5,ps.y2(),ps.x2(),ps.y2()-5);
					
					SDL_SetRenderDrawColor(ren,0,0,255,200);
					SDL_RenderDrawLine(ren,ps.x,ps.y,ps.x+4,ps.y);
					SDL_RenderDrawLine(ren,ps.x,ps.y,ps.x,ps.y+4);
					SDL_RenderDrawLine(ren,ps.x2(),ps.y,ps.x2()-4,ps.y);
					SDL_RenderDrawLine(ren,ps.x2(),ps.y,ps.x2(),ps.y+4);
					SDL_RenderDrawLine(ren,ps.x,ps.y2(),ps.x+4,ps.y2());
					SDL_RenderDrawLine(ren,ps.x,ps.y2(),ps.x,ps.y2()-4);
					SDL_RenderDrawLine(ren,ps.x2(),ps.y2(),ps.x2()-4,ps.y2());
					SDL_RenderDrawLine(ren,ps.x2(),ps.y2(),ps.x2(),ps.y2()-4);
				}
			}
			
			void DEBUG_DisplayPresentLimit()
			{
				SetRenderColor({0,255,0,127});
				for (int i=0;i<=2;++i)
				{
					SDL_Rect rct=PosizeToSDLRect(PresentLimit.Shrink(i));
					SDL_RenderDrawRect(ren,&rct);
				}
			}
			
//			stack <SDL_Texture*> RenderTargetStack;
//			
//			inline void SetRenderTarget(SDL_Texture *tex)
//			{
//				RenderTargetStack.push(tex);
//				SDL_SetRenderTarget(ren,tex);
//			}
//			
//			inline void PopRenderTarget()
//			{
//				RenderTargetStack.pop();
//				SDL_SetRenderTarget(ren,RenderTargetStack.empty()?NULL:RenderTargetStack.top());
//			}
			
			PUI_Window(const Posize &winps,const string &title,unsigned int flagWin,unsigned int flagRen);
			
			~PUI_Window();
	};
	set <PUI_Window*> PUI_Window::AllWindow;
	map <unsigned int,PUI_Window*> PUI_Window::WinOfSDLWinID;
	int PUI_Window::WindowCnt=0;
	bool PUI_Window::NeedFreshScreenAll=1;
	const int PUI_Window::PUI_WINPS_CENTER=SDL_WINDOWPOS_CENTERED,
			  PUI_Window::PUI_WINPS_UNDEFINE=SDL_WINDOWPOS_UNDEFINED;
	
	PUI_Window *MainWindow=NULL,
			   *CurrentWindow=NULL;
	
	SDL_Texture *CreateTextureFromSurface(SDL_Surface *sur)
	{return CurrentWindow->CreateTextureFromSurface(sur);}
	
	SDL_Texture *CreateTextureFromSurfaceAndDelete(SDL_Surface *sur)
	{return CurrentWindow->CreateTextureFromSurfaceAndDelete(sur);}
	
	void SetVirturalPosEvent(const Point &pt,int type)//1:motion 2: down 3:up
	{
		
	}
	
	Point PUI_GetGlobalMousePoint()
	{
		Point re;
		SDL_GetGlobalMouseState(&re.x,&re.y);
		return re;
	}
	
	inline int PUI_SetGlobalMousePoint(const Point &pt)
	{return SDL_WarpMouseGlobal(pt.x,pt.y);}
	
	void PUI_SetShowCursor(bool enable)
	{SDL_ShowCursor(enable?SDL_ENABLE:SDL_DISABLE);}
	
	bool PUI_GetShowCursor()
	{return SDL_ShowCursor(SDL_QUERY)==SDL_ENABLE;}
	
	int PUI_SetRelativeMouseMode(bool enable)
	{return SDL_SetRelativeMouseMode(enable?SDL_TRUE:SDL_FALSE);}
	
	//Special functions:
	//...
	Uint32 PUI_EVENT_UpdateTimer=-1;
	struct PUI_UpdateTimerData
	{
		Widgets *tar=NULL;//the target to uptate
		int cnt=0;//0:Stop >0:use same interval cnt times(at this time stack is empty) -1:use same interval infinite times  -2:use intervals in stack;
		stack <Uint32> sta;
		atomic_char *enableFlag=NULL;//0:stop(set by timerFunc) 1:running 2:stop(set by outside func) 3:stop and delete atomic(set by ouside func)
									 //when set 2,timer will stop,and it will be set 0 when stoped; It won't be deleted after Data decontructed
		void *data=NULL;
	
		PUI_UpdateTimerData(Widgets *_tar,int _cnt=0,atomic_char *_enableflag=NULL,void *_data=NULL)
		:tar(_tar),cnt(_cnt),enableFlag(_enableflag),data(_data) {}
		
		PUI_UpdateTimerData() {}
	};
	
	Uint32 PUI_UpdateTimer(Uint32 interval,void *param)//mainly used to support animation effect
	{
		PUI_UpdateTimerData *p=(PUI_UpdateTimerData*)param;
//		PUI_DD[3]<<"PUI_UpdateTimer:  cnt "<<(p->cnt)<<endl;
	    
	    if (p->enableFlag!=NULL)
	    	if (*(p->enableFlag)!=1)
	    	{
	    		if (*(p->enableFlag)==3)
	    			delete p->enableFlag;
				else *(p->enableFlag)=0;
	    		delete p;
	    		return 0;
			}
			
	    SDL_Event event;
	    SDL_UserEvent userevent;
	    userevent.type=PUI_EVENT_UpdateTimer;
	    userevent.code=p->cnt==-2?p->sta.size():p->cnt;//how many times to run it needs
	    userevent.data1=p->tar;//target widgets
	    userevent.data2=p->data;
	    event.type=SDL_USEREVENT;
	    event.user=userevent;
	    SDL_PushEvent(&event);
	    
	    if (p->cnt==-2)//use interval in stack
		    if (p->sta.empty())
			{
		    	if (p->enableFlag!=NULL)
		    		if (*(p->enableFlag)==3)
	    				delete p->enableFlag;
					else *(p->enableFlag)=0;
				delete p;
		    	return 0;
			}
			else
			{
				Uint32 t=p->sta.top();
				p->sta.pop();
				if (t==0)
					t=1;
				return t;
			}
	    else if (p->cnt==-1)//infinite times
	    	return interval;
	    else 
			if (p->cnt==0)//run to end
		    {
		    	if (p->enableFlag!=NULL) 
		    		if (*(p->enableFlag)==3)
	    				delete p->enableFlag;
					else *(p->enableFlag)=0;
		    	delete p;
		    	return 0;
		    }
		    else//this time end
			{
				--p->cnt;
				return interval;	
			}
	}
	
	void SetCurrentIMEWindowPos(const Point &cursorPt,const Posize &excludePS,PUI_Window *Win)
	{
	#if defined(USE_WINDOWS_IME)
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if (SDL_GetWindowWMInfo(Win->GetSDLWindows(),&info))
			if (info.subsystem==SDL_SYSWM_WINDOWS)
			{
				HIMC imc=ImmGetContext(info.info.win.window);
				
				COMPOSITIONFORM cf;
				cf.dwStyle=CFS_FORCE_POSITION;
				cf.ptCurrentPos.x=cursorPt.x;
				cf.ptCurrentPos.y=cursorPt.y+16;
				ImmSetCompositionWindow(imc,&cf); 
				
				CANDIDATEFORM candf;
				candf.dwIndex=0;
				candf.dwStyle=CFS_CANDIDATEPOS;
				candf.ptCurrentPos.x=cursorPt.x;
				candf.ptCurrentPos.y=cursorPt.y+14;
				candf.rcArea.bottom=excludePS.y2();
				candf.rcArea.left=excludePS.x;
				candf.rcArea.right=excludePS.x2();
				candf.rcArea.top=excludePS.y;
				ImmSetCandidateWindow(imc,&candf);
				
				ImmReleaseContext(info.info.win.window,imc);
			}
	#endif
	}
	
	void TurnOnOffIMEWindow(bool on,PUI_Window *Win)//There still exsits bugs related to this functions
	{
		static bool lastState=1;
		if (on==lastState) return;
		lastState=on;
	#if defined(USE_WINDOWS_IME)
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if (SDL_GetWindowWMInfo(Win->GetSDLWindows(),&info))
			if (info.subsystem==SDL_SYSWM_WINDOWS)
			{
				HIMC imc=ImmGetContext(info.info.win.window);
				ImmSetOpenStatus(imc,on);
				ImmReleaseContext(info.info.win.window,imc);
			}
	#endif
	}
	
	//Widgets:
	//...
	
	class PosizeEX
	{
		friend class Widgets;
		protected: 
			Widgets *wg=NULL;
			PosizeEX *nxt=NULL;
//			int UsedCnt=0;
			
			PosizeEX()//Ban construct PosizeEX
			{
				
			}
			
		public:
			virtual void GetrPS(Posize &ps)=0;
			
			virtual ~PosizeEX()
			{
				if (nxt!=NULL)
					delete nxt;
			}
	};
	 
	class Widgets
	{
		friend bool PresentWidgets();
		friend void PresentWidgets(PUI_Window *win,Posize lmt);
		friend void PresentWidgets(Widgets *tar);
		friend void UpdateWidgetsPosize(Widgets *wg);
		friend void UpdateWidgetsPosize(PUI_Window *win);
		friend int SolveEvent(SDL_Event &event);
		friend class PUI_Window;
		
		public:
			enum WidgetType
			{
				WidgetType_Widgets=0,
				WidgetType_Layer,
				WidgetType_Button,
				WidgetType_LargeLayerWithScrollBar,
				WidgetType_TinyText,
				WidgetType_CheckBox,
				WidgetType_SingleChoiceButton,
				WidgetType_Slider,
				WidgetType_TwinLayerWithDivideLine,
				WidgetType_ShapedPictureButton,
				WidgetType_PictureBox,
				WidgetType_SimpleListView,
				WidgetType_SimpleListView_MultiColor,
				WidgetType_ProgressBar,
				WidgetType_Menu1,
				WidgetType_SwitchButton,
				WidgetType_TextEditLine,
				WidgetType_SimpleBlockView,
				WidgetType_TextEditBox,
				WidgetType_TabLayer,
				WidgetType_AddressSection,
				WidgetType_FileAddressSection,
				WidgetType_TinyText2,
				WidgetType_SimpleTreeView1,
				WidgetType_SimpleTreeView,
				WidgetType_DetailedListView,
				WidgetType_MessageBoxLayer,
				WidgetType_MessageBoxButton,
				WidgetType_ListViewTemplate,
				WidgetType_LayerForListViewTemplate,
				WidgetType_SingleChoiceList,
				WidgetType_DropDownButton,
				WidgetType_EventLayer,
				WidgetType_PhotoViewer,
				WidgetType_BlockViewTemplate,
				WidgetType_LayerForBlockViewTemplate,
				WidgetType_BorderRectLayer,
				WidgetType_FullFillSlider,
				WidgetType_DragableLayer,
				
				WidgetType_UserAssignStartID
			};
			
			static int GetNewWidgetTypeID()
			{
				static int re=WidgetType_UserAssignStartID;
				return ++re;
			}
			
		protected:
			static map <int,Widgets*> WidgetsID;
			static queue <Widgets*> WidgetsToDeleteAfterEvent;
			
			int ID=0;
			WidgetType Type=WidgetType_Widgets;
			int Depth=0;
			bool MultiWidgets=0;
			Posize rPS=ZERO_POSIZE,gPS=ZERO_POSIZE,//Relative Posize,Global Posize
				   CoverLmt=ZERO_POSIZE;//Last limit posize,used for partial update
			PUI_Window *Win=NULL;
			PosizeEX *PsEx=NULL;
			bool Enabled=1,
				 Selected=0;
			Widgets *fa=NULL,
					*preBrother=NULL,
					*nxtBrother=NULL,
					*childWg=NULL;
			bool NeedLoseFocus=0;
//				 PresentThisLater=0,
//				 SolvePosEventFirst=0, 
//				 LimitPosIngPS=1;
			
			void SetDelayDeleteThis()
			{WidgetsToDeleteAfterEvent.push(this);}
			
			void SetNeedLoseFocus()
			{
				if (NeedLoseFocus)
				{
					PUI_DD[1]<<"NeedLoseFocus "<<ID<<" have been set! Nothing would Happen."<<endl;
					return;
				}
				NeedLoseFocus=1;
				PUI_Window::LoseFocusLinkTable *p=new PUI_Window::LoseFocusLinkTable(this);
				p->nxt=Win->LoseFocusWgHead;
				Win->LoseFocusWgHead=p;
				++Win->LoseFocusState;
			}
			
			void RemoveNeedLoseFocus()
			{
				if (!NeedLoseFocus)
				{
					PUI_DD[1]<<"NeedLoseFocus of Widgets "<<ID<<" have been removed! Nothing would happen."<<endl;
					return;
				}
				NeedLoseFocus=0;
				PUI_Window::LoseFocusLinkTable *p=Win->LoseFocusWgHead;
				if (Win->LoseFocusWgHead->wg==this)
				{
					Win->LoseFocusWgHead=Win->LoseFocusWgHead->nxt;
					delete p;
					--Win->LoseFocusState;
					return;
				}
				else
				{
					for (PUI_Window::LoseFocusLinkTable *q=Win->LoseFocusWgHead->nxt;q;p=q,q=q->nxt)
						if (q->wg==this)
						{
							p->nxt=q->nxt;
							delete q;
							q=p;
							--Win->LoseFocusState;
							return;
						}
					PUI_DD[2]<<"Failed to find this LoseFocusWidget "<<ID<<endl;
				}
			}
			
			int AutoGetID()
			{
				static int re=1e4;
				++re;
				while (WidgetsID.find(re)!=WidgetsID.end())
					++re;
				return re;
			}

			void SplitFromFa()//Split this Node from linktable
			{
				if (fa==NULL) return;
				if (fa->childWg==this)
					fa->childWg=nxtBrother;
				else preBrother->nxtBrother=nxtBrother;
				if (nxtBrother!=NULL)
					nxtBrother->preBrother=preBrother;
				fa=preBrother=nxtBrother=NULL;
			}
			
			void UpdateFa()
			{
				Win=fa->Win;
				Depth=fa->Depth+1;
				if (nxtBrother!=NULL)
					nxtBrother->UpdateFa();
				if (childWg!=NULL)
					childWg->UpdateFa();
			}
			
			const Posize& GetFaCoverLmt()
			{
				if (fa==NULL)
					return gPS;
				else return fa->CoverLmt;
			}
			
			virtual void CalcPsEx()
			{
				if (PsEx!=NULL)
					PsEx->GetrPS(rPS);
				Posize lastPs=gPS;
//				Win->SetPresentArea(gPS);
				if (fa!=NULL)
					gPS=rPS+fa->gPS;
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
//				Win->SetPresentArea(gPS);
			}
			
			virtual void ReceiveKeyboardInput() {}
			
			virtual void CheckEvent() {}
			
			virtual void CheckPos() {}
			
			virtual void Show(Posize &lmt) {}
			
			virtual void _SolveEvent()
			{
				if (!Win->NeedSolveEvent) 
					return;
				if (nxtBrother!=NULL)
					nxtBrother->_SolveEvent();
				if (!Enabled) return;
				if (childWg!=NULL)
					childWg->_SolveEvent();
				if (Win->NeedSolveEvent)
					CheckEvent();
			}
			
			virtual void _SolvePosEvent()
			{
				if (!Win->NeedSolvePosEvent)
					return;
				if (Enabled)
					if (/*!LimitPosIngPS||*/gPS.In(Win->NowPos()))
//						if (SolvePosEventFirst)
//						{
//							CheckPos();
//							if (childWg!=NULL)
//								childWg->_SolvePosEvent();
//						}
//						else
						{
							if (childWg!=NULL)
								childWg->_SolvePosEvent();
							if (Win->NeedSolvePosEvent)
								CheckPos();
						}
				if (nxtBrother!=NULL)
					nxtBrother->_SolvePosEvent();
			}
			
			virtual void _PresentWidgets(Posize lmt)
			{
				if (nxtBrother!=NULL)
					nxtBrother->_PresentWidgets(lmt);
				if (Enabled)
				{
					lmt=lmt&gPS;
//					if (PresentThisLater)
//					{
//						if (childWg!=NULL)
//							childWg->_PresentWidgets(lmt);
////						if (lmt.Size()!=0)//??
//							Show(lmt);
//						if (DEBUG_EnableWidgetsShowInTurn)
//							SDL_Delay(DEBUG_WidgetsShowInTurnDelayTime),
//							SDL_RenderPresent(Win->ren);
//					}
//					else
					{
						Show(lmt);
						if (DEBUG_EnableWidgetsShowInTurn)
							SDL_Delay(DEBUG_WidgetsShowInTurnDelayTime),
							SDL_RenderPresent(Win->ren);
						if (childWg!=NULL)
							childWg->_PresentWidgets(lmt);
					}
				}
			}
			
			virtual void _UpdateWidgetsPosize()
			{
				if (nxtBrother!=NULL)
					nxtBrother->_UpdateWidgetsPosize();
				if (!Enabled) return;
				CalcPsEx();
				if (childWg!=NULL)
					childWg->_UpdateWidgetsPosize();
			}
			
			static inline void SolveEventOf(Widgets *tar)//To break the limit of class permission between base and derived class. 
			{tar->_SolveEvent();}
			
			static inline void SolvePosEventOf(Widgets *tar)//They are ugly,but useful...
			{tar->_SolvePosEvent();}
			
			static inline void PresentWidgetsOf(Widgets *tar,const Posize &ps)
			{tar->_PresentWidgets(ps);}
			
			static inline void UpdateWidgetsPosizeOf(Widgets *tar)
			{tar->_UpdateWidgetsPosize();}
			
			Widgets()//Ban construct widgets.
			{
				
			}
			
		public:
			static Widgets* GetWidgetsByID(int _ID)
			{
				auto mp=WidgetsID.find(_ID);
				if (mp==WidgetsID.end())
					return NULL;
				else return mp->second;
			}
			
			static const map <int,Widgets*>& GetWidgetsIDmap()
			{return WidgetsID;}
			
			static void SetDelayDeleteWidget(Widgets *tar)
			{
				if (tar!=NULL)
					WidgetsToDeleteAfterEvent.push(tar);
			}
			
			inline void ForceUpdateWidgetTreePosize()
			{_UpdateWidgetsPosize();}
			
			void DelayDelete()
			{WidgetsToDeleteAfterEvent.push(this);}
			
			void ClearAllChilds()//Be careful??
			{
				for (Widgets *p=childWg;p;p=p->nxtBrother)
					p->DelayDelete();
			}
			
			inline int GetID()
			{return ID;}
		
			void SetID(int _ID)//if _ID==0 auto set ID
			{
				if (_ID==0)
					_ID=AutoGetID();
				if (ID==_ID) return;
				if (ID!=0)
					WidgetsID.erase(ID);
				if (WidgetsID.find(_ID)==WidgetsID.end())
					WidgetsID[ID=_ID]=this;
				else PUI_DD[2]<<"ID is Occupied :"<<_ID<<endl;
			}
				
			inline WidgetType GetType()
			{return Type;}
			
//			Posize& GetrPS_Ref()
//			{return rPS;}
			
//			Posize& GetgPS_Ref()
//			{return gPS;}
			
			inline Posize GetrPS()
			{return rPS;}
			
			virtual void SetrPS(const Posize &ps)//virtual??
			{
				CoverLmt=rPS=ps;
				//...
				if (Win!=NULL)
				{
					Win->SetNeedUpdatePosize();
					Win->NeedFreshScreen=1;
				}
			}
			
			inline Posize GetgPS()
			{return gPS;}
			
			const inline Posize& GetCoverLmt()
			{return CoverLmt;}
			
			inline void SetEnabled(bool bo)
			{
				Enabled=bo;
				Win->NeedFreshScreen=1;
				Win->SetPresentArea(gPS);
				Win->SetNeedUpdatePosize();
			}
			
			inline bool GetEnabled()
			{return Enabled;}
			
			inline PUI_Window* GetWin()
			{return Win;}

			inline Widgets* GetFa()
			{return fa;}
			
			inline int GetDepth()
			{return Depth;}
			
			void SetFa(Widgets *_fa)//Add in the linktable head 
			{
				SplitFromFa();
				if (_fa==NULL) return;
				fa=_fa;
				Win=fa->Win;
				Depth=fa->Depth+1;
				nxtBrother=fa->childWg;
				if (nxtBrother!=NULL)
					nxtBrother->preBrother=this;
				fa->childWg=this;
				if (childWg!=NULL)
					childWg->UpdateFa();
			}
			
			void AddPsEx(PosizeEX *psex)
			{
				if (PsEx!=NULL)
					psex->nxt=PsEx;
				PsEx=psex;
				PsEx->wg=this;
				CalcPsEx();//??
				Win->SetNeedUpdatePosize();
			}
			
			void RemoveAllPsEx()
			{
				delete PsEx;
				PsEx=0; 
			}
			
			virtual ~Widgets()
			{
				PUI_DD[0]<<"Delete Widgets "<<ID<<endl;
				delete PsEx;
				while (childWg!=NULL)
					delete childWg;
				SplitFromFa();
				if (NeedLoseFocus)
					RemoveNeedLoseFocus();
				if (ID!=0)
					WidgetsID.erase(ID),ID=0;
				if (Enabled)
				{
					Win->NeedFreshScreen=1;
					Win->SetPresentArea(gPS);
				}
			}
	};
	
	map <int,Widgets*> Widgets::WidgetsID;
	queue <Widgets*> Widgets::WidgetsToDeleteAfterEvent;
	
	class PosizeEX_Fa6:public PosizeEX
	{
		protected:
			Uint8 xNotConsider=0,yNotConsider=0;//faDep=1;//faDep:relative to which father(Cannot use yet)
			double ra,rb,rc,rd;//Positive:pixels Negative:proportion
						
		public:
			virtual void GetrPS(Posize &ps)
			{
				if (nxt!=NULL) nxt->GetrPS(ps);//Linktable:Run GetrPS in the order it created 
				Posize psfa=wg->GetFa()->GetrPS();
					
				double a=ra<0?(-ra)*psfa.w:ra,
					   b=rb<0?(-rb)*psfa.w:rb,
					   c=rc<0?(-rc)*psfa.h:rc,
					   d=rd<0?(-rd)*psfa.h:rd;
				switch (xNotConsider)
				{
					case 1: ps.w=a;ps.SetX2_ChangeX(psfa.w-b-1);break;
					case 2: ps.x=a;ps.SetX2(psfa.w-b-1);break;
					case 3: ps.x=a;ps.w=b;break;
				}
				
				switch (yNotConsider)
				{
					case 1: ps.h=c;ps.SetY2_ChangeY(psfa.h-d-1);break;
					case 2: ps.y=c;ps.SetY2(psfa.h-d-1);break;
					case 3: ps.y=c;ps.h=d;break;
				}
			}
			
			PosizeEX_Fa6(Uint8 _xn,Uint8 _yn,double a,double b,double c,double d)
			:xNotConsider(_xn),yNotConsider(_yn),ra(a),rb(b),rc(c),rd(d)
			{
				if (!InRange(xNotConsider,0,3)) PUI_DD[2]<<"PosizeEX: xNotConsiderValue is set "<<xNotConsider<<" which cannot be resolved!"<<endl;
				if (!InRange(yNotConsider,0,3)) PUI_DD[2]<<"PosizeEX: yNotConsiderValue is set "<<yNotConsider<<" which cannot be resolved!"<<endl;
			}
	};
	
	class PosizeEX_MidFa:public PosizeEX
	{
		protected:
			Posize CenterShift;
		
		public:
			virtual void GetrPS(Posize &ps)
			{
				if (nxt!=NULL) nxt->GetrPS(ps);
				Posize psfa=wg->GetFa()->GetrPS();
				
				if (CenterShift.Size()!=0)
					ps.w=CenterShift.w,
					ps.h=CenterShift.h;
				ps.x=(psfa.w-ps.w>>1)+CenterShift.x;
				ps.y=(psfa.h-ps.h>>1)+CenterShift.y;
			}
			
			PosizeEX_MidFa(const Point &centreshift=ZERO_POINT)
			:CenterShift(centreshift.x,centreshift.y,0,0) {}
			
			PosizeEX_MidFa(const Posize &centreshift)
			:CenterShift(centreshift) {}
	};
	
	class PosizeEX_MidFa_Single:public PosizeEX
	{
		protected:
			bool IsY;
			int delta;
					
		public:
			virtual void GetrPS(Posize &ps)
			{
				if (nxt!=NULL) nxt->GetrPS(ps);
				Posize psfa=wg->GetFa()->GetrPS();
				
				if (IsY) ps.y=(psfa.h-ps.h>>1)+delta;
				else ps.x=(psfa.w-ps.w>>1)+delta;
			}
			
			PosizeEX_MidFa_Single(bool isY,int _delta=0):IsY(isY),delta(_delta) {}
	};
	
	class PosizeEX_Nearby:public PosizeEX
	{
		protected:
			Widgets *tar=NULL;
			
			
		public:
			virtual void GetrPS(Posize &ps)
			{
				
			}

	};
	
	class Layer:public Widgets
	{
		protected:
			RGBA LayerColor=RGBA_NONE;
		
			virtual void Show(Posize &lmt)
			{
				Win->RenderFillRect(lmt,LayerColor);
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			void SetLayerColor(const RGBA &co)
			{
				LayerColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}

			Layer(int _ID,Widgets *_fa,const Posize &_rPS)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create Layer "<<ID<<endl;
				Type=WidgetType_Layer;
				SetFa(_fa);
				SetrPS(_rPS);
			}
			
			Layer(int _ID,Widgets *_fa,PosizeEX *psex)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create Layer "<<ID<<endl;
				Type=WidgetType_Layer;
				SetFa(_fa);
				AddPsEx(psex);
			}
	};
	
	void PUI_Window::SetBackgroundColor(const RGBA &co=RGBA_WHITE)
	{
		if (_BackGroundLayer!=NULL)
			_BackGroundLayer->SetLayerColor(co);
		else PUI_DD[2]<<"BackGroundLayer of Window "<<WindowTitle<<" is not created!"<<endl;
	}
	
	PUI_Window::PUI_Window(const Posize &winps,const string &title="",unsigned int flagWin=SDL_WINDOW_RESIZABLE,unsigned int flagRen=SDL_RENDERER_ACCELERATED)
	:WinPS(winps),PresentLimit(winps.ToOrigin()),WindowTitle(title)
	{
		PUI_DD[0]<<"Create window "<<WindowTitle<<endl;
		++WindowCnt;
		AllWindow.insert(this);
		win=SDL_CreateWindow(WindowTitle.c_str(),WinPS.x,WinPS.y,WinPS.w,WinPS.h,flagWin);
		ren=SDL_CreateRenderer(win,-1,flagRen);
		SDL_SetRenderDrawBlendMode(ren,SDL_BLENDMODE_BLEND);
		WinOfSDLWinID[SDLWinID=SDL_GetWindowID(win)]=this;
		
//		CurrentWindow=this;

		_BackGroundLayer=new Layer(0,NULL,WinPS.ToOrigin());
		_MenuLayer=new Layer(0,NULL,WinPS.ToOrigin());
		_BackGroundLayer->Win=_MenuLayer->Win=this;
		_BackGroundLayer->SetLayerColor(RGBA_WHITE);
		PresentLimit|=WinPS.ToOrigin();
	}

	PUI_Window::~PUI_Window()
	{
		PUI_DD[0]<<"Delete window "<<WindowTitle<<endl;
		
		if (CloseFunc!=NULL)
			delete CloseFunc;
		
		delete _BackGroundLayer;
		delete _MenuLayer;
		
		WinOfSDLWinID.erase(SDLWinID);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		AllWindow.erase(this);
		--WindowCnt;
	}
	
	template <class T> class Button:public Widgets
	{
		protected:
//			bool EnableAnimation=1;
			int stat=0;//0:NoFocus 1:Focus 2:Down
			string Text;
			void (*func)(T&)=NULL;
			T funcData;
			RGBA ButtonColor[3],//stat0,1,2
				 TextColor=RGBA_NONE;
				//if RGBA_NONE (or !HaveColor) ,use default ThemeColor
			
			void InitDefualtColor()
			{
				for (int i=0;i<=2;++i)
					ButtonColor[i]=RGBA_NONE;
			}
			
			virtual void CheckEvent()
			{
				
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							RemoveNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							PUI_DD[0]<<"Button "<<ID<<" click"<<endl;
							stat=2;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							PUI_DD[0]<<"Button "<<ID<<" function"<<endl;
							if (func!=NULL)
								func(funcData);
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				Win->RenderFillRect(lmt,ButtonColor[stat]?ButtonColor[stat]:ThemeColor[stat?(stat==1?3:5):1]);
				
//				static SDL_Texture *tex=CreateTextureFromSurfaceAndDelete(CreateSpotLightSurface(80,{255,255,255,160}));
//				if (stat!=0)//Test of spotLight effect
//					RenderCopyWithLmt(tex,Win->NowPos()-Point(80,80),lmt&gPS);
				
				Win->RenderDrawText(Text,gPS,lmt,0,TextColor?TextColor:ThemeColor.MainTextColor[0]);

				Win->Debug_DisplayBorder(gPS);
			}
		
		public:
			void SetTextColor(const RGBA &co)
			{
				TextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetButtonColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					ButtonColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"SetButtonColor error: p "<<p<<" is not in range[0,2]"<<endl;
			}
			
			inline void SetButtonText(const string &_text)
			{
				Text=_text;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void TriggerFunc()
			{
				if (func!=NULL)
					func(funcData);
			}
			
			inline T& GetFuncData()
			{return funcData;}
			
			void SetFunc(void (*_func)(T&),const T &_funcdata)
			{
				func=_func;
				funcData=_funcdata;
			}
			
			Button(int _ID,Widgets *_fa,Posize _rPS,const string &_text,void (*_func)(T&),const T &_funcdata)
			:Text(_text),func(_func),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create Button "<<ID<<endl;
				Type=WidgetType_Button;
				SetFa(_fa);
				SetrPS(_rPS);
				InitDefualtColor();
			}
			
			Button(int _ID,Widgets *_fa,PosizeEX *psex,const string &_text,void (*_func)(T&),const T &_funcdata)
			:Text(_text),func(_func),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create Button "<<ID<<endl;
				Type=WidgetType_Button;
				SetFa(_fa);
				AddPsEx(psex);
				InitDefualtColor();
			}
	};
	
	class TinyText:public Widgets
	{
		protected:
			int Mode=0;//0:mid -1:Left 1:Right
			string text;
			PUI_Font font;
//			bool AutoW=0;
			RGBA TextColor=RGBA_NONE;
			
			virtual void Show(Posize &lmt)
			{
				Win->RenderDrawText(text,gPS,lmt,Mode,TextColor,font);
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			inline void SetTextColor(const RGBA &co)
			{
				TextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetText(const string &str)
			{
				text=str;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline string GetText()
			{return text;}
			
			inline void SetFontSize(int size)
			{
				font.Size=size;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetFont(const PUI_Font &tar)
			{
				font=tar;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			TinyText(int _ID,Widgets *_fa,const Posize &_rPS,const string &str,int _mode=0,const RGBA &textColor=RGBA_NONE)
			:text(str),Mode(_mode),TextColor(textColor)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TinyText "<<ID<<endl;
				Type=WidgetType_TinyText;
				SetFa(_fa);
				SetrPS(_rPS);
			}
			
			TinyText(int _ID,Widgets *_fa,PosizeEX *psex,const string &str,int _mode=0,const RGBA &textColor=RGBA_NONE)
			:text(str),Mode(_mode),TextColor(textColor)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TinyText "<<ID<<endl;
				Type=WidgetType_TinyText;
				SetFa(_fa);
				AddPsEx(psex);
			}
	};
	
	class TinyText2:public TinyText
	{
		protected:
			void (*func)(void*,string&)=NULL;
			void *funcData=NULL;
			
			virtual void Show(Posize &lmt)
			{
				if (func!=NULL)
					func(funcData,text);
				Win->RenderDrawText(text,gPS,lmt,Mode,TextColor,font);
				
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			inline void SetUpdateFunc(void (*_func)(void*,string&),void *_funcdata)
			{
				func=_func;
				funcData=_funcdata;
			}
			
			TinyText2(int _ID,Widgets *_fa,const Posize &_rps,const string &str,int _mode=0,void (*_func)(void*,string&)=NULL,void *_funcdata=NULL)
			:TinyText(_ID,_fa,_rps,str,_mode),func(_func)
			{
				PUI_DD[0]<<"Create TinyText2 "<<ID<<endl;
				Type=WidgetType_TinyText2;
				if (_funcdata==CONST_THIS)
					funcData=this;
				else funcData=_funcdata;
			}
		
			TinyText2(int _ID,Widgets *_fa,PosizeEX *psex,const string &str,int _mode=0,void (*_func)(void*,string&)=NULL,void *_funcdata=NULL)
			:TinyText(_ID,_fa,psex,str,_mode),func(_func)
			{
				PUI_DD[0]<<"Create TinyText2 "<<ID<<endl;
				Type=WidgetType_TinyText2;
				if (_funcdata==CONST_THIS)
					funcData=this;
				else funcData=_funcdata;
			}
	};

	template <class T> class CheckBox:public Widgets
	{
		protected:
			bool on=0;
			int stat=0;//0:Up_NoFocus 1:Up_Focus 2:Down (same with Button)
			void (*func)(T&,bool)=NULL;//bool: on
			T funcData;
			RGBA BorderColor[3],
				 ChooseColor=RGBA_NONE;
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							RemoveNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							PUI_DD[0]<<"CheckBox "<<ID<<" click"<<endl;
							stat=2;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							on=!on;
							PUI_DD[0]<<"CheckBox "<<ID<<" switch to "<<on<<endl;
							if (func!=NULL)
								func(funcData,on);
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						Win->StopSolvePosEvent();
						break;
				}
			}

			virtual void Show(Posize &lmt)
			{
				Win->RenderDrawRectWithLimit(gPS,BorderColor[stat]?BorderColor[stat]:ThemeColor[stat?(stat==1?3:5):1],lmt);
				if (on)
					Win->RenderFillRect(lmt&gPS.Shrink(3),ChooseColor?ChooseColor:ThemeColor[5]);

				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			inline void SetBorderColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					BorderColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"CheckBox: SetBorderColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
		 	inline void SetChooseColor(const RGBA &co)
			{
				ChooseColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline T& GetFuncData()
			{return funcData;}
			
			void SetFunc(void (*_func)(T&,bool),const T &_funcdata)
			{
				func=_func;
				funcData=_funcdata;
			}
			
			void SetOnOff(bool _on,bool TriggerFunc=1)
			{
				if (on==_on) return;
				on=_on;
				if (TriggerFunc)
					if (func!=NULL)
						func(funcData,on);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline bool GetOnOff()
			{return on;}
			
			CheckBox(int _ID,Widgets *_fa,Posize _rPS,bool defaultOnOff,void(*_func)(T&,bool),const T &_funcData)
			:on(defaultOnOff),func(_func),funcData(_funcData)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create CheckBox "<<ID<<endl;
				Type=WidgetType_CheckBox;
				SetFa(_fa);
				SetrPS(_rPS);
				for (int i=0;i<=2;++i)
					BorderColor[i]=RGBA_NONE;
			}
			
			CheckBox(int _ID,Widgets *_fa,PosizeEX* psex,bool defaultOnOff,void(*_func)(T&,bool),const T &_funcData)
			:on(defaultOnOff),func(_func),funcData(_funcData)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create CheckBox "<<ID<<endl;
				Type=WidgetType_CheckBox;
				SetFa(_fa);
				AddPsEx(psex);
				for (int i=0;i<=2;++i)
					BorderColor[i]=RGBA_NONE;
			}
	};
	
	template <class T> class SwitchButton:public CheckBox <T>//This is also a widget to test animation
	{
		protected:
			RGBA ChunkColor=RGBA_NONE;
			double chunkPercent=0;
			int ChunkWidth=12,
				UpdateInterval=10,
				UpdataCnt=10;
			SDL_TimerID IntervalTimerID=0;
			
			virtual void CheckEvent()
			{
				const SDL_Event &event=*this->Win->GetNowSolvingEvent();
				if (event.type==SDL_USEREVENT)
					if (event.user.type==PUI_EVENT_UpdateTimer)
						if (event.user.data1==this)
						{
							if (event.user.code==0)
								IntervalTimerID=0;
							chunkPercent=this->on?1-event.user.code*1.0/UpdataCnt:event.user.code*1.0/UpdataCnt;
							this->Win->StopSolveEvent();
							this->Win->SetNeedFreshScreen();
							this->Win->SetPresentArea(this->gPS);
						}
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*this->Win->GetNowSolvingEvent();
				if (this->Win->GetNowSolvingPosEventMode()==1)
				{
					if (this->stat!=0)
						if (!this->CoverLmt.In(this->Win->NowPos()))
						{
							this->stat=0;
							this->RemoveNeedLoseFocus();
							this->Win->SetNeedFreshScreen();
							this->Win->SetPresentArea(this->gPS);
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							PUI_DD[0]<<"SwitchButton: "<<this->ID<<" click"<<endl;
							this->stat=2;
							this->Win->StopSolvePosEvent();
							this->Win->SetNeedFreshScreen();
							this->Win->SetPresentArea(this->gPS);
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (this->stat==2)
						{
							this->on=!this->on;
							PUI_DD[0]<<"SwitchButton: "<<this->ID<<" switch to "<<this->on<<endl;
							if (this->func!=NULL)
								this->func(this->funcData,this->on);
							if (IntervalTimerID!=0)
								SDL_RemoveTimer(IntervalTimerID);
							IntervalTimerID=SDL_AddTimer(UpdateInterval,PUI_UpdateTimer,new PUI_UpdateTimerData(this,UpdataCnt));
							this->stat=1;
							this->Win->StopSolvePosEvent();
							this->Win->SetNeedFreshScreen();
							this->Win->SetPresentArea(this->gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (this->stat==0)
						{
							this->stat=1;
							this->SetNeedLoseFocus();
							this->Win->SetNeedFreshScreen();
							this->Win->SetPresentArea(this->gPS);
						}
						this->Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				this->Win->RenderDrawRectWithLimit(this->gPS,this->BorderColor[this->stat]?this->BorderColor[this->stat]:ThemeColor[this->stat?(this->stat==1?3:5):1],lmt);
				this->Win->RenderDrawRectWithLimit(this->gPS.Shrink(1),this->BorderColor[this->stat]?this->BorderColor[this->stat]:ThemeColor[this->stat?(this->stat==1?3:5):1],lmt);
				
				this->Win->RenderFillRect(this->gPS.Shrink(4)&lmt,this->BorderColor[this->stat]?this->BorderColor[this->stat]:ThemeColor.BackgroundColor[this->stat?(this->stat==1?3:5):1]); 
				this->Win->RenderFillRect(Posize(this->gPS.x+4,this->gPS.y+4,(this->gPS.w-ChunkWidth)*chunkPercent-4,this->gPS.h-8)&lmt,this->ChooseColor?this->ChooseColor:ThemeColor[1]);
				this->Win->RenderFillRect(Posize(this->gPS.x+(this->gPS.w-ChunkWidth)*chunkPercent,this->gPS.y,ChunkWidth,this->gPS.h)&lmt,ChunkColor?ChunkColor:ThemeColor.MainTextColor[0]);//ThisColor OK?
				
				this->Win->Debug_DisplayBorder(this->gPS);
			}
		
		public:
			inline void SetChunkColor(const RGBA &co)
			{
				ChunkColor=co;
				this->Win->SetNeedFreshScreen();
				this->Win->SetPresentArea(this->gPS);
			}
			
			inline void SetChunkWidth(int w)
			{
				ChunkWidth=w;
				this->Win->SetNeedFreshScreen();
				this->Win->SetPresentArea(this->gPS);
			}
			
			SwitchButton(int _ID,Widgets *_fa,Posize _rPS,bool defaultOnOff,void(*_func)(T&,bool),const T &_funcData)
			:CheckBox<T>(_ID,_fa,_rPS,defaultOnOff,_func,_funcData),chunkPercent(defaultOnOff)
			{
				PUI_DD[0]<<"Create SwitchButton "<<this->ID<<endl;
				this->Type=this->WidgetType_SwitchButton;
			}
			
			SwitchButton(int _ID,Widgets *_fa,PosizeEX* psex,bool defaultOnOff,void(*_func)(T&,bool),const T &_funcData)
			:CheckBox<T>(_ID,_fa,psex,defaultOnOff,_func,_funcData),chunkPercent(defaultOnOff)
			{
				PUI_DD[0]<<"Create SwitchButton "<<this->ID<<endl;
				this->Type=this->WidgetType_SwitchButton;
			}
	};
	
	template <class T> class SingleChoiceButton:public Widgets
	{
		protected :
			int ButtonCnt=0,
				CurrentChoose=-1,//count from 0;
				ChosenChoice=-1,
				eachChoiceHeight=20,
				ringD1=10,ringD2=12,
				stat=0;//0:Up_NoFocus 1:Up_Focus 2:Down
			SDL_Texture *ringTex=NULL,
						*roundTex=NULL;
			vector <string> Text;
			void (*func)(T&,const string&,int)=NULL;//int:choose
			T funcData=NULL;
			RGBA TextColor=RGBA_NONE,
				 ButtonColor[2],
				 RingColor[2];//ring,round
			
			int GetCurrentChoose(int y)
			{
				int re=(Win->NowPos().y-gPS.y)/eachChoiceHeight;
				if (InRange(re,0,ButtonCnt-1))
					return re;
				else return -1;
			}
			
			virtual void CheckEvent()
			{
				
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							Win->SetPresentArea(Posize(gPS.x,gPS.y+CurrentChoose*eachChoiceHeight,gPS.w,eachChoiceHeight));
							CurrentChoose=-1;
							RemoveNeedLoseFocus();
							Win->SetNeedFreshScreen();
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							if (InRange(CurrentChoose,0,ButtonCnt-1))
								Win->SetPresentArea(Posize(gPS.x,gPS.y+CurrentChoose*eachChoiceHeight,gPS.w,eachChoiceHeight));
							CurrentChoose=GetCurrentChoose(Win->NowPos().y);
							if (CurrentChoose!=-1)
							{
								PUI_DD[0]<<"SingleChoiceButton "<<ID<<" click "<<CurrentChoose<<endl;
								stat=2;
								Win->StopSolvePosEvent();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(Posize(gPS.x,gPS.y+CurrentChoose*eachChoiceHeight,gPS.w,eachChoiceHeight));
							}
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							if (InRange(CurrentChoose,0,ButtonCnt-1))
							Win->SetPresentArea(Posize(gPS.x,gPS.y+CurrentChoose*eachChoiceHeight,gPS.w,eachChoiceHeight));
							if (InRange(ChosenChoice,0,ButtonCnt-1))
								Win->SetPresentArea(Posize(gPS.x,gPS.y+ChosenChoice*eachChoiceHeight,gPS.w,eachChoiceHeight));
							CurrentChoose=GetCurrentChoose(Win->NowPos().y);
							if (CurrentChoose!=-1)
							{
								ChosenChoice=CurrentChoose;
								PUI_DD[0]<<"SingleChoiceButton "<<ID<<" choose "<<CurrentChoose<<endl;
								if (func!=NULL)
									func(funcData,Text[CurrentChoose],CurrentChoose);
								stat=1;
							}
							if (InRange(ChosenChoice,0,ButtonCnt-1))
								Win->SetPresentArea(Posize(gPS.x,gPS.y+ChosenChoice*eachChoiceHeight,gPS.w,eachChoiceHeight));
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							CurrentChoose=GetCurrentChoose(Win->NowPos().y);
							stat=1;
							SetNeedLoseFocus();
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							if (CurrentChoose!=-1)
								Win->SetPresentArea(Posize(gPS.x,gPS.y+CurrentChoose*eachChoiceHeight,gPS.w,eachChoiceHeight));
						}
						else
							if (CurrentChoose!=GetCurrentChoose(Win->NowPos().y))
							{
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(Posize(gPS.x,gPS.y+CurrentChoose*eachChoiceHeight,gPS.w,eachChoiceHeight));
								CurrentChoose=GetCurrentChoose(Win->NowPos().y);
								if (!InRange(CurrentChoose,0,ButtonCnt-1))
									CurrentChoose=-1;
								else Win->SetPresentArea(Posize(gPS.x,gPS.y+CurrentChoose*eachChoiceHeight,gPS.w,eachChoiceHeight));
							}
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			void Show(Posize &lmt)
			{
				for (int i=0;i<ButtonCnt;++i)
				{
					if (i==CurrentChoose)
						Win->RenderFillRect(lmt&Posize(gPS.x,gPS.y+i*eachChoiceHeight,gPS.w,eachChoiceHeight),ButtonColor[stat-1]?ButtonColor[stat-1]:ThemeColor[stat-1]);
					if (ringTex==NULL)
						ringTex=CreateTextureFromSurfaceAndDelete(CreateRingSurface(ringD1,ringD2,RingColor[0]?RingColor[0]:ThemeColor[5]));
					Win->RenderCopyWithLmt(ringTex,Posize(gPS.x,gPS.y+i*eachChoiceHeight,eachChoiceHeight,eachChoiceHeight).Shrink(eachChoiceHeight-ringD2>>1),lmt);
					if (i==ChosenChoice)
					{
						if (roundTex==NULL)
							roundTex=CreateTextureFromSurfaceAndDelete(CreateRingSurface(0,ringD1-4,RingColor[1]?RingColor[1]:ThemeColor[7]));
						Win->RenderCopyWithLmt(roundTex,Posize(gPS.x,gPS.y+i*eachChoiceHeight,eachChoiceHeight,eachChoiceHeight).Shrink(eachChoiceHeight-ringD1+4>>1),lmt);
					}
					Win->RenderDrawText(Text[i],{gPS.x+eachChoiceHeight,gPS.y+i*eachChoiceHeight,gPS.w-eachChoiceHeight,eachChoiceHeight},lmt,-1,TextColor);
					
					Win->Debug_DisplayBorder({gPS.x,gPS.y+i*eachChoiceHeight,gPS.w,eachChoiceHeight});
				}
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			inline void SetTextColor(const RGBA &co)
			{
				TextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetButtonColor(int p,const RGBA &co)
			{
				if (p==0||p==1)
				{
					ButtonColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"SingleChoiceButton: SetButtonColor: p "<<p<<" is not 0 or 1"<<endl;
			}
			
			inline void SetRingColor(int p,const RGBA &co)
			{
				if (p==0||p==1)
				{
					RingColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"SingleChoiceButton: SetRingColor: p "<<p<<" is not 0 or 1"<<endl;
			}
			
			virtual void SetrPS(const Posize &ps)//ignore h;
			{
				CoverLmt=gPS=rPS=ps;
				rPS.h=ButtonCnt*eachChoiceHeight;
				Win->SetNeedFreshScreen();
				Win->SetNeedUpdatePosize();
			}
			
			inline T& GetFuncData()
			{return funcData;}
			
			inline void SetFunc(void (*_func)(T&,const string&,int),const T &_funcdata)
			{func=_func;funcData=_funcdata;}
			
			void SetAccentData(int h,int d1,int d2)
			{
				eachChoiceHeight=h;
				rPS.h=ButtonCnt*eachChoiceHeight;
				ringD1=d1;ringD2=d2;
				if (ringTex!=NULL)
					SDL_DestroyTexture(ringTex),
					ringTex=NULL;
				if (roundTex!=NULL)
					SDL_DestroyTexture(roundTex),
					roundTex=NULL;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}
			
			SingleChoiceButton* AddChoice(const string &str)
			{
				Text.push_back(str);
				++ButtonCnt;
				rPS.h=ButtonCnt*eachChoiceHeight;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				return this;
			}
			
			void ClearAllChoice()
			{
				Text.clear();
				ButtonCnt=0;
				rPS.h=0;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}
			
//			void EraseChoice(int p)
			
			void SetChoice(int p)
			{
				if (InRange(p,0,ButtonCnt-1))
				{
					if (InRange(ChosenChoice,0,ButtonCnt-1))
						Win->SetPresentArea(Posize(gPS.x,gPS.y+ChosenChoice*eachChoiceHeight,gPS.w,eachChoiceHeight));
					ChosenChoice=p;
					if (InRange(ChosenChoice,0,ButtonCnt-1))
						Win->SetPresentArea(Posize(gPS.x,gPS.y+ChosenChoice*eachChoiceHeight,gPS.w,eachChoiceHeight));
					if (func!=NULL)
						func(funcData,Text[ChosenChoice],p);
					Win->SetNeedFreshScreen();
				}
				else PUI_DD[2]<<"SingleChoiceButton: SetChoice: p "<<p<<" is not in Range[0,ButtonCnt "<<ButtonCnt<<")"<<endl;
			}
			
			inline int GetChoice()
			{return ChosenChoice;}
			
			void SetChoiceText(int p,const string &str)
			{
				if (ButtonCnt==0) return;
				p=EnsureInRange(p,0,ButtonCnt-1);
				Text[p]=str;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(Posize(gPS.x,gPS.y+p*eachChoiceHeight,gPS.w,eachChoiceHeight));
			}
			
			inline string GetChoiceText(int p=-1)
			{
				if (ButtonCnt==0) return "";
				if (p==-1) return Text[ChosenChoice];
				else return Text[EnsureInRange(p,0,ButtonCnt-1)];
			}
			
			void TriggerFunc(int p)
			{
				if (func==NULL)
					return;
				ChosenChoice=EnsureInRange(p,0,ButtonCnt-1);
				func(funcData,Text[ChosenChoice],ChosenChoice);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			virtual ~SingleChoiceButton()
			{
				PUI_DD[0]<<"Delete SingleChoiceButton "<<ID<<endl;
				ClearAllChoice();
				if (ringTex!=NULL)
					SDL_DestroyTexture(ringTex);
				if (roundTex!=NULL)
					SDL_DestroyTexture(roundTex);
			}
			
			SingleChoiceButton(int _ID,Widgets *_fa,const Posize &_rps,void(*_func)(T&,const string&,int),const T &_funcData)
			:func(_func),funcData(_funcData)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SingleChoiceButton "<<ID<<endl;
				Type=WidgetType_SingleChoiceButton;
				SetFa(_fa);
				SetrPS(_rps);
				for (int i=0;i<=1;++i)
					ButtonColor[i]=RingColor[i]=RGBA_NONE;
			}
			
			SingleChoiceButton(int _ID,Widgets *_fa,PosizeEX* psex,void(*_func)(T&,const string&,int),const T &_funcData)
			:func(_func),funcData(_funcData)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SingleChoiceButton "<<ID<<endl;
				Type=WidgetType_SingleChoiceButton;
				SetFa(_fa);
				AddPsEx(psex);
				for (int i=0;i<=1;++i)
					ButtonColor[i]=RingColor[i]=RGBA_NONE;
			}
	};
	
	template <class T> class Button2:public Button<T>
	{
		
	};
	
	template <class T> class ShapedPictureButton:public Widgets
	{
		protected:
			string Text;
			SDL_Texture *pic[3];//this pic should have the same size with rPS
			SDL_Surface *sur0=NULL;//mask
			bool AutoDeletePic[3],
				 AutoDeleteSur0=0,
//				 OnlyOnePic=1,//only use sur0,pic0,pic1 and 2 auto create;(not usable yet)
				 ThroughBlankPixel=1;
			int stat=0,//0:Up_NoFocus 1:Up_Focus 2:Down
				ThroughLmtValue=0;//rgba.a not greater than this will through
			void (*func)(T&,ShapedPictureButton <T>*)=NULL;
			T funcData;
			RGBA TextColor=RGBA_NONE;
		
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos())||(ThroughBlankPixel&&GetSDLSurfacePixel(sur0,Win->NowPos()-gPS.GetLU()).a<=ThroughLmtValue))
						{
							stat=0;
							RemoveNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
							if (!ThroughBlankPixel||GetSDLSurfacePixel(sur0,Win->NowPos()-gPS.GetLU()).a>ThroughLmtValue)
							{
								PUI_DD[0]<<"ShapedPictureButton "<<ID<<" click"<<endl;
								stat=2;
								Win->StopSolvePosEvent();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							if (!ThroughBlankPixel||GetSDLSurfacePixel(sur0,Win->NowPos()-gPS.GetLU()).a>ThroughLmtValue)
							{
								PUI_DD[0]<<"ShapedPictureButton "<<ID<<" function"<<endl;
								if (func!=NULL)
									func(funcData,this);
								stat=1;
								Win->StopSolvePosEvent();
							}
							else
							{
								stat=0;
								RemoveNeedLoseFocus();
							}
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (!ThroughBlankPixel||GetSDLSurfacePixel(sur0,Win->NowPos()-gPS.GetLU()).a>ThroughLmtValue)
						{
							if (stat==0)
							{
								PUI_DD[0]<<"ShapedPictureButton "<<ID<<" focus"<<endl;
								stat=1;
								SetNeedLoseFocus();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
							Win->StopSolvePosEvent();
						}
						break;
				}
			}		
			
			virtual void Show(Posize &lmt)//So short ^v^
			{
				Win->RenderCopyWithLmt(pic[stat],gPS,lmt);
				Win->RenderDrawText(Text,gPS,lmt);

				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			inline void SetTextColor(const RGBA &co)
			{
				TextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetMaskPic(SDL_Surface *sur,bool AutoDelete,bool UseThisSurAsPic=0)
			{
				if (sur==NULL) return;
				if (AutoDeleteSur0)
					SDL_FreeSurface(sur0);
				sur0=sur;
				AutoDeleteSur0=AutoDelete;
				if (UseThisSurAsPic)
					PUI_DD[2]<<"ShapedPictureButton: UseThisSurAsPic is not usable yet"<<endl;
			}
			
			void SetButtonPic(int p,SDL_Texture *tex,bool AutoDelete)
			{
				if (InRange(p,0,2))
				{
					if (AutoDeletePic[p])
						SDL_DestroyTexture(pic[p]);
					pic[p]=tex;
					AutoDeletePic[p]=AutoDelete;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"ShapedPictureButton: SetButtonPic: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			inline T& GetFuncData()
			{return funcData;}
			
			void SetFunc(void (*_func)(T&,ShapedPictureButton<T>*),const T &_funcdata)
			{
				func=_func;
				funcData=_funcdata;
			}
			
			inline void SetText(const string &_text)
			{
				Text=_text;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
		 	inline void SetThroughBlankPixel(int x=-1)//x==-1 means it won't through any pixel
			{
				if (x==-1)
					ThroughBlankPixel=0;
				else ThroughBlankPixel=1,ThroughLmtValue=x;
			}
			
			virtual ~ShapedPictureButton()
			{
				PUI_DD[0]<<"Delete ShapedPictureButton "<<ID<<"\n";
				for (int i=0;i<=2;++i)
					if (AutoDeletePic[i])
						SDL_DestroyTexture(pic[i]);
				if (AutoDeleteSur0)
					SDL_FreeSurface(sur0);
			}
			
			ShapedPictureButton(int _ID,Widgets *_fa,const Posize &_rPS,void (*_func)(T&,ShapedPictureButton<T>*),const T &_funcdata)
			:func(_func),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create ShapedPictureButton "<<ID<<endl;
				Type=WidgetType_ShapedPictureButton;
				SetFa(_fa);
				SetrPS(_rPS);
				for (int i=0;i<=2;++i)
					pic[i]=NULL,
					AutoDeletePic[i]=0;
			}
			
			ShapedPictureButton(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(T&,ShapedPictureButton<T>*),const T &_funcdata)
			:func(_func),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create ShapedPictureButton "<<ID<<endl;
				Type=WidgetType_ShapedPictureButton;
				SetFa(_fa);
				AddPsEx(psex);
				for (int i=0;i<=2;++i)
					pic[i]=NULL,
					AutoDeletePic[i]=0;
			}
	};
	
	class DragableLayer:public Widgets
	{
		protected:
			int DragMode=1;//1:MouseLeftClick 2:MouseRightClick 3:MouseLeftDoubleClick
			bool Draging=0;
			RGBA LayerColor=RGBA_NONE;
			Uint32 TimerInterval=12;
			SDL_TimerID IntervalTimerID=0;
			Point PinPt;
			
			void SetTimerOnOff(bool on)
			{
				if (Draging==on) return;
				if (on) IntervalTimerID=SDL_AddTimer(TimerInterval,PUI_UpdateTimer,new PUI_UpdateTimerData(this,-1));
				else SDL_RemoveTimer(IntervalTimerID);
				Draging=on;
			}
			
			virtual void CheckEvent()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (event.type==SDL_USEREVENT)
					if (event.user.type==PUI_EVENT_UpdateTimer)
						if (event.user.data1==this)
						{
							if (!Win->IsKeyboardFocused())
								SetTimerOnOff(0);
							int m;
							Point pt;
							m=SDL_GetGlobalMouseState(&pt.x,&pt.y);
							if ((DragMode==1||DragMode==3)&&!(m&SDL_BUTTON_LMASK)||DragMode==2&&!(m&SDL_BUTTON_RMASK))
								SetTimerOnOff(0);
							else Win->SetWindowPos(pt-PinPt);
							Win->StopSolveEvent();
						}
			}
			
			virtual void CheckPos()
			{
				if (Draging||Win->GetNowSolvingPosEventMode()==1)
					return;
				
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT&&(DragMode==1||DragMode==3&&event.button.clicks==2)||DragMode==2&&event.button.button==SDL_BUTTON_RIGHT)
						{
							PinPt=Win->NowPos();
							SetTimerOnOff(1);
							Win->StopSolvePosEvent();
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						SetTimerOnOff(0);
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				Win->RenderFillRect(lmt,LayerColor);
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			void SetLayerColor(const RGBA &co)
			{
				LayerColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetDragMode(int mode)
			{DragMode=mode;}
			
			~DragableLayer()
			{
				SetTimerOnOff(0);
			}
			
			DragableLayer(int _ID,Widgets *_fa,const Posize &_rPS)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create DragableLayer "<<ID<<endl;
				Type=WidgetType_DragableLayer;
				SetFa(_fa);
				SetrPS(_rPS);
			}
			
			DragableLayer(int _ID,Widgets *_fa,PosizeEX *psex)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create DragableLayer "<<ID<<endl;
				Type=WidgetType_DragableLayer;
				SetFa(_fa);
				AddPsEx(psex);
			}
	};
	
	class LargeLayerWithScrollBar:public Widgets
	{
		protected:
			Posize ckPs_Ri,ckPs_Bu,ckBGps_Ri,ckBGps_Bu;//Right,Down
			bool EnableRightBar=0,EnableButtomBar=0;//Remember to adjust after changing the LargeAreaPosize!
			Layer* LargeAreaLayer=NULL;
			Posize largeAreaPS;
			Point ckCentreDelta={0,0};//Current click point to Chunk centre
			int ScrollBarWidth=16,
				MinBarLength=5,
				MinLargeAreaW=0,//when largearea is small than this, it is enabled
				MinLargeAreaH=0,//0 means no limit
				CustomWheelSensibility=0;//0 means use global
			bool AutoHideScrollBar=0,
				 EnableBarGap=1,
				 EnableBarSideButton=0,//I don't want to realize it immediatly...
				 EnableSmoothWheelScroll=0,
				 SmoothScrollData_Direction_y=1;
			int SmoothScrollData_Delta=0,
				SmoothScrollData_LeftCnt=0;
			SDL_TimerID SmoothScrollTimerID=0;
			int stat=0;//0:Up_NoFocus 1:Up_Focus_Bar_Right 2:Up_Focus_Bar_Buttom 3:Up_Focus_Chunk_Right 4:Up_Focus_Chunk_Buttom
					   //5:Down_Scroll_Chunk_Right 6:Down_Scroll_Chunk_Buttom 7:Up_Focus_LargeLayer__NotFocus_Bar
					   //9:Up_Focus_Button_Right_Minus 10:Down_Click_Button_Right_Minus 11:Up_Focus_Button_Right_Plus 12:Down_Click_Button_Right_Plus
					   //13:Up_Focus_Button_Buttom_Minus 14:Down_Click_Button_Buttom_Minus 15:Up_Focus_Button_Buttom_Plus 16:Down_Click_Button_Buttom_Plus
			RGBA BackgroundBarColor[4]={RGBA_NONE,RGBA_NONE,RGBA_NONE,RGBA_NONE},
				 BorderColor[4]={RGBA_NONE,RGBA_NONE,RGBA_NONE,RGBA_NONE},
				 ChunkColor[4]={RGBA_NONE,RGBA_NONE,RGBA_NONE,RGBA_NONE},
				 SideButtonColor[4]={RGBA_NONE,RGBA_NONE,RGBA_NONE,RGBA_NONE};
				 //Too many colors?
			
			class PosizeEX_LargeLayer:public PosizeEX
			{
				protected: 
					LargeLayerWithScrollBar *tar=NULL;
					
				public:
					virtual void GetrPS(Posize &ps)
					{
						if (nxt!=NULL) nxt->GetrPS(ps);
						ps=tar->largeAreaPS;
					}
					
					PosizeEX_LargeLayer(LargeLayerWithScrollBar *_tar)
					:tar(_tar) {}
			};
			
			void UpdateChunkPs()
			{
				if (EnableRightBar)
				{
					ckBGps_Ri={gPS.x2()-ScrollBarWidth+1,gPS.y,ScrollBarWidth,gPS.h};
					if (EnableBarGap)
						ckPs_Ri=ckBGps_Ri.Shrink(2);
					else ckPs_Ri=ckBGps_Ri;
					ckPs_Ri.h=EnsureInRange((ckBGps_Ri.h-EnableBarSideButton*2*ScrollBarWidth)*gPS.h*1.0/largeAreaPS.h,MinBarLength,ckBGps_Ri.h-EnableBarGap*4-EnableBarSideButton*2*ScrollBarWidth);
					ckPs_Ri.y=(long long)(ckBGps_Ri.h-EnableBarSideButton*2*ScrollBarWidth-EnableBarGap*4-ckPs_Ri.h)*(-largeAreaPS.y)*1.0/(largeAreaPS.h-gPS.h+EnableButtomBar*ScrollBarWidth)+ckBGps_Ri.y+EnableBarGap*2+EnableBarSideButton*ScrollBarWidth;
				}
				else ckPs_Ri=ckBGps_Ri=ZERO_POSIZE;

				if (EnableButtomBar)
				{
					ckBGps_Bu={gPS.x,gPS.y2()-ScrollBarWidth+1,EnableRightBar?gPS.w-ScrollBarWidth:gPS.w,ScrollBarWidth};
					if (EnableBarGap)
						ckPs_Bu=ckBGps_Bu.Shrink(2);
					else ckPs_Bu=ckBGps_Bu;
					ckPs_Bu.w=EnsureInRange((ckBGps_Bu.w-EnableBarSideButton*2*ScrollBarWidth)*gPS.w*1.0/largeAreaPS.w,MinBarLength,ckBGps_Bu.w-EnableBarGap*4-EnableBarSideButton*2*ScrollBarWidth);
					ckPs_Bu.x=(long long)(ckBGps_Bu.w-EnableBarSideButton*2*ScrollBarWidth-EnableBarGap*4-ckPs_Bu.w)*(-largeAreaPS.x)*1.0/(largeAreaPS.w-gPS.w+EnableRightBar*ScrollBarWidth)+ckBGps_Bu.x+EnableBarGap*2+EnableBarSideButton*ScrollBarWidth;			
				}
				else ckPs_Bu=ckBGps_Bu=ZERO_POSIZE;
			}
			
			virtual void CalcPsEx()
			{
				if (PsEx!=NULL)
					PsEx->GetrPS(rPS);
				Posize lastPs=gPS;
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
				CoverLmt=gPS&GetFaCoverLmt();
				ResizeLL(MinLargeAreaW==0?0:max(rPS.w,MinLargeAreaW),MinLargeAreaH==0?0:max(rPS.h,MinLargeAreaH));//LargeArea may not change,but rPS may change,this function will also update large state
			}
			
		public:
			void SetViewPort(int ope,double val)//ope:: 1:SetPosX 2:..Y 3:SetPosPercent 4:..Y 5:MoveX 6:..Y
			{
				switch (ope)
				{
					case 1: largeAreaPS.x=-val;	break;
					case 2: largeAreaPS.y=-val;	break;
					case 3:	largeAreaPS.x=(rPS.w-EnableRightBar*ScrollBarWidth-largeAreaPS.w)*val;	break;
					case 4: largeAreaPS.y=(rPS.h-EnableButtomBar*ScrollBarWidth-largeAreaPS.h)*val;	break;
					case 5:	largeAreaPS.x-=val;	break;
					case 6:	largeAreaPS.y-=val;	break;
					default:
						PUI_DD[2]<<"SetViewPort ope "<<ope<<" is not in Range[1,6]"<<endl;
						return;
				}
				if (rPS.w==largeAreaPS.w) largeAreaPS.x=0;
				else largeAreaPS.x=EnsureInRange(largeAreaPS.x,rPS.w-EnableRightBar*ScrollBarWidth-largeAreaPS.w,0);
				if (rPS.h==largeAreaPS.h) largeAreaPS.y=0;
				else largeAreaPS.y=EnsureInRange(largeAreaPS.y,rPS.h-EnableButtomBar*ScrollBarWidth-largeAreaPS.h,0);
				UpdateChunkPs();
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void ResizeLL(int _ww,int _hh)//_ww or _hh is 0 means not change this; 
			{
//				PUI_DD[0]<<"ResizeLL "<<_ww<<" "<<_hh<<endl;
				if (_ww!=0) largeAreaPS.w=EnsureInRange(_ww,rPS.w,1e9);//?? +1 -1??
				else largeAreaPS.w=EnsureInRange(largeAreaPS.w,rPS.w,1e9);
				if (_hh!=0) largeAreaPS.h=EnsureInRange(_hh,rPS.h,1e9);
				else largeAreaPS.h=EnsureInRange(largeAreaPS.h,rPS.h,1e9);
				EnableButtomBar=largeAreaPS.w>rPS.w;
				EnableRightBar=largeAreaPS.h>rPS.h;
				if (rPS.w==largeAreaPS.w) largeAreaPS.x=0;
				else largeAreaPS.x=EnsureInRange(largeAreaPS.x,rPS.w-EnableRightBar*ScrollBarWidth-largeAreaPS.w,0);
				if (rPS.h==largeAreaPS.h) largeAreaPS.y=0;
				else largeAreaPS.y=EnsureInRange(largeAreaPS.y,rPS.h-EnableButtomBar*ScrollBarWidth-largeAreaPS.h,0);
				UpdateChunkPs();
				Win->SetNeedUpdatePosize();
			}
		
		protected:
			void SetSmoothScrollBar(bool IsY,int delta)//On testing...
			{
				SmoothScrollData_Direction_y=IsY;
				SmoothScrollData_Delta+=delta;
				SmoothScrollData_LeftCnt=10;
				if (SmoothScrollTimerID==0)
			 		SmoothScrollTimerID=SDL_AddTimer(10,PUI_UpdateTimer,new PUI_UpdateTimerData(this,-1));//Careful! There exist a bug that the widget is deconstructed but the timer is still running(Use a class to manage the widgets' pointer may be a good way)
			}
			
			virtual void CheckEvent()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (event.type==SDL_MOUSEWHEEL)
					switch (stat)
					{
						case 0: break;
						case 1:	case 3:
//							PUI_DD[3]<<"Wheel H \n";
							if (EnableRightBar)
							{
								if (EnableSmoothWheelScroll) SetSmoothScrollBar(1,-event.wheel.y*(WheelSensibility==0?CustomWheelSensibility:WheelSensibility));
								else SetViewPort(6,-event.wheel.y*(WheelSensibility==0?CustomWheelSensibility:WheelSensibility));
								Win->StopSolveEvent();
							}
							break;
						case 2:	case 4:
//							PUI_DD[3]<<"Wheel W \n";
							if (EnableButtomBar)
							{
								if (EnableSmoothWheelScroll) SetSmoothScrollBar(0,-event.wheel.y*(WheelSensibility==0?CustomWheelSensibility:WheelSensibility));
								else SetViewPort(5,-event.wheel.y*(WheelSensibility==0?CustomWheelSensibility:WheelSensibility));
								Win->StopSolveEvent();
							}
							break;
						case 7:
							if (EnableRightBar)
							{
//								PUI_DD[3]<<"Wheel H \n";
								if (EnableSmoothWheelScroll) SetSmoothScrollBar(1,-event.wheel.y*(WheelSensibility==0?CustomWheelSensibility:WheelSensibility));
								else SetViewPort(6,-event.wheel.y*(WheelSensibility==0?CustomWheelSensibility:WheelSensibility));
								Win->StopSolveEvent();
							}
							else if (EnableButtomBar)
							{
//								PUI_DD[3]<<"Wheel W \n";
								if (EnableSmoothWheelScroll) SetSmoothScrollBar(0,-event.wheel.y*(WheelSensibility==0?CustomWheelSensibility:WheelSensibility));
								else SetViewPort(5,-event.wheel.y*(WheelSensibility==0?CustomWheelSensibility:WheelSensibility));
								Win->StopSolveEvent();
							}
							break;
						default :
							PUI_DD[2]<<"LargeLayerWithScrollBar "<<ID<<":Wheel Such state "<<stat<<" is not considered yet!"<<endl;
					}
				else if (event.type==SDL_USEREVENT)
					if (event.user.type==PUI_EVENT_UpdateTimer)
						if (event.user.data1==this)
							if (SmoothScrollData_LeftCnt>0)
							{
								int d=SmoothScrollData_Delta/SmoothScrollData_LeftCnt;
								if (d==0)
									d=SmoothScrollData_Delta;
								SetViewPort(SmoothScrollData_Direction_y?6:5,d);
								SmoothScrollData_Delta-=d;
								SmoothScrollData_LeftCnt--;
								if (SmoothScrollData_Delta==0||SmoothScrollData_LeftCnt<=0)
									if (SmoothScrollTimerID!=0)
									{
										SDL_RemoveTimer(SmoothScrollTimerID);
										SmoothScrollTimerID=0;
									}
								Win->StopSolveEvent();
							}
			}
			
			/*
			//0:Up_NoFocus 1:Up_Focus_Bar_Right 2:Up_Focus_Bar_Buttom 3:Up_Focus_Chunk_Right 4:Up_Focus_Chunk_Buttom
			//5:Down_Scroll_Chunk_Right 6:Down_Scroll_Chunk_Buttom 7:Up_Focus_LargeLayer__NotFocus_Bar
			//9:Up_Focus_Button_Right_Minus 10:Down_Click_Button_Right_Minus 11:Up_Focus_Button_Right_Plus 12:Down_Click_Button_Right_Plus
			//13:Up_Focus_Button_Buttom_Minus 14:Down_Click_Button_Buttom_Minus 15:Up_Focus_Button_Buttom_Plus 16:Down_Click_Button_Buttom_Plus
			*/
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							RemoveNeedLoseFocus();
							if (EnableRightBar)
								Win->SetNeedFreshScreen(),      
								Win->SetPresentArea(ckBGps_Ri);
							if (EnableButtomBar)             
								Win->SetNeedFreshScreen(),      
								Win->SetPresentArea(ckBGps_Bu);
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
							if (ckBGps_Ri.In(Win->NowPos()))
							{
								PUI_DD[0]<<"Sc.Down H"<<ID<<endl;
								stat=5;
								Win->SetOccupyPosWg(this);
								Win->StopSolvePosEvent();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(ckBGps_Ri);
								if (ckPs_Ri.In(Win->NowPos()))
									ckCentreDelta.y=Win->NowPos().y-ckPs_Ri.midY();
								else
								{
									ckCentreDelta.y=0;
									SetViewPort(4,(Win->NowPos().y-ckPs_Ri.h/2-ckBGps_Ri.y-EnableBarGap*2-EnableBarSideButton*ScrollBarWidth)*1.0/(ckBGps_Ri.h-EnableBarGap*4-EnableBarSideButton*2*ScrollBarWidth-ckPs_Ri.h));
								}
							}
							else if (ckBGps_Bu.In(Win->NowPos()))
							{
								PUI_DD[0]<<"Sc.Down W"<<ID<<endl;
								stat=6;
								Win->SetOccupyPosWg(this);
								Win->StopSolvePosEvent();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(ckBGps_Bu);
								if (ckPs_Bu.In(Win->NowPos()))
									ckCentreDelta.x=Win->NowPos().x-ckPs_Bu.midX();
								else
								{
									ckCentreDelta.x=0;
									SetViewPort(3,(Win->NowPos().x-ckPs_Bu.w/2-ckBGps_Bu.x-EnableBarGap*2-EnableBarSideButton*ScrollBarWidth)*1.0/(ckBGps_Bu.w-EnableBarGap*4-EnableBarSideButton*2*ScrollBarWidth-ckPs_Bu.w));
								}
							}
						break;
					case SDL_MOUSEBUTTONUP:
						if (stat==5||stat==6)
						{
							PUI_DD[0]<<"Sc.Up "<<ID<<"\n";
							Win->SetOccupyPosWg(NULL);
							if (ckBGps_Ri.In(Win->NowPos())) stat=3;
							else if (ckPs_Bu.In(Win->NowPos())) stat=4;
							else if (ckBGps_Ri.In(Win->NowPos())) stat=1;
							else if (ckBGps_Bu.In(Win->NowPos())) stat=2;
							else if (gPS.In(Win->NowPos())) stat=7;
							else stat=0;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
					
					case SDL_MOUSEMOTION://The code order here is very important
						if (stat==5)
							SetViewPort(4,(Win->NowPos().y-ckCentreDelta.y-ckPs_Ri.h/2-ckBGps_Ri.y-EnableBarGap*2-EnableBarSideButton*ScrollBarWidth)*1.0/(ckBGps_Ri.h-EnableBarGap*4-EnableBarSideButton*2*ScrollBarWidth-ckPs_Ri.h)),
							Win->StopSolvePosEvent();
						else if (stat==6)
							SetViewPort(3,(Win->NowPos().x-ckCentreDelta.x-ckPs_Bu.w/2-ckBGps_Bu.x-EnableBarGap*2-EnableBarSideButton*ScrollBarWidth)*1.0/(ckBGps_Bu.w-EnableBarGap*4-EnableBarSideButton*2*ScrollBarWidth-ckPs_Bu.w)), 
							Win->StopSolvePosEvent();
						else
						{
							if (stat==0)
								SetNeedLoseFocus();
							bool MouseEventFlagChangeFlag=1;
							int temp_stat=stat;
							if (ckPs_Ri.In(Win->NowPos())) stat=3;
							else if (ckPs_Bu.In(Win->NowPos())) stat=4;
							else if (ckBGps_Ri.In(Win->NowPos())) stat=1;
							else if (ckBGps_Bu.In(Win->NowPos())) stat=2;
							else stat=7,MouseEventFlagChangeFlag=0;
							
							if (MouseEventFlagChangeFlag)
								Win->StopSolvePosEvent();
							if (temp_stat!=stat)
							{
								if (EnableRightBar)
									Win->SetNeedFreshScreen(),
									Win->SetPresentArea(ckBGps_Ri);
								if (EnableButtomBar)
									Win->SetNeedFreshScreen(),
									Win->SetPresentArea(ckBGps_Bu);
							}
						}
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				if (EnableRightBar)
				{
					RGBA colBG,colBorder,colCk,colButton;
					switch (stat)
					{
						case 0:	case 2:	case 4:	case 6:
							colBG=BackgroundBarColor[0]?BackgroundBarColor[0]:RGBA(255,255,255,63);
							colBorder=BorderColor[0]?BorderColor[0]:ThemeColor[1];
							colCk=ChunkColor[0]?ChunkColor[0]:ThemeColor[1];
							colButton=SideButtonColor[0]?SideButtonColor[0]:ThemeColor[0];
							break;
						case 1:	case 7:	
							colBG=BackgroundBarColor[1]?BackgroundBarColor[1]:RGBA(255,255,255,127);
							colBorder=BorderColor[1]?BorderColor[1]:ThemeColor[3];
							colCk=ChunkColor[1]?ChunkColor[1]:ThemeColor[3];
							colButton=SideButtonColor[1]?SideButtonColor[1]:ThemeColor[2];
							break;
						case 3:	
							colBG=BackgroundBarColor[2]?BackgroundBarColor[2]:RGBA(255,255,255,191);
							colBorder=BorderColor[2]?BorderColor[2]:ThemeColor[5];
							colCk=ChunkColor[2]?ChunkColor[2]:ThemeColor[5];
							colButton=SideButtonColor[1]?SideButtonColor[1]:ThemeColor[2];
							break;
						case 5:	
							colBG=BackgroundBarColor[3]?BackgroundBarColor[2]:RGBA(255,255,255,255);
							colBorder=BorderColor[3]?BorderColor[3]:ThemeColor[7];
							colCk=ChunkColor[3]?ChunkColor[3]:ThemeColor[7];
							colButton=SideButtonColor[1]?SideButtonColor[1]:ThemeColor[2];
							break;
						case 9:	
						case 10:
						case 11:
						case 12:
						case 13:
						case 14:
						case 15:
						case 16:
						default:
							PUI_DD[2]<<"LargeLayerWithScrollBar "<<ID<<":Such state "<<stat<<" is not considered yet!"<<endl;
					}
					Win->RenderFillRect(lmt&ckBGps_Ri,colBG);
					Win->RenderDrawRectWithLimit(ckBGps_Ri,colBorder,lmt);
					Win->RenderFillRect(lmt&ckPs_Ri,colCk);
					if (EnableBarSideButton)
						;
					
					Win->Debug_DisplayBorder(ckBGps_Ri);
				}
				
				if (EnableButtomBar)
				{
					RGBA colBG,colBorder,colCk,colButton;
					switch (stat)
					{
						case 0:	case 1:	case 3:	case 5:
							LargeLayerWithScrollBar_Show_case0:
							colBG=BackgroundBarColor[0]?BackgroundBarColor[0]:RGBA(255,255,255,63);
							colBorder=BorderColor[0]?BorderColor[0]:ThemeColor[1];
							colCk=ChunkColor[0]?ChunkColor[0]:ThemeColor[1];
							colButton=SideButtonColor[0]?SideButtonColor[0]:ThemeColor[0];
							break;
						case 2:
							LargeLayerWithScrollBar_Show_case2:
							colBG=BackgroundBarColor[1]?BackgroundBarColor[1]:RGBA(255,255,255,127);
							colBorder=BorderColor[1]?BorderColor[1]:ThemeColor[3];
							colCk=ChunkColor[1]?ChunkColor[1]:ThemeColor[3];
							colButton=SideButtonColor[1]?SideButtonColor[1]:ThemeColor[2];
							break;
						case 4:	
							colBG=BackgroundBarColor[2]?BackgroundBarColor[2]:RGBA(255,255,255,191);
							colBorder=BorderColor[2]?BorderColor[2]:ThemeColor[5];
							colCk=ChunkColor[2]?ChunkColor[2]:ThemeColor[5];
							colButton=SideButtonColor[1]?SideButtonColor[1]:ThemeColor[2];
							break;
						case 6:	
							colBG=BackgroundBarColor[3]?BackgroundBarColor[2]:RGBA(255,255,255,255);
							colBorder=BorderColor[3]?BorderColor[3]:ThemeColor[7];
							colCk=ChunkColor[3]?ChunkColor[3]:ThemeColor[7];
							colButton=SideButtonColor[1]?SideButtonColor[1]:ThemeColor[2];
							break;
						case 7:
							if (EnableRightBar) goto LargeLayerWithScrollBar_Show_case0;
							else goto LargeLayerWithScrollBar_Show_case2;
						case 9:	
						case 10:
						case 11:
						case 12:
						case 13:
						case 14:
						case 15:
						case 16:
						default:
							PUI_DD[2]<<"LargeLayerWithScrollBar "<<ID<<":Such state "<<stat<<" is not considered yet!"<<endl;
					}
					Win->RenderFillRect(lmt&ckBGps_Bu,colBG);
					Win->RenderDrawRectWithLimit(ckBGps_Bu,colBorder,lmt);
					Win->RenderFillRect(lmt&ckPs_Bu,colCk);
					if (EnableBarSideButton)
						;
					
					Win->Debug_DisplayBorder(ckBGps_Bu);
				}
				
				Win->Debug_DisplayBorder(gPS);
				Win->Debug_DisplayBorder(largeAreaPS+gPS);
			}
			
			virtual void _PresentWidgets(Posize lmt)
			{
				if (nxtBrother!=NULL)
					PresentWidgetsOf(nxtBrother,lmt);
				if (Enabled)
				{
					lmt=lmt&gPS;
					if (childWg!=NULL)
						PresentWidgetsOf(childWg,lmt);
					Show(lmt);
					if (DEBUG_EnableWidgetsShowInTurn)
						SDL_Delay(DEBUG_WidgetsShowInTurnDelayTime),
						SDL_RenderPresent(Win->GetSDLRenderer());
				}
			}
			
			virtual void _SolvePosEvent()
			{
				if (!Win->IsNeedSolvePosEvent())
					return;
				if (Enabled)
					if (gPS.In(Win->NowPos()))
					{
						CheckPos();
						if (childWg!=NULL)
							SolvePosEventOf(childWg);
						if (Win->IsNeedSolvePosEvent())
							if (Win->GetNowSolvingEvent()->type==SDL_FINGERMOTION)
								if (stat==7)
								{
		//							SetViewPort(5,-event.tfinger.dx*Win->WinPS.w);
									SetViewPort(6,-Win->GetNowSolvingEvent()->tfinger.dy*Win->GetWinPS().h);
									Win->SetPresentArea(gPS);
									Win->StopSolvePosEvent();
								}
					}
				if (nxtBrother!=NULL)
					SolvePosEventOf(nxtBrother);
			}
			
		public:
			inline LargeLayerWithScrollBar* SetLargeAreaColor(const RGBA &co)
			{
				LargeAreaLayer->SetLayerColor(co);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
				return this;
			}
			
			inline LargeLayerWithScrollBar* SetBackgroundBarColor(int p,const RGBA &co)
			{
				if (InRange(p,0,3))
				{
					BackgroundBarColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(ckBGps_Bu|ckBGps_Ri);
				}
				else PUI_DD[1]<<"SetBackgroundBarColor: p "<<p<<" is not in Range[0,3]"<<endl;
				return this;
			}
			
			inline LargeLayerWithScrollBar* SetBorderColor(int p,const RGBA &co)
			{
				if (InRange(p,0,3))
				{
					BorderColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(ckBGps_Bu|ckBGps_Ri);
				}
				else PUI_DD[1]<<"SetBorderColor: p "<<p<<" is not in Range[0,3]"<<endl;
				return this;
			}
			
			inline LargeLayerWithScrollBar* SetChunkColor(int p,const RGBA &co)
			{
				if (InRange(p,0,3))
				{
					ChunkColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(ckBGps_Bu|ckBGps_Ri);
				}
				else PUI_DD[1]<<"ChunkColor: p "<<p<<" is not in Range[0,3]"<<endl;
				return this;
			}
			
			inline LargeLayerWithScrollBar* SetSideButtonColor(int p,const RGBA &co)
			{
				if (InRange(p,0,3))
				{
					SideButtonColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(ckBGps_Bu|ckBGps_Ri);
				}
				else PUI_DD[1]<<"SideButtonColor: p "<<p<<" is not in Range[0,3]"<<endl;
				return this;
			}
			
			inline void SetEnableBarGap(bool enable)
			{
				EnableBarGap=enable;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(ckBGps_Bu|ckBGps_Ri);
			}
			
			inline void SetEnableBarSideButton(bool enable)
			{
				EnableBarSideButton=enable;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(ckBGps_Bu|ckBGps_Ri);
			}
			
			inline void SetCustomWheelSensibility(int s)
			{CustomWheelSensibility=s;}
			
			inline bool GetButtomBarEnableState()
			{return EnableButtomBar;}
			
			inline bool GetRightBarEnableState()
			{return EnableRightBar;}
			
			inline int GetScrollBarWidth()
			{return ScrollBarWidth;}
			
			inline double GetScrollBarPercentX()
			{
				if (EnableButtomBar)
					return largeAreaPS.x*1.0/(rPS.w-EnableRightBar*ScrollBarWidth-largeAreaPS.w);
				else return 0;
				
			}
			
			inline double GetScrollBarPercentY()
			{
				if (EnableRightBar)
					return largeAreaPS.y*1.0/(rPS.h-EnableButtomBar*ScrollBarWidth-largeAreaPS.h);
				else return 0;
			}
			
			inline void SetScrollBarWidth(int w)
			{
				ScrollBarWidth=w;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
				Win->SetNeedUpdatePosize();
			}
			
			inline void SetMinLargeAreaSize(int _w,int _h)
			{
				MinLargeAreaW=_w;
				MinLargeAreaH=_h;
				Win->SetNeedUpdatePosize();
			}
			
//			inline void SetEnableSmoothWheelScroll(bool enable)
//			{EnableSmoothWheelScroll=enable;}
			
			inline Layer* LargeArea()
			{return LargeAreaLayer;}
			
			LargeLayerWithScrollBar(int _ID,Widgets *_fa,const Posize &_rps,const Posize &_largeareaPS)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create LargeLayerWithScrollBar "<<ID<<endl;
				Type=WidgetType_LargeLayerWithScrollBar;
				SetFa(_fa);
				SetrPS(_rps);
				largeAreaPS=_largeareaPS;
				ResizeLL(0,0);
				LargeAreaLayer=new Layer(0,this,new PosizeEX_LargeLayer(this));
				MultiWidgets=1;
			}
			
			LargeLayerWithScrollBar(int _ID,Widgets *_fa,PosizeEX *psex,const Posize &_largeareaPS)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create LargeLayerWithScrollBar "<<ID<<endl;
				Type=WidgetType_LargeLayerWithScrollBar;
				SetFa(_fa);
				AddPsEx(psex);
				largeAreaPS=_largeareaPS;
				ResizeLL(0,0);
				LargeAreaLayer=new Layer(0,this,new PosizeEX_LargeLayer(this));
				MultiWidgets=1;
			}
	};
	
	class StackLargeLayer:public Widgets
	{
		
	};
	
	class TwinLayerWithDivideLine:public Widgets
	{
		protected:
			bool VerticalDivide=1;
			Layer *LayerAreaA=NULL,//if Vertical ,AreaA is up ,else AreaA is Left
				  *LayerAreaB=NULL;
			int DivideLineShowWidth=2,
				DivideLineEventWidth=6,
				ResizeMode=0;//0:keep percent not changing 1:keep A not changing 2:keep B (It will satisfy LimitA and LimitB first) 
			double DivideLinePos=0.5,//Similar with Slider
				   LimitA=-0.1,LimitB=-0.1;//positive or 0:min pixels in self side; negative [-1,0):percent in self side; negative(other) other side max pixels
			Posize DLEventPs,DLShowPs;
			int stat=0;//same with Slider
			RGBA DivideLineColor[3]={RGBA_NONE,RGBA_NONE,RGBA_NONE};
			
			void GetAreaPosize(Posize &ps,bool IsB)
			{
				if (IsB)
					if (VerticalDivide)	ps={rPS.w*DivideLinePos+DivideLineShowWidth/2,0,ceil(rPS.w*(1-DivideLinePos)-DivideLineShowWidth/2),rPS.h};
					else ps={0,rPS.h*DivideLinePos+DivideLineShowWidth/2,rPS.w,ceil(rPS.h*(1-DivideLinePos)-DivideLineShowWidth/2)};
				else
					if (VerticalDivide) ps={0,0,rPS.w*DivideLinePos-DivideLineShowWidth/2,rPS.h};
					else ps={0,0,rPS.w,rPS.h*DivideLinePos-DivideLineShowWidth/2};
			}
			
			class PosizeEX_TwinLayer:public PosizeEX
			{
				protected: 
					TwinLayerWithDivideLine *tar;
					bool IsB;
					
				public:
					virtual void GetrPS(Posize &ps)
					{
						if (nxt!=NULL) nxt->GetrPS(ps);
						tar->GetAreaPosize(ps,IsB);
					}
					
					PosizeEX_TwinLayer(TwinLayerWithDivideLine *_tar,bool isB)
					:tar(_tar),IsB(isB) {}
			};
			
			void SetDivideLinePos(double per)
			{
				Win->SetPresentArea(DLShowPs);
				DivideLinePos=EnsureInRange(per,LimitA>=0?LimitA/(VerticalDivide?rPS.w:rPS.h):(LimitA<-1?1+LimitA/(VerticalDivide?rPS.w:rPS.h):-LimitA),LimitB>=0?1-LimitB/(VerticalDivide?rPS.w:rPS.h):(LimitB<-1?-LimitB/(VerticalDivide?rPS.w:rPS.h):1+LimitB));
				if (VerticalDivide)
					DLEventPs={gPS.x+gPS.w*DivideLinePos-DivideLineEventWidth/2,gPS.y,DivideLineEventWidth,gPS.h},
					DLShowPs={gPS.x+gPS.w*DivideLinePos-DivideLineShowWidth/2,gPS.y,DivideLineShowWidth,gPS.h};
				else
					DLEventPs={gPS.x,gPS.y+gPS.h*DivideLinePos-DivideLineEventWidth/2,gPS.w,DivideLineEventWidth},
					DLShowPs={gPS.x,gPS.y+gPS.h*DivideLinePos-DivideLineShowWidth/2,gPS.w,DivideLineShowWidth};
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(DLShowPs);
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!(DLEventPs&CoverLmt).In(Win->NowPos()))
							{
								PUI_DD[0]<<"TwinLayerWithDivideLine "<<ID<<" losefocus"<<endl;
								stat=0;
								RemoveNeedLoseFocus();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(DLShowPs);
							}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
							if (DLEventPs.In(Win->NowPos()))
							{
								PUI_DD[0]<<"TwinLayerWithDivideLine "<<ID<<" click"<<endl;
								stat=2;
								Win->SetOccupyPosWg(this); 
								Win->StopSolvePosEvent();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(DLShowPs);
							}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							PUI_DD[0]<<"TwinLayerWithDivideLine "<<ID<<" loose"<<endl;
							stat=1;
							Win->SetOccupyPosWg(NULL);
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(DLShowPs);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat==2)
						{
//							PUI_DD[0]<<"TwinLayerWithDivideLine "<<ID<<" shift"<<endl;
							SetDivideLinePos(VerticalDivide?(Win->NowPos().x-gPS.x)*1.0/gPS.w:(Win->NowPos().y-gPS.y)*1.0/gPS.h);
							Win->StopSolvePosEvent(); 
						}
						else if (stat==0)
						{
							if ((DLEventPs&CoverLmt).In(Win->NowPos())) 
							{
								PUI_DD[0]<<"TwinLayerWithDivideLine "<<ID<<" focus"<<endl;
								stat=1;
								SetNeedLoseFocus();
								Win->StopSolvePosEvent();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(DLShowPs);
							}
						}
						else
							if ((DLEventPs&CoverLmt).In(Win->NowPos()))
								Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				Win->RenderFillRect(DLShowPs&lmt,DivideLineColor[stat]?DivideLineColor[stat]:ThemeColor[stat*2+1]);
				
				Win->Debug_DisplayBorder(gPS); 
				Win->Debug_DisplayBorder(DLEventPs);
				Win->Debug_DisplayBorder(DLShowPs);
			}
			
			virtual void CalcPsEx()
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)
					PsEx->GetrPS(rPS);
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
				
				if (!(lastPs==gPS))
					switch (ResizeMode)
					{
						case 0:
							SetDivideLinePos(DivideLinePos);
							break;
						case 1:
							if (VerticalDivide) SetDivideLinePos((LayerAreaA->GetrPS().w+DivideLineShowWidth/2.0)/rPS.w);
							else SetDivideLinePos((LayerAreaA->GetrPS().h+DivideLineShowWidth/2.0)/rPS.h);
							break;
						case 2:
							if (VerticalDivide) SetDivideLinePos(1-(LayerAreaB->GetrPS().w+DivideLineShowWidth/2.0)/rPS.w);
							else SetDivideLinePos(1-(LayerAreaB->GetrPS().h+DivideLineShowWidth/2.0)/rPS.h);
							break;
						default:
							PUI_DD[2]<<"TwinLayerWithDivideLin: ResizeMode "<<ResizeMode<<" is not in Range[0,2]"<<endl;
					}
			}
			
			virtual void _PresentWidgets(Posize lmt)
			{
				if (nxtBrother!=NULL)
					PresentWidgetsOf(nxtBrother,lmt);
				if (Enabled)
				{
					lmt=lmt&gPS;
					if (childWg!=NULL)
						PresentWidgetsOf(childWg,lmt);
					Show(lmt);
					if (DEBUG_EnableWidgetsShowInTurn)
						SDL_Delay(DEBUG_WidgetsShowInTurnDelayTime),
						SDL_RenderPresent(Win->GetSDLRenderer());
				}
			}
			
			virtual void _SolvePosEvent()
			{
				if (!Win->IsNeedSolvePosEvent())
					return;
				if (Enabled)
					if (gPS.In(Win->NowPos()))
					{
						CheckPos();
						if (childWg!=NULL)
							SolvePosEventOf(childWg);
					}
				if (nxtBrother!=NULL)
					SolvePosEventOf(nxtBrother);
			}
			
		public:
			inline void SetDivideLineColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					DivideLineColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(DLShowPs);
				}
				else PUI_DD[1]<<"TwinLayerWithDivideLine: SetDivideLineColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			inline void SetAreaAColor(const RGBA &co)
			{LayerAreaA->SetLayerColor(co);}
			
			inline void SetAreaBColor(const RGBA &co)
			{LayerAreaB->SetLayerColor(co);}
			
			inline void SetDivideLineWidth(int showWidth,int eventWidth)
			{
				DivideLineShowWidth=showWidth;
				DivideLineEventWidth=eventWidth;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}
			
			inline void SetDivideLineMode(int mode,double _limitA,double _limitB)
			{
				ResizeMode=mode;
				LimitA=_limitA;
				LimitB=_limitB;
				SetDivideLinePos(DivideLinePos);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}
			
			void SetDivideLinePosition(double per)
			{SetDivideLinePos(per);}
			
			inline Layer* AreaA()
			{return LayerAreaA;}
			
			inline Layer* AreaB()
			{return LayerAreaB;}
			
			TwinLayerWithDivideLine(int _ID,Widgets *_fa,const Posize &_rps,bool _verticalDivide,double initPercent=0.5)
			:VerticalDivide(_verticalDivide)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TwinLayerWithDivideLine "<<ID<<endl;
				Type=WidgetType_TwinLayerWithDivideLine;
				SetFa(_fa);
				SetrPS(_rps);
				LayerAreaA=new Layer(0,this,new PosizeEX_TwinLayer(this,0));
				LayerAreaB=new Layer(0,this,new PosizeEX_TwinLayer(this,1));
				SetDivideLinePos(initPercent);
				MultiWidgets=1;
			}
			
			TwinLayerWithDivideLine(int _ID,Widgets *_fa,PosizeEX *psex,bool _verticalDivide,double initPercent=0.5)
			:VerticalDivide(_verticalDivide)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TwinLayerWithDivideLine "<<ID<<endl;
				Type=WidgetType_TwinLayerWithDivideLine;
				SetFa(_fa);
				AddPsEx(psex);
				LayerAreaA=new Layer(0,this,new PosizeEX_TwinLayer(this,0));
				LayerAreaB=new Layer(0,this,new PosizeEX_TwinLayer(this,1));
				SetDivideLinePos(initPercent);
				MultiWidgets=1;
			}
	};
	
	template <class T> class EventLayer:public Widgets
	{
		protected:
			int (*func)(T&,const SDL_Event&)=NULL;//return 1 to solve it,0 means not solve
			T funcdata;
			
			virtual void CheckEvent()
			{
				if (func!=NULL)
					if (func(funcdata,*Win->GetNowSolvingEvent())!=0)
						Win->StopSolveEvent();
			}
			
		public:
			
			inline void SetFuncData(const T &_funcdata)
			{funcdata=_funcdata;}
			
			inline void SetFunc(int (*_func)(T&,const SDL_Event&),const T &_funcdata)
			{
				func=_func;
				funcdata=_funcdata;
			}
			
			EventLayer(int _ID,Widgets *_fa,int (*_func)(T&,const SDL_Event&),T _funcdata)
			:func(_func),funcdata(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create EventLayer "<<ID<<endl;
				Type=WidgetType_EventLayer;
				SetFa(_fa);
			}
	};
	
	template <class T> class TabLayer:public Widgets
	{
		protected:
			struct TabLayerData
			{
				Layer *lay=NULL;
				string title;
				RGBA TabColor[3],//none,focus/current,click
					 TextColor=RGBA_NONE;
				int TabWidth=50;
				SharedTexturePtr pic;
				T funcData;
				
				TabLayerData()
				{
					for (int i=0;i<=2;++i)
						TabColor[i]=RGBA_NONE;
				}
				
				TabLayerData(Layer *_lay,const string &_title,const SharedTexturePtr &_pic,const T &_funcdata)
				:lay(_lay),title(_title),pic(_pic),funcData(_funcdata) 
				{
					for (int i=0;i<=2;++i)
						TabColor[i]=RGBA_NONE;
				}
			};
			
			vector <TabLayerData> EachTabLayer;
			int TabCnt=0;
			int stat=0,//0:none 1:focus 2:leftclick 3:rightclick
				TabHeight=24,
				LeftMostShowLayer=0,
				FocusingPos=-1,
				CurrentShowLayerPos=-1;//-1:means no layer
			Range TabLayerWidthRange={100,400};
			bool ShowCloseX=0,
				 EnableDrag=0,
				 EnableTabScrollAnimation=0,
				 EnableSwitchGradientAnimation=0;
			RGBA TabBarBackgroundColor=RGBA_NONE;
			void (*func)(T&,int,int)=NULL;//int1:which int2:stat (1:switch to 2:rightClick)
			
			class PosizeEX_TabLayer:public PosizeEX
			{
				protected: 
					TabLayer *tar=NULL;
					
				public:
					virtual void GetrPS(Posize &ps)
					{
						if (nxt!=NULL) nxt->GetrPS(ps);
						ps=Posize(0,tar->TabHeight,tar->rPS.w,tar->rPS.h-tar->TabHeight);
					}
					
					PosizeEX_TabLayer(TabLayer *_tar)
					:tar(_tar) {}
			};
			
			int GetTabFromPos(const Point &pt) 
			{
				if (TabCnt==0) return -1;
				if ((CoverLmt&Posize(gPS.x,gPS.y,gPS.w,TabHeight)).In(pt))
				{
					for (int i=LeftMostShowLayer,w=0;i<TabCnt;w+=EachTabLayer[i++].TabWidth)
						if (pt.x-gPS.x<w+EachTabLayer[i].TabWidth)
							return i;
					return -1;
				}
				else return -1;
			}

			virtual void CheckEvent()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_MOUSEWHEEL:
						if (Posize(gPS.x,gPS.y,gPS.w,TabHeight).In(Win->NowPos()))
						{
							if (event.wheel.y>0)
							{
								if (LeftMostShowLayer>0)
								{
									LeftMostShowLayer=max(0,LeftMostShowLayer-event.wheel.y);
									Win->SetNeedFreshScreen();
									Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TabHeight));
								}
							}
							else
							{
								int w=0;
								for (int i=LeftMostShowLayer;i<TabCnt;++i)
									w+=EachTabLayer[i].TabWidth;
								if (w>gPS.w)
								{
									for (int i=-event.wheel.y;i>=1&&LeftMostShowLayer<TabCnt&&w>gPS.w;--i)
										w-=EachTabLayer[LeftMostShowLayer++].TabWidth;
									Win->SetNeedFreshScreen();
									Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TabHeight));
								}
							}
							Win->StopSolveEvent();
						}
						break;
						
					case SDL_USEREVENT:
						if (event.user.type==PUI_EVENT_UpdateTimer)
							if (event.user.data1==this)
							{
								
							}
						break;
				}
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
							{
								stat=0;
								FocusingPos=-1;
								RemoveNeedLoseFocus();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						FocusingPos=GetTabFromPos(Win->NowPos());
						if (FocusingPos!=-1)
						{
							stat=2;
							if (CurrentShowLayerPos!=FocusingPos)
							{
								PUI_DD[0]<<"TabLayer "<<ID<<" Switch layer to "<<FocusingPos<<endl;
								if (CurrentShowLayerPos!=-1)
									EachTabLayer[CurrentShowLayerPos].lay->SetEnabled(0);
								CurrentShowLayerPos=FocusingPos;
								EachTabLayer[CurrentShowLayerPos].lay->SetEnabled(1);
								if (func!=NULL)
									func(EachTabLayer[CurrentShowLayerPos].funcData,CurrentShowLayerPos,1);
								Win->SetNeedUpdatePosize();
							}
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						if (event.button.button==SDL_BUTTON_RIGHT)
							stat=3;
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							if (stat==3)
								if (func!=NULL)
									func(EachTabLayer[FocusingPos].funcData,FocusingPos,2); 
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
					
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							FocusingPos=GetTabFromPos(Win->NowPos());
							if (FocusingPos!=-1)
							{
								stat=1;
								SetNeedLoseFocus();
								Win->SetNeedFreshScreen();
								Win->StopSolvePosEvent();
								Win->SetPresentArea(gPS);
							}
						}
						else
						{
							int p=GetTabFromPos(Win->NowPos());
							if (p==FocusingPos)
								Win->StopSolvePosEvent();
							else if (p==-1)
							{
								stat=0;
								FocusingPos=-1;
								RemoveNeedLoseFocus();
								Win->SetNeedFreshScreen();
								Win->StopSolvePosEvent();
								Win->SetPresentArea(gPS);
							}
							else
							{
								FocusingPos=p;
								Win->SetNeedFreshScreen();
								Win->StopSolvePosEvent();
								Win->SetPresentArea(gPS);
							}
						}
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				Win->RenderFillRect(Posize(gPS.x,gPS.y,gPS.w-1,TabHeight)&lmt,TabBarBackgroundColor?TabBarBackgroundColor:ThemeColor.BackgroundColor[1]);
				if (TabCnt>0)
				{
					Posize TabPs(gPS.x,gPS.y,EachTabLayer[LeftMostShowLayer].TabWidth,TabHeight);
					for (int i=LeftMostShowLayer;i<TabCnt&&TabPs.x<gPS.x2();++i)
					{
						int colorPos=FocusingPos==i?(stat==2||stat==3?2:1):CurrentShowLayerPos==i;
						Win->RenderFillRect(TabPs&lmt,EachTabLayer[i].TabColor[colorPos]?EachTabLayer[i].TabColor[colorPos]:ThemeColor[colorPos*2+1]);
						if (EachTabLayer[i].pic()!=NULL)
							Win->RenderCopyWithLmt(EachTabLayer[i].pic(),Posize(TabPs.x,TabPs.y,TabHeight,TabHeight),lmt);
						Win->RenderDrawText(EachTabLayer[i].title,TabPs,lmt,0,EachTabLayer[i].TextColor?EachTabLayer[i].TextColor:ThemeColor.MainTextColor[0]);
						
						Win->Debug_DisplayBorder(TabPs);
						
						TabPs.x+=TabPs.w+1;
						TabPs.w=EachTabLayer[i].TabWidth;
					}
				}
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			void SwitchLayer(int p)
			{
				if (TabCnt==0) return;
				p=EnsureInRange(p,0,TabCnt-1);
				if (CurrentShowLayerPos!=p)
				{
					PUI_DD[0]<<"TabLayer "<<ID<<" Switch layer to "<<p<<endl;
					if (CurrentShowLayerPos!=-1)
						EachTabLayer[CurrentShowLayerPos].lay->SetEnabled(0);
					CurrentShowLayerPos=p;
					EachTabLayer[p].lay->SetEnabled(1);
					if (func!=NULL)
						func(EachTabLayer[p].funcData,p,1);
					Win->SetNeedUpdatePosize();
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
			}
			
			Layer* PushbackLayer(const string &title,const T &_funcdata,const SharedTexturePtr &pic=SharedTexturePtr(NULL))
			{
				Layer *re=new Layer(0,this,new PosizeEX_TabLayer(this));
				EachTabLayer.push_back(TabLayerData(re,title,pic,_funcdata));
				int w=0;
				TTF_SizeUTF8(PUI_Font()(),title.c_str(),&w,NULL);
				EachTabLayer[TabCnt].TabWidth=TabLayerWidthRange.EnsureInRange(w+10);
				++TabCnt;
				SwitchLayer(TabCnt-1);
				return re;
			}
			
			Layer* AddLayer(int pos,const string &title,const T &_funcdata,const SharedTexturePtr &pic=SharedTexturePtr(NULL))
			{
				pos=EnsureInRange(pos,0,TabCnt);
				Layer *re=new Layer(0,this,new PosizeEX_TabLayer(this));
				EachTabLayer.insert(EachTabLayer.begin()+pos,TabLayerData(re,title,pic,_funcdata));
				if (pos<=CurrentShowLayerPos)
					++CurrentShowLayerPos;
				int w=0;
				TTF_SizeUTF8(PUI_Font()(),title.c_str(),&w,NULL);
				EachTabLayer[pos].TabWidth=TabLayerWidthRange.EnsureInRange(w+10);
				++TabCnt;
				SwitchLayer(pos);
				return re;
			}
			
			void DeleteLayer(int pos)
			{
				if (TabCnt==0) return;
				pos=EnsureInRange(pos,0,TabCnt-1);
				if (pos==CurrentShowLayerPos)
					if (pos==0)
						if (TabCnt>=2)
						{
							SwitchLayer(1);
							CurrentShowLayerPos=0;
						}	
						else CurrentShowLayerPos=-1;
					else SwitchLayer(pos-1);
				else
					if (pos<CurrentShowLayerPos)
						--CurrentShowLayerPos;
				delete EachTabLayer[pos].lay;
				EachTabLayer.erase(EachTabLayer.begin()+pos);
				--TabCnt;
			}
			
			inline void SetTabBarBackgroundColor(const RGBA &co)
			{
				TabBarBackgroundColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TabHeight));
			}
			
			inline void SetTabColor(int pos,int p,const RGBA &co)
			{
				pos=EnsureInRange(pos,0,TabCnt-1);
				if (InRange(p,0,2))
				{
					EachTabLayer[pos].TabColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TabHeight));
				}
				else PUI_DD[1]<<"TabLayer: SetTabColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			inline void SetTabTextColor(int pos,const RGBA &co)
			{
				EachTabLayer[EnsureInRange(pos,0,TabCnt-1)].TextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TabHeight));
			}
			
			void SetTabTitle(int pos,const string &str)
			{
				pos=EnsureInRange(pos,0,TabCnt-1);
				EachTabLayer[pos].title=str;
				int w=0;
				TTF_SizeUTF8(PUI_Font()(),str.c_str(),&w,NULL);
				EachTabLayer[pos].TabWidth=TabLayerWidthRange.EnsureInRange(w+10);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TabHeight));
			}
			
			void SetTabPic(int pos,const SharedTexturePtr &pic)
			{
				EachTabLayer[EnsureInRange(pos,0,TabCnt-1)].pic=pic;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TabHeight));
			}
			
			void SetTabHeight(int h)
			{
				Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,max(h,TabHeight)));
				TabHeight=h;
				Win->SetNeedFreshScreen();
				Win->SetNeedUpdatePosize();
			}
			
			void SetTabLayerWidthRange(const Range &ran)
			{
				TabLayerWidthRange=ran;
				for (int i=0,w;i<TabCnt;++i)
				{
					TTF_SizeUTF8(PUI_Font()(),EachTabLayer[i].title.c_str(),&w,NULL);
					EachTabLayer[i].TabWidth=TabLayerWidthRange.EnsureInRange(EachTabLayer[i].TabWidth);
				}
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TabHeight));
			}
			
			inline void SetFunc(void (*_func)(T&,int,int))
			{func=_func;}
			
			inline void SetTabFuncdata(int p,void *_funcdata)
			{EachTabLayer[p].funcData=_funcdata;}
			
			inline int GetTabcnt()
			{return TabCnt;}
			
			inline int GetCurrenShowLayerPos()
			{return CurrentShowLayerPos;}
			
			Layer* operator [] (int p)
			{return EachTabLayer[EnsureInRange(p,0,TabCnt-1)].lay;}
			
			Layer* Tab(int p)
			{return EachTabLayer[EnsureInRange(p,0,TabCnt-1)].lay;}
			
			TabLayer(int _ID,Widgets *_fa,PosizeEX *psex)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TabLayer "<<ID<<endl;
				Type=WidgetType_TabLayer;
				SetFa(_fa);
				AddPsEx(psex);
			}
			
			TabLayer(int _ID,Widgets *_fa,const Posize &_rps)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TabLayer "<<ID<<endl;
				Type=WidgetType_TabLayer;
				SetFa(_fa);
				SetrPS(_rps);
			}
	};
	
	class BorderRectLayer:public Layer
	{
		protected:
			RGBA BorderColor=RGBA_NONE;
			int BorderWidth=1;
			
			virtual void Show(Posize &lmt)
			{
				for (int i=1;i<=BorderWidth;++i)
					Win->RenderDrawRectWithLimit(gPS.Shrink(i-1),BorderColor?BorderColor:ThemeColor[1],lmt);
				Win->RenderFillRect(lmt&gPS.Shrink(BorderWidth-1),LayerColor);
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			void SetBorderColor(const RGBA &co)
			{
				BorderColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetBorderWidth(int _w)
			{
				if (_w<0) return;
				BorderWidth=_w;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}

			BorderRectLayer(int _ID,Widgets *_fa,const Posize &_rps):Layer(_ID,_fa,_rps)
			{
				PUI_DD[0]<<"Create BorderRectLayer "<<ID<<endl;
				Type=WidgetType_BorderRectLayer;
			}
			
			BorderRectLayer(int _ID,Widgets *_fa,PosizeEX *psex):Layer(_ID,_fa,psex)
			{
				PUI_DD[0]<<"Create BorderRectLayer "<<ID<<endl;
				Type=WidgetType_BorderRectLayer;
			}
	};
	
	class BlurEffectLayer:public Widgets
	{
		protected:
			
			
		public:
			
			
	}; 
	
	template <class T> class Slider:public Widgets
	{
		protected:
			bool Vertical;
			Posize ckPs={0,0,8,8};
			int barWidth=3;
			double Percent=0;
			int stat=0;//0:Up_NoFocus 1:Up_Focus_Chunk 2:Down_Slide_Chunk
			void (*func)(T&,double,bool)=NULL;//double: percent bool:IsLooseMouse(set 0 when continuous changing)
			T funcData;
			RGBA BarColor[3],
				 ChunkColor[3];
			
			void SetChunkPs(const Point &pt,bool bo)
			{
				if (Vertical)
					ckPs.y=EnsureInRange(pt.y-ckPs.h/2,gPS.y,gPS.y2()-ckPs.h),
					Percent=(ckPs.y-gPS.y)*1.0/(gPS.h-ckPs.h);
				else 
					ckPs.x=EnsureInRange(pt.x-ckPs.w/2,gPS.x,gPS.x2()-ckPs.w),
					Percent=(ckPs.x-gPS.x)*1.0/(gPS.w-ckPs.w);
				if (func!=NULL)
					func(funcData,Percent,bo);
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
							{
								PUI_DD[0]<<"Slider "<<ID<<" losefocus"<<endl;
								stat=0;
								RemoveNeedLoseFocus();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							PUI_DD[0]<<"Slider "<<ID<<" click"<<endl;
							stat=2;
							SetChunkPs(Win->NowPos(),0);
							Win->SetOccupyPosWg(this);
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							PUI_DD[0]<<"Slider "<<ID<<" loose"<<endl;
							stat=1;
							SetChunkPs(Win->NowPos(),1);
							Win->SetOccupyPosWg(NULL);
//							RemoveNeedLoseFocus();
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat==2)
						{
							PUI_DD[0]<<"Slider "<<ID<<" slider"<<endl;
							Win->SetPresentArea(ckPs);
							SetChunkPs(Win->NowPos(),0);
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(ckPs);
							Win->StopSolvePosEvent(); 
						}
						else if (stat==0)
						{
							PUI_DD[0]<<"Slider "<<ID<<" focus"<<endl;
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				if (Vertical)
					Win->RenderFillRect(Posize(gPS.x+(gPS.w-barWidth>>1),gPS.y,barWidth,gPS.h)&lmt,BarColor[stat]?BarColor[stat]:ThemeColor[stat*2]);
				else Win->RenderFillRect(Posize(gPS.x,gPS.y+(gPS.h-barWidth>>1),gPS.w,barWidth)&lmt,BarColor[stat]?BarColor[stat]:ThemeColor[stat*2]);
				Win->RenderFillRect(ckPs&lmt,ChunkColor[stat]?ChunkColor[stat]:ThemeColor[stat*2+1]); 
				
				Win->Debug_DisplayBorder(gPS);
			}
			
			virtual void CalcPsEx()
			{
				if (PsEx!=NULL)
					PsEx->GetrPS(rPS);
				Posize lastPs=gPS;
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
				
				if (Vertical) 
					ckPs.x=gPS.x,ckPs.y=gPS.y+(gPS.h-ckPs.h)*Percent,ckPs.w=gPS.w;
				else ckPs.x=gPS.x+(gPS.w-ckPs.w)*Percent,ckPs.y=gPS.y,ckPs.h=gPS.h;
			}
			
		public:
			inline double operator () ()//??
			{return Percent;}
			
			inline void SetBarColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					BarColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"Slider: SetBarColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			inline void SetChunkColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					ChunkColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"Slider: SetChunkColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			void SetPercent(double per,bool triggerFunc=1)
			{
				if (stat==2) return;
				Percent=EnsureInRange(per,0,1);
				Win->SetPresentArea(ckPs);
				if (Vertical) ckPs.y=gPS.y+(gPS.h-ckPs.h)*Percent;
				else ckPs.x=gPS.x+(gPS.w-ckPs.w)*Percent;
				if (triggerFunc)
					if (func!=NULL)
						func(funcData,Percent,1);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(ckPs);
			}
			
			void SetPercent_Delta(double per,bool triggerFunc=1)
			{SetPercent(Percent+per,triggerFunc);}
			
			inline void SetFunc(void (*_func)(T&,double,bool),const T &_funcdata)
			{
				func=_func;
				funcData=_funcdata;
			}
			
			inline void SetBarWidth(int w)
			{
				barWidth=max(0,w);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetChunkWidth(int w)
			{
				Win->SetPresentArea(ckPs);
				if (Vertical)
					ckPs.h=w,ckPs.y=gPS.y+(gPS.h-ckPs.h)*Percent;
				else ckPs.w=w,ckPs.x=gPS.x+(gPS.w-ckPs.w)*Percent;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(ckPs);
			}
			
			Slider(int _ID,Widgets *_fa,const Posize &_rps,bool _vertical,void (*_func)(T&,double,bool),const T &_funcdata)
			:Vertical(_vertical),func(_func),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create Slider "<<ID<<endl;
				Type=WidgetType_Slider;
				SetFa(_fa);
				SetrPS(_rps);
				for (int i=0;i<=2;++i)
					ChunkColor[i]=BarColor[i]=RGBA_NONE;
			}
			
			Slider(int _ID,Widgets *_fa,PosizeEX *psex,bool _vertical,void (*_func)(T&,double,bool),const T &_funcdata)
			:Vertical(_vertical),func(_func),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create Slider "<<ID<<endl;
				Type=WidgetType_Slider;
				SetFa(_fa);
				AddPsEx(psex);
				for (int i=0;i<=2;++i)
					ChunkColor[i]=BarColor[i]=RGBA_NONE;
			}
	};
	
	template <class T> class FullFillSlider:public Widgets
	{
		protected:
			bool Vertical;
			double Percent=0;
			int stat=0;//0:Up_NoFocus 1:Up_Focus_Chunk 2:Down_Slide_Chunk
			void (*func)(T&,double,bool)=NULL;//double: percent bool:IsLooseMouse(set 0 when continuous changing)
			T funcData;
			RGBA BarColor[3],
				 ChunkColor[3];
			
			void SetChunkPs(const Point &pt,bool bo)
			{
				if (Vertical) Percent=(pt.y-gPS.y)*1.0/gPS.h;
				else Percent=(pt.x-gPS.x)*1.0/gPS.w;
				if (func!=NULL)
					func(funcData,Percent,bo);
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
							{
								PUI_DD[0]<<"FullFillSlider "<<ID<<" losefocus"<<endl;
								stat=0;
								RemoveNeedLoseFocus();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							PUI_DD[0]<<"FullFillSlider "<<ID<<" click"<<endl;
							stat=2;
							SetChunkPs(Win->NowPos(),0);
							Win->SetOccupyPosWg(this);
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							PUI_DD[0]<<"FullFillSlider "<<ID<<" loose"<<endl;
							stat=1;
							SetChunkPs(Win->NowPos(),1);
							Win->SetOccupyPosWg(NULL);
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat==2)
						{
							PUI_DD[0]<<"FullFillSlider "<<ID<<" slider"<<endl;
							double per=Percent;
							SetChunkPs(Win->NowPos(),0);
							Win->SetNeedFreshScreen();
							Win->SetPresentArea((Vertical?Posize(0,min(per,Percent)*gPS.h,gPS.w,ceil(fabs(per-Percent)*gPS.h)):Posize(min(per,Percent)*gPS.w,0,ceil(fabs(per-Percent)*gPS.w),gPS.h))+gPS);
							Win->StopSolvePosEvent(); 
						}
						else if (stat==0)
						{
							PUI_DD[0]<<"FullFillSlider "<<ID<<" focus"<<endl;
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				RGBA BarCo=BarColor[stat]?BarColor[stat]:ThemeColor[stat*2],
					 ChkCo=ChunkColor[stat]?ChunkColor[stat]:ThemeColor[stat*2+1];
				if (Vertical)
					Win->RenderFillRect(Posize(gPS.x,gPS.y,gPS.w,gPS.h*Percent)&lmt,ChkCo),
					Win->RenderFillRect(Posize(gPS.x,gPS.y+gPS.h*Percent,gPS.w,gPS.h-gPS.h*Percent)&lmt,BarCo);
				else
					Win->RenderFillRect(Posize(gPS.x,gPS.y,gPS.w*Percent,gPS.h)&lmt,ChkCo),
					Win->RenderFillRect(Posize(gPS.x+gPS.w*Percent,gPS.y,gPS.w-gPS.w*Percent,gPS.h)&lmt,BarCo);
					
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			inline void SetBarColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					BarColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"FullFillSlider: SetBarColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			inline void SetChunkColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					ChunkColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"FullFillSlider: SetChunkColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			void SetPercent(double per,bool triggerFunc=1)
			{
				if (stat==2) return;
				per=EnsureInRange(per,0,1);
				Win->SetPresentArea((Vertical?Posize(0,min(per,Percent)*gPS.h,gPS.w,ceil(fabs(per-Percent)*gPS.h)):Posize(min(per,Percent)*gPS.w,0,ceil(fabs(per-Percent)*gPS.w),gPS.h))+gPS);
				Percent=per;
				if (triggerFunc)
					if (func!=NULL)
						func(funcData,Percent,1);
				Win->SetNeedFreshScreen();
			}
			
			void SetPercent_Delta(double per,bool triggerFunc=1)
			{SetPercent(Percent+per,triggerFunc);}
			
			inline void SetFunc(void (*_func)(T&,double,bool),const T &_funcdata)
			{
				func=_func;
				funcData=_funcdata;
			}
			
			FullFillSlider(int _ID,Widgets *_fa,const Posize &_rps,bool _vertical,void (*_func)(T&,double,bool),const T &_funcdata)
			:Vertical(_vertical),func(_func),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create FullFillSlider "<<ID<<endl;
				Type=WidgetType_FullFillSlider;
				SetFa(_fa);
				SetrPS(_rps);
				for (int i=0;i<=2;++i)
					ChunkColor[i]=BarColor[i]=RGBA_NONE;
			}
			
			FullFillSlider(int _ID,Widgets *_fa,PosizeEX *psex,bool _vertical,void (*_func)(T&,double,bool),const T &_funcdata)
			:Vertical(_vertical),func(_func),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create FullFillSlider "<<ID<<endl;
				Type=WidgetType_FullFillSlider;
				SetFa(_fa);
				AddPsEx(psex);
				for (int i=0;i<=2;++i)
					ChunkColor[i]=BarColor[i]=RGBA_NONE;
			}	
	};
	
	class ProgressBar:public Widgets
	{
		protected:
			double Percent=0;
			bool Vertical=0;
//			double (*UpdateFunc)(void *)=NULL;//return -1 means not change;
//			void *funcData=NULL;
			RGBA BackgroundColor=RGBA_NONE,
				 BarColor=RGBA_NONE,
				 FullColor=RGBA_NONE;
			
			virtual void Show(Posize &lmt)
			{
//				if (UpdateFunc!=NULL)
//				{
//					double per=UpdateFunc(funcData);
//					if (per!=-1)
//						Percent=EnsureInRange(per,0,1);
//				}

				Win->RenderFillRect(gPS&lmt,BackgroundColor?BackgroundColor:ThemeColor[0]);
				Win->RenderFillRect((Vertical?Posize(gPS.x,gPS.y+gPS.h*(1-Percent),gPS.w,gPS.h*Percent):Posize(gPS.x,gPS.y,gPS.w*Percent,gPS.h)).Shrink(0)&lmt,
									Percent==1?(FullColor?FullColor:ThemeColor[7]):(BarColor?BarColor:ThemeColor[4]));
				
				Win->Debug_DisplayBorder(gPS);
			}

		public:
			inline void SetBackgroundColor(const RGBA &co)
			{
				BackgroundColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetBarColor(const RGBA &co)
			{
				BarColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetFullColor(const RGBA &co)
			{
				FullColor=co;
				if (Percent==1)
				{
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
			}
			
			inline void SetPercent(double per)
			{
				per=EnsureInRange(per,0,1);
				if (per==Percent) return;
				if (per==1||Percent==1)
					Win->SetPresentArea(gPS);
				if (Vertical)
					Win->SetPresentArea(Posize(gPS.x,gPS.y+gPS.h*(1-min(Percent,per))-1,gPS.w,gPS.h*fabs(per-Percent)+2));
				else Win->SetPresentArea(Posize(gPS.x+gPS.w*min(Percent,per)-1,gPS.y,gPS.w*fabs(per-Percent)+2,gPS.h));
				Percent=per;  
				Win->SetNeedFreshScreen();
			}
			
//			inline void SetUpdateFunc(double (*func)(void*),void *_funcdata)
//			{
//				UpdateFunc=func;
//				funcData=_funcdata;
//			}
			
			ProgressBar(int _ID,Widgets *_fa,const Posize &_rps,bool _vertical=0/*,double (*func)(void*)=NULL,void *_funcdata=NULL*/)
			:Vertical(_vertical)//,UpdateFunc(func)
			{
				SetID(_ID);
				PUI_DD[0]<<"CreateProgessBar "<<ID<<endl;
				Type=WidgetType_ProgressBar;
				SetFa(_fa);
				SetrPS(_rps);
//				if (_funcdata==CONST_THIS)
//					funcData=this;
//				else funcData=_funcdata;
//				if (UpdateFunc!=NULL)
//				{
//					double per=UpdateFunc(funcData);
//					if (per!=-1)
//						Percent=EnsureInRange(per,0,1);
//				}
			}
			
			ProgressBar(int _ID,Widgets *_fa,PosizeEX *psex,bool _vertical=0/*,double (*func)(void*)=NULL,void *_funcdata=NULL*/)
			:Vertical(_vertical)//,UpdateFunc(func)
			{
				SetID(_ID);
				PUI_DD[0]<<"CreateProgessBar "<<ID<<endl;
				Type=WidgetType_ProgressBar;
				SetFa(_fa);
				AddPsEx(psex);
//				if (_funcdata==CONST_THIS)
//					funcData=this;
//				else funcData=_funcdata;
//				if (UpdateFunc!=NULL)
//				{
//					double per=UpdateFunc(funcData);
//					if (per!=-1)
//						Percent=EnsureInRange(per,0,1);
//				}
			}
	};
	
	template <class T> class PictureBox:public Widgets
	{
		protected:
			SDL_Texture *src=NULL;
			int stat=0;//0:NotFocus 1:Focus 2:Down_Left_TwiceClick 3:Down_Right 4:Down_Left_OnceClick
			Posize srcPS;
			int Mode=0;//0:Fullfill rPS 1:show part that can see in original size,pin centre(MM)
					   //2:like 1,pin LU 3:like 1,pin RU 4:like 1,pin LD 5:like 1,pin RD
					   //6:like 1,pin MU 7:like 1,pin MD 8:like 1,pin ML 9:like 1,pin MR (Mid,Left,Right,Up,Down)
					   //11:show all that can be seen with fiexd w/h rate,pin centre(MM)
					   //12:like 11,pin U 13:like 11,pin D 14:like 11,pin L 15:like 11,pin R 
					   //21:fill box and show most pic,pin centre(MM)
					   //22:like 21,pin LU 23:like 21,pin RU 24:like 21,pin LD 25:like 21,pin RD
					   //26:like 21,pin MU 27:like 21,pin MD 28:like 21,pin ML 29:like 21,pin MR
			bool AutoDeletePic=0;
			void (*func)(T&,int)=NULL;//int: 0:None 1:Left_Click 2:Left_Double_Click 3:Right_Click
			T funcData;
			RGBA BackGroundColor=RGBA_BLACK;

			virtual void CheckPos()
			{
				if (func==NULL) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							RemoveNeedLoseFocus();
						}
					return;
				}

				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						PUI_DD[0]<<"PictureBox "<<ID<<" click"<<endl;
						if (event.button.button==SDL_BUTTON_LEFT)
							if (event.button.clicks==2)
								stat=2;
							else stat=4;
						else if (event.button.button==SDL_BUTTON_RIGHT)
							stat=3;
						else stat=4;
						Win->StopSolvePosEvent();
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (InRange(stat,2,4))
						{
							if (func!=NULL)
							{
								PUI_DD[0]<<"PictureBox "<<ID<<" function "<<(stat==2||stat==4?"left":"right")<<endl;
								func(funcData,stat==4?1:stat);
								Win->StopSolvePosEvent();
							}
							stat=1;
						}
						break;
					
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
						}
						Win->StopSolvePosEvent();
						break;
				}
			}

			virtual void Show(Posize &lmt)
			{
				Win->RenderFillRect(gPS&lmt,BackGroundColor);
				if (src!=NULL)
					switch (Mode)
					{
						case 0:	Win->RenderCopyWithLmt(src,gPS,lmt);														break;
						case 1:	Win->RenderCopyWithLmt(src,Point(gPS.x+(gPS.w-srcPS.w>>1),gPS.y+(gPS.h-srcPS.h>>1)),lmt);	break;
						case 2: Win->RenderCopyWithLmt(src,gPS.GetLU(),lmt);												break;
						case 3: Win->RenderCopyWithLmt(src,Point(gPS.x2()-srcPS.w,gPS.y),lmt);								break;
						case 4: Win->RenderCopyWithLmt(src,Point(gPS.x,gPS.y2()-srcPS.h),lmt);								break;
						case 5: Win->RenderCopyWithLmt(src,Point(gPS.x2()-srcPS.w,gPS.y2()-srcPS.h),lmt);					break;
						case 6: Win->RenderCopyWithLmt(src,Point(gPS.x+(gPS.w-srcPS.w>>1),gPS.y),lmt);						break;
						case 7: Win->RenderCopyWithLmt(src,Point(gPS.x+(gPS.w-srcPS.w>>1),gPS.y2()-srcPS.h),lmt);			break;
						case 8: Win->RenderCopyWithLmt(src,Point(gPS.x,gPS.y+(gPS.h-srcPS.h>>1)),lmt);						break;
						case 9: Win->RenderCopyWithLmt(src,Point(gPS.x2()-srcPS.w,gPS.y+(gPS.h-srcPS.h>>1)),lmt);			break;
						case 11:Win->RenderCopyWithLmt(src,(srcPS.Slope()>=rPS.Slope()?Posize((gPS.w-gPS.h*srcPS.InverseSlope())/2,0,gPS.h*srcPS.InverseSlope(),gPS.h)
									:Posize(0,(gPS.h-gPS.w*srcPS.Slope())/2,gPS.w,gPS.w*srcPS.Slope()))+gPS,lmt);			break;
						case 12:Win->RenderCopyWithLmt(src,(srcPS.Slope()>=rPS.Slope()?Posize((gPS.w-gPS.h*srcPS.InverseSlope())/2,0,gPS.h*srcPS.InverseSlope(),gPS.h)
									:Posize(0,0,gPS.w,gPS.w*srcPS.Slope()))+gPS,lmt);										break;
						case 13:Win->RenderCopyWithLmt(src,(srcPS.Slope()>=rPS.Slope()?Posize((gPS.w-gPS.h*srcPS.InverseSlope())/2,0,gPS.h*srcPS.InverseSlope(),gPS.h)
									:Posize(0,gPS.h-gPS.w*srcPS.Slope(),gPS.w,gPS.w*srcPS.Slope()))+gPS,lmt);				break;
						case 14:Win->RenderCopyWithLmt(src,(srcPS.Slope()>=rPS.Slope()?Posize(0,0,gPS.h*srcPS.InverseSlope(),gPS.h)
									:Posize(0,(gPS.h-gPS.w*srcPS.Slope())/2,gPS.w,gPS.w*srcPS.Slope()))+gPS,lmt);			break;
						case 15:Win->RenderCopyWithLmt(src,(srcPS.Slope()>=rPS.Slope()?Posize(gPS.w-gPS.h*srcPS.InverseSlope(),0,gPS.h*srcPS.InverseSlope(),gPS.h)
									:Posize(0,(gPS.h-gPS.w*srcPS.Slope())/2,gPS.w,gPS.w*srcPS.Slope()))+gPS,lmt);			break;
						case 21:Win->RenderCopyWithLmt(src,(srcPS.Slope()>=rPS.Slope()?Posize(0,(srcPS.h-srcPS.w*gPS.Slope())/2,srcPS.w,ceil(srcPS.w*gPS.Slope()))
									:Posize((srcPS.w-srcPS.h*gPS.InverseSlope())/2,0,ceil(srcPS.h*gPS.InverseSlope()),srcPS.h)),gPS,lmt); break;
						default:
							PUI_DD[2]<<"PictureBox: Mode "<<Mode<<" is not in valid range!"<<endl;
							Win->RenderCopyWithLmt(src,gPS,lmt);
					}
					
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			inline void SetBackgroundColor(const RGBA &co)
			{
				BackGroundColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetPicture(SDL_Texture *tex,bool AutoDelete,int _mode=0)
			{
				if (AutoDeletePic)
					SDL_DestroyTexture(src);
				src=tex;
				AutoDeletePic=AutoDelete;
				Mode=_mode;
				srcPS=GetTexturePosize(src);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetFunc(void (*_func)(T&,int),const T &_funcdata)
			{
				func=_func;
				funcData=_funcdata;
			}
			
			virtual ~PictureBox()
			{
				PUI_DD[0]<<"Delete PictureBox "<<ID<<endl;
				if (AutoDeletePic)
					SDL_DestroyTexture(src);
			}

			PictureBox(int _ID,Widgets *_fa,const Posize &_rps)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create PictureBox "<<ID<<endl;
				Type=WidgetType_PictureBox;
				SetFa(_fa);
				SetrPS(_rps);
			}
			
			PictureBox(int _ID,Widgets *_fa,PosizeEX *psex)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create PictureBox "<<ID<<endl;
				Type=WidgetType_PictureBox;
				SetFa(_fa);
				AddPsEx(psex);
			}
	};
	
	class PhotoViewer:public Widgets
	{
		protected:
			SharedTexturePtr tex;
			SDL_Surface *sur=NULL;
			LargeLayerWithScrollBar *fa2=NULL;
			Posize srcPS=ZERO_POSIZE;
			int stat=0;//1:Up 2.Down_Left 3.Down_Right
			double lastPer=1,
				   per=1,
				   delta0=0.2,
				   delta=0,
				   minPer=1,
				   maxPer=16;
//				   fingerDist1;
			bool SurfaceMode=0,
				 AutoFreeSurface=0;
			Point LastPt=ZERO_POINT,
				  ScaleCentre=ZERO_POINT;
			void (*RightClickFunc)(void*)=NULL;
			void *RightClickFuncData=NULL;

			void RecalcPic()
			{
				rPS.w=srcPS.w*per;
				rPS.h=srcPS.h*per;
				if (rPS.w<=fa2->GetrPS().w)
				{
					rPS.x=(fa2->GetrPS().w-rPS.w)/2;
					fa2->ResizeLL(fa2->GetrPS().w,0);
				}
				else
				{
					rPS.x=0;
					fa2->ResizeLL(rPS.w,0);
					fa2->SetViewPort(1,-(ScaleCentre.x-(ScaleCentre.x-gPS.x)*per/lastPer-fa2->GetgPS().x));
				}
				
				if (rPS.h<=fa2->GetrPS().h)
				{
					rPS.y=(fa2->GetrPS().h-rPS.h)/2;
					fa2->ResizeLL(0,fa2->GetrPS().h);
				}
				else
				{
					rPS.y=0;
					fa2->ResizeLL(0,rPS.h);
					fa2->SetViewPort(2,-(ScaleCentre.y-(ScaleCentre.y-gPS.y)*per/lastPer-fa2->GetgPS().y));
				}
			}

			void MovePic(const Point &pt)
			{
				fa2->SetViewPort(5,LastPt.x-pt.x);
				fa2->SetViewPort(6,LastPt.y-pt.y);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
				LastPt=pt;
			}
			
			virtual void CheckEvent()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (event.type==SDL_MOUSEWHEEL)
					if (stat!=0)
					{
						delta=per*delta0;
						lastPer=per;
						per+=event.wheel.y*delta;
						per=EnsureInRange(per,minPer,maxPer);
						RecalcPic();
						Win->StopSolveEvent();
					}
			}

			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							RemoveNeedLoseFocus();
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						PUI_DD[0]<<"Button Down In Photoviewer "<<ID<<endl;
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							stat=2;
							ScaleCentre=LastPt=Win->NowPos();
							Win->SetOccupyPosWg(this);
						}
						else if (event.button.button==SDL_BUTTON_RIGHT)
						{
							stat=3;
						}
						Win->StopSolvePosEvent();
						break;
					
					case SDL_MOUSEBUTTONUP:
						PUI_DD[0]<<"Mouse button up in Photoviewer "<<ID<<endl;
						if (stat==2)
						{
							MovePic(Win->NowPos());
							Win->SetOccupyPosWg(NULL);
						}
						else if (stat==3)
							if (RightClickFunc!=NULL)
								RightClickFunc(RightClickFuncData);
						stat=1;
						Win->StopSolvePosEvent();
						break;
						
					case SDL_MOUSEMOTION:
						ScaleCentre=Win->NowPos();
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
						}
						if (stat==2)
							MovePic(Win->NowPos());
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				lmt=gPS&lmt;
				Win->RenderCopyWithLmt(tex(),srcPS,gPS,lmt);
				Win->Debug_DisplayBorder(gPS);
			}
			
			virtual void CalcPsEx()
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)	
					PsEx->GetrPS(rPS);
				static Posize lastFa2Ps=ZERO_POSIZE;
				if (tex()!=NULL&&(fa2->GetrPS().w!=lastFa2Ps.w||fa2->GetrPS().h!=lastFa2Ps.h))
				{
					minPer=min(1.0,min(fa2->GetrPS().w*1.0/srcPS.w,fa2->GetrPS().h*1.0/srcPS.h));
					lastPer=per=EnsureInRange(per,minPer,maxPer);
					RecalcPic();
					lastFa2Ps=fa2->GetrPS();
				}
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}
			
		public:			
			virtual void SetBackGroundColor(RGBA co)
			{fa2->SetLargeAreaColor(co);}
			
			void AddPsEx(PosizeEX *psex)
			{fa2->AddPsEx(psex);}
	
			void SetPic(const SharedTexturePtr &_tex)
			{
				if (SurfaceMode)
					if (AutoFreeSurface)
					{
						SDL_FreeSurface(sur);
						sur=NULL;
						AutoFreeSurface=0;
					}
				SurfaceMode=0;
				tex=_tex;
				if (!tex) return;
				srcPS=GetTexturePosize(tex());
				lastPer=per=minPer=min(1.0,min(fa2->GetrPS().w*1.0/srcPS.w,fa2->GetrPS().h*1.0/srcPS.h));
				RecalcPic();
			}
			
//			void SetPic(SDL_Surface *_sur,bool _AutoFreeSurface=1)
//			{
//				src=SharedTexturePtr();
//				if (SurfaceMode)
//					if (AutoFreeSurface)
//						SDL_FreeSurface(sur);
//				sur=_sur;
//				AutoFreeSurface=_AutoFreeSurface;
//				SurfaceMode=1;
//				src=
//			}

			void SetRightClickFunc(void (*_rightclickfunc)(void*)=NULL,void *rightclickfuncdata=NULL)
			{
				RightClickFunc=_rightclickfunc;
				RightClickFuncData=rightclickfuncdata;
			}
	
			~PhotoViewer()
			{
				PUI_DD[0]<<"Delete PhotoViewer "<<ID<<endl;
				if (SurfaceMode)
					if (AutoFreeSurface)
						SDL_FreeSurface(sur);
			}
			
			PhotoViewer(int _ID,Widgets *_fa,PosizeEX *psex)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create PhotoViewer "<<ID<<endl;
				Type=WidgetType_PhotoViewer;
				fa2=new LargeLayerWithScrollBar(0,_fa,psex,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				fa2->SetScrollBarWidth(8);
				rPS=ZERO_POSIZE;
			}
			
			PhotoViewer(int _ID,Widgets *_fa,const Posize &_rPS)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create PhotoViewer "<<ID<<endl;
				Type=WidgetType_PhotoViewer;
				fa2=new LargeLayerWithScrollBar(0,_fa,_rPS,_rPS.ToOrigin());
				SetFa(fa2->LargeArea());
				fa2->SetScrollBarWidth(8);
				rPS=ZERO_POSIZE;
			}
	};
	
	template <class T> class SimpleListView:public Widgets
	{
		protected:
			int ListCnt=0;
			vector <string> Text;
			vector <T> FuncData;//It means this widget's FuncData would be deconstructed when deleted
			void (*func)(T&,int,int)=NULL;//int1:Pos(CountFrom 0)   int2: 0:None 1:Left_Click 2:Left_Double_Click 3:Right_Click
			T BackgroundFuncData;
			LargeLayerWithScrollBar *fa2=NULL;
			int stat=0,//0:Up_NoFocus 1:Up_Focus_Row 2:Down_Left_TwiceClick 3:Down_Right 4:Down_Left_OnceClick
				FocusChoose=-1,
				ClickChoose=-1;
			int EachHeight=24,
				Interval=2,
				TextMode=-1;
			RGBA TextColor=RGBA_NONE,
				 RowColor[3];
				 
			inline Posize GetLinePosize(int pos)
			{
				if (pos==-1) return ZERO_POSIZE;
				else return Posize(gPS.x,gPS.y+pos*(EachHeight+Interval),gPS.w,EachHeight);
			}
			
			inline int GetLineFromPos(int y)
			{
				int re=(Win->NowPos().y-gPS.y)/(EachHeight+Interval);
				if ((y-gPS.y)%(EachHeight+Interval)<EachHeight&&InRange(re,0,ListCnt-1))
					return re;
				else return -1;
			}
			
				 
			virtual void CheckEvent()
			{
				
			}
			
			virtual void CheckPos()
			{
				if (ListCnt==0||!Win->IsNeedSolvePosEvent()) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
							{
								stat=0;
								Win->SetPresentArea(GetLinePosize(FocusChoose));
								FocusChoose=-1;
								RemoveNeedLoseFocus();
								Win->SetNeedFreshScreen();
							}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (ClickChoose!=-1)
							Win->SetPresentArea(GetLinePosize(ClickChoose));
						if (FocusChoose!=-1)
							Win->SetPresentArea(GetLinePosize(FocusChoose));
						ClickChoose=FocusChoose=GetLineFromPos(Win->NowPos().y);
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							if (event.button.clicks==2)
								stat=2;
							else stat=4;
						}
						else if (event.button.button==SDL_BUTTON_RIGHT)
							stat=3;
						else stat=4,PUI_DD[1]<<"SimpleListView: Unknown click button,use it as left click once"<<endl;
						Win->StopSolvePosEvent();
						Win->SetNeedFreshScreen();
						if (ClickChoose!=-1)
							Win->SetPresentArea(GetLinePosize(ClickChoose));
						if (FocusChoose!=-1)
							Win->SetPresentArea(GetLinePosize(FocusChoose));
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							PUI_DD[0]<<"SimpleListView "<<ID<<" func "<<ClickChoose<<" "<<(ClickChoose!=-1?Text[ClickChoose]:"")<<endl;
							if (func!=NULL)
								func(ClickChoose!=-1?FuncData[ClickChoose]:BackgroundFuncData,ClickChoose,stat==4?1:stat);
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
						}
						break;
					
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
						}
						else
						{
							int pos=(Win->NowPos().y-gPS.y)/(EachHeight+Interval);
							if ((Win->NowPos().y-gPS.y)%(EachHeight+Interval)<EachHeight&&InRange(pos,0,ListCnt-1))
							{
								if (pos!=FocusChoose)
								{
									if (FocusChoose!=-1)
										Win->SetPresentArea(Posize(gPS.x,gPS.y+FocusChoose*(EachHeight+Interval),gPS.w,EachHeight));
									Win->SetPresentArea(Posize(gPS.x,gPS.y+pos*(EachHeight+Interval),gPS.w,EachHeight));
									FocusChoose=pos;
									Win->SetNeedFreshScreen();
								}
							}
							else
								if (FocusChoose!=-1)
								{
									Win->SetPresentArea(Posize(gPS.x,gPS.y+FocusChoose*(EachHeight+Interval),gPS.w,EachHeight));
									FocusChoose=-1;
									Win->SetNeedFreshScreen();
								}
						}
						Win->StopSolvePosEvent();
						break;	
				}
			}

			virtual void Show(Posize &lmt)
			{
				if (ListCnt==0) return;
				int ForL=(-fa->GetrPS().y)/(EachHeight+Interval),
					ForR=ForL+fa2->GetgPS().h/(EachHeight+Interval)+1;
				ForL=EnsureInRange(ForL,0,ListCnt-1);
				ForR=EnsureInRange(ForR,0,ListCnt-1);
				if (!InRange(ClickChoose,0,ListCnt-1)) ClickChoose=-1;
				if (!InRange(FocusChoose,0,ListCnt-1)) FocusChoose=-1;
//				PUI_DD[3]<<"SLV "<<ForL<<" "<<ForR<<" "<<ClickChoose<<" "<<FocusChoose<<endl;
				Posize RowPs=Posize(gPS.x,gPS.y+ForL*(EachHeight+Interval),gPS.w,EachHeight);
				for (int i=ForL;i<=ForR;RowPs.y+=EachHeight+Interval,++i)
				{
					Win->RenderFillRect(RowPs&lmt,RowColor[ClickChoose==i?2:FocusChoose==i]?RowColor[ClickChoose==i?2:FocusChoose==i]:ThemeColor[ClickChoose==i?4:(FocusChoose==i?2:0)]);
					Win->RenderDrawText(Text[i],TextMode==-1?RowPs+Point(8,0):(TextMode==1?RowPs-Point(24,0):RowPs),lmt,TextMode,TextColor?TextColor:ThemeColor.MainTextColor[0]);
					Win->Debug_DisplayBorder(RowPs);
				}	
				Win->Debug_DisplayBorder(gPS);
			}

			virtual void CalcPsEx()//??
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)	
					PsEx->GetrPS(rPS);
				rPS.w=fa2->GetrPS().w;
				rPS.h=EnsureInRange(ListCnt*(EachHeight+Interval)-Interval,fa2->GetrPS().h,1e9);
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				fa2->ResizeLL(rPS.w,rPS.h);
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}
			
		public:
			inline void SetTextColor(const RGBA &co)
			{
				TextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}

			inline void SetRowColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					RowColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"SimpleListView: SetRowColor: p "<<p<<" is not in range[0,2]"<<endl;
			}			
			
			void SetBackGroundColor(const RGBA &co)
			{fa2->SetLargeAreaColor(co);}

			inline void SetRowHeightAndInterval(int _height,int _interval)
			{
				EachHeight=_height;
				Interval=_interval;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}
			
			inline void SetTextMode(int mode)
			{
				TextMode=mode;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetListFunc(void (*_func)(T&,int,int))
			{func=_func;}
			
			void SetListContent(int p,const string &str,const T &_funcdata)//p: 0<=p<ListCnt:SetInP >=ListCnt:SetInLast <0:SetInFirst
			{
				p=EnsureInRange(p,0,ListCnt);
				Text.insert(Text.begin()+p,str);
				FuncData.insert(FuncData.begin()+p,_funcdata);
				++ListCnt;
				if (ClickChoose>=p) ++ClickChoose;
				rPS.h=EnsureInRange(ListCnt*(EachHeight+Interval)-Interval,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
//				CalcPsEx();//??
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);//?? It don't need fresh such big area
			}
			
			void DeleteListContent(int p)//p: 0<=p<ListCnt:SetInP >=ListCnt:DeleteLast <0:DeleteFirst
			{
				if (!ListCnt) return;
				p=EnsureInRange(p,0,ListCnt-1);
				Text.erase(Text.begin()+p);
				FuncData.erase(FuncData.begin()+p);
				--ListCnt;
				if (FocusChoose==ListCnt)
					FocusChoose=-1;
				if (ClickChoose>p) --ClickChoose;
				else if (ClickChoose==p) ClickChoose=-1;
				rPS.h=EnsureInRange(ListCnt*(EachHeight+Interval)-Interval,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
//				CalcPsEx();
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void ClearListContent()
			{
				if (!ListCnt) return;
				PUI_DD[0]<<"Clear SimpleListViewContent: ListCnt:"<<ListCnt<<endl;
				FocusChoose=ClickChoose=-1;
				Text.clear();
				FuncData.clear();
				ListCnt=0;
				rPS.h=fa2->GetrPS().h;
				fa2->ResizeLL(0,rPS.h);
				fa2->SetViewPort(1,0);//??
				fa2->SetViewPort(2,0);
//				CalcPsEx();
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			SimpleListView* PushbackContent(const string &str,const T &_funcdata)
			{
				SetListContent(1e9,str,_funcdata);
				return this;
			}
			
			int Find(const T &_funcdata)
			{
				for (int i=0;i<ListCnt;++i)
					if (FuncData[i]==_funcdata)
						return i;
				return -1;
			}
			
			void SetText(int p,const string &str)
			{
				p=EnsureInRange(p,0,ListCnt-1);
				Text[p]=str;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS&GetLinePosize(p));
			}
			
			void SetSelectLine(int p)
			{
				if (!InRange(p,-1,ListCnt-1))
					return;
				if (p==-1)
					Win->SetPresentArea(GetLinePosize(ClickChoose)&CoverLmt);
				else if (!InRange(p*(Interval+EachHeight)+fa->GetrPS().y,0,fa2->GetrPS().h-Interval-EachHeight))
					fa2->SetViewPort(2,p*(Interval+EachHeight));
				ClickChoose=p;
				Win->SetNeedFreshScreen();
			}
			
			inline int GetListCnt()
			{return ListCnt;}
			
			T& GetFuncData(int p)
			{
				p=EnsureInRange(p,0,ListCnt-1);
				return FuncData[p];
			}
			
			T& operator [] (int p)
			{
				p=EnsureInRange(p,0,ListCnt-1);
				return FuncData[p];
			}
			
			void SetBackgroundFuncData(const T &data)
			{BackgroundFuncData=data;}
			
			void AddPsEx(PosizeEX *psex)//?? Need virtual?
			{fa2->AddPsEx(psex);}
			
			virtual void SetrPS(const Posize &ps)
			{fa2->SetrPS(ps);}
			
			SimpleListView(int _ID,Widgets *_fa,const Posize &_rps,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SimpleListView "<<ID<<endl;
				Type=WidgetType_SimpleListView;
				fa2=new LargeLayerWithScrollBar(0,_fa,_rps,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS={0,0,_rps.w,0};
				for (int i=0;i<=2;++i)
					RowColor[i]=RGBA_NONE;
			}
			
			SimpleListView(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SimpleListView "<<ID<<endl;
				Type=WidgetType_SimpleListView;
				fa2=new LargeLayerWithScrollBar(0,_fa,psex,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS=ZERO_POSIZE;
				for (int i=0;i<=2;++i)
					RowColor[i]=RGBA_NONE;
			}
	};
	
	template <class T> class SimpleListView_MultiColor:public SimpleListView <T> 
	{
		public:
			struct EachRowColor0
			{
				RGBA row[3],
					 text=RGBA_NONE;
					
				EachRowColor0(const RGBA &row0,const RGBA &row1,const RGBA &row2,const RGBA &_text=RGBA_NONE)
				:row{row0,row1,row2},text(_text) {}
				
				EachRowColor0():row{RGBA_NONE,RGBA_NONE,RGBA_NONE} {}
			};
			static EachRowColor0 EachRowColor_NONE;
		
		protected:
			vector <EachRowColor0> EachRowColor;
			
			virtual void Show(Posize &lmt)
			{
				if (this->ListCnt==0) return;//Oh ! Too many this pointers! >_<
				int ForL=(-this->fa->GetrPS().y)/(this->EachHeight+this->Interval),
					ForR=ForL+this->fa2->GetgPS().h/(this->EachHeight+this->Interval)+1;
				ForL=EnsureInRange(ForL,0,this->ListCnt-1);
				ForR=EnsureInRange(ForR,0,this->ListCnt-1);
				if (!InRange(this->ClickChoose,0,this->ListCnt-1)) this->ClickChoose=-1;
				if (!InRange(this->FocusChoose,0,this->ListCnt-1)) this->FocusChoose=-1;
				Posize RowPs=Posize(this->gPS.x,this->gPS.y+ForL*(this->EachHeight+this->Interval),this->gPS.w,this->EachHeight);
				for (int i=ForL;i<=ForR;RowPs.y+=this->EachHeight+this->Interval,++i)
				{
					this->Win->RenderFillRect(RowPs&lmt,this->EachRowColor[i].row[this->ClickChoose==i?2:this->FocusChoose==i]?this->EachRowColor[i].row[this->ClickChoose==i?2:this->FocusChoose==i]:
						(this->RowColor[this->ClickChoose==i?2:this->FocusChoose==i]?this->RowColor[this->ClickChoose==i?2:this->FocusChoose==i]:ThemeColor[this->ClickChoose==i?4:(this->FocusChoose==i?2:0)]));//so long! *_*
					this->Win->RenderDrawText(this->Text[i],this->TextMode==-1?RowPs+Point(8,0):(this->TextMode==1?RowPs-Point(24,0):RowPs),lmt,this->TextMode,this->EachRowColor[i].text?this->EachRowColor[i].text:(this->TextColor?this->TextColor:ThemeColor.MainTextColor[0]));
					this->Win->Debug_DisplayBorder(RowPs);
				}	
				this->Win->Debug_DisplayBorder(this->gPS);
			}
		
		public:
			void SetListContent(int p,const string &str,const T &_funcdata,const EachRowColor0 &co=EachRowColor_NONE)
			{
				p=EnsureInRange(p,0,this->ListCnt);
				EachRowColor.insert(EachRowColor.begin()+p,co);
				SimpleListView <T>::SetListContent(p,str,_funcdata);
			}
			
			SimpleListView_MultiColor* PushbackContent(const string &str,const T &_funcdata,const EachRowColor0 &co=EachRowColor_NONE)
			{
				SetListContent(1e9,str,_funcdata,co);
				return this;
			}
			
			void DeleteListContent(int p)
			{
				if (!this->ListCnt) return;
				p=EnsureInRange(p,0,this->ListCnt-1);
				EachRowColor.erase(EachRowColor.begin()+p);
				SimpleListView <T>::DeleteListContent(p);
			}

			void ClearListContent()
			{
				if (!this->ListCnt) return;
				PUI_DD[0]<<"Clear SimpleListView_MultiColor_Content: ListCnt:"<<this->ListCnt<<endl;
				EachRowColor.clear();
				SimpleListView <T>::ClearListContent();
			}
			
			void SetRowColor(int p,const EachRowColor0 &co=EachRowColor_NONE)
			{
				p=EnsureInRange(p,0,this->ListCnt-1);
				EachRowColor[p]=co;
				this->Win->SetNeedFreshScreen();
				this->Win->SetPresentArea(this->gPS&this->GetLinePosize(p));
			}
			
			void SetDefaultColor(const EachRowColor0 &co=EachRowColor_NONE)
			{
				for (int i=0;i<=2;++i)
					SimpleListView <T>::SetRowColor(i,co.row[i]);
				SimpleListView <T>::SetTextColor(co.text);
			}
			
			SimpleListView_MultiColor(int _ID,Widgets *_fa,const Posize &_rps,void (*_func)(T&,int,int)=NULL)
			:SimpleListView <T>(_ID,_fa,_rps,_func)
			{
				PUI_DD[0]<<"Create SimpleListView_MultiColor "<<this->ID<<endl;
				this->Type=Widgets::WidgetType_SimpleListView_MultiColor;
			}
			
			SimpleListView_MultiColor(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(T&,int,int)=NULL)
			:SimpleListView <T>(_ID,_fa,psex,_func)
			{
				PUI_DD[0]<<"Create SimpleListView_MultiColor "<<this->ID<<endl;
				this->Type=Widgets::WidgetType_SimpleListView_MultiColor;
			}
	};
	template <class T> typename SimpleListView_MultiColor<T>::EachRowColor0 SimpleListView_MultiColor<T>::EachRowColor_NONE;
	
	template <class T> class SimpleBlockView:public Widgets
	{
		public:
			struct BlockViewData
			{
				string MainText,
					   SubText1,
					   SubText2;
				T FuncData;
				SharedTexturePtr pic;
				RGBA MainTextColor=RGBA_NONE,
				 	 SubTextColor1=RGBA_NONE,
				 	 SubTextColor2=RGBA_NONE,
					 BlockColor[3];//NoFocusRow,FocusBlock,ClickBlock
				
				BlockViewData(const T &funcdata,const string &maintext,const string &subtext1,const string &subtext2,const SharedTexturePtr &_pic)
				:FuncData(funcdata),MainText(maintext),SubText1(subtext1),SubText2(subtext2),pic(_pic)
				{
					for (int i=0;i<=2;++i)
						BlockColor[i]=RGBA_NONE;
				}
				
				BlockViewData()
				{
					for (int i=0;i<=2;++i)
						BlockColor[i]=RGBA_NONE;
				}
			};
			
		protected:
			int BlockCnt=0;
			vector <BlockViewData> BlockData;
			void (*func)(T&,int,int)=NULL;//int1:Pos(CountFrom 0,-1 means background)   int2: 0:None 1:Left_Click 2:Left_Double_Click 3:Right_Click
			T BackgroundFuncData;
			LargeLayerWithScrollBar *fa2=NULL;
			int stat=0,//0:Up_NoFocus 1:Up_Focus_Row 2:Down_Left_TwiceClick 3:Down_Right 4:Down_Left_OnceClick
				FocusChoose=-1,
				ClickChoose=-1,
				EachLineBlocks=1;//update this after changing rPS or EachPs
			Posize EachPs={5,5,240,80};//min(w,h) is the pic edge length, if w>=h ,the text will be show on right,else on buttom
			RGBA MainTextColor=RGBA_NONE,//These colors are default color
				 SubTextColor1=RGBA_NONE,
				 SubTextColor2=RGBA_NONE,
				 BlockColor[3];//NoFocusRow,FocusBlock,ClickBlock
			bool EnablePic=1,
				 EnableCheckBox=0,
				 EnableDrag=0,
				 DisableKeyboardEvent=0;
			
			Posize GetBlockPosize(int p)//get specific Block Posize
			{
				if (p==-1) return ZERO_POSIZE;
				return Posize(gPS.x+p%EachLineBlocks*(EachPs.x+EachPs.w),gPS.y+p/EachLineBlocks*(EachPs.y+EachPs.h),EachPs.w,EachPs.h);
			}

			int InBlockPosize(const Point &pt)//return In which Posize ,if not exist,return -1
			{
				int re=(pt.y-gPS.y+EachPs.y)/(EachPs.y+EachPs.h)*EachLineBlocks+(pt.x-gPS.x+EachPs.x)/(EachPs.x+EachPs.w);
				if (InRange(re,0,BlockCnt-1)&&(GetBlockPosize(re)&CoverLmt).In(pt))
					return re;
				else return -1;					
			}
			
			virtual void CheckEvent()
			{
				if (BlockCnt==0||DisableKeyboardEvent) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_KEYDOWN:
						int MoveSelectPosDelta=0;
						switch (event.key.keysym.sym)
						{
							case SDLK_RETURN:
								if (InRange(ClickChoose,0,BlockCnt-1))
								{
//									if (...)//Mod??
									PUI_DD[0]<<"SimpleBlockView "<<ID<<": func "<<ClickChoose<<" "<<BlockData[ClickChoose].MainText<<endl;
									if (func!=NULL)
										func(BlockData[ClickChoose].FuncData,ClickChoose,2);
									Win->StopSolveEvent();
								}
								break;
							case SDLK_LEFT:
								if (MoveSelectPosDelta==0)
									MoveSelectPosDelta=-1;
							case SDLK_RIGHT:
								if (MoveSelectPosDelta==0)
									MoveSelectPosDelta=1;
							case SDLK_UP:
								if (MoveSelectPosDelta==0)
									MoveSelectPosDelta=-EachLineBlocks;
							case SDLK_DOWN:
								if (MoveSelectPosDelta==0)
									MoveSelectPosDelta=EachLineBlocks;
								if (ClickChoose!=-1)
								{
									Win->SetPresentArea(GetBlockPosize(ClickChoose));
									ClickChoose=EnsureInRange(ClickChoose+MoveSelectPosDelta,0,BlockCnt-1);
									if (ClickChoose/EachLineBlocks*(EachPs.y+EachPs.h)+fa->GetrPS().y<0)
										fa2->SetViewPort(2,ClickChoose/EachLineBlocks*(EachPs.y+EachPs.h));
									else if (ClickChoose/EachLineBlocks*(EachPs.y+EachPs.h)+fa->GetrPS().y>fa2->GetrPS().h-EachPs.y-EachPs.h)
										fa2->SetViewPort(2,(ClickChoose/EachLineBlocks+1)*(EachPs.y+EachPs.h)-fa2->GetrPS().h);
								}
								else ClickChoose=0;
								Win->SetPresentArea(GetBlockPosize(ClickChoose));
								Win->SetNeedFreshScreen();
								Win->StopSolveEvent();
								break;
						}
				}
			}
			
			virtual void CheckPos()
			{
				if (/*BlockCnt==0||*/!Win->IsNeedSolvePosEvent()) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
							{
								stat=0;
								Win->SetPresentArea(GetBlockPosize(FocusChoose));
								FocusChoose=-1;
								RemoveNeedLoseFocus();
								Win->SetNeedFreshScreen();
							}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						Win->SetPresentArea(GetBlockPosize(ClickChoose));
						Win->SetPresentArea(GetBlockPosize(FocusChoose));
						ClickChoose=FocusChoose=InBlockPosize(Win->NowPos());
						Win->SetPresentArea(GetBlockPosize(ClickChoose));
						Win->SetPresentArea(GetBlockPosize(FocusChoose));
//						if (ClickChoose!=-1)
//						{
							if (event.button.button==SDL_BUTTON_LEFT)
							{
								if (event.button.clicks==2)
									stat=2;
								else stat=4;
							}
							else if (event.button.button==SDL_BUTTON_RIGHT)
								stat=3;
							else stat=4,PUI_DD[1]<<"SimpleBlockView "<<ID<<": Unknown click button,use it as left click once"<<endl;
//						}
						Win->StopSolvePosEvent();
						Win->SetNeedFreshScreen();
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							if (ClickChoose>=0)
								PUI_DD[0]<<"SimpleBlockView "<<ID<<": func "<<ClickChoose<<" "<<BlockData[ClickChoose].MainText<<endl;
							if (func!=NULL)
								func(ClickChoose==-1?BackgroundFuncData:BlockData[ClickChoose].FuncData,ClickChoose,stat==4?1:stat);
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
						}
						break;
					
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							FocusChoose=InBlockPosize(Win->NowPos());
							Win->SetPresentArea(GetBlockPosize(FocusChoose));
							Win->SetNeedFreshScreen();
						}
						else
						{
							int pos=InBlockPosize(Win->NowPos());
							if (pos!=FocusChoose)
							{
								Win->SetPresentArea(GetBlockPosize(FocusChoose));
								Win->SetPresentArea(GetBlockPosize(pos));
								FocusChoose=pos;
								Win->SetNeedFreshScreen();
							}	
						}
						Win->StopSolvePosEvent();
						break;	
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				if (BlockCnt==0) return;
				int ForL=-fa->GetrPS().y/(EachPs.y+EachPs.h)*EachLineBlocks,
					ForR=((-fa->GetrPS().y+fa2->GetgPS().h)/(EachPs.y+EachPs.h)+1)*EachLineBlocks-1;
				ForL=EnsureInRange(ForL,0,BlockCnt-1);
				ForR=EnsureInRange(ForR,0,BlockCnt-1);
//				if (!InRange(ClickChoose,0,ListCnt-1)) ClickChoose=-1;
//				if (!InRange(FocusChoose,0,ListCnt-1)) FocusChoose=-1;

				Posize BlockPs=GetBlockPosize(ForL);
				for (int i=ForL;i<=ForR;++i)
				{
					Win->RenderFillRect(BlockPs&lmt,BlockData[i].BlockColor[ClickChoose==i?2:FocusChoose==i]?BlockData[i].BlockColor[ClickChoose==i?2:FocusChoose==i]:(BlockColor[ClickChoose==i?2:FocusChoose==i]?BlockColor[ClickChoose==i?2:FocusChoose==i]:ThemeColor[ClickChoose==i?4:(FocusChoose==i?2:0)]));
					
					if (EnablePic&&BlockData[i].pic()!=NULL)
					{
						Win->RenderCopyWithLmt(BlockData[i].pic(),Posize(BlockPs.x,BlockPs.y,min(EachPs.w,EachPs.h),min(BlockPs.w,BlockPs.h)),lmt);
						Win->Debug_DisplayBorder(Posize(BlockPs.x,BlockPs.y,min(EachPs.w,EachPs.h),min(BlockPs.w,BlockPs.h)));
					}
					
					if (EnablePic)
						if (EachPs.w>=EachPs.h)
						{
							Win->RenderDrawText(BlockData[i].MainText,Posize(BlockPs.x+BlockPs.h+8,BlockPs.y,BlockPs.w-BlockPs.h-8,BlockPs.h/3),lmt,-1,BlockData[i].MainTextColor?BlockData[i].MainTextColor:(MainTextColor?MainTextColor:ThemeColor.MainTextColor[0]));
							Win->RenderDrawText(BlockData[i].SubText1,Posize(BlockPs.x+BlockPs.h+8,BlockPs.y+EachPs.h/3,BlockPs.w-BlockPs.h-8,BlockPs.h/3),lmt,-1,BlockData[i].SubTextColor1?BlockData[i].SubTextColor1:(SubTextColor1?SubTextColor1:ThemeColor.BackgroundColor[6]));
							Win->RenderDrawText(BlockData[i].SubText2,Posize(BlockPs.x+BlockPs.h+8,BlockPs.y+EachPs.h/3*2,BlockPs.w-BlockPs.h-8,BlockPs.h/3),lmt,-1,BlockData[i].SubTextColor2?BlockData[i].SubTextColor2:(SubTextColor2?SubTextColor2:ThemeColor.BackgroundColor[6]));
						}
						else Win->RenderDrawText(BlockData[i].MainText,Posize(BlockPs.x,BlockPs.y+EachPs.w,BlockPs.w,BlockPs.h-BlockPs.w),lmt,0,BlockData[i].MainTextColor?BlockData[i].MainTextColor:(MainTextColor?MainTextColor:ThemeColor.MainTextColor[0]));
					else
					{
						Win->RenderDrawText(BlockData[i].MainText,Posize(BlockPs.x+8,BlockPs.y,BlockPs.w-8,BlockPs.h/3),lmt,-1,BlockData[i].MainTextColor?BlockData[i].MainTextColor:(MainTextColor?MainTextColor:ThemeColor.MainTextColor[0]));
						Win->RenderDrawText(BlockData[i].SubText1,Posize(BlockPs.x+8,BlockPs.y+EachPs.h/3,BlockPs.w-8,BlockPs.h/3),lmt,-1,BlockData[i].SubTextColor1?BlockData[i].SubTextColor1:(SubTextColor1?SubTextColor1:ThemeColor.BackgroundColor[6]));
						Win->RenderDrawText(BlockData[i].SubText2,Posize(BlockPs.x+8,BlockPs.y+EachPs.h/3*2,BlockPs.w-8,BlockPs.h/3),lmt,-1,BlockData[i].SubTextColor2?BlockData[i].SubTextColor2:(SubTextColor2?SubTextColor2:ThemeColor.BackgroundColor[6]));
					}
					
 					Win->Debug_DisplayBorder(BlockPs);
					
					if ((i+1)%EachLineBlocks==0)
						BlockPs.y+=EachPs.y+EachPs.h,BlockPs.x=gPS.x;
					else BlockPs.x+=EachPs.x+EachPs.w;
				}
				
				Win->Debug_DisplayBorder(gPS);
			}

			virtual void CalcPsEx()
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)	
					PsEx->GetrPS(rPS);
				rPS.w=fa2->GetrPS().w;
				EachLineBlocks=max(1,(rPS.w+EachPs.x)/(EachPs.x+EachPs.w));
				rPS.h=EnsureInRange(ceil(BlockCnt*1.0/EachLineBlocks)*(EachPs.y+EachPs.h)-EachPs.y,fa2->GetrPS().h,1e9);
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				fa2->ResizeLL(rPS.w,rPS.h);
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}
			
		public:
			inline void SetMainTextColor(const RGBA &co)
			{
				MainTextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetSubTextColor1(const RGBA &co)
			{
				SubTextColor1=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetSubTextColor2(const RGBA &co)
			{
				SubTextColor2=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}

			inline void SetBlockColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					BlockColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"SimpleBlockView "<<ID<<": SetBlockColor: p "<<p<<" is not in range[0,2]"<<endl;
			}			
			
			void SetBackGroundColor(const RGBA &co)
			{fa2->SetLargeAreaColor(co);}

			void SetEachBlockPosize(const Posize &ps)
			{
				EachPs=ps;//Other data would be claculated in CalcPsex (?)
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}

			inline void SetBlockFunc(void (*_func)(T&,int,int))
			{func=_func;}
			
			void SetBlockContent(int p,const BlockViewData &data)//p: 0<=p<ListCnt:SetInP >=ListCnt:SetInLast <0:SetInFirst
			{
				p=EnsureInRange(p,0,BlockCnt);
				BlockData.insert(BlockData.begin()+p,data);
				++BlockCnt;
				if (ClickChoose>=p) ++ClickChoose;
				rPS.h=EnsureInRange(ceil(BlockCnt*1.0/EachLineBlocks)*(EachPs.y+EachPs.h)-EachPs.y,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);//?? It don't need fresh such big area
			}
			
			void DeleteBlockContent(int p)//p: 0<=p<ListCnt:SetInP >=ListCnt:DeleteLast <0:DeleteFirst
			{
				if (!BlockCnt) return;
				p=EnsureInRange(p,0,BlockCnt-1);
				BlockData.erase(BlockData.begin()+p);
				--BlockCnt;
				if (FocusChoose==BlockCnt) FocusChoose=-1;
				if (ClickChoose>p) --ClickChoose;
				else if (ClickChoose==p) ClickChoose=-1;
				rPS.h=EnsureInRange(ceil(BlockCnt*1.0/EachLineBlocks)*(EachPs.y+EachPs.h)-EachPs.y,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void ClearBlockContent()
			{
				if (!BlockCnt) return;
				PUI_DD[0]<<"Clear SimpleBlockView "<<ID<<" content: BlockCnt:"<<BlockCnt<<endl;
				FocusChoose=ClickChoose=-1;
				BlockData.clear();
				BlockCnt=0;
				rPS.h=fa2->GetrPS().h;
				fa2->ResizeLL(0,rPS.h);
				fa2->SetViewPort(1,0);
				fa2->SetViewPort(2,0);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetSelectBlock(int p)
			{
				if (!InRange(p,0,BlockCnt-1))
					return;
				ClickChoose=p;
				if (!InRange(p/EachLineBlocks*(EachPs.y+EachPs.h)+fa->GetrPS().y,0,fa2->GetrPS().h-EachPs.y-EachPs.h))
					fa2->SetViewPort(2,p/EachLineBlocks*(EachPs.y+EachPs.h));
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline int GetCurrentSelectBlock()
			{return ClickChoose;}
			
			SimpleBlockView* PushbackContent(const BlockViewData &data)
			{
				SetBlockContent(1e9,data);
				return this;
			}
			
			inline void SetBackgroundFuncData(const T &data)
			{BackgroundFuncData=data;}
			
			inline int GetBlockCnt()
			{return BlockCnt;}
			
			inline bool Empty()
			{return BlockCnt==0;}
			
			int Find(const T &_funcdata)
			{
				for (int i=0;i<BlockCnt;++i)
					if (BlockData[i].FuncData==_funcdata)
						return i;
				return -1;
			}
			
			BlockViewData& GetBlockData(int p)
			{
				p=EnsureInRange(p,0,BlockCnt);
				return BlockData[p];
			}
			
			T& GetFuncData(int p)
			{
				p=EnsureInRange(p,0,BlockCnt-1);
				return BlockData[p].FuncData;
			}
			
			inline T& GetBackgroundFuncData()
			{return BackgroundFuncData;}
			
			T& operator [] (int p)
			{
				p=EnsureInRange(p,0,BlockCnt-1);
				return BlockData[p].FuncData;
			}
			
			void SetUpdateBlock(int p)
			{
				if (!InRange(p,0,BlockCnt-1))
					return;
				Posize bloPs=GetBlockPosize(p);
				if ((bloPs&CoverLmt).Size()!=0)
				{
					Win->SetPresentArea(bloPs);
					Win->SetNeedFreshScreen();
				}
			}
			
			Range GetCurrentShowRange()
			{
				Range re;
				re.l=-fa->GetrPS().y/(EachPs.y+EachPs.h)*EachLineBlocks;
				re.r=((-fa->GetrPS().y+fa2->GetgPS().h)/(EachPs.y+EachPs.h)+1)*EachLineBlocks-1;
				re.l=EnsureInRange(re.l,0,BlockCnt-1);
				re.r=EnsureInRange(re.r,0,BlockCnt-1);
				return re;
			}
			
			void AddPsEx(PosizeEX *psex)
			{fa2->AddPsEx(psex);}
			
			virtual void SetrPS(const Posize &ps)
			{fa2->SetrPS(ps);}
			
			void SetEnablePic(bool enable)
			{
				EnablePic=enable;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			LargeLayerWithScrollBar* GetFa2()
			{return fa2;}
			
			inline void SetDisableKeyboardEvent(bool disable)
			{DisableKeyboardEvent=disable;}
			
			SimpleBlockView(int _ID,Widgets *_fa,const Posize &_rps,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SimpleBlockView "<<ID<<endl;
				Type=WidgetType_SimpleBlockView;
				fa2=new LargeLayerWithScrollBar(0,_fa,_rps,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS={0,0,_rps.w,0};
				for (int i=0;i<=2;++i)
					BlockColor[i]=RGBA_NONE;
			}
			
			SimpleBlockView(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SimpleBlockView "<<ID<<endl;
				Type=WidgetType_SimpleBlockView;
				fa2=new LargeLayerWithScrollBar(0,_fa,psex,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS=ZERO_POSIZE;
				for (int i=0;i<=2;++i)
					BlockColor[i]=RGBA_NONE;
			}
	};
	
	template <class T> class SimpleTreeView1:public Widgets//It is not designed so good...
	{
		public:
			struct TreeViewData
			{
				friend SimpleTreeView1;
				protected:
					int dep=0;
					bool unfolded=0;
					int subTreeSize=1;
				
				public:
					bool unfoldble=0;
					string text;
					T FuncData;
					SharedTexturePtr pic;
					RGBA textColor=RGBA_NONE,
						 NodeColor[3];
						 
					inline int GetDep()
					{return dep;}
					
					inline int GetSubTreeSize()
					{return subTreeSize;}
					
					TreeViewData(const string &_text,const T &_funcdata,bool _unfoldble,const SharedTexturePtr &_pic=SharedTexturePtr(NULL))
					:text(_text),FuncData(_funcdata),pic(_pic),unfoldble(_unfoldble)
					{
						for (int i=0;i<=2;++i)
							NodeColor[i]=RGBA_NONE;
					}
					
					TreeViewData()
					{
						for (int i=0;i<=2;++i)
							NodeColor[i]=RGBA_NONE;
					}
			};
			
		protected:
			vector <TreeViewData> NodesData;
			void (*func)(T&,SimpleTreeView1<T>*,int,int)=NULL;//funcdata,this,click_type(stat),pos
			LargeLayerWithScrollBar *fa2=NULL;
			int stat=0,//0:Up_NoFocus 1:Up_Focus_Node 2:Down_Left_TwiceClick 3:Down_Right 4:Down_Left_OnceClick 5:click unfold
				NodeCnt=0,
				ClickChoose=-1,
				FocusChoose=-1;
			int EachHeight=30,
				TabWidth=10;
			bool ShowAlign=0;
			RGBA textColor=RGBA_NONE,
				 NodeColor[3];
			SharedTexturePtr TriTex[6];

			Posize GetNodePosize(int pos)
			{
				if (!InRange(pos,0,NodeCnt-1)) return ZERO_POSIZE;
				else return Posize(gPS.x,gPS.y+pos*EachHeight,gPS.w,EachHeight);
			}
			
			int GetNodeFromPos(int y)
			{
				int re=(y-gPS.y)/EachHeight;
				if (!InRange(re,0,NodeCnt-1))
					re=-1;
				return re;
			}
			
			void GetTriTex(int p)
			{
				
			}

			virtual void CheckEvent()
			{
				
			}
			
			virtual void CheckPos()
			{
				if (NodeCnt==0) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
							{
								stat=0;
								if (FocusChoose!=-1)
									Win->SetPresentArea(GetNodePosize(FocusChoose));
								FocusChoose=-1;
								RemoveNeedLoseFocus();
								Win->SetNeedFreshScreen();
							}
					return;
				}
				
				switch (event.type)
				{	
					case SDL_MOUSEBUTTONDOWN:
						if (ClickChoose!=-1)
							Win->SetPresentArea(GetNodePosize(ClickChoose));
						if (FocusChoose!=-1)
							Win->SetPresentArea(GetNodePosize(FocusChoose));
						ClickChoose=FocusChoose=GetNodeFromPos(Win->NowPos().y);
						if (ClickChoose!=-1)
							if (event.button.button==SDL_BUTTON_LEFT)
								if (NodesData[ClickChoose].unfoldble&&InRange(Win->NowPos().x,NodesData[ClickChoose].dep*TabWidth,NodesData[ClickChoose].dep*TabWidth+EachHeight))
									stat=5;
								else if (event.button.clicks==2)
									stat=2;
								else stat=4;
							else if (event.button.button==SDL_BUTTON_RIGHT)
								stat=3;
							else stat=4,PUI_DD[1]<<"SimpleTreeView1: Unknown click button,use it as left click once"<<endl;
						Win->StopSolvePosEvent();
						Win->SetNeedFreshScreen();
						if (ClickChoose!=-1)
							Win->SetPresentArea(GetNodePosize(ClickChoose));
						if (FocusChoose!=-1)
							Win->SetPresentArea(GetNodePosize(FocusChoose));
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							PUI_DD[0]<<"SimpleTreeView1 "<<ID<<" func "<<stat<<" "<<ClickChoose<<" "<<NodesData[ClickChoose].text<<endl;
							if (func!=NULL)
								func(NodesData[ClickChoose].FuncData,this,stat,ClickChoose);
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(GetNodePosize(ClickChoose));
						}
						break;
					
					case SDL_MOUSEMOTION:
						int pos=GetNodeFromPos(Win->NowPos().y);
						if (pos!=-1)
						{
							if (stat==0)
							{
								stat=1;
								SetNeedLoseFocus();
							}
							else if (pos!=FocusChoose)
								if (FocusChoose!=-1)
									Win->SetPresentArea(GetNodePosize(FocusChoose));
							Win->SetPresentArea(GetNodePosize(pos));
							FocusChoose=pos;
							Win->SetNeedFreshScreen();
						}
						else
							if (FocusChoose!=-1)
							{
								Win->SetPresentArea(GetNodePosize(FocusChoose));
								FocusChoose=-1;
								Win->SetNeedFreshScreen();
							}
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				if (NodeCnt==0) return;
				int ForL=EnsureInRange(-fa->GetrPS().y/EachHeight,0,NodeCnt-1),
					ForR=EnsureInRange(ForL+fa2->GetgPS().h/EachHeight+1,0,NodeCnt-1);
				for (int i=ForL;i<=ForR;++i)
				{
					Posize NodePs=GetNodePosize(i);
					TreeViewData &tvd=NodesData[i];
					int col=ClickChoose==i?2:FocusChoose==i;
					Win->RenderFillRect(NodePs&lmt,!tvd.NodeColor[col]?(!NodeColor[col]?ThemeColor[col*2]:NodeColor[col]):tvd.NodeColor[col]);
					Win->RenderDrawText(tvd.text,Posize(NodePs.x+tvd.dep*TabWidth+EachHeight*2,NodePs.y,NodePs.w-tvd.dep*TabWidth-EachHeight*2,EachHeight),lmt,-1,!tvd.textColor?(!textColor?ThemeColor.MainTextColor[0]:textColor):tvd.textColor);
					if (tvd.unfoldble)
					{
						int p=stat==5?2:FocusChoose==i;
						if (tvd.unfolded)
							p+=3;
						if (!TriTex[p])
							GetTriTex(p);
						Win->RenderCopyWithLmt(TriTex[p].GetPic(),Posize(NodePs.x+tvd.dep*TabWidth,NodePs.y,EachHeight,EachHeight),lmt); 
					}
					if (tvd.pic.GetPic()!=NULL)
						Win->RenderCopyWithLmt(tvd.pic.GetPic(),Posize(NodePs.x+tvd.dep*TabWidth+EachHeight,NodePs.y,EachHeight,EachHeight),lmt);
				}
				Win->Debug_DisplayBorder(gPS);
			}
			
			virtual void CalcPsEx()//??
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)	
					PsEx->GetrPS(rPS);
				rPS.w=fa2->GetrPS().w;
				rPS.h=EnsureInRange(NodeCnt*EachHeight,fa2->GetrPS().h,1e9);
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				fa2->ResizeLL(rPS.w,rPS.h);
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}
			
		public:
			inline void SetTextColor(const RGBA &co)
			{
				textColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}

			inline void SetNodeColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					NodeColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"SimpleTreeView1: SetNodeColor: p "<<p<<" is not in range[0,2]"<<endl;
			}			
			
			void SetBackGroundColor(const RGBA &co)
			{fa2->SetLargeAreaColor(co);}

			inline void SetNodeHeight(int h)
			{
				EachHeight=h;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}
			
			inline void SetTreeViewFunc(void (*_func)(T&,SimpleTreeView1<T>*,int,int))
			{func=_func;}
			
			int GetSubNodeCnt(int faNode,bool recursive=0)
			{
				faNode=EnsureInRange(faNode,-1,NodeCnt-1);
				int re=0,subDep=faNode==-1?0:NodesData[faNode].dep+1;
				for (int i=faNode+1;i<NodeCnt&&NodesData[i].dep>=subDep;++i)
					if (NodesData[i].dep==subDep||recursive&&NodesData[i].dep>subDep)
						++re;
				return re;
			}
			
			int SetSubNode(int faNode,int p,TreeViewData &data)//if faNode==-1 insert in top level(dep 0)
			{
				PUI_DD[0]<<"SimpleTreeView1 "<<ID<<" SetSubNode "<<faNode<<" "<<p<<endl;
				faNode=EnsureInRange(faNode,-1,NodeCnt);
				if (faNode!=-1)
					data.dep=NodesData[faNode].dep+1;
				p=max(p,0);
				for (int i=faNode+1;i<=NodeCnt;++i)
				{
					if (p==0||i==NodeCnt||NodesData[i].dep<data.dep)
					{
						NodesData.insert(NodesData.begin()+i,data);
						++NodeCnt;
						if (ClickChoose>=i) ++ClickChoose;
						rPS.h=EnsureInRange(NodeCnt*EachHeight,fa2->GetrPS().h,1e9);
						fa2->ResizeLL(0,rPS.h);
						Win->SetNeedUpdatePosize();
						Win->SetNeedFreshScreen();
						Win->SetPresentArea(gPS);
						return i;
					}
					if (NodesData[i].dep==data.dep)
						--p;
				}
			}
			
			int PushbackNode(int faNode,TreeViewData &data)
			{return SetSubNode(faNode,1e9,data);}
			
			int SetSubNodeMulti(int faNode,int p,vector <TreeViewData> &data)
			{
				PUI_DD[0]<<"SimpleTreeView1 "<<ID<<" SetSubNodeMulti "<<faNode<<" "<<p<<endl;
				faNode=EnsureInRange(faNode,-1,NodeCnt);
				int dataDep=faNode==-1?0:NodesData[faNode].dep+1;
				if (faNode!=-1)
					for (int i=0;i<data.size();++i)
						data[i].dep=dataDep;
				p=max(p,0);
				for (int i=faNode+1;i<=NodeCnt;++i)
				{
					if (p==0||i==NodeCnt||NodesData[i].dep<dataDep)
					{
						NodesData.insert(NodesData.begin()+i,data.begin(),data.end());
						NodeCnt+=data.size();
						if (ClickChoose>=i) ClickChoose+=data.size();
						rPS.h=EnsureInRange(NodeCnt*EachHeight,fa2->GetrPS().h,1e9);
						fa2->ResizeLL(0,rPS.h);
						Win->SetNeedUpdatePosize();
						Win->SetNeedFreshScreen();
						Win->SetPresentArea(gPS);
						return i;
					}
					if (NodesData[i].dep==dataDep)
						--p;
				}
			}
			
			void DeleteSubNodeMulti(int faNode,int p,int cnt)
			{
				if (NodeCnt==0||cnt<=0) return;
				PUI_DD[0]<<"SimpleTreeView1 "<<ID<<" DeleteSubNodeMulti "<<faNode<<" "<<p<<" "<<cnt<<endl;
				faNode=EnsureInRange(faNode,-1,NodeCnt-1);
				int delL=-1,delR=-1,delDep=faNode==-1?0:NodesData[faNode].dep+1;
				p=max(p,0);
				for (int i=faNode+1;i<NodeCnt;++i)
				{
					if (p==0)
						delL=i;
					if (NodesData[i].dep==delDep)
						if (p==0)
							--cnt;
						else --p;
					else if (NodesData[i].dep<delDep)
						break;
					delR=i;
					if (cnt==0)
						break;
				}
				if (delL!=-1)
				{
					NodesData.erase(NodesData.begin()+delL,NodesData.begin()+delR+1);
					NodeCnt-=delR-delL+1;
					if (FocusChoose>=NodeCnt) FocusChoose=-1;
					if (ClickChoose>=delR+1) ClickChoose-=delR-delL+1;
					else if (ClickChoose>delL) ClickChoose=-1;
					rPS.h=EnsureInRange(NodeCnt*EachHeight,fa2->GetrPS().h,1e9);
					fa2->ResizeLL(0,rPS.h);
					Win->SetNeedUpdatePosize();
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
			}
			
			void DeleteAllSubNode(int faNode)
			{DeleteSubNodeMulti(faNode,0,GetSubNodeCnt(faNode));}
			
			void DeleteSubNode(int faNode,int p)
			{DeleteSubNodeMulti(faNode,p,1);}
			
			void ClearAllNode()
			{
				if (!NodeCnt) return;
				PUI_DD[0]<<"SimpleTreeView1 "<<ID<<" ClearAllNode "<<NodeCnt<<endl;
				FocusChoose=ClickChoose=-1;
				NodesData.clear();
				NodeCnt=0;
				rPS.h=fa2->GetrPS().h;
				fa2->ResizeLL(0,rPS.h);
				fa2->SetViewPort(1,0);
				fa2->SetViewPort(2,0);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetSelectNode(int p)
			{
				if (!InRange(p,0,NodeCnt-1))
					return;
				ClickChoose=p;
				if (!InRange(p*EachHeight+fa->GetrPS().y,0,fa2->GetrPS().h-EachHeight))
					fa2->SetViewPort(2,(p-1)*EachHeight);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			int Find(const T &_funcdata)
			{
				for (int i=0;i<NodeCnt;++i)
					if (NodesData[i].FuncData==_funcdata)
						return i;
				return -1;
			}
			
			TreeViewData& GetNodeData(int p)
			{return NodesData[EnsureInRange(p,0,NodeCnt-1)];}
			
			T& GetFuncData(int p)
			{return NodesData[EnsureInRange(p,0,NodeCnt-1)].FuncData;}
			
			T& operator [] (int p)
			{return NodesData[EnsureInRange(p,0,NodeCnt-1)].FuncData;}
			
			inline int GetNodeCnt()
			{return NodeCnt;}
			
			void AddPsEx(PosizeEX *psex)
			{fa2->AddPsEx(psex);}
			
			virtual void SetrPS(const Posize &ps)
			{fa2->SetrPS(ps);}
			
			SimpleTreeView1(int _ID,Widgets *_fa,const Posize &_rps,void (*_func)(T&,SimpleTreeView1<T>*,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SimpleTreeView1 "<<ID<<endl;
				Type=WidgetType_SimpleTreeView1;
				fa2=new LargeLayerWithScrollBar(0,_fa,_rps,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS={0,0,_rps.w,0};
				for (int i=0;i<=2;++i)
					NodeColor[i]=RGBA_NONE;
			}
			
			SimpleTreeView1(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(T&,SimpleTreeView1<T>*,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SimpleTreeView1 "<<ID<<endl;
				Type=WidgetType_SimpleTreeView1;
				fa2=new LargeLayerWithScrollBar(0,_fa,psex,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS=ZERO_POSIZE;
				for (int i=0;i<=2;++i)
					NodeColor[i]=RGBA_NONE;
			}
	};
	
	template <class T> class DetailedListView:public Widgets
	{
		/*	Example:
				name  |   size   |    date    |    type    |   ....
			1	xxx			xxx			xxx			xxx
			2	xxx			xxx			xxx			xxx
		*/
		public:
			struct DetailedListViewData
			{
//				friend DetailedListView;
//				protected:
					vector <string> Text;
					vector <RGBA> SpecificTextColor;//if Empty,use the defalut Color
					SharedTexturePtr pic;
					T FuncData;
				
				public:
					DetailedListViewData() {}
			};
			
			struct DetailedListViewColumn
			{
//				friend DetailedListView;
				string TagName;
				int TextDisplayMode=0,
					Width=100;
				RGBA ColumnTextColor=RGBA_NONE;
			
				DetailedListViewColumn(const string &tagname,const int &w=100,const int &displaymode=0,const RGBA &co=RGBA_NONE)
				:TagName(tagname),TextDisplayMode(displaymode),Width(w),ColumnTextColor(co) {}
				
				DetailedListViewColumn() {}
			};
		
		protected:
			vector <DetailedListViewColumn> ColumnInfo;
			vector <DetailedListViewData> ListData;
			int ListCnt=0;
			void (*func)(T&,int,int)=NULL;//int1:Pos(CountFrom 0,-1 means background)   int2: 0:None 1:Left_Click 2:Left_Double_Click 3:Right_Click
			T BackgroundFuncData;
//			int (*CompFunc)(const string&,const string&,const string&,int)=NULL;//ItemA,ItemB,TagName,ColumnPos  //<:-1 ==:0 >:1 if ==:Sort by ColumnOrder 
			void (*SortFunc)(vector <DetailedListViewData>&,const string&,int,bool)=NULL;//TagName,ColumnPos,SortDirection
			LargeLayerWithScrollBar *fa2=NULL;
			int stat=0,//0:Up_NoFocus 1:Up_Focus_Row 2:Down_Left_TwiceClick 3:Down_Right 4:Down_Left_OnceClick 
					   //5:Up_Focus_Top 6:Down_LeftClick_Top 7:Down_RightClick_Top 8:Up_Focus_ResizeBar 9:Down_Scroll_ResizeBar
				ColumnAndResizeChoose=-1,
				FocusChoose=-1,
				ClickChoose=-1,
				ResizeColumnLeftX=0;
			int TopHeigth=30,
				TopResizeWidth=2,
				EachHeight=24,
				Interval=2,
				MainTextPos=0,
				SortByColumn=0;
			bool SortDirection=0;//0:small to big 1:big to small
			bool EnableAutoMainTextPos=1,
				 EnableCheckBox=0,
				 EnableDrag=0,
				 EnableKeyboardEvent=0,
				 EnableSerialNumber=0;
			RGBA TopResizeBarColor[3],
				 TopColumnColor[3],
				 EachRowColor[3],
				 MainTextColor=RGBA_NONE,
				 SubTextColor=RGBA_GRAY_8[5];
			
			inline int GetLineFromPos(int y)
			{
				int re=(Win->NowPos().y-gPS.y-TopHeigth)/(EachHeight+Interval);
				if ((y-gPS.y-TopHeigth)%(EachHeight+Interval)<EachHeight&&InRange(re,0,ListCnt-1))
					return re;
				else return -1;
			}
			
			virtual void CheckEvent()
			{
				
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							if (stat<=4)
								Win->SetPresentArea(GetLinePosize(FocusChoose));
							else Win->SetPresentArea(Posize(fa2->GetgPS().x,fa2->GetgPS().y,fa2->GetgPS().w,TopHeigth));
							stat=0;
							FocusChoose=-1;
							ColumnAndResizeChoose=-1;
							RemoveNeedLoseFocus();
							Win->SetNeedFreshScreen();
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (Win->NowPos().y-fa2->GetgPS().y<=TopHeigth)
						{
							stat=5;
							int x=Win->NowPos().x-gPS.x,s=gPS.x;
							if (FocusChoose!=-1)
								Win->SetPresentArea(GetLinePosize(FocusChoose));
							FocusChoose=ColumnAndResizeChoose=-1;
							for (int i=0;i<ColumnInfo.size();x-=ColumnInfo[i].Width,s+=ColumnInfo[i].Width,++i)
								if (x<=ColumnInfo[i].Width-TopResizeWidth)
								{
									ColumnAndResizeChoose=i;
									if (event.button.button==SDL_BUTTON_LEFT)
									{
										stat=6;
										if (SortByColumn==i)
											SortDirection=!SortDirection;
										else SortByColumn=i,SortDirection=0;
										if (EnableAutoMainTextPos)
											MainTextPos=SortByColumn;
									}
									else if (event.button.button==SDL_BUTTON_RIGHT)
										stat=7;
									break;
								}
								else if (x<=ColumnInfo[i].Width)
								{
									stat=9;
									ResizeColumnLeftX=s;
									ColumnAndResizeChoose=i;
									Win->SetOccupyPosWg(this);
									break;
								}
							Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TopHeigth));
						}
						else
						{
							if (ClickChoose!=-1)
								Win->SetPresentArea(GetLinePosize(ClickChoose));
							if (FocusChoose!=-1)
								Win->SetPresentArea(GetLinePosize(FocusChoose));
							ClickChoose=FocusChoose=GetLineFromPos(Win->NowPos().y);
							if (event.button.button==SDL_BUTTON_LEFT)
								if (event.button.clicks==2)
									stat=2;
								else stat=4;
							else if (event.button.button==SDL_BUTTON_RIGHT)
								stat=3;
							else stat=4,PUI_DD[1]<<"DetailedListView: Unknown click button,use it as left click once"<<endl;
							if (ClickChoose!=-1)
								Win->SetPresentArea(GetLinePosize(ClickChoose));
							if (FocusChoose!=-1)
								Win->SetPresentArea(GetLinePosize(FocusChoose));
						}
						Win->StopSolvePosEvent();
						Win->SetNeedFreshScreen();
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							if (stat<=4)
							{
								PUI_DD[0]<<"DetailedListView "<<ID<<" func "<<ClickChoose<<" "<<stat<<endl;
								if (func!=NULL)
									func(ClickChoose==-1?BackgroundFuncData:ListData[ClickChoose].FuncData,ClickChoose,stat==4?1:stat);
								stat=1;
							}
							else if (stat==6)
							{
								PUI_DD[0]<<"DetailedListView "<<ID<<" Sort by "<<ColumnInfo[SortByColumn].TagName<<" "<<SortByColumn<<" "<<SortDirection<<endl;
								if (SortFunc!=NULL)
									SortFunc(ListData,ColumnInfo[SortByColumn].TagName,SortByColumn,SortDirection);
								Win->SetPresentArea(gPS);
								stat=5;
							}
							else if (stat==7)
							{
								PUI_DD[2]<<"DetailedListView "<<ID<<" RightClick ToColumn Cannot be used yet!"<<endl;
								stat=5;
							}
							else if (stat==9)
							{
								stat=8;
								Win->SetOccupyPosWg(NULL);
								Win->SetPresentArea(Posize(gPS.x,gPS.y,gPS.w,TopHeigth));
							}
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
						}
						break;
					
					case SDL_MOUSEMOTION:
						if (stat==9)
						{
							ColumnInfo[ColumnAndResizeChoose].Width=EnsureInRange(Win->NowPos().x-ResizeColumnLeftX,10,1e9);
							Win->SetPresentArea(CoverLmt);
							Win->SetNeedFreshScreen();
							Win->SetNeedUpdatePosize();
						}
						else 
						{
							if (stat==0)
							{
								stat=1;
								SetNeedLoseFocus();
								Win->SetNeedFreshScreen();
							}
							
							if (InRange(Win->NowPos().y,fa2->GetgPS().y,fa2->GetgPS().y+TopHeigth))
							{
								if (FocusChoose!=-1)
								{
									Win->SetPresentArea(GetLinePosize(FocusChoose));
									FocusChoose=-1;
									Win->SetNeedFreshScreen();
								}
								
								if (stat==6||stat==7)
								{
									
								}
								else
								{
									int x=Win->NowPos().x-gPS.x,lastChoose=ColumnAndResizeChoose,lastStat=stat;
									ColumnAndResizeChoose=-1;
									stat=5;
									for (int i=0;i<ColumnInfo.size();x-=ColumnInfo[i].Width,++i)
										if (x<=ColumnInfo[i].Width-TopResizeWidth)
										{
											ColumnAndResizeChoose=i;
											stat=5;
											break;
										}
										else if (x<=ColumnInfo[i].Width)
										{
											ColumnAndResizeChoose=i;
											stat=8;
											break;
										}
									if (lastChoose!=ColumnAndResizeChoose||lastStat!=stat)
									{
										Win->SetPresentArea(Posize(gPS.x,fa2->GetgPS().y,gPS.w,TopHeigth));
										Win->SetNeedFreshScreen();
									}
								}
							}
							else
							{
								if (ColumnAndResizeChoose!=-1)
								{
									ColumnAndResizeChoose=-1;
									Win->SetPresentArea(Posize(gPS.x,fa2->GetgPS().y,gPS.w,TopHeigth));
									Win->SetNeedFreshScreen();
								}
								
								int pos=GetLineFromPos(Win->NowPos().y);
								if (pos!=-1)
								{
									if (pos!=FocusChoose)
									{
										if (FocusChoose!=-1)
											Win->SetPresentArea(GetLinePosize(FocusChoose));
										Win->SetPresentArea(GetLinePosize(pos));
										FocusChoose=pos;
										Win->SetNeedFreshScreen();
									}
								}
								else
									if (FocusChoose!=-1)
									{
										Win->SetPresentArea(GetLinePosize(FocusChoose));
										FocusChoose=-1;
										Win->SetNeedFreshScreen();
									}
							}
						}
						Win->StopSolvePosEvent();
						break;	
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				if (ListCnt!=0)
				{
					int ForL=(-fa->GetrPS().y)/(EachHeight+Interval),
						ForR=ForL+(fa2->GetgPS().h-TopHeigth)/(EachHeight+Interval)+1;
					ForL=EnsureInRange(ForL,0,ListCnt-1);
					ForR=EnsureInRange(ForR,0,ListCnt-1);
					if (!InRange(ClickChoose,0,ListCnt-1)) ClickChoose=-1;
					if (!InRange(FocusChoose,0,ListCnt-1)) FocusChoose=-1;
	
					Posize ps(gPS.x,gPS.y+TopHeigth+ForL*(EachHeight+Interval),0,EachHeight);
					for (int i=ForL;i<=ForR;++i)
					{
						ps.w=gPS.w;
						Win->RenderFillRect(ps&lmt,EachRowColor[ClickChoose==i?2:FocusChoose==i]?EachRowColor[ClickChoose==i?2:FocusChoose==i]:(ClickChoose==i?ThemeColor[3]:(FocusChoose==i?ThemeColor[1]:RGBA_NONE)));
						Win->Debug_DisplayBorder(ps);
						for (int j=0;j<ColumnInfo.size()&&j<=ListData[i].Text.size();++j)
						{
							ps.w=ColumnInfo[j].Width;
							Win->RenderDrawText(ListData[i].Text[j],ps,lmt,ColumnInfo[j].TextDisplayMode,j<ListData[i].SpecificTextColor.size()?ListData[i].SpecificTextColor[j]:(j==MainTextPos?(MainTextColor?MainTextColor:ThemeColor.MainTextColor[0]):SubTextColor));
							Win->Debug_DisplayBorder(ps);
							ps.x+=ps.w;
						}
						if (!!ListData[i].pic)
							Win->RenderCopyWithLmt(ListData[i].pic(),Posize(gPS.x,ps.y,EachHeight,EachHeight),lmt);
						ps.x=gPS.x;
						ps.y+=EachHeight+Interval;
					}
				}

				Posize ps(gPS.x,fa2->GetgPS().y,gPS.w,TopHeigth);
				Win->Debug_DisplayBorder(ps);
				for (int i=0;i<ColumnInfo.size();++i)
				{
					ps.w=ColumnInfo[i].Width-TopResizeWidth;
					Win->RenderFillRect(lmt&ps,TopColumnColor[i==ColumnAndResizeChoose&&InRange(stat,5,7)?(stat==5?1:2):0]?TopColumnColor[i==ColumnAndResizeChoose&&InRange(stat,5,7)?(stat==5?1:2):0]:ThemeColor[i==ColumnAndResizeChoose&&InRange(stat,5,7)?(stat==5?2:4):0]);
					Win->RenderDrawText(ColumnInfo[i].TagName,ps,lmt,ColumnInfo[i].TextDisplayMode,ColumnInfo[i].ColumnTextColor?ColumnInfo[i].ColumnTextColor:ThemeColor.MainTextColor[0]);
					ps.x+=ps.w+TopResizeWidth;
					Win->RenderFillRect(lmt&Posize(ps.x-TopResizeWidth,ps.y,TopResizeWidth,ps.h),TopResizeBarColor[i==ColumnAndResizeChoose&&InRange(stat,8,9)?(stat==9?2:1):0]?TopResizeBarColor[i==ColumnAndResizeChoose&&InRange(stat,8,9)?(stat==9?2:1):0]:ThemeColor[i==ColumnAndResizeChoose&&InRange(stat,8,9)?(stat==9?5:3):1]);
				}
				Win->Debug_DisplayBorder(gPS);
			}

			virtual void CalcPsEx()//??
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)	
					PsEx->GetrPS(rPS);
				rPS.w=0;
				for (auto vp:ColumnInfo)//??
					rPS.w+=vp.Width;
				rPS.h=EnsureInRange(ListCnt*(EachHeight+Interval)-Interval+TopHeigth,fa2->GetrPS().h,1e9);
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				fa2->ResizeLL(rPS.w,rPS.h);
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}

		public:
			inline Posize GetLinePosize(int pos)
			{
				if (pos==-1) return ZERO_POSIZE;
				else return Posize(gPS.x,gPS.y+TopHeigth+pos*(EachHeight+Interval),gPS.w,EachHeight);
			}
			
			inline void SetTopResizeBarColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					TopResizeBarColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(Posize(gPS.x,fa2->GetgPS().y,gPS.w,TopHeigth));
				}
				else PUI_DD[1]<<"DetailedListView: SetTopResizeBarColor: p "<<p<<" is not in range[0,2]"<<endl;
			}
			
			inline void SetTopColumnColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					TopColumnColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(Posize(gPS.x,fa2->GetgPS().y,gPS.w,TopHeigth));
				}
				else PUI_DD[1]<<"DetailedListView: SetTopColumnColor: p "<<p<<" is not in range[0,2]"<<endl;
			}
			
			inline void SetEachRowColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					EachRowColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(CoverLmt);
				}
				else PUI_DD[2]<<"DetailedListView: SetEachRowColor: p "<<p<<" is not in range[0,2]"<<endl;
			}
			
			inline void SetMainTextColor(const RGBA &co)
			{
				MainTextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}
			
			inline void SetSubTextColor(const RGBA &co)
			{
				SubTextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}
			
			inline void SetBackgroundColor(const RGBA &co)
			{fa2->SetLargeAreaColor(co);}
			
			void SortBy(int sortbycolumn,bool sortdirection)
			{
				SortByColumn=sortbycolumn;
				SortDirection=sortdirection;
				PUI_DD[0]<<"DetailedListView "<<ID<<" Sort by "<<ColumnInfo[SortByColumn].TagName<<" "<<SortByColumn<<" "<<SortDirection<<endl;
				if (SortFunc!=NULL)
					SortFunc(ListData,ColumnInfo[SortByColumn].TagName,SortByColumn,SortDirection);
				Win->SetPresentArea(gPS);
			}
			
			inline void SetMainTextPos(int pos)
			{
				if (MainTextPos!=pos)
				{
					MainTextPos=pos;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(CoverLmt);
				}
			}
			
			inline void SetEnableAutoMainTextPos(bool en)
			{
				EnableAutoMainTextPos=en;
				SetMainTextPos(SortByColumn);
			}

			inline void SetTopHeightAndResizeWidth(int topheight=30,int topresizewidth=2)
			{
				TopHeigth=topheight;
				TopResizeWidth=topresizewidth;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}

			inline void SetEachHeightAndInterval(int eachheight=24,int interval=2)
			{
				EachHeight=eachheight;
				Interval=interval;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}
			
			inline void SetFunc(void (*_func)(T&,int,int))
			{func=_func;}

			inline void SetBackgroundFuncdata(const T &bgdata)
			{BackgroundFuncData=bgdata;}
			
			void SetSortFunc(void (*_SortFunc)(vector <DetailedListViewData>&,const string&,int,bool))
			{SortFunc=_SortFunc;}

			void SetColumnInfo(int p,const DetailedListViewColumn &column)
			{
				p=EnsureInRange(p,0,ColumnInfo.size());
				ColumnInfo.insert(ColumnInfo.begin()+p,column);
				if (ColumnAndResizeChoose>=p) ++ColumnAndResizeChoose;
				if (MainTextPos>=p) ++MainTextPos; 
				if (SortByColumn>=p) ++SortByColumn;
				Win->SetNeedUpdatePosize();//Is it just OK?
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}
			
			void DeleteColumnInfo(int p)
			{
				if (ColumnInfo.emtpy()) return;
				p=EnsureInRange(p,0,ColumnInfo.size()-1);
				ColumnInfo.erase(ColumnInfo.begin()+p);
				if (ColumnAndResizeChoose>p) --ColumnAndResizeChoose;
				else if (ColumnAndResizeChoose==p) ColumnAndResizeChoose=-1;
				if (SortByColumn>p) --SortByColumn;
				else if (SortByColumn==p) SortByColumn=0;
				if (EnableAutoMainTextPos) MainTextPos=SortByColumn;
				else
					if (MainTextPos>p) --MainTextPos;
					else if (MainTextPos==p) MainTextPos=0;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}
			
			DetailedListView* PushbackColumnInfo(const DetailedListViewColumn &column)
			{
				SetColumnInfo(1e9,column);
				return this;
			}
			
			inline int GetColumnCnt()
			{return ColumnInfo.size();}

			void SetListContent(int p,const DetailedListViewData &info)
			{
				p=EnsureInRange(p,0,ListCnt);
				ListData.insert(ListData.begin()+p,info);
				++ListCnt;
				if (ClickChoose>=p) ++ClickChoose;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}
			
			DetailedListView* PushbackListContent(const DetailedListViewData &info)
			{
				SetListContent(1e9,info);
				return this;
			}
			
			void DeleteListContent(int p)
			{
				if (!ListCnt) return;
				p=EnsureInRange(p,0,ListCnt-1);
				ListData.erase(ListData.begin()+p);
				--ListCnt;
				if (ClickChoose>p) --ClickChoose;
				else if (ClickChoose==p) ClickChoose=-1;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}
			
			void ClearListContent()
			{
				if (!ListCnt) return;
				PUI_DD[0]<<"Clear DetailedListView "<<ID<<" Content: ListCnt "<<ListCnt<<endl;
				FocusChoose=ClickChoose=-1;
				ListData.clear();
				ListCnt=0;
				fa2->SetViewPort(2,0);
				Win->SetNeedFreshScreen();
				Win->SetNeedUpdatePosize();
				Win->SetPresentArea(CoverLmt);
			}
			
			void UpdateListContent(int p,const DetailedListViewData &info)
			{
				p=EnsureInRange(p,0,ListCnt-1);
				ListData[p]=info;
				if (ClickChoose>=p) ++ClickChoose;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(CoverLmt);
			}
			
			inline int GetListCnt()
			{return ListCnt;}
				
			inline T& GetFuncData(int p)
			{return ListData[EnsureInRange(p,0,ListCnt-1)].FuncData;}

			inline T& operator [] (int p)
			{return ListData[EnsureInRange(p,0,ListCnt-1)].FuncData;}
			
			int Find(const T &_funcdata)
			{
				for (int i=0;i<ListCnt;++i)
					if (ListData[i]==_funcdata)
						return i;
				return -1;
			}
			
			inline DetailedListViewData& GetListData(int p)//For breakthrough class limit??
			{return ListData[EnsureInRange(p,0,ListCnt-1)];}
			
			inline DetailedListViewColumn& GetColumnData(int p)
			{return ColumnInfo[EnsureInRange(p,0,ColumnInfo.size())];}
			
			void AddPsEx(PosizeEX *psex)
			{fa2->AddPsEx(psex);}
			
			virtual void SetrPS(const Posize &ps)
			{fa2->SetrPS(ps);}
			
			DetailedListView(int _ID,Widgets *_fa,const Posize &_rps,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create DetailedListView "<<ID<<endl;
				Type=WidgetType_DetailedListView;
				fa2=new LargeLayerWithScrollBar(0,_fa,_rps,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS={0,0,_rps.w,TopHeigth};
				for (int i=0;i<=2;++i)
					TopResizeBarColor[i]=TopColumnColor[i]=EachRowColor[i]=RGBA_NONE;
			}
			
			DetailedListView(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create DetailedListView "<<ID<<endl;
				Type=WidgetType_DetailedListView;
				fa2=new LargeLayerWithScrollBar(0,_fa,psex,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS=ZERO_POSIZE;
				for (int i=0;i<=2;++i)
					TopResizeBarColor[i]=TopColumnColor[i]=EachRowColor[i]=RGBA_NONE;
			}
	};
	
	template <class T> class Menu1;
	template <class T> struct MenuData
	{
		friend class Menu1 <T>;
		protected:
			int type=0;//0:none 1:normal_func 2:divide_line 3:sub_menu1
			void (*func)(T&)=NULL;
			T funcData;
			char hotKey=0;//case-insensitive
			SharedTexturePtr pic;
			vector <MenuData<T> > subMenu;
			bool enable=1;
			string text;
			int subMenuW=200;
			
		public:
			MenuData(const string &_text,void (*_func)(T&),const T &_funcdata,char _hotkey=0,const SharedTexturePtr &_pic=SharedTexturePtr(NULL),bool _enable=1)
			:type(1),text(_text),func(_func),funcData(_funcdata),hotKey(_hotkey),pic(_pic),enable(_enable) {}
			
			MenuData(int p)//any num is OK
			:type(2) {}
			
			MenuData(const vector <MenuData> &_submenu,const string &_text,int _submenuWidth=200,char _hotkey=0,const SharedTexturePtr &_pic=SharedTexturePtr(NULL),bool _enable=1)
			:type(3),text(_text),subMenuW(_submenuWidth),subMenu(_submenu),hotKey(_hotkey),pic(_pic),enable(_enable) {}
	};
	
	template <class T> class Menu1:public Widgets//This widget have many differences,it is not so easy to solve it  QAQ
	{
		protected:
			vector <MenuData<T> > MenuDataArray;
			int stat=0,//0:Up_NoFocus 1:Up_Focus 2:Down
				spos1=0,spos2=0,//show pos start(1) and end(2), when not exceed ,spos1 is 0, spos2 is ManudataCnt-1,means all in range
				pos=-1,//-1:no pos -2:ExceedTop -3:ExceedButtom
				subMenupos=-1,//-1 means not have subMenu
				EachHeight=20,//Each/2 is the height of exceed_button
				DivideLineHeight=3,
				BorderWidth=4,
				MenudataCnt=0;
			bool Exceed=0,
				 IsSubMenu=0,
//				 autoDeleteMenuData=0,
				 autoAdjustMenuPosition=1;
//				 NeedDelete=0;
			Menu1 <T> *rootMenu=NULL;
			SDL_Texture *triTex[2]={NULL,NULL};//enable,disable
			RGBA TextColor[2],//enable,disable
				 DivideLineColor=RGBA_NONE,
				 MenuColor[3];//no focus(background),focus,click
			
			void SetDelSubMenu()//create or delete subMenu
			{
				if (!InRange(pos,0,MenudataCnt-1))
				{
					if (subMenupos!=-1)
					{
						SetDelayDeleteWidget(childWg);
//						delete childWg;
						subMenupos=-1;
					}
					return;
				}
				
				if (subMenupos==-1||pos!=subMenupos)
				{
					if (pos!=subMenupos)//delete current subMenu
					{
						SetDelayDeleteWidget(childWg);
//						delete childWg;
						subMenupos=-1;
					}
					MenuData <T> &md=MenuDataArray[pos];
					if (md.type==3&&md.enable)
//						if (childWg==NULL)
						{
							Point pt(rPS.w-2*BorderWidth,BorderWidth);
							if (Exceed)
								pt.y+=BorderWidth/2;
							for (int i=spos1;i<pos-1;++i)
								switch (md.type)
								{
									case 1:	case 3:	pt.y+=EachHeight;	break;
									case 2:	pt.y+=DivideLineHeight;		break;
								}
							new Menu1 <T> (0,this,pt,md.subMenu,md.subMenuW);
							Win->SetNeedUpdatePosize();
							Win->SetNeedFreshScreen();
							subMenupos=pos;
						}
				}
			}

			int GetPos(int y)
			{
				if (!InRange(y,BorderWidth,rPS.h-BorderWidth-1))
					return -1;
				if (Exceed)
				{
					if (InRange(y-BorderWidth,0,EachHeight/2-1))//Is top exceed_button
						return -2;
					if (InRange(rPS.h-y-BorderWidth-1,0,EachHeight/2-1))//Is buttom exceed_button
						return -3;
					y-=EachHeight/2+BorderWidth;
				}
				else y-=BorderWidth;
				for (int i=spos1;i<MenudataCnt&&y>BorderWidth&&i<=spos2;++i)
					switch (MenuDataArray[i].type)
					{
						case 1:	case 3:
							if (InRange(y,0,EachHeight-1))
								return i;
							y-=EachHeight;
							break;
						case 2:
							if (InRange(y,0,DivideLineHeight-1))
								return i;
							y-=DivideLineHeight;
							break;
					}
				return -1;
			}
			
			virtual void CheckEvent()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_KEYDOWN:
						char hotKeyPressed=-1;
						switch (event.key.keysym.sym)
						{
							case SDLK_a: hotKeyPressed='A';	break;
							case SDLK_b: hotKeyPressed='B';	break;
							case SDLK_c: hotKeyPressed='C';	break;
							case SDLK_d: hotKeyPressed='D';	break;
							case SDLK_e: hotKeyPressed='E';	break;
							case SDLK_f: hotKeyPressed='F';	break;
							case SDLK_g: hotKeyPressed='G';	break;
							case SDLK_h: hotKeyPressed='H';	break;
							case SDLK_i: hotKeyPressed='I';	break;
							case SDLK_j: hotKeyPressed='J';	break;
							case SDLK_k: hotKeyPressed='K';	break;
							case SDLK_l: hotKeyPressed='L';	break;
							case SDLK_m: hotKeyPressed='M';	break;
							case SDLK_n: hotKeyPressed='N';	break;
							case SDLK_o: hotKeyPressed='O';	break;
							case SDLK_p: hotKeyPressed='P';	break;
							case SDLK_q: hotKeyPressed='Q';	break;
							case SDLK_r: hotKeyPressed='R';	break;
							case SDLK_s: hotKeyPressed='S';	break;
							case SDLK_t: hotKeyPressed='T';	break;
							case SDLK_u: hotKeyPressed='U';	break;
							case SDLK_v: hotKeyPressed='V';	break;
							case SDLK_w: hotKeyPressed='W';	break;
							case SDLK_x: hotKeyPressed='X';	break;
							case SDLK_y: hotKeyPressed='Y';	break;
							case SDLK_z: hotKeyPressed='Z';	break;
							
							case SDLK_ESCAPE:
								SetDelayDeleteThis();
								Win->StopSolveEvent();
								break;
							
							case SDLK_UP:
								stat=1;
								pos=(pos-1+MenudataCnt)%MenudataCnt;
								if (MenuDataArray[pos].type==2)//Only once because normally there are not two divideline near
									pos=(pos-1+MenudataCnt)%MenudataCnt;
								Win->SetNeedFreshScreen();
								Win->StopSolveEvent();
								break;
							
							case SDLK_DOWN:
								stat=1;
								pos=(pos+1+MenudataCnt)%MenudataCnt;
								if (MenuDataArray[pos].type==2)
									pos=(pos+1+MenudataCnt)%MenudataCnt;
								Win->SetNeedFreshScreen();
								Win->StopSolveEvent();
								break;
							
							case SDLK_LEFT:
								if (rootMenu!=this)
								{
									SetDelayDeleteThis();
									Win->StopSolveEvent();
								}
								break;
							
							case SDLK_RIGHT:
								if (InRange(pos,0,MenudataCnt-1))
									if (MenuDataArray[pos].type==3)
									{
										SetDelSubMenu();
										Win->StopSolveEvent();
									}
								break;
							
							case SDLK_RETURN:
							case SDLK_SPACE:
								if (InRange(pos,0,MenudataCnt-1))
								{
									MenuData <T> &md=MenuDataArray[pos];
									if (md.enable)
									{
										if (md.type==1)
										{
											if (md.func!=NULL)
												md.func(md.funcData);
											SetDelayDeleteWidget(rootMenu);
										}
										else if (md.type==3)
											SetDelSubMenu();
										Win->SetNeedFreshScreen();
										Win->StopSolveEvent();
									}
								}
								break;
						}
						if (hotKeyPressed!=-1)
						{
							for (int i=0;i<MenudataCnt;++i)
							{
								char ch=MenuDataArray[i].hotKey;
								if (InRange(ch,'a','z'))
									ch+='A'-'a';
								if (ch==hotKeyPressed)
								{
									pos=i;
									MenuData <T> &md=MenuDataArray[i];
									if (md.enable)
										if (md.type==1)
										{
											if (md.func!=NULL)
												md.func(md.funcData);
											SetDelayDeleteWidget(rootMenu);
										}
										else if (md.type==3)
											SetDelSubMenu();
									Win->SetNeedFreshScreen();
									Win->StopSolveEvent();
									break;
								}
							}
							if (Win->IsNeedSolveEvent())
								cout<<'\a';
						}
						break;
				}
			}

			virtual void CheckPos()//There still exists some bugs I think,I will fix it when found
			{
//				if (!Win->NeedSolvePosEvent||MenuDataArray==NULL) return;
				if (MenuDataArray.empty()) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				Posize LMT=Win->GetWinPS().ToOrigin();
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						PUI_DD[0]<<"Menu1 "<<ID<<" click"<<endl;
						if ((gPS&LMT).In(Win->NowPos()))
						{
							if (event.button.button==SDL_BUTTON_LEFT)
								if (gPS.Shrink(BorderWidth).In(Win->NowPos()))
								{
									stat=2;
									pos=GetPos(Win->NowPos().y-gPS.y);
									Win->SetNeedFreshScreen();
								}
								Win->StopSolvePosEvent();
						}
						else
						{
							stat=0;
							Win->SetNeedFreshScreen();
//							delete this;
							SetDelayDeleteThis();
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2&&Win->IsNeedSolvePosEvent())
						{
							PUI_DD[0]<<"Menu1 "<<ID<<" function"<<endl;
							if (gPS.Shrink(BorderWidth).In(Win->NowPos()))
							{
								stat=1;
								pos=GetPos(Win->NowPos().y-gPS.y);
								SetDelSubMenu();
								Win->SetNeedFreshScreen();
								Win->StopSolvePosEvent(); 
								if (InRange(pos,0,MenudataCnt-1))
								{
									MenuData <T> &md=MenuDataArray[pos];
									if (md.type==1&&md.enable)
									{
										if (md.func!=NULL)
											md.func(md.funcData);
//										delete rootMenu;
										SetDelayDeleteWidget(rootMenu);
									}
								}
								else if (pos==-2)
								{
									PUI_DD[2]<<"not usable yet"<<endl;
								}
								else if (pos==-3)
								{
									PUI_DD[2]<<"not usable yet"<<endl;
								}
							}
						}
						break;
					
					case SDL_MOUSEMOTION:
						if ((gPS&LMT).In(Win->NowPos()))
						{
							int pos_r=pos;
							pos=GetPos(Win->NowPos().y-gPS.y);
							if (pos!=pos_r)
							{
//								stat=InRange(pos,0,MenudataCnt-1);
								stat=1;
								SetDelSubMenu();
								Win->SetNeedFreshScreen();
							}
							Win->StopSolvePosEvent();
						}
						else
						{
							if (stat==1)
								Win->SetNeedFreshScreen();
							stat=0;
							pos=-1;
						}
						break;
				}
			}
			
			virtual void _SolvePosEvent()
			{
				if (!Win->IsNeedSolvePosEvent())
					return;
				if (Enabled)
				{
					if (childWg!=NULL)
						SolvePosEventOf(childWg);
					if (Win->IsNeedSolvePosEvent())
						CheckPos();
				}
				if (nxtBrother!=NULL)
					SolvePosEventOf(nxtBrother);
			}
			
			virtual void Show(Posize &lmt)
			{
				lmt=Win->GetWinPS().ToOrigin();
				Win->RenderFillRect(gPS.Shrink(1)&lmt,MenuColor[0]?MenuColor[0]:ThemeColor.BackgroundColor[0]);
				Win->RenderDrawRectWithLimit(gPS,MenuColor[stat]?MenuColor[stat]:ThemeColor[stat*2],lmt);
				int y=BorderWidth;
				if (Exceed)
				{

				}
				
				for (int i=spos1;i<MenudataCnt;++i)
				{
					if (y+EachHeight>rPS.h-BorderWidth)
						break;
					MenuData <T> &md=MenuDataArray[i];
					switch (md.type)
					{
						case 1:
							if (stat!=0&&pos==i)
								Win->RenderFillRect((Posize(BorderWidth,y,rPS.w-BorderWidth*2,EachHeight)+gPS)&lmt,MenuColor[stat]?MenuColor[stat]:ThemeColor[stat*2]);
							if (md.pic()!=NULL)
								Win->RenderCopyWithLmt(md.pic(),Posize(BorderWidth,y,EachHeight,EachHeight).Shrink(2)+gPS,lmt);
							Win->RenderDrawText(md.hotKey!=0?md.text+"("+md.hotKey+")":md.text,Posize(BorderWidth+EachHeight+8,y,rPS.w-EachHeight-8-BorderWidth*2,EachHeight)+gPS,lmt,-1,TextColor[!md.enable]?TextColor[!md.enable]:ThemeColor.MainTextColor[!md.enable]);
							y+=EachHeight;
							break;
						case 2:
							Win->RenderFillRect((Posize(BorderWidth+EachHeight+4,y+DivideLineHeight/3,rPS.w-BorderWidth*2-EachHeight-4,max(DivideLineHeight/3,1))+gPS)&lmt,DivideLineColor?DivideLineColor:ThemeColor[1]);
							y+=DivideLineHeight;
							break;
						case 3:
							if (stat!=0&&pos==i)
								Win->RenderFillRect((Posize(BorderWidth,y,rPS.w-BorderWidth*2,EachHeight)+gPS)&lmt,MenuColor[stat]?MenuColor[stat]:ThemeColor[stat*2]);
							if (md.pic()!=NULL)
								Win->RenderCopyWithLmt(md.pic(),Posize(BorderWidth,y,EachHeight,EachHeight).Shrink(2)+gPS,lmt);
							Win->RenderDrawText(md.hotKey!=0?md.text+"("+md.hotKey+")":md.text,Posize(BorderWidth+EachHeight+8,y,rPS.w-EachHeight-8-BorderWidth*2-EachHeight/2,EachHeight)+gPS,lmt,-1,TextColor[!md.enable]?TextColor[!md.enable]:ThemeColor.MainTextColor[!md.enable]);
							if (triTex[!md.enable]==NULL)
								triTex[!md.enable]=CreateTextureFromSurfaceAndDelete(CreateTriangleSurface(EachHeight/4,EachHeight/2,Point(0,0),Point(0,EachHeight/2-1),Point(EachHeight/4-1,EachHeight/4),TextColor[!md.enable]?TextColor[!md.enable]:ThemeColor.MainTextColor[!md.enable]));
							Win->RenderCopyWithLmt(triTex[!md.enable],Posize(rPS.w-BorderWidth*2-EachHeight/4,y+EachHeight/4,EachHeight/4,EachHeight/2)+gPS,lmt);
							y+=EachHeight;
							break;
					}
					spos2=i;
				}
				
				if (Exceed)
				{
					
				}

				Win->Debug_DisplayBorder(gPS);
			}

			int GetHFromData()
			{
				int re=BorderWidth*2;
				for (int i=0;i<MenudataCnt;++i)//??
					switch (MenuDataArray[i].type)
					{
						case 1: case 3:	re+=EachHeight;	break;
						case 2:	re+=DivideLineHeight;	break;
					}
				return re;
			}
			
			Menu1(int _ID,Menu1 <T> *_faMenu1,const Point &pt,const vector <MenuData<T> > &menudata,int w=200)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create (Sub)Menu1 "<<ID<<endl;
				Type=WidgetType_Menu1;
				SetFa(_faMenu1);
				SetMenuData(menudata);
				SetrPS((Win->GetWinPS().ToOrigin()-fa->GetgPS()).Shrink(1).EnsureIn({pt.x,pt.y,w,GetHFromData()}));
				IsSubMenu=1;
				rootMenu=_faMenu1->rootMenu;
				for (int i=0;i<=1;++i)
					TextColor[i]=RGBA_NONE;
				for (int i=0;i<=2;++i)
					MenuColor[i]=RGBA_NONE;
			}
			
		public:
			void SetTextColor(bool p,const RGBA &co)
			{
				TextColor[p]=co;
				Win->SetNeedFreshScreen();
				if (triTex[p]!=NULL)
					SDL_DestroyTexture(triTex[p]),triTex[p]=NULL;
			}

			inline void SetDivideLineColor(const RGBA &co)
			{
				DivideLineColor=co;
				Win->SetNeedFreshScreen();
			}

			inline void SetMenuColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
					MenuColor[p]=co,Win->SetNeedFreshScreen();
				else PUI_DD[1]<<"Menu1: SetMenuColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			void SetEachHeight(int h)
			{
				EachHeight=h;
				GetHFromData();
				if (triTex[0]!=NULL)
					SDL_DestroyTexture(triTex[0]),triTex[0]=NULL;
				if (triTex[1]!=NULL)
					SDL_DestroyTexture(triTex[1]),triTex[1]=NULL;		
				Win->SetNeedFreshScreen();
			}
			
			void SetDivideLineHeight(int h)
			{
				DivideLineHeight=h;
				GetHFromData();
				Win->SetNeedFreshScreen();
			}
			
			void SetBorderWidth(int w)
			{
				BorderWidth=w;
				GetHFromData();
				Win->SetNeedFreshScreen();
			}

			void SetMenuData(const vector <MenuData<T> > &data)
			{
				MenuDataArray=data;
				MenudataCnt=MenuDataArray.size();
				rPS.h=GetHFromData();
				Win->SetNeedFreshScreen();
			}
			
			virtual ~Menu1()
			{
				PUI_DD[0]<<"Delete Menu1 "<<ID<<endl;
				if (IsSubMenu)
					((Menu1<T>*)fa)->subMenupos=-1;
			}
			
			Menu1(int _ID,const vector <MenuData<T> > &menudata,int w=256,PUI_Window *_win=CurrentWindow)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create Menu1 "<<ID<<endl;
				Type=WidgetType_Menu1;
				SetFa(_win->MenuLayer());
				SetMenuData(menudata);
				SetrPS(Win->GetWinPS().ToOrigin().EnsureIn({Win->NowPos().x,Win->NowPos().y,w,GetHFromData()}));
				rootMenu=this;
				for (int i=0;i<=1;++i)
					TextColor[i]=RGBA_NONE;
				for (int i=0;i<=2;++i)
					MenuColor[i]=RGBA_NONE;
			}
	};
	
	class MessageBoxLayer:public Widgets
	{
		protected:
			string MsgBoxTitle;
			int stat=0;//0:None 1:Focus 2:DownDrag 3:Focus_X 4:Down_X
			bool WarningOn=0,
				 ShowTopAreaColor=0;
			int ClickOutsideReaction=3,//0:NONE 1:Close this 2:Warning and not transfer event 3:Warning and ring and not transfer event
				TopAreaHeight=30,//influence the size of X and Title text print area
				DragbleMode=2;//0:Undragble 1:Only top area dragble 2:All blank area dragble
			Point pinPtInLayer=ZERO_POINT;
			RGBA BorderColor[4]={RGBA_NONE,RGBA_NONE,RGBA_NONE,{200,0,0,255}},//no focus(background),focus,drag,warning
				 TitleTextColor=RGBA_NONE,
				 TopAreaColor=RGBA_NONE,
				 BackgroundColor=RGBA_WHITE;
			void (*CloseTriggerFunc)(void *)=NULL;
			void *CloseFuncData=NULL;
			bool Closing=0;
			
			virtual void CheckEvent()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (event.type==SDL_KEYDOWN)
					if (event.key.keysym.sym==SDLK_ESCAPE)
					{
						Close();
						Win->StopSolveEvent();
					}
			}
			
			virtual void CheckPos()//??
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (!gPS.In(Win->NowPos()))
						{
							if (!Win->IsNeedSolvePosEvent()) return;
							PUI_DD[0]<<"MessageBox "<<ID<<" click outside"<<endl;
							stat=0;
							if (ClickOutsideReaction==1)
								Close();
							else if (ClickOutsideReaction==2||ClickOutsideReaction==3)
							{
								PUI_DD[0]<<"MessageBox "<<ID<<" Warning!"<<endl;
								WarningOn=1;
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
								if (ClickOutsideReaction==3)
									putchar(7);
							}
							else ;
							if (ClickOutsideReaction!=0)
								Win->StopSolvePosEvent();
						}
						else if (event.button.button==SDL_BUTTON_LEFT)
						{
							WarningOn=0;
							if (!Win->IsNeedSolvePosEvent())
								return;
							PUI_DD[0]<<"MessageBox "<<ID<<" click Layer"<<endl;
							if (Win->NowPos().x>=gPS.x2()-TopAreaHeight&&Win->NowPos().y<=gPS.y+TopAreaHeight)
								stat=4;
							else if (DragbleMode==1&&Win->NowPos().y<=gPS.y+TopAreaHeight||DragbleMode==2)
							{
								PUI_DD[0]<<"MessageBox "<<ID<<" drag"<<endl;
								stat=2;
								Win->SetOccupyPosWg(this);
								pinPtInLayer=Win->NowPos()-gPS.GetLU();
							}
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
							Win->StopSolvePosEvent();
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (Win->IsNeedSolvePosEvent())
							if (stat==2)
							{
								PUI_DD[0]<<"MessgeBox "<<ID<<" stop drag"<<endl;
								stat=1;
								Win->SetOccupyPosWg(NULL);
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
								Win->StopSolvePosEvent();
							}
							else if (stat==4&&gPS.In(Win->NowPos()))
							{
								PUI_DD[0]<<"MessgeBox "<<ID<<" loose button"<<endl;
								stat=3;
								Close();
								Win->StopSolvePosEvent();
							}
						break;

					case SDL_MOUSEMOTION:
						if (!Win->IsNeedSolvePosEvent()) return;//??
						if (stat==2)
						{
							Point newPt=Win->NowPos()-pinPtInLayer;
							rPS.x=newPt.x;
							rPS.y=newPt.y;
							rPS=Win->GetWinPS().ToOrigin().EnsureIn(rPS);
							Win->StopSolvePosEvent();
							Win->SetNeedUpdatePosize();
							Win->SetNeedFreshScreen();
						}
						else
							if (gPS.In(Win->NowPos()))
							{
//								PUI_DD[0]<<"MessageBox "<<ID<<" focus"<<endl;
								if (Win->NowPos().x>=gPS.x2()-TopAreaHeight&&Win->NowPos().y<=gPS.y+TopAreaHeight)
								{
									if (stat!=3&&stat!=4)
										stat=3;
								}
								else stat=1;
								Win->StopSolvePosEvent();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
							else 
								if (stat!=0)
								{
									stat=0;
									Win->SetNeedFreshScreen();
									Win->SetPresentArea(gPS);
								}
						break;
				}
			}
			
			virtual void _SolvePosEvent()
			{
				if (Enabled)
				{
					if (childWg!=NULL)
						SolvePosEventOf(childWg);
					CheckPos();
				}
				if (nxtBrother!=NULL)
					SolvePosEventOf(nxtBrother);
			}
			
			virtual void Show(Posize &lmt)
			{
				if (ShowTopAreaColor)
					Win->RenderFillRect(Posize(gPS.x,gPS.y,gPS.w,TopAreaHeight)&lmt,TopAreaColor?TopAreaColor:ThemeColor[1]),
					Win->RenderFillRect(Posize(gPS.x,gPS.y+TopAreaHeight,gPS.w,gPS.h-TopAreaHeight)&lmt,BackgroundColor);
				else Win->RenderFillRect(gPS&lmt,BackgroundColor);
				Win->RenderDrawText(MsgBoxTitle,Posize(gPS.x+16,gPS.y,gPS.w,TopAreaHeight),lmt,-1,TitleTextColor?TitleTextColor:ThemeColor.MainTextColor[0]);
				//Temp
				Win->RenderFillRect(Posize(gPS.x2()-TopAreaHeight,gPS.y,TopAreaHeight,TopAreaHeight).Shrink(3)&lmt,BorderColor[stat>=3?stat-2:0]?BorderColor[stat>=3?stat-2:0]:ThemeColor[(stat>=3?stat-2:0)*2]);
				Win->RenderDrawText("X",Posize(gPS.x2()-TopAreaHeight,gPS.y,TopAreaHeight,TopAreaHeight).Shrink(3),lmt);
				
				Win->RenderDrawRectWithLimit(gPS,WarningOn?BorderColor[3]:(BorderColor[stat>=3?stat-2:stat]?BorderColor[stat>=3?stat-2:stat]:ThemeColor[(stat>=3?stat-2:stat)*2]),lmt);
				Win->Debug_DisplayBorder(gPS);
			}
			
		public:
			inline void SetBorderColor(int p,const RGBA &co)
			{
				if (InRange(p,0,3))
				{
					BorderColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"MessageBox "<<ID<<" SetBorderColor error : p "<<p<<" is not in range[0,3]"<<endl;
			}
			
			inline void SetTitleTextColor(const RGBA &co)
			{
				TitleTextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetTopAreaColor(const RGBA &co)
			{
				TopAreaColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetBackgroundColor(const RGBA &co)
			{
				BackgroundColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetMessageBoxTitle(const string &title)
			{
				MsgBoxTitle=title;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetClickOutsideReactions(int reaction)
			{ClickOutsideReaction=reaction;}
			
			inline void SetTopAreaHeight(int heigth)
			{TopAreaHeight=heigth;}
			
			inline void SetDragbleMode(int dragblemode)
			{DragbleMode=dragblemode;}
			
			inline void EnableShowTopAreaColor(bool on)
			{
				ShowTopAreaColor=on;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
				
			inline void Close()
			{
				if (Closing) return;
				Closing=1;
				if (CloseTriggerFunc!=NULL)
					CloseTriggerFunc(CloseFuncData);
				SetDelayDeleteThis();
			}
			
			void SetCloseTriggerFunc(void (*func)(void*),void *funcdata=NULL)
			{
				CloseTriggerFunc=func;
				CloseFuncData=funcdata;
			}
			
			virtual ~MessageBoxLayer()
			{
				if (Win->GetOccupyPosWg()==this)
					Win->SetOccupyPosWg(NULL);
			}
			
			MessageBoxLayer(int _ID,const string &title,int _w,int _h,PUI_Window *_win=CurrentWindow)
			:MsgBoxTitle(title)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create MessageBoxLayer "<<ID<<endl;
				Type=WidgetType_MessageBoxLayer;
				SetFa(_win->MenuLayer());
				SetrPS({fa->GetgPS().w-_w>>1,fa->GetgPS().h-_h>>1,_w,_h});
			}
			
			MessageBoxLayer(int _ID,const string &title,const Posize &_ps0,PUI_Window *_win=CurrentWindow)
			:MsgBoxTitle(title)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create MessageBoxLayer "<<ID<<endl;
				Type=WidgetType_MessageBoxLayer;
				SetFa(_win->MenuLayer());
				SetrPS(_ps0);
			}
	};
	
	class MessageBoxLayerWindow:public Widgets
	{
		protected:
			
		public:
			
	};
	
	template <class T> class MessageBoxButton:public MessageBoxLayer
	{
		protected:
			struct MessageBoxButtonData
			{
				int p=-1;
				MessageBoxButton *wg=NULL;
				void (*func)(T&)=NULL;
				T funcdata=NULL;
				
				MessageBoxButtonData(int _p,MessageBoxButton *_wg,void (*_func)(T&),const T &_funcdata)
				:p(_p),wg(_wg),func(_func),funcdata(_funcdata) {}
			};
			
			vector <Button<MessageBoxButtonData*>*> Bu;
			vector <MessageBoxButtonData*> BuDa;
			string MsgText;
			RGBA MsgTextColor=RGBA_NONE;
			int ButtonCnt=0,CloseTriggerButton=-1;
			int ButtonWidth=100,ButtonHeigth=30;
			bool ButtonClicked=0;
			
			Posize GetButtonPosize(int p)
			{
				switch (ButtonCnt)
				{
					case 1:
						return Posize(gPS.w-ButtonWidth>>1,gPS.h-40,ButtonWidth,ButtonHeigth);
					case 2:
						if (p==1) return Posize(gPS.w/2-ButtonWidth>>1,gPS.h-40,ButtonWidth,ButtonHeigth);
						else return Posize(gPS.w/2*3-ButtonWidth>>1,gPS.h-40,ButtonWidth,ButtonHeigth);
					case 3:
						if (p==1) return Posize(30,gPS.h-40,ButtonWidth,ButtonHeigth);
						else if (p==2) return Posize(gPS.w-ButtonWidth>>1,gPS.h-40,ButtonWidth,ButtonHeigth);
						else return Posize(gPS.w-30-ButtonWidth,gPS.h-40,ButtonWidth,ButtonHeigth);
					default:
						PUI_DD[2]<<"MessageBoxButton "<<ID<<":There is too many buttons! It is not supported yet!"<<endl;
						break;
				}
			}
			
			class PosizeEX_ButtonInMsgBox:public PosizeEX
			{
				protected:
					MessageBoxButton *tar=NULL;
					int p;
					
				public:
					virtual void GetrPS(Posize &ps)
					{
						if (nxt!=NULL) nxt->GetrPS(ps);
						ps=tar->GetButtonPosize(p);
					}
					
					PosizeEX_ButtonInMsgBox(MessageBoxButton *_tar,int _p):tar(_tar),p(_p) {}
			};
			
			static void ButtonFunction(MessageBoxButtonData *&data)
			{
				if (data->func!=NULL)
					data->func(data->funcdata);
				data->wg->ButtonClicked=1;
				data->wg->Close();
			}
			
			static void MsgButtonCloseTriggerFunc(void *funcdata)
			{
				MessageBoxButton *p=(MessageBoxButton*)funcdata;
				if (p->ButtonClicked||p->CloseTriggerButton==-1) return;
				if (p->BuDa[p->CloseTriggerButton]->func!=NULL)
					p->BuDa[p->CloseTriggerButton]->func(p->BuDa[p->CloseTriggerButton]->funcdata);
			}
			
		public:
			inline void SetMsgTextColor(const RGBA &co)
			{
				MsgTextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}

			Button<T>* GetButton(int p)
			{
				if (InRange(p,1,ButtonCnt))
					return Bu[p-1];
				else return NULL;
			}
			
			void AddButton(const string &_text,void (*_func)(T&),const T &_funcdata)
			{
				++ButtonCnt;
				BuDa.push_back(new MessageBoxButtonData(ButtonCnt,this,_func,_funcdata));
				Bu.push_back(new Button<MessageBoxButtonData*>(0,this,new PosizeEX_ButtonInMsgBox(this,ButtonCnt),_text,ButtonFunction,BuDa.back()));
			}
			
			inline void SetButtonWidth(int width)
			{
				ButtonWidth=width;
				Win->SetNeedUpdatePosize();
			}
			
			inline void SetCloseTriggerButton(int p)
			{CloseTriggerButton=EnsureInRange(p,-1,ButtonCnt-1);}
			
			virtual ~MessageBoxButton()
			{
				for (auto vp:BuDa)
					delete vp;
			}
			
			MessageBoxButton(int _ID,const string &title,const string &msgText,int _w=400,PUI_Window *_win=CurrentWindow)
			:MessageBoxLayer(_ID,title,_w,120,_win),MsgText(msgText)
			{
				PUI_DD[0]<<"Create MessageBoxButton "<<ID<<endl;
				Type=WidgetType_MessageBoxButton;
				new TinyText(0,this,new PosizeEX_Fa6(2,2,20,30,40,50),msgText,-1);//??
				EnableShowTopAreaColor(1);//??
				SetDragbleMode(1);
				MultiWidgets=1;
				CloseTriggerFunc=MsgButtonCloseTriggerFunc;
				CloseFuncData=this;
			}
	};
	
	template <class T> class SingleChoiceList:public Widgets
	{
		public:
			struct SingleChoiceListData
			{
				string Text;
				T funcdata;
				
				SingleChoiceListData(const string &_text,const T &_funcdata)
				:Text(_text),funcdata(_funcdata) {}
				
				SingleChoiceListData() {}
			};
			
		protected:
			vector <SingleChoiceListData> ListData;
			int ListCnt=0,
				FocusChoose=-1,
				stat=0;//0:not focus 1:focus 2:down
			int EachLineHeigth=20;
			bool EnableKeyboardEvent=1;
			LargeLayerWithScrollBar *fa2=NULL;
			void (*func)(T&,int)=NULL;//int:pos
			RGBA TextColor,
				 ButtonColor[3],
				 BorderColor[3];
			
			void InitDefaultColor()
			{
				for (int i=0;i<=2;++i)
					ButtonColor[i]=BorderColor[i]=RGBA_NONE;
				TextColor=RGBA_NONE;
			}
			
			int GetChoosePos(int y)//y is global
			{
				int pos=(y-fa2->GetgPS().y)/EachLineHeigth;
				if (InRange(pos,0,ListCnt-1))
					return pos;
				else return -1;
			}
			
			Posize GetChoicePosize(int p)
			{
				if (p==-1) return ZERO_POSIZE;
				else return Posize(gPS.x,fa2->GetgPS().y+p*EachLineHeigth,gPS.w,EachLineHeigth);
			}
			
			virtual void CheckEvent()
			{
				if (EnableKeyboardEvent)
				{
					const SDL_Event &event=*Win->GetNowSolvingEvent();
					if (event.type==SDL_KEYDOWN)
						switch (event.key.keysym.sym)
						{
							case SDLK_UP:
								if (FocusChoose>0)
									SetSelectLine(FocusChoose-1);
								break;
							case SDLK_DOWN:
								if (FocusChoose<ListCnt-1)
									SetSelectLine(FocusChoose+1);
								break;
							case SDLK_RETURN:
								if (InRange(FocusChoose,0,ListCnt-1))
									if (func!=NULL)
										func(ListData[FocusChoose].funcdata,FocusChoose);
								Win->StopSolvePosEvent();
								SetDelayDeleteWidget(fa2);
								break;
						}
				}
			}
			
			virtual void CheckPos()
			{
				if (!Win->IsNeedSolvePosEvent()) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (!fa2->GetgPS().In(Win->NowPos()))
							SetDelayDeleteWidget(fa2);
						else if (event.button.button==SDL_BUTTON_LEFT)
						{
							Win->SetPresentArea(GetChoicePosize(FocusChoose));
							FocusChoose=GetChoosePos(Win->NowPos().y);
							stat=2;
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(GetChoicePosize(FocusChoose));
							Win->StopSolvePosEvent();
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							if (InRange(FocusChoose,0,ListCnt-1))
							{
								PUI_DD[0]<<"SingleChoiceList "<<ID<<" choose "<<ListData[FocusChoose].Text<<endl;
								if (func!=NULL)
									func(ListData[FocusChoose].funcdata,FocusChoose);
							}
							stat=1;
							Win->StopSolvePosEvent();
							SetDelayDeleteWidget(fa2);
						}
						break;

					case SDL_MOUSEMOTION:
						if (fa2->GetgPS().In(Win->NowPos()))
						{
							if (stat==0)
								stat=1;
							Win->SetPresentArea(GetChoicePosize(FocusChoose));
							FocusChoose=GetChoosePos(Win->NowPos().y);
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(GetChoicePosize(FocusChoose));
							Win->StopSolvePosEvent();
						}
						else 
							if (stat!=0)
							{
								stat=0;
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(GetChoicePosize(FocusChoose));
								FocusChoose=-1;
							}
						break;
				}
			}
			
//			virtual void _SolvePosEvent()
//			{
//				if (Enabled)
//				{
//					if (childWg!=NULL)
//						SolvePosEventOf(childWg);
//					CheckPos();
//				}
//				if (nxtBrother!=NULL)
//					SolvePosEventOf(nxtBrother);
//			}
			
			virtual void Show(Posize &lmt)
			{
				if (ListCnt==0) return;
				int ForL=(-fa->GetrPS().y)/EachLineHeigth,
					ForR=ForL+fa2->GetgPS().h/EachLineHeigth+1;
				ForL=EnsureInRange(ForL,0,ListCnt-1);
				ForR=EnsureInRange(ForR,0,ListCnt-1);
				if (!InRange(FocusChoose,0,ListCnt-1)) FocusChoose=-1;
				Posize RowPs=Posize(gPS.x,gPS.y+ForL*EachLineHeigth,gPS.w,EachLineHeigth);
				for (int i=ForL;i<=ForR;RowPs.y+=EachLineHeigth,++i)
				{
					Win->RenderFillRect(RowPs&lmt,ButtonColor[FocusChoose==i?stat:0]?ButtonColor[FocusChoose==i?stat:0]:ThemeColor[FocusChoose==i?stat*2:0]);
					Win->RenderDrawText(ListData[i].Text,RowPs+Point(8,0),lmt,-1,TextColor?TextColor:ThemeColor.MainTextColor[0]);
					Win->Debug_DisplayBorder(RowPs);
				}	
				Win->RenderDrawRectWithLimit(fa2->GetgPS(),BorderColor[stat]?BorderColor[stat]:ThemeColor[stat*2+1],lmt);
				Win->Debug_DisplayBorder(gPS);
			}

			virtual void CalcPsEx()
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)	
					PsEx->GetrPS(rPS);
				rPS.w=fa2->GetrPS().w;
				rPS.h=EnsureInRange(ListCnt*EachLineHeigth,fa2->GetrPS().h,1e9);
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				fa2->ResizeLL(rPS.w,rPS.h);
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}
			
		public:
			inline void SetTextColor(const RGBA &co)
			{
				TextColor=1;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(fa2->GetgPS());
			}

			inline void SetButtonColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					ButtonColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(fa2->GetgPS());
				}
				else PUI_DD[1]<<"SingleChoiceList:"<<ID<<" SetButtonColor error : p "<<p<<" is not in range[0,2]"<<endl;
			}
			
			inline void SetBorderColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					BorderColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(fa2->GetgPS());
				}
				else PUI_DD[1]<<"SingleChoiceList:"<<ID<<" SetBorderColor error : p "<<p<<" is not in range[0,2]"<<endl;
			}
			
			void SetFunc(void (*_func)(T&,int)=NULL)
			{func=_func;}

			void SetEnableKeyboardEvent(bool en)
			{EnableKeyboardEvent=en;}
			
			void SetEachLineHeigth(int h=20)
			{
				EachLineHeigth=h;
				Win->SetNeedUpdatePosize();
			}

			void SetListContent(int p,const SingleChoiceListData &data)
			{
				p=EnsureInRange(p,0,ListCnt);
				ListData.insert(ListData.begin()+p,data);
				++ListCnt;
				rPS.h=EnsureInRange(ListCnt*EachLineHeigth,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void DeleteListContent(int p)
			{
				if (!ListCnt) return;
				p=EnsureInRange(p,0,ListCnt-1);
				ListData.erase(ListData.begin()+p);
				--ListCnt;
				rPS.h=EnsureInRange(ListCnt*EachLineHeigth,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			SingleChoiceList* PushbackContent(const SingleChoiceListData &data)
			{
				SetListContent(1e9,data);
				return this;
			}
			
			SingleChoiceList* PushbackContent(const string &_text,const T &_funcdata)
			{
				SetListContent(1e9,SingleChoiceListData(_text,_funcdata));
				return this;
			}
			
			void SetSelectLine(int p)
			{
				if (!InRange(p,0,ListCnt-1))
					return;
				FocusChoose=p;
				if (!InRange(p*EachLineHeigth+fa->GetrPS().y,0,fa2->GetrPS().h-EachLineHeigth))
					fa2->SetViewPort(2,p*EachLineHeigth);
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(fa2->GetgPS());
			}
			
			SingleChoiceListData& GetListData(int p)
			{
				p=EnsureInRange(p,0,ListCnt-1);
				return ListData[p];
			}
			
			T& GetFuncData(int p)
			{
				p=EnsureInRange(p,0,ListCnt-1);
				return ListData[p].funcdata;
			}
			
			inline int GetListCnt()
			{return ListCnt;}

			void Close()
			{SetDelayDeleteWidget(fa2);}
			
			virtual void SetrPS(const Posize &ps)
			{fa2->SetrPS(ps);}
			
			~SingleChoiceList()
			{
				RemoveNeedLoseFocus();//??
			}
			
			SingleChoiceList(int _ID,const Posize &_gps,void (*_func)(T&,int)=NULL,PUI_Window *_win=CurrentWindow)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create SingleChoiceList "<<ID<<endl;
				Type=WidgetType_SingleChoiceList;
				fa2=new LargeLayerWithScrollBar(0,_win->MenuLayer(),_gps,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS={0,0,_gps.w,0};
				InitDefaultColor();
				SetNeedLoseFocus();//??
			}
	};
	
	template <class T> class DropDownButton:public Widgets
	{
		public:
			struct DropDownButtonData
			{
				string Text;
				T funcdata;
				
				DropDownButtonData(const string &_text,const T &_funcdata)
				:Text(_text),funcdata(_funcdata) {}
				
				DropDownButtonData() {}
			};
			
		protected:
			string CurrentText;
			vector <DropDownButtonData> ChoiceData;
			int stat=0;//same with button
			void (*func)(T&,int)=NULL;//int:pos
			int CurrentChoose=-1;
			int ChoiceListEachHeight=20;//20 is the default height of SingleChoiceList
			bool AutoChooseWhenOnlyOneChoice=0;
			RGBA TextColor=RGBA_NONE,
				 BackgroundColor=RGBA_NONE,
				 BorderColor[3];
			
			void InitDefaultColor()
			{
				for (int i=0;i<=2;++i)
					BorderColor[i]=RGBA_NONE;
			}
			
			void OpenChoiceList()
			{
				if (ChoiceData.empty())
					return;
				Posize tarPs;
				if (Win->GetWinPS().h-gPS.y2()>ChoiceData.size()*ChoiceListEachHeight)
					tarPs=Posize(gPS.x,gPS.y2()+1,gPS.w,ChoiceData.size()*ChoiceListEachHeight);
				else if (gPS.y>ChoiceData.size()*ChoiceListEachHeight)
					tarPs=Posize(gPS.x,gPS.y-ChoiceData.size()*ChoiceListEachHeight-1,gPS.w,ChoiceData.size()*ChoiceListEachHeight);
				else if ((Win->GetWinPS().h-gPS.y2())*1.5>=gPS.y)
					tarPs=Posize(gPS.x,gPS.y2()+1,gPS.w,Win->GetWinPS().h-gPS.y2());
				else tarPs=Posize(gPS.x,0,gPS.w,gPS.y-1);
				
				auto scl=new SingleChoiceList<DropDownButton<T>*>(0,tarPs,[](DropDownButton *&funcdata,int p)->void{funcdata->SetSelectChoice(p,1);},Win);
				scl->SetEachLineHeigth(ChoiceListEachHeight);
				for (auto &vp:ChoiceData)
					scl->PushbackContent(vp.Text,this);
			}

			virtual void CheckEvent()
			{
				
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							RemoveNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							PUI_DD[0]<<"DropDownButton "<<ID<<" click"<<endl;
							stat=2;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							PUI_DD[0]<<"DropDownButton "<<ID<<" open choice list"<<endl;
							if (ChoiceData.size()==1&&AutoChooseWhenOnlyOneChoice)
								SetSelectChoice(0,1);
							else OpenChoiceList();
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				Win->RenderFillRect(lmt,BackgroundColor?BackgroundColor:ThemeColor.BackgroundColor[0]);
				Win->RenderDrawRectWithLimit(gPS,BorderColor[stat]?BorderColor[stat]:ThemeColor[stat?(stat==1?3:5):1],lmt);
				Win->RenderDrawText(CurrentText,gPS+Point(8,0),lmt,-1,TextColor?TextColor:ThemeColor.MainTextColor[0]);
				Win->Debug_DisplayBorder(gPS);
			}
		
		public:
			void SetTextColor(const RGBA &co)
			{
				TextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetBackgroundColor(const RGBA &co)
			{
				BackgroundColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetBorderColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					BorderColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"DropDownButton "<<ID<<" SetBorderColor error : p "<<p<<" is not in range[0,2]"<<endl;
			}
			
			void SetChoiceData(int p,const DropDownButtonData &data)
			{
				p=EnsureInRange(p,0,ChoiceData.size());
				if (p<=CurrentChoose)
					++CurrentChoose;
				ChoiceData.insert(ChoiceData.begin()+p,data);
			}
			
			void SetChoiceData(int p,const string &_text,const T &_funcdata)
			{SetChoiceData(p,DropDownButtonData(_text,_funcdata));}
			
			void DeleteChoiceData(int p)
			{
				if (ChoiceData.empty()) return;
				p=EnsureInRange(p,0,ChoiceData.size()-1);
				if (p==CurrentChoose) CurrentChoose=-1;
				else if (p<CurrentChoose) --CurrentChoose;
				ChoiceData.erase(ChoiceData.begin()+p);
			}

			void ClearChoiceData()
			{
				if (ChoiceData.empty()) return;
				ChoiceData.clear();
				CurrentChoose=-1;
			}
			
			DropDownButtonData& GetChoiceData(int p)
			{
				if (!ChoiceData.empty())
					return ChoiceData[EnsureInRange(p,0,ChoiceData.size()-1)];
			}

			DropDownButton <T>* PushbackChoiceData(const DropDownButtonData &data)
			{
				SetChoiceData(1e9,data);
				return this;
			}
			
			DropDownButton <T>* PushbackChoiceData(const string &_text,const T &_funcdata)
			{
				SetChoiceData(1e9,DropDownButtonData(_text,_funcdata));
				return this;
			}
			
			inline int GetChoiceCnt()
			{return ChoiceData.size();}

			void SetSelectChoice(int p,bool TrigerFunc)
			{
				if (ChoiceData.empty()) return;
				p=EnsureInRange(p,0,ChoiceData.size()-1);
				CurrentChoose=p;
				CurrentText=ChoiceData[p].Text;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
				if (TrigerFunc)
					if (func!=NULL)
						func(ChoiceData[p].funcdata,p);
			}
			
			void SetCurrentText(const string &_text)
			{
				CurrentText=_text;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline int GetCurrentChoosePos()
			{return CurrentChoose;}
			
			inline DropDownButtonData& GetCurrentChooseData()
			{return ChoiceData[CurrentChoose];}
			
			inline void SetChoiceListEachLineHeight(int h)
			{ChoiceListEachHeight=h;}
			
			inline void SetAutoChooseWhenOnlyOneChoice(bool en)
			{AutoChooseWhenOnlyOneChoice=en;}
			
			inline void SetFunc(void (*_func)(T&,int))
			{func=_func;}
			
			DropDownButton(int _ID,Widgets *_fa,const Posize &_rps,const string &_currentText="",void (*_func)(T&,int)=NULL)
			:CurrentText(_currentText),func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create DropDownButton "<<ID<<endl;
				SetFa(_fa);
				SetrPS(_rps);
				InitDefaultColor();
			}

			DropDownButton(int _ID,Widgets *_fa,PosizeEX *psex,const string &_currentText="",void (*_func)(T&,int)=NULL)
			:CurrentText(_currentText),func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create DropDownButton "<<ID<<endl;
				SetFa(_fa);
				AddPsEx(psex);
				InitDefaultColor();
			}
	};
	
	class TitleMenu:public Widgets
	{
		protected:
			
		public:
			
	};
	
	template <class T> class AddressSection:public Widgets
	{
		public:
			struct AddressSectionData
			{
				friend class AddressSection <T>;
				protected:
					string text;
					int w=0;
				
				public:
					T data;
					
					void SetText(const string &str)
					{
						text=str;
						TTF_SizeUTF8(PUI_Font()(),text.c_str(),&w,NULL);
						w+=6;
					}
					
					inline const string GetText()
					{return text;}
					
					AddressSectionData(const string &str,const T &da)
					:text(str),data(da)
					{
						TTF_SizeUTF8(PUI_Font()(),text.c_str(),&w,NULL);
						w+=6;
					}
			};
			
		protected:
			//  |>|XXX|<|YYYYYYYYY|<|ZZZZ|
			//Defualt used for FileAddress(Section)
			
			int stat=0;//0:Up_NoFocus 1:Up_Focus i 2:LeftClick i
			vector <AddressSectionData> AddrSecData;
			void (*func)(void*,AddressSection*,T&,int)=NULL;//funcData,this,data,NowFocus
			void *funcData=NULL;
			int SectionCnt=0,
				JointWidth=14,
				NowFocus=0;//0:not in +i:in i -i:in i list     //Do remember +-1 when using AddrSecData
			SharedTexturePtr TriangleTex[3][2];
			RGBA BackgroundColor[6],//0:no_focus_Joint 1:focus_Joint 2:down_Joint 3:no_focus_Section 4:focus_Section 5:down_Section
				 JointTriangleColor[3],//0:no_focus_Joint 1:focus_Joint 2:down_Joint
				 TextColor=RGBA_NONE;
			
			int GetChosenSection(int x)
			{
				if (SectionCnt==0) return 0;
				int p=SectionCnt-1,w=0;
				while (p>=0&&w<=gPS.w)
					w+=AddrSecData[p--].w+JointWidth;
				p++;
				while (p<SectionCnt&&x>AddrSecData[p].w+JointWidth)
					x-=AddrSecData[p++].w+JointWidth;
				if (p==SectionCnt) return 0;
				else if (x<=JointWidth) return -p-1;
				else return p+1;
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							NowFocus=0;
							RemoveNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button==SDL_BUTTON_LEFT)
							if ((NowFocus=GetChosenSection(Win->NowPos().x-gPS.x))!=0)
							{
								PUI_DD[0]<<"AddressSection "<<ID<<" click "<<NowFocus<<endl;
								stat=2;
								Win->StopSolvePosEvent();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat==2)
						{
							PUI_DD[0]<<"AddressSection "<<ID<<" function "<<NowFocus<<endl;
							if (func!=NULL)
								func(funcData,this,AddrSecData[abs(NowFocus)-1].data,NowFocus);
							stat=1;
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
							Win->StopSolvePosEvent();
						}
						break;
						
					case SDL_MOUSEMOTION:
						int lastFocus=NowFocus;
						if ((NowFocus=GetChosenSection(Win->NowPos().x-gPS.x))!=lastFocus)
						{
							if (NowFocus==0)
							{
								stat=0;
								RemoveNeedLoseFocus();
							}
							else
							{
								stat=1;
								SetNeedLoseFocus();
							}
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						if (NowFocus!=0)
							Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				if (SectionCnt!=0)
				{
					int p=SectionCnt-1,w=0;
					while (p>=0&&w<=gPS.w)
						w+=AddrSecData[p--].w+JointWidth;
					Posize ps(gPS.x,gPS.y,0,gPS.h);
					for (int i=p+1;i<SectionCnt;++i)
					{
						ps.w=JointWidth;
						int col=stat!=0&&-NowFocus-1==i?stat:0;
						Win->RenderFillRect(ps&lmt,BackgroundColor[col]?BackgroundColor[col]:ThemeColor[2*col+1]);
						Win->RenderCopyWithLmt(TriangleTex[col][i==0](),ps,lmt);
						ps.x+=ps.w;
						
						ps.w=AddrSecData[i].w;
						col=stat!=0&&NowFocus-1==i?stat:0;
						Win->RenderFillRect(ps&lmt,BackgroundColor[col+3]?BackgroundColor[col+3]:ThemeColor[2*(col+1)]);
						Win->RenderDrawText(AddrSecData[i].text,ps,lmt,0,TextColor?TextColor:ThemeColor.MainTextColor[0]);
						ps.x+=ps.w;
					}
				}

				Win->Debug_DisplayBorder(gPS);
			}
			
			void ReCreateTriangleTex()
			{
				if (rPS.h<=0||JointWidth<=0) return;
				for (int i=0;i<=2;++i)
				{
					TriangleTex[i][0]=TriangleTex[i][1]=SharedTexturePtr(NULL);
					TriangleTex[i][0]=SharedTexturePtr(CreateTextureFromSurfaceAndDelete(CreateTriangleSurface(JointWidth,gPS.h,{3,gPS.h>>1},{JointWidth-3,4},{JointWidth-3,gPS.h-4},JointTriangleColor[i])));
					TriangleTex[i][1]=SharedTexturePtr(CreateTextureFromSurfaceAndDelete(CreateTriangleSurface(JointWidth,gPS.h,{3,4},{3,gPS.h-4},{JointWidth-3,gPS.h>>1},JointTriangleColor[i])));
				}
			}
			
		public:
			inline void SetBackgroundColor(int p,const RGBA &co)
			{
				if (InRange(p,0,5))
				{
					BackgroundColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"SetBackgroundColor error : p "<<p<<" is not in range[0,5]"<<endl;
			}
			
			void SetJointTriangleColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					JointTriangleColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
					ReCreateTriangleTex();
				}
				else PUI_DD[1]<<"SetJointTriangleColor error : p "<<p<<" is not in range[0,2]"<<endl;
			}
			
			inline void SetTextColor(const RGBA &co)
			{
				TextColor=co;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void Clear()
			{
				AddrSecData.clear();
				SectionCnt=0;
				stat=0;
				NowFocus=0;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			AddressSection* PushbackSection(const string &str,const T &da)
			{
				AddrSecData.push_back(AddressSectionData(str,da));
				SectionCnt++;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
				return this;
			}
			
			void Popback()
			{
				AddrSecData.pop_back();
				SectionCnt--;
				if (abs(NowFocus)>SectionCnt)
					NowFocus=0;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void PopbackMulti(int cnt)
			{
				if (cnt<=0) return;
				cnt=min(cnt,SectionCnt);
				AddrSecData.erase(AddrSecData.begin()+SectionCnt-cnt,AddrSecData.end());
				SectionCnt-=cnt;
				if (abs(NowFocus)>SectionCnt)
					NowFocus=0;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			T& GetSectionData(int p)
			{return AddrSecData[p].data;}
			
			inline int GetSectionCnt()
			{return SectionCnt;}
			
			void SetJointWidth(int w)
			{
				if (JointWidth==w)
					return;
				JointWidth=w;
				ReCreateTriangleTex();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetFunc(void (*_func)(void*,AddressSection*,T&,int),void *_funcData=NULL)
			{
				func=_func;
				funcData=_funcData;
			}
			
			AddressSection(int _ID,Widgets *_fa,const Posize &_rPS,void (*_func)(void*,AddressSection*,T&,int)=NULL,void *_funcData=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create AddressSection "<<ID<<endl;
				Type=WidgetType_AddressSection;
				SetFa(_fa);
				SetrPS(_rPS);
				for (int i=0;i<=2;++i)
					for (int j=0;j<=1;++j)
						TriangleTex[i][j]=SharedTexturePtr(NULL);
				for (int i=0;i<=6;++i)
					BackgroundColor[i]=RGBA_NONE;
				JointTriangleColor[0]={250,250,250,255};
				JointTriangleColor[1]={240,240,240,255};
				JointTriangleColor[2]={230,230,230,255};
				ReCreateTriangleTex();
			}
			
			AddressSection(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(void*,AddressSection*,T&,int)=NULL,void *_funcData=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create AddressSection "<<ID<<endl;
				Type=WidgetType_AddressSection;
				SetFa(_fa);
				AddPsEx(psex);
				for (int i=0;i<=2;++i)
					for (int j=0;j<=1;++j)
						TriangleTex[i][j]=SharedTexturePtr(NULL);
				for (int i=0;i<=6;++i)
					BackgroundColor[i]=RGBA_NONE;
				JointTriangleColor[0]={250,250,250,255};
				JointTriangleColor[1]={240,240,240,255};
				JointTriangleColor[2]={230,230,230,255};
				ReCreateTriangleTex();
			}
	};

	#ifdef PAL_BASICFUNCTIONS_FILE_CPP
	class FileAddressSection:public AddressSection <string>//the Menu1 was reconstructed as template that this class should change too...
	{
		protected:
			void (*func0)(void*,const string&)=NULL;
			void *funcdata0=NULL;
			
			struct FileAddrSecFuncForMenuData
			{
				FileAddressSection *FileAddrSec=NULL;
				string path,name;
				int pos;
				
				FileAddrSecFuncForMenuData(FileAddressSection *_FileAddrSec,const string &_path,const string &_name,int _pos)
				:FileAddrSec(_FileAddrSec),path(_path),name(_name),pos(_pos) {}
			};
			
			static void FileAddrSecFuncForMenu(void *&_funcdata)
			{
//				PUI_DD[3]<<"FileAddrSecFuncForMenu"<<endl;
				FileAddrSecFuncForMenuData *data=(FileAddrSecFuncForMenuData*)_funcdata;
				if (data->pos==1)
				{
					data->FileAddrSec->Clear();
					data->FileAddrSec->PushbackSection(data->name,data->name);
					if (data->FileAddrSec->func0!=NULL)
						data->FileAddrSec->func0(data->FileAddrSec->funcdata0,data->name);
				}
				else
				{
					data->FileAddrSec->PopbackMulti(data->FileAddrSec->GetSectionCnt()-data->pos+1);
					data->FileAddrSec->PushbackSection(data->name,data->path+"\\"+data->name);
					if (data->FileAddrSec->func0!=NULL)
						data->FileAddrSec->func0(data->FileAddrSec->funcdata0,data->path+"\\"+data->name);
				}
				delete data;
			}
			
			static void FileAddrSecFunc(void *_funcdata,AddressSection <string> *addrSec,string &path,int pos)
			{
//				PUI_DD[3]<<"FileAddrSecFunc. pos "<<pos<<endl;
				FileAddressSection *FileAddrSec=(FileAddressSection*)_funcdata;
				if (pos>0)
				{
					addrSec->PopbackMulti(addrSec->GetSectionCnt()-pos);
					if (FileAddrSec->func0!=NULL)
						FileAddrSec->func0(FileAddrSec->funcdata0,path);
				}
				else
				{
					vector <MenuData<void*> > menudata;
					if (pos==-1)
					{
						for (char panfu[3]="C:";panfu[0]<='Z';++panfu[0])
							menudata.push_back(MenuData<void*>(panfu,FileAddrSecFuncForMenu,new FileAddrSecFuncForMenuData(FileAddrSec,"",panfu,1)));
					}
					else
					{
						string pa=GetPreviousBeforeBackSlash(path);
						vector <string>* allDir=GetAllFile_UTF8(pa,0);
						for (auto vp:*allDir)
							menudata.push_back(MenuData<void*>(vp,FileAddrSecFuncForMenu,new FileAddrSecFuncForMenuData(FileAddrSec,pa,vp,-pos)));
						delete allDir;
					}
					new Menu1<void*>(0,menudata);
				}
			}
			
		public:
			inline void SetFunc(void (*_func0)(void*,const string&),void *_funcdata0=NULL)
			{
				func0=_func0;
				funcdata0=_funcdata0;
			}
			
			void SetAddress(const string &path)
			{
				Clear();
				int p=0,q=0;
				while (q<path.length())
				{
					if (path[q]=='\\')
					{
						PushbackSection(path.substr(p,q-p),path.substr(0,q));
						p=q+1;
					}
					++q;
				}
				PushbackSection(path.substr(p,q-p),path);
			}
			
			FileAddressSection(int _ID,Widgets *_fa,const Posize &_rps,const string &initPath,void (*_func0)(void*,const string&)=NULL,void *_funcdata0=NULL)
			:AddressSection(_ID,_fa,_rps),func0(_func0),funcdata0(_funcdata0)
			{
				PUI_DD[0]<<"Create FileAddressSection "<<ID<<endl;
				Type=WidgetType_FileAddressSection;
				AddressSection<string>::SetFunc(FileAddrSecFunc,this);
				SetAddress(initPath);
			}
			
			FileAddressSection(int _ID,Widgets *_fa,PosizeEX *psex,const string &initPath,void (*_func0)(void*,const string&)=NULL,void *_funcdata0=NULL)
			:AddressSection(_ID,_fa,psex),func0(_func0),funcdata0(_funcdata0)
			{
				PUI_DD[0]<<"Create FileAddressSection "<<ID<<endl;
				Type=WidgetType_FileAddressSection;
				AddressSection<string>::SetFunc(FileAddrSecFunc,this);
				SetAddress(initPath);
			}
	};
	#endif
	
//	class SpinBox:public Widgets
//	{
//		protected:
//			
//			
//		public:
//			
//			
//			
//	};
	
	template <class T> class TextEditLine:public Widgets
	{
		protected:
			stringUTF8 Text,
					   editingText;
			vector <int> ChWidth;
//			vector <RGBA> ChColor;
			bool Editing=0,
				 IntervalTimerOn=0,
				 ShowBorder=1;
			int stat=0,//0:no focus 1:focus 2:down_left 3:down_right
				StateInput=0,
				ChHeight=0,
				SumWidth=0,
				BorderWidth=3,
				LengthLimit=128,
				EditingTextCursorPos,
				CursorPosX=0;
//				LastEditingTimeStamp=0;
			Range pos={0,0},//this condition l>r is allowed //pos1:pos.l pos2:pos.r(also means current cursor)
				  ShowPos={0,0};
//			Uint32 UpdateSposInterval=300;
//			SDL_TimerID IntervalTimerID;
			void (*EnterFunc)(T&,const stringUTF8&,TextEditLine<T>*,bool)=NULL;//bool:0:change 1:enter
		 	T funcData;
			RGBA TextColor[3],
				 BackgroundColor[4],//bg,choosepart stat0,1,2/3
				 BorderColor[4];//stat0,1,2/3 overlimit
			
//			void TurnOnOffIntervalTimer(bool on)
//			{
//				if (IntervalTimerOn==on) return;
//				if (IntervalTimerOn=on)
//					IntervalTimerID=SDL_AddTimer(UpdateSposInterval,PUI_UpdateTimer,new PUI_UpdateTimerData(this,-1));
//				else
//				{
//					SDL_RemoveTimer(IntervalTimerID);
//					IntervalTimerID=0;
//				}
//			}
			
			void SetSposFromAnother(bool fromSposL)//used when one edge of the showPos is set,to calc the other side
			{
				int i,s;
				if (fromSposL)
				{
					for (i=ShowPos.l,s=BorderWidth;i<Text.length()&&s<=rPS.w-BorderWidth;++i)
						s+=ChWidth[i];
					if (s<=rPS.w-BorderWidth) ShowPos.r=Text.length();
					else ShowPos.r=i-1;
				}
				else
				{
					for (i=ShowPos.r-1,s=rPS.w-BorderWidth;i>=0&&s>=BorderWidth;--i)
						s-=ChWidth[i];
					if (s>=BorderWidth) ShowPos.l=0;
					else ShowPos.l=i+2;
				}
			}
		
			void SetSposFromPos2()//adjust ShowPos to ensure pos2 in ShowPos
			{
				if (pos.r<ShowPos.l)
				{
					ShowPos.l=pos.r;
					SetSposFromAnother(1);
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else if (ShowPos.r<pos.r)
				{
					ShowPos.r=pos.r;
					SetSposFromAnother(0);
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
			}
			
			int GetPos(int x)//return char interval index:  0,ch0,1,ch1,2,ch2,3
			{
//				LastPos2X=x;
				if (x<BorderWidth)
				{
					for (int s=BorderWidth,i=0,j=ShowPos.l;j>=0;--j)
					{
						if (x>s-ChWidth[j]/2) return i+ShowPos.l;
						s-=ChWidth[j];
						--i;
						if (x>=s) return max(0,i+ShowPos.l);
					}
					return 0;
				}
				else
				{
					for (int s=BorderWidth,i=0,j=ShowPos.l;j<ChWidth.size();++j)
					{
						if (x<s+ChWidth[j]/2) return i+ShowPos.l;
						s+=ChWidth[j];
						++i;
						if (x<=s) return i+ShowPos.l;
					}
					return Text.length();
				}
			}
		public:
			void ClearText(bool triggerFunc=1)
			{
				SumWidth=0;
				ChWidth.clear();
				Text.clear();
				pos=ShowPos=ZERO_RANGE;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
				if (triggerFunc)
					if (EnterFunc!=NULL)
						EnterFunc(funcData,Text,this,0);
			}
			
			void AddText(int p,const stringUTF8 &strUtf8)//p means the interval index,it will be auto Ensured in range[0,Text.length()]
			{
				p=EnsureInRange(p,0,Text.length());
				int len=min(LengthLimit-Text.length(),strUtf8.length());
				Text.insert(p,strUtf8,0,len);
				vector <int> chw;
				for (int i=0,w,h;i<len;++i)
				{
					TTF_SizeUTF8(PUI_Font()(),strUtf8[i].c_str(),&w,&h);
					chw.push_back(w);
					SumWidth+=w;
					ChHeight=max(ChHeight,h);
				}
				ChWidth.insert(ChWidth.begin()+p,chw.begin(),chw.end());
				pos.l=pos.r=p+len;
				SetSposFromPos2();
				if (EnterFunc!=NULL)
					EnterFunc(funcData,Text,this,0);
			}
			
			void SetText(const stringUTF8 &strUtf8,bool triggerFunc=1)
			{
				ClearText(0);
				Text=strUtf8;
				if (Text.length()>LengthLimit)
					Text.erase(LengthLimit,Text.length()-LengthLimit);
				for (int i=0,w,h;i<Text.length();++i)
				{
					TTF_SizeUTF8(PUI_Font()(),Text[i].c_str(),&w,&h);
					ChWidth.push_back(w);
					SumWidth+=w;
					ChHeight=max(ChHeight,h);
				}
				pos.l=pos.r=Text.length();
//				SetSposFromPos2();//??
//				if (SumWidth>rPS.w-BorderWidth*2)
//				{
//					ShowPos.r=Text.length();
//					SetSposFromAnother(0);
//				}
//				else 
//				{
					ShowPos.l=0;
					SetSposFromAnother(1);
//				}
				if (triggerFunc)
					if (EnterFunc!=NULL)
						EnterFunc(funcData,Text,this,0);
			}
			
			void append(const stringUTF8 &strUtf8)
			{AddText(Text.length(),strUtf8);}
			
			void DeleteText(int p,int len)
			{
				for (int i=p;i<p+len;++i)
					SumWidth-=ChWidth[i];
				ChWidth.erase(ChWidth.begin()+p,ChWidth.begin()+p+len);
				Text.erase(p,len);
				pos.l=pos.r=p;
				if (ShowPos.r>Text.length())//??next 8 lines?
					ShowPos.r=Text.length();
				SetSposFromPos2();
				if (ShowPos.l>Text.length())
				{
					ShowPos.r=Text.length();
					SetSposFromAnother(0);
				}
				if (EnterFunc!=NULL)
					EnterFunc(funcData,Text,this,0);
			}
			
			void DeleteTextBack()
			{
				if (Text.length()==0)
					PUI_DD[1]<<"TextEditLine: "<<ID<<" DeleteTextBack error: Text is empty"<<endl;
				else DeleteText(Text.length()-1,1);
			}
			
			void DeleteTextCursor()
			{
				if (pos.Len0())
					if (pos.r==0) return;
					else DeleteText(pos.r-1,1);
				else DeleteText(min(pos.l,pos.r),abs(pos.Length()));
			}
			
			void AddTextCursor(const stringUTF8 &strUtf8)
			{
				if (!pos.Len0())
					DeleteTextCursor();
				AddText(pos.r,strUtf8);
			}
			
			void SetCursorPos(int p)
			{
				pos.l=pos.r=EnsureInRange(p,0,Text.length());
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}

			void SetCursorPos(const Range &rg)
			{
				pos=rg;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			stringUTF8 GetSelectedTextUTF8()
			{
				if (pos.Len0()) return stringUTF8();
				else return Text.substrUTF8(min(pos.l,pos.r),abs(pos.Length()));
			}
			
			string GetSelectedText()
			{
				if (pos.Len0()) return "";
				else return Text.substr(min(pos.l,pos.r),abs(pos.Length()));
			}
			
			const stringUTF8& GetTextUTF8()
			{return Text;}
			
			string GetText()
			{return Text.cppString();}

			void StopTextInput()
			{
				if (StateInput)
				{
					StateInput=0;
					TurnOnOffIMEWindow(0,Win);
					Editing=0;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
					Win->SetKeyboardInputWg(NULL);
					editingText.clear();
				}
			}
			
			void StartTextInput(const Range &rg)
			{
				SetCursorPos(rg);
				if (!StateInput)
				{
					StateInput=1;
					TurnOnOffIMEWindow(1,Win);
					Win->SetKeyboardInputWg(this);
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
			}
			
			void StartTextInput(int p)
			{
				p=EnsureInRange(p,0,Text.length());
				StartTextInput({p,p});
			}
			
			void StartTextInput()
			{StartTextInput({0,Text.length()});}
			
		protected:
			virtual void ReceiveKeyboardInput()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_TEXTINPUT:
					{
						string str=event.text.text;
						GetRidOfEndChar0(str);
						if (!str.empty())
							AddTextCursor(stringUTF8(str));
						Win->StopSolveEvent();
						break;
					}
					case SDL_TEXTEDITING:
					{
						PUI_DD[0]<<"TextEditLine: "<<ID<<" Editing text: start "<<event.edit.start<<", len "<<event.edit.length<<" "<<event.edit.length<<endl;
						editingText=event.edit.text;
						EditingTextCursorPos=event.edit.start;
//						LastEditingTimeStamp=event.edit.timestamp;
						Win->StopSolveEvent();
						if (!pos.Len0())
							DeleteTextCursor();	
						SetCurrentIMEWindowPos({CursorPosX,gPS.y},gPS,Win);
						break;
					}
				}
				Editing=editingText.length()!=0;
//				if (!Editing) 
//					PUI_DD[0]<<"TextEditLine: "<<ID<<" Not editing"<<endl;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			virtual void CheckEvent()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_KEYDOWN:
						if (StateInput)
						{
							if (!Editing)
							{
								bool StillNeedSolveEvent=0;
//								if (event.key.timestamp-LastEditingTimeStamp>0)//?? 
									switch (event.key.keysym.sym)
									{
										case SDLK_BACKSPACE:
											PUI_DD[0]<<"TextEditLine:"<<ID<<" DeleteTextBack"<<endl;
											DeleteTextCursor();
											Win->SetNeedFreshScreen();
											Win->SetPresentArea(gPS);
											break;
											
										case SDLK_LEFT:
											if (pos.r>0)
											{
												pos.l=--pos.r;
												SetSposFromPos2();
												Win->SetNeedFreshScreen();
												Win->SetPresentArea(gPS);
											}
											break;
										
										case SDLK_RIGHT:
											if (pos.r<Text.length())
											{
												pos.l=++pos.r;
												SetSposFromPos2();
												Win->SetNeedFreshScreen();
												Win->SetPresentArea(gPS);
											}
											break;
											
										case SDLK_RETURN:
										case SDLK_KP_ENTER:
										if (EnterFunc!=NULL)
											EnterFunc(funcData,Text,this,1);
											break;
										
										case SDLK_HOME:
											pos.l=pos.r=0;
											SetSposFromPos2();
											Win->SetNeedFreshScreen();
											Win->SetPresentArea(gPS);
											break;
										
										case SDLK_END:
											pos.l=pos.r=Text.length();
											SetSposFromPos2();
											Win->SetNeedFreshScreen();
											Win->SetPresentArea(gPS);
											break;
											
										case SDLK_v:
											if (event.key.keysym.mod&KMOD_CTRL)
											{
												char *s=SDL_GetClipboardText();
												stringUTF8 str=s;
												SDL_free(s);
												if (str.empty())
													break;
												if (!pos.Len0())
													DeleteTextCursor();
												AddText(pos.r,str);
												SetSposFromPos2();
											}
											break;
										
										case SDLK_c:
											if (event.key.keysym.mod&KMOD_CTRL)
												if (!pos.Len0())
													SDL_SetClipboardText(GetSelectedText().c_str());
											break;
											
										case SDLK_x:
											if (event.key.keysym.mod&KMOD_CTRL)
												if (!pos.Len0())
												{
													SDL_SetClipboardText(GetSelectedText().c_str());
													DeleteTextCursor();
													Win->SetNeedFreshScreen();
													Win->SetPresentArea(gPS);
												}
											break;
											
										case SDLK_z:
											if (event.key.keysym.mod&KMOD_CTRL)
											{
												PUI_DD[2]<<"TextEditLine: "<<ID<<" ctrl z cannot use yet"<<endl;
											}
											break;
										
										case SDLK_a:
											if (event.key.keysym.mod&KMOD_CTRL)
												SetCursorPos({0,Text.length()});
											break;
										
										case SDLK_ESCAPE:
											StateInput=0;
											TurnOnOffIMEWindow(0,Win);
											Editing=0;
											Win->SetNeedFreshScreen();
											Win->SetPresentArea(gPS);
											Win->SetKeyboardInputWg(NULL);
											editingText.clear();
											PUI_DD[0]<<"TextEditLine:"<<ID<<" Stop input"<<endl;
											break;
										default:
											StillNeedSolveEvent=1;
									}
								if (!StillNeedSolveEvent)
									Win->StopSolveEvent(); 
							}
							else Win->StopSolveEvent();
						break;
					}
						
					case SDL_USEREVENT:
						if (event.user.type==PUI_EVENT_UpdateTimer)
							if (event.user.data1==this)
							{
								SetSposFromPos2();
								pos.r=GetPos(Win->NowPos().y-gPS.y);
//								pos2=GetPos(LastPos2X);
							}
						break;
				}
			}
			
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (!CoverLmt.In(Win->NowPos()))
						if (StateInput)
						{
							if (event.type==SDL_MOUSEBUTTONDOWN)
								if (event.button.button==SDL_BUTTON_LEFT)
								{
									stat=0;
									StateInput=0;
									TurnOnOffIMEWindow(0,Win);
									Editing=0;
									Win->SetNeedFreshScreen();
									Win->SetPresentArea(gPS);
									Win->SetKeyboardInputWg(NULL);
									RemoveNeedLoseFocus();
									editingText.clear();
									PUI_DD[0]<<"TextEditLine:"<<ID<<" Stop input"<<endl;
								}
						}
						else if (stat!=0)
						{
							stat=0;
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
							RemoveNeedLoseFocus();
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						PUI_DD[0]<<"TextEditLine "<<ID<<" click"<<endl;
						if (!Editing)
							if (event.button.button==SDL_BUTTON_LEFT)
							{
								stat=2;
								Win->SetOccupyPosWg(this);
								pos.l=pos.r=GetPos(Win->NowPos().x-gPS.x);
								PUI_DD[0]<<"TextEditLine: "<<ID<<" Start  input"<<endl;
							}
							else if (event.button.button==SDL_BUTTON_RIGHT)
							{
								stat=3;
								//Not usable yet
							}
						Win->StopSolvePosEvent();
						Win->SetNeedFreshScreen();
						Win->SetPresentArea(gPS);
						break;
						
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							PUI_DD[0]<<"TextEditLine:"<<ID<<" up"<<endl;
							if (stat==2)
								Win->SetOccupyPosWg(NULL);
							stat=1;
							StateInput=1;
							TurnOnOffIMEWindow(1,Win);
//							Win->KeyboardInputWgState=1;
							Win->SetKeyboardInputWg(this);
//							SDL_Rect rct=PosizeToSDLRect(gPS);
//							SDL_SetTextInputRect(&rct);
//							SDL_StartTextInput();
//							TurnOnOffIntervalTimer(0);
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat>=2)
						{
							if (!Editing)
							{
								pos.r=GetPos(Win->NowPos().x-gPS.x);
//								PUI_DD[3]<<"pos.r "<<pos.r<<endl;
								SetSposFromPos2();
//								if (InRange(Win->NowPos().x,gPS.x+BorderWidth,gPS.x2()-BorderWidth))
//									TurnOnOffIntervalTimer(0);
//								else TurnOnOffIntervalTimer(1);
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
						}
						else if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						Win->StopSolvePosEvent();
						break;
				}
			}

			virtual void Show(Posize &lmt)
			{
				int editingTextLen=-Text.length();
				if (StateInput&&Editing)
				{
					AddText(pos.r,editingText);
					editingTextLen+=Text.length();
					SetCursorPos(pos.r-editingTextLen+EditingTextCursorPos);
				}
				else editingTextLen=0;
				
				Win->RenderFillRect(lmt&gPS,BackgroundColor[0]?BackgroundColor[0]:ThemeColor.BackgroundColor[0]);
				if (ShowBorder)
					Win->RenderDrawRectWithLimit(gPS,BorderColor[Text.length()==LengthLimit?3:EnsureInRange(stat,0,2)]?BorderColor[Text.length()==LengthLimit?3:EnsureInRange(stat,0,2)]:ThemeColor[stat*2+1],lmt);
				int x=BorderWidth,w=0,m=min(pos.l,pos.r),M=max(pos.l,pos.r);
				for (int i=ShowPos.l;i<Text.length()&&i<=ShowPos.r;++i)
					if (i<m) x+=ChWidth[i];
					else if (i>=M) break;
					else w+=ChWidth[i];
				if (StateInput&&pos.Len0())//Draw cursor 
					Win->RenderFillRect(Posize(!Editing?CursorPosX=x+gPS.x:x+gPS.x,BorderWidth+gPS.y,2,rPS.h-BorderWidth*2)&lmt,BackgroundColor[1+EnsureInRange(stat,0,2)]?BackgroundColor[1+EnsureInRange(stat,0,2)]:ThemeColor[2*EnsureInRange(stat,0,2)+1]);
				Win->RenderFillRect(Posize(x+gPS.x,BorderWidth+gPS.y,w,rPS.h-BorderWidth*2)&lmt,BackgroundColor[1+EnsureInRange(stat,0,2)]?BackgroundColor[1+EnsureInRange(stat,0,2)]:ThemeColor[2*EnsureInRange(stat,0,2)+1]);
				
				int i,s;
				if (Text.length()!=0)
					for (i=ShowPos.l,s=BorderWidth;i<Text.length()&&s+ChWidth[i]<=rPS.w-BorderWidth;s+=ChWidth[i++])
					{
						int colorPos=editingTextLen!=0&&InRange(i,pos.r-EditingTextCursorPos,pos.r-EditingTextCursorPos+editingTextLen-1)?2:InRange(i,m,M-1);
						SDL_Texture *tex=Win->CreateRGBATextTexture(Text[i].c_str(),TextColor[colorPos]?TextColor[colorPos]:(colorPos==2?ThemeColor[6]:ThemeColor.MainTextColor[colorPos]));
						Win->RenderCopyWithLmt(tex,Posize(s,rPS.h-ChHeight>>1,ChWidth[i],ChHeight)+gPS,lmt&gPS.Shrink(BorderWidth));
						SDL_DestroyTexture(tex);
					}
				ShowPos.r=i;

				if (StateInput&&Editing)
					DeleteText(pos.r-EditingTextCursorPos,editingTextLen);
				
				Win->Debug_DisplayBorder(gPS);
			}
			
			void InitDefaultColor()
			{
				for (int i=0;i<=2;++i)
					TextColor[i]=BackgroundColor[i]=BorderColor[i]=RGBA_NONE;
				BackgroundColor[3]=RGBA_NONE;
				BorderColor[3]={255,0,0,200};
			}
			
		public:
			inline void SetTextColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					TextColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"TextEditLine: "<<ID<<" SetTextColor p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			inline void SetBackgroundColor(int p,const RGBA &co)
			{
				if (InRange(p,0,3))
				{
					BackgroundColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"TextEditLine: "<<ID<<" SetBackgroundColor p "<<p<<" is not in Range[0,3]"<<endl;
			}
			
			inline void SetBorderColor(int p,const RGBA &co)
			{
				if (InRange(p,0,3))
				{
					BorderColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"TextEditLine: "<<ID<<" SetBorderColor p "<<p<<" is not in Range[0,3]"<<endl;
			}
			
			inline void SetShowBorder(bool bo)
			{
				ShowBorder=bo;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetBorderWidth(int w)
			{
				BorderWidth=w;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetLengthLimit(int len)
			{
				LengthLimit=len;
				if (len<Text.length())
				{
					PUI_DD[2]<<"TextExitLine: "<<ID<<" set LengthLimit less than Text.length() cannot use yet"<<endl;
//					Win->SetNeedFreshScreen();
//					Win->SetPresentArea(gPS);
				}				
			}
			
			inline void SetEnterFunc(void (*_enterfunc)(T&,const stringUTF8&,TextEditLine<T>*,bool),const T &_funcdata)
			{
				EnterFunc=_enterfunc;
				funcData=_funcdata;
			}
			
			inline void SetEnterFuncData(const T &_funcdata)
			{funcData=_funcdata;}
			
			inline T& GetEnterFuncData()
			{return funcData;}

			virtual ~TextEditLine()
			{
				PUI_DD[0]<<"Delete TextEditLine "<<ID<<endl;
//				TurnOnOffIntervalTimer(0);
				if (StateInput)
					Win->SetKeyboardInputWg(NULL),
					TurnOnOffIMEWindow(0,Win);
			}
			
			TextEditLine(int _ID,Widgets *_fa,const Posize &_rps,void (*_enterfunc)(T&,const stringUTF8&,TextEditLine<T>*,bool),const T &_funcdata)
			:EnterFunc(_enterfunc),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TextEditLine: "<<ID<<endl;
				Type=WidgetType_TextEditLine;
				SetFa(_fa);
				SetrPS(_rps);
				InitDefaultColor();
			}
			
			TextEditLine(int _ID,Widgets *_fa,PosizeEX *psex,void (*_enterfunc)(T&,const stringUTF8&,TextEditLine<T>*,bool),const T &_funcdata)
			:EnterFunc(_enterfunc),funcData(_funcdata)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TextEditLine: "<<ID<<endl;
				Type=WidgetType_TextEditLine;
				SetFa(_fa);
				AddPsEx(psex);
				InitDefaultColor();
			}
	};
	
	template <class T> class TextEditBox:public Widgets
	{
		protected:
			struct EachChData
			{
				SharedTexturePtr tex[2];
				int timeCode[2]={0,0};
			};
			
			SharedTexturePtr ASCIItex[128][2];
			int timeCodeOfASCIItex[128][2];
			
			SplayTree <stringUTF8_WithData <EachChData> > Text;
			stringUTF8 editingText;
			vector <int> TextLengthCount;
			stack <int> TextLengthChange;
			int MaxTextLength=0;
			int EachChWidth=12,//chinese character is twice the english character width
				EachLineHeight=24;
			PUI_Font font;
			string FontPath="";
			LargeLayerWithScrollBar *fa2=NULL;
			bool Editing=0,
				 ShowBorder=1,
				 AutoNextLine=0,//realize it ten years later =_=|| 
				 ShowSpecialChar=0,
				 EnableScrollResize=0,
				 EnableEdit=1;
			int stat=0,//0:no focus 1:focus 2:down_left 3:down_right
				StateInput=0,
				BorderWidth=3,
				EditingTextCursorPos,
				NewestCode=1;
			void (*EachChangeFunc)(T&,TextEditBox*,bool)=NULL;//bool:Is inner change//It is not so beautiful...
			T EachChangeFuncdata;
			void (*RightClickFunc)(T&,const Point&,TextEditBox*)=NULL;
			T RightClickFuncData;
			Point Pos=ZERO_POINT,Pos_0=ZERO_POINT,//This pos(_0) is not related to pixels
				  EditingPosL=ZERO_POINT,EditingPosR=ZERO_POINT,
				  LastCursorPos=ZERO_POINT;//this is related to pixels
			RGBA TextColor[3],
				 BackgroundColor[4],//bg,choosepart stat0,1,2/3
				 BorderColor[3];//stat0,1,2/3
			
			void InitDefualtColor()
			{
				for (int i=0;i<=2;++i)
					TextColor[i]=BackgroundColor[i]=BorderColor[i]=RGBA_NONE;
				BackgroundColor[3]=RGBA_NONE;
			}
			
			inline bool CheckWidth(const string &str)
			{return str.length()>=2;}
			
			int GetChLenOfEachWidth(const string &str)
			{
				if (str=="\t") return 4;
				return 1+CheckWidth(str);
			}
			
			int GetStrLenOfEachWidth(const stringUTF8_WithData <EachChData> &strUtf8)
			{
				int re=0;
				for (int i=0;i<strUtf8.length();++i)
					re+=GetChLenOfEachWidth(strUtf8[i]);
				return re;
			}
			
			int GetChWidth(const string &str)
			{
				if (str=="\t") return 4*EachChWidth;
				return EachChWidth+CheckWidth(str)*EachChWidth;
			}
			
			inline int StrLenWithoutSlashNR(const stringUTF8_WithData <EachChData> &strUtf8)
			{
				if (strUtf8.length()==0) return 0;
				if (strUtf8[strUtf8.length()-1]=="\n")
					if (strUtf8.length()>=2&&strUtf8[strUtf8.length()-2]=="\r")
						return strUtf8.length()-2;
					else return strUtf8.length()-1;
				else if (strUtf8[strUtf8.length()-1]=="\r")
					if (strUtf8.length()>=2&&strUtf8[strUtf8.length()-2]=="\n")
						return strUtf8.length()-2;
					else return strUtf8.length()-1;
				else return strUtf8.length();
			}
			
			inline Point EnsurePosValid(const Point &pt)
			{return Point(EnsureInRange(pt.x,0,StrLenWithoutSlashNR(Text[pt.y])),EnsureInRange(pt.y,0,Text.size()-1));}
			
			int GetTextLength(int line,int len)
			{
				stringUTF8_WithData <EachChData> &strUtf8=Text[line];
				int w=0;
				for (int i=0;i<len;++i)
					w+=GetChWidth(strUtf8[i]);
				return w;
			}
			
			Point GetPos(const Point &pt)
			{
				int i=0,w,LineY=EnsureInRange((pt.y-gPS.y)/EachLineHeight,0,Text.size()-1),
					x=pt.x-gPS.x-BorderWidth;
				stringUTF8_WithData <EachChData> &strUtf8=Text[LineY];
				int strlen=StrLenWithoutSlashNR(strUtf8);
				while (i<strlen)
				{
					w=GetChWidth(strUtf8[i]);
					if (x<=w/2) return Point(i,LineY);
					++i;
					x-=w;
				}
				return Point(i,LineY);
			}
			
			void SetShowPosFromPos()//??
			{
				if (BorderWidth+EachLineHeight*Pos.y<-fa->GetrPS().y)
					fa2->SetViewPort(2,BorderWidth+EachLineHeight*Pos.y);
				else if (BorderWidth+EachLineHeight*(Pos.y+1)>-fa->GetrPS().y+fa2->GetrPS().h-fa2->GetButtomBarEnableState()*fa2->GetScrollBarWidth())
					fa2->SetViewPort(2,BorderWidth+EachLineHeight*(Pos.y+1)-fa2->GetrPS().h+fa2->GetButtomBarEnableState()*fa2->GetScrollBarWidth());
				int w=GetTextLength(Pos.y,Pos.x);
				if (BorderWidth+w<-fa->GetrPS().x+EachChWidth)
					fa2->SetViewPort(1,BorderWidth+w-EachChWidth);
				else if (BorderWidth+w>-fa->GetrPS().x+fa2->GetgPS().w-fa2->GetRightBarEnableState()*fa2->GetScrollBarWidth()-EachChWidth)
					fa2->SetViewPort(1,BorderWidth+w-fa2->GetgPS().w+fa2->GetRightBarEnableState()*fa2->GetScrollBarWidth()+EachChWidth);
			}
			
			void ResizeTextBoxHeight()
			{
				fa2->ResizeLL(0,EachLineHeight*Text.size()+2*BorderWidth);
				rPS.h=max(fa2->GetrPS().h,EachLineHeight*Text.size()+2*BorderWidth);
			}
			
			void ApplyTextLenChange()
			{
//				PUI_DD[3]<<"TextEditBox "<<ID<<" ApplyTextLenChange Start"<<endl;
				int newMaxLen=MaxTextLength;
				while (!TextLengthChange.empty())
				{
					int len=TextLengthChange.top();
					TextLengthChange.pop();
					if (len>=0)
					{
						if (len>=TextLengthCount.size())
							TextLengthCount.resize(len+1,0);
						TextLengthCount[len]++;
						newMaxLen=max(newMaxLen,len);
					}
					else TextLengthCount[-len]--;
				}
				
				if (TextLengthCount[newMaxLen]==0)
					for (int i=newMaxLen-1;i>=0;--i)
						if (TextLengthCount[i]>0)
						{
							newMaxLen=i;
							break;
						}
				if (newMaxLen!=MaxTextLength)
				{
					MaxTextLength=newMaxLen;
					fa2->ResizeLL(MaxTextLength*EachChWidth+BorderWidth*2,0);
					rPS.w=max(fa2->GetrPS().w,MaxTextLength*EachChWidth+BorderWidth*2);
				}
//				PUI_DD[3]<<"TextEditBox "<<ID<<" ApplyTextLenChange End"<<endl;
			}
			
			void AddTextLenCnt(int len)
			{
//				PUI_DD[3]<<"TextEditBox "<<ID<<" AddTextLenCnt Start "<<len<<endl;
				TextLengthChange.push(len);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" AddTextLenCnt End"<<endl;
			}
			
			void SubtractTextLenCnt(int len)
			{
//				PUI_DD[3]<<"TextEditBox "<<ID<<" SubtractTextLenCnt Start "<<len<<endl;
				TextLengthChange.push(-len);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" SubtractTextLenCnt End"<<endl;
			}

			virtual void CalcPsEx()
			{
				if (PsEx!=NULL)
					PsEx->GetrPS(rPS);
				Posize lastPs=gPS;
				fa2->ResizeLL(MaxTextLength*EachChWidth+BorderWidth*2,0);//??
				rPS.w=max(fa2->GetrPS().w,MaxTextLength*EachChWidth+BorderWidth*2);
				ResizeTextBoxHeight();
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
				Posize fa2gPS=fa2->GetgPS();//??
				if (fa2->GetRightBarEnableState())
					fa2gPS.w-=fa2->GetScrollBarWidth();
				if (fa2->GetButtomBarEnableState())
					fa2gPS.h-=fa2->GetScrollBarWidth();
				CoverLmt=gPS&GetFaCoverLmt()&fa2gPS;
			}
			
//			int MaintainNearbyLines(int p,int cnt)
//			{
//				
//			}

			bool InSelectedRange(const Point &pt0,const Point &pt1,const Point &pt2)//[L,R)
			{
				if (pt1==pt2) return 0;
				if (pt1.y==pt2.y)
					return pt0.y==pt1.y&&InRange(pt0.x,min(pt1.x,pt2.x),max(pt1.x,pt2.x)-1);
				else
					if (InRange(pt0.y,min(pt1.y,pt2.y),max(pt1.y,pt2.y)))
						if (pt1.y<pt2.y)
							if (pt0.y==pt1.y)
								return pt0.x>=pt1.x;
							else if (pt0.y==pt2.y)
								return pt0.x<pt2.x;
							else return 1;
						else
							if (pt0.y==pt2.y)
								return pt0.x>=pt2.x;
							else if (pt0.y==pt1.y)
								return pt0.x<pt1.x;
							else return 1;
					else return 0;
			}
		
			void _Clear(bool isInnerChange)
			{
				if (!EnableEdit&&isInnerChange) return;
				Text.clear();
				editingText.clear();
				TextLengthCount.clear();
				MaxTextLength=0;
				Text.insert(0,stringUTF8("\r\n"));
				AddTextLenCnt(2);
				Pos=Pos_0={0,0};
				ApplyTextLenChange();
				ResizeTextBoxHeight();
				SetShowPosFromPos();
				Editing=0;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
				if (EachChangeFunc!=NULL)
					EachChangeFunc(EachChangeFuncdata,this,isInnerChange);
			}
			
			void _AddNewLine(int p,const stringUTF8 &strUtf8,bool isInnerChange)
			{
				if (strUtf8.length()==0||!EnableEdit&&isInnerChange) return;
//				PUI_DD[3]<<"TextEditBox "<<ID<<" AddNewLine Start"<<endl;
				int i=0,j=0;
				while (i<strUtf8.length())
				{
					if (strUtf8[i]=="\r")
					{
						if (i<strUtf8.length()-1&&strUtf8[i+1]=="\n")
							++i;
						stringUTF8 substr=strUtf8.substrUTF8(j,i-j+1);
						Text.insert(p++,substr);
						AddTextLenCnt(GetStrLenOfEachWidth(substr));
						j=i+1;
					}
					else if (strUtf8[i]=="\n")
					{
						if (i<strUtf8.length()-1&&strUtf8[i+1]=="\r")
							++i;
						stringUTF8 substr=strUtf8.substrUTF8(j,i-j+1);
						Text.insert(p++,substr);
						AddTextLenCnt(GetStrLenOfEachWidth(substr));
						j=i+1;
					}
					++i;
				}
				if (j<strUtf8.length())
				{
					stringUTF8 substr=strUtf8.substrUTF8(j,strUtf8.length()-j)+stringUTF8("\r\n");
					Text.insert(p++,substr);
					AddTextLenCnt(GetStrLenOfEachWidth(substr));
				}
				Pos=Pos_0={StrLenWithoutSlashNR(Text[p-1]),p-1};
				ApplyTextLenChange();
				ResizeTextBoxHeight();
				SetShowPosFromPos();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
				if (EachChangeFunc!=NULL)
					EachChangeFunc(EachChangeFuncdata,this,isInnerChange);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" AddNewLine End"<<endl;
			}
			
			void _AddText(Point pt,const stringUTF8 &strUtf8,bool isInnerChange)
			{
//				PUI_DD[3]<<"TextEditBox "<<ID<<" AddText Start"<<endl;
				if (strUtf8.length()==0||!EnableEdit&&isInnerChange) return;
				if (InRange(pt.y,0,Text.size()-1))
				{
					stringUTF8_WithData <EachChData> &str2=Text[pt.y];
					stringUTF8_WithData <EachChData> str3=str2.substrUTF8_WithData(pt.x,str2.length()-pt.x);
					SubtractTextLenCnt(GetStrLenOfEachWidth(str2));
					str2.erase(pt.x,str2.length()-pt.x);
					AddTextLenCnt(GetStrLenOfEachWidth(str2));
					
					int i=0,j=0;
					while (i<strUtf8.length())
					{
						if (strUtf8[i]=="\r")
						{
							if (i<strUtf8.length()-1&&strUtf8[i+1]=="\n")
								++i;
							if (j==0)
							{
								SubtractTextLenCnt(GetStrLenOfEachWidth(str2));
								str2.append(strUtf8.substrUTF8(j,i-j+1));
								AddTextLenCnt(GetStrLenOfEachWidth(str2));
								pt.y++;
								pt.x=0;
							}
							else
							{
								stringUTF8 substr=strUtf8.substrUTF8(j,i-j+1);
								Text.insert(pt.y++,substr);
								AddTextLenCnt(GetStrLenOfEachWidth(substr));
							}
							j=i+1;
						}
						else if (strUtf8[i]=="\n")
						{
							if (i<strUtf8.length()-1&&strUtf8[i+1]=="\r")
								++i;
							if (j==0)
							{
								SubtractTextLenCnt(GetStrLenOfEachWidth(str2));
								str2.append(strUtf8.substrUTF8(j,i-j+1));
								AddTextLenCnt(GetStrLenOfEachWidth(str2));
								pt.y++;
								pt.x=0;
							}
							else
							{
								stringUTF8 substr=strUtf8.substrUTF8(j,i-j+1);
								Text.insert(pt.y++,substr);
								AddTextLenCnt(GetStrLenOfEachWidth(substr));
							}
							j=i+1;
						}
						++i;
					}
					if (j<strUtf8.length())
						if (j==0)
						{
							SubtractTextLenCnt(GetStrLenOfEachWidth(str2));
							Pos=Pos_0={str2.length()+strUtf8.length(),pt.y};
							str2.append(strUtf8);
							str2.append(str3);
							AddTextLenCnt(GetStrLenOfEachWidth(str2));
						}
						else
						{
							stringUTF8_WithData <EachChData>  str4=strUtf8.substrUTF8(j,strUtf8.length()-j);
							str4.append(str3);
							Text.insert(pt.y,str4);
							AddTextLenCnt(GetStrLenOfEachWidth(str4));
							Pos=Pos_0={StrLenWithoutSlashNR(Text[pt.y]),pt.y};
							ResizeTextBoxHeight();
						}
					else
					{
						Text.insert(pt.y,str3);
						AddTextLenCnt(GetStrLenOfEachWidth(str3));
						Pos=Pos_0={StrLenWithoutSlashNR(Text[pt.y]),pt.y};
						ResizeTextBoxHeight();
					}
					ApplyTextLenChange();
					SetShowPosFromPos();
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
					if (EachChangeFunc!=NULL)
						EachChangeFunc(EachChangeFuncdata,this,isInnerChange);
				}
				else _AddNewLine(EnsureInRange(pt.y,0,Text.size()),strUtf8,isInnerChange);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" AddText End"<<endl;
			}
			
			void _SetText(const stringUTF8 &strUtf8,bool isInnerChange)
			{
				if (!EnableEdit&&isInnerChange) return;
				_Clear(isInnerChange);
				_AddText({0,0},strUtf8,isInnerChange);
			}
			
			void _AppendNewLine(const stringUTF8 &strUtf8,bool isInnerChange)
			{_AddNewLine(Text.size(),strUtf8,isInnerChange);}
			
			void _DeleteLine(int p,bool isInnerChange)
			{
//				PUI_DD[3]<<"TextEditBox "<<ID<<" DeleteLine Start"<<endl;
				if (!EnableEdit&&isInnerChange) return;
				p=EnsureInRange(p,0,Text.size()-1);
				SubtractTextLenCnt(GetStrLenOfEachWidth(Text[p]));
				Text.erase(p);
				Pos=p==0?Point(0,0):Point(p-1,StrLenWithoutSlashNR(Text[p-1]));
				Pos=Pos_0=EnsurePosValid(Pos);
				ResizeTextBoxHeight();
				ApplyTextLenChange();
				SetShowPosFromPos();
				if (EachChangeFunc!=NULL)
					EachChangeFunc(EachChangeFuncdata,this,isInnerChange);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" DeleteLine End"<<endl;
			}
			
			void _DeleteText(Point pt1,Point pt2,bool isInnerChange)
			{
				if (pt1==pt2||!EnableEdit&&isInnerChange) return;
//				PUI_DD[3]<<"TextEditBox "<<ID<<" DeleteText Start"<<endl;
				if (pt1.y==pt2.y)
				{
					if (pt1.x>pt2.x)
						swap(pt1,pt2);
					Pos=pt1;
					stringUTF8_WithData <EachChData>  &strUtf8=Text[pt1.y];
					SubtractTextLenCnt(GetStrLenOfEachWidth(strUtf8));
					strUtf8.erase(pt1.x,pt2.x-pt1.x);
					AddTextLenCnt(GetStrLenOfEachWidth(strUtf8));
				}
				else
				{
					if (pt1.y>pt2.y)
						swap(pt1,pt2);
					Pos=pt1;
					if (InRange(pt1.x,0,Text[pt1.y].length()-1))
					{
						stringUTF8_WithData <EachChData>  &strUtf8=Text[pt1.y];
						SubtractTextLenCnt(GetStrLenOfEachWidth(strUtf8));
						strUtf8.erase(pt1.x,strUtf8.length()-pt1.x);
						AddTextLenCnt(GetStrLenOfEachWidth(strUtf8));
					}
					if (InRange(pt2.x,1,StrLenWithoutSlashNR(Text[pt2.y])))
					{
						stringUTF8_WithData <EachChData> &strUtf8=Text[pt2.y];
						SubtractTextLenCnt(GetStrLenOfEachWidth(strUtf8));
						strUtf8.erase(0,pt2.x);
						AddTextLenCnt(GetStrLenOfEachWidth(strUtf8));
					}
					stringUTF8_WithData <EachChData> &strUtf8=Text[pt1.y];
					SubtractTextLenCnt(GetStrLenOfEachWidth(strUtf8));
					strUtf8.append(Text[pt2.y]);
					AddTextLenCnt(GetStrLenOfEachWidth(strUtf8));
					SubtractTextLenCnt(GetStrLenOfEachWidth(Text[pt2.y]));
					Text.erase(pt2.y);
					for (int i=pt2.y-1;i>=pt1.y+1;--i)//It is necessary use --i that the index in SplayTree would change after erase
					{
						SubtractTextLenCnt(GetStrLenOfEachWidth(Text[i]));
						Text.erase(i);
					}
					ResizeTextBoxHeight();
				}
				Pos=Pos_0=EnsurePosValid(Pos);
				ApplyTextLenChange();
				SetShowPosFromPos();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);//...
				if (EachChangeFunc!=NULL)
					EachChangeFunc(EachChangeFuncdata,this,isInnerChange);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" DeleteText End"<<endl;
			}
			
			void _DeleteTextBack(int len,bool isInnerChange)
			{
//				PUI_DD[3]<<"TextEditBox "<<ID<<" DeleteTextBack Start"<<endl;
				if (!EnableEdit&&isInnerChange) return;
				Point pt=Pos;
				if (len>Pos.x)
				{
					len-=Pos.x+1;
					pt.y--;
					if (pt.y<0)
						pt={0,0};
					else
					{
						while (len>StrLenWithoutSlashNR(Text[pt.y])&&pt.y>=1)
							len-=StrLenWithoutSlashNR(Text[pt.y--])+1;
						pt.x=max(0,StrLenWithoutSlashNR(Text[pt.y])-len);
					}
				}
				else pt.x-=len;
				_DeleteText(EnsurePosValid(pt),Pos,isInnerChange);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" DeleteTextBack End"<<endl;
			}
			
			void _DeleteTextCursor(bool isInnerChange)
			{
//				PUI_DD[3]<<"TextEditBox "<<ID<<" DeleteTextCursor Start"<<endl;
				if (Pos==Pos_0)
					_DeleteTextBack(1,isInnerChange);
				else _DeleteText(Pos,Pos_0,isInnerChange);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" DeleteTextCursor End"<<endl;
			}
			
			void _AddTextCursor(const stringUTF8 &strUtf8,bool isInnerChange)
			{
//				PUI_DD[3]<<"TextEditBox "<<ID<<" AddTextCursor Start"<<endl;
				if (!(Pos==Pos_0))
					_DeleteTextCursor(isInnerChange);
				_AddText(Pos,strUtf8,isInnerChange);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" AddTextCursor End"<<endl;
			}
		
		public:
			inline void Clear()
			{_Clear(0);}
			
			inline void AddNewLine(int p,const stringUTF8 &strUtf8)
			{_AddNewLine(p,strUtf8,0);}
			
			inline void AddText(const Point &pt,const stringUTF8 &strUtf8)
			{_AddText(pt,strUtf8,0);}
			
			inline void SetText(const stringUTF8 &strUtf8)
			{_SetText(strUtf8,0);}
			
			inline void AppendNewLine(const stringUTF8 &strUtf8)
			{_AppendNewLine(strUtf8,0);}
			
			inline void DeleteLine(int p)
			{_DeleteLine(p,0);}
			
			inline void DeleteText(const Point &pt1,const Point &pt2)
			{_DeleteText(pt1,pt2,0);}
			
			inline void DeleteTextBack(int len=1)
			{_DeleteTextBack(len,0);}
			
			inline void DeleteTextCursor()
			{_DeleteTextCursor(0);}
			
			inline void AddTextCursor(const stringUTF8 &strUtf8)
			{_AddTextCursor(strUtf8,0);}
			
			void MoveCursorPos(int delta)//?? 
			{
				if (Text.size()==0) return;
//				PUI_DD[3]<<"TextEditBox "<<ID<<" MoveCursorPos Start "<<delta<<endl;
				if (delta>0)
					if (delta>StrLenWithoutSlashNR(Text[Pos.y])-Pos.x)
					{
						delta-=StrLenWithoutSlashNR(Text[Pos.y])-Pos.x+1;
						Pos.y++;
						if (Pos.y>=Text.size())
							Pos={StrLenWithoutSlashNR(Text[Text.size()-1]),Text.size()-1};
						else
						{
							while (delta>StrLenWithoutSlashNR(Text[Pos.y])&&Pos.y<Text.size()-1)
								delta-=StrLenWithoutSlashNR(Text[Pos.y++])+1;
							Pos.x=min(delta,StrLenWithoutSlashNR(Text[Pos.y]));
						}
					}
					else Pos.x+=delta;
				else
					if ((delta=-delta)>Pos.x)
					{
						delta-=Pos.x+1;
						Pos.y--;
						if (Pos.y<0)
							Pos={0,0};
						else
						{
							while (delta>StrLenWithoutSlashNR(Text[Pos.y])&&Pos.y>0)
								delta-=StrLenWithoutSlashNR(Text[Pos.y--])+1;
							Pos.x=max(0,StrLenWithoutSlashNR(Text[Pos.y])-delta);
						}
					}
					else Pos.x-=delta;
				Pos_0=Pos;
				SetShowPosFromPos();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
//				PUI_DD[3]<<"TextEditBox "<<ID<<" MoveCursorPos End "<<delta<<endl;
			}
			
			void MoveCursorPosUpDown(int downDelta)
			{
				downDelta=EnsureInRange(downDelta,-Pos.y,Text.size()-1-Pos.y);
				if (downDelta==0) return;
				int w=GetTextLength(Pos.y,Pos.x);
				Pos.y+=downDelta;
				Pos.x=0;
				stringUTF8_WithData <EachChData> &strUtf8=Text[Pos.y];
				int strlen=StrLenWithoutSlashNR(strUtf8);
				for (int i=0;i<strlen&&w>0;Pos.x=++i)
					w-=GetChWidth(strUtf8[i]);
				Pos_0=Pos;
				SetShowPosFromPos();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetCursorPos(const Point &pt)
			{
				Pos=Pos_0=EnsurePosValid(pt);
				SetShowPosFromPos();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);//...
			}
			
			void SetCursorPos(const Point &pt1,const Point &pt2)
			{
				Pos=pt1;
				Pos_0=pt2;
				SetShowPosFromPos();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			stringUTF8 GetSelectedTextUTF8()
			{
				stringUTF8 re;
				if (Pos==Pos_0)
					return re;
				Point pt1=Pos,pt2=Pos_0;
				if (pt1.y>pt2.y||pt1.y==pt2.y&&pt1.x>pt2.x)
					swap(pt1,pt2);
				if (pt1.y==pt2.y)
					re.append(Text[pt1.y].substrUTF8(pt1.x,pt2.x-pt1.x));
				else
				{
					re.append(Text[pt1.y].substrUTF8(pt1.x,Text[pt1.y].length()-pt1.x));
					for (int i=pt1.y+1;i<=pt2.y-1;++i)
						re.append(Text[i].StringUTF8());
					re.append(Text[pt2.y].substrUTF8(0,pt2.x));
				}
				return re;	
			}
			
			string GetSelectedText()
			{return GetSelectedTextUTF8().cppString();}
			
			stringUTF8 GetLineUTF8(int p)
			{return Text[EnsureInRange(p,0,Text.size()-1)].StringUTF8();}
			
			string GetLine(int p)
			{return Text[EnsureInRange(p,0,Text.size()-1)].cppString();}
			
			stringUTF8 GetAllTextUTF8()
			{
				stringUTF8 re;
				for (int i=0;i<Text.size();++i)
					re.append(Text[i].StringUTF8());
				return re;
			}
			
			string GetAllText()
			{return GetAllTextUTF8().cppString();}
			
			void SetEachCharSize(int w,int h)
			{
				EachChWidth=w;
				EachLineHeight=h;
				fa2->ResizeLL(MaxTextLength*EachChWidth+BorderWidth*2,EachLineHeight*Text.size()+2*BorderWidth);
				rPS.w=max(fa2->GetrPS().w,MaxTextLength*EachChWidth+BorderWidth*2);
				rPS.h=max(fa2->GetrPS().h,EachLineHeight*Text.size()+2*BorderWidth);
				SetShowPosFromPos();
			}
			
			inline void SetFontSize(int size)
			{
				if (font.Size==size) return;
				font.Size=size;
				++NewestCode;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetFont(const PUI_Font &tar)
			{
				font=tar;
				++NewestCode;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline int GetLinesCount()
			{return Text.size();}
			
			inline Point GetCursorPos()
			{return Pos;}
			
		protected:
			virtual void ReceiveKeyboardInput()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_TEXTINPUT:
					{
						string str=event.text.text;
						GetRidOfEndChar0(str);
						if (!str.empty())
						{
							if (!(EditingPosL==EditingPosR)) 
							{
								Point oldPos=EditingPosL;//It's very ugly =_=|| 
								_DeleteText(EditingPosL,EditingPosR,1);
								SetCursorPos(oldPos);
								EditingPosL=EditingPosR=ZERO_POINT;
							}
							_AddTextCursor(stringUTF8(str),1);
						}
						Win->StopSolveEvent();
						break;
					}
					case SDL_TEXTEDITING:
					{
						PUI_DD[0]<<"TextEditBox: "<<ID<<" Editing text: start "<<event.edit.start<<", len "<<event.edit.length<<endl;
						editingText=event.edit.text;
						EditingTextCursorPos=event.edit.start;
						Win->StopSolveEvent();
						
						Editing=editingText.length()!=0;
						if (Editing)
						{
							if (!(Pos==Pos_0))
								_DeleteTextCursor(1);
							if (!(EditingPosL==EditingPosR))
								_DeleteText(EditingPosL,EditingPosR,1);
							EditingPosL=Pos;
							_AddTextCursor(editingText,1);
							EditingPosR=Pos;
							MoveCursorPos(EditingTextCursorPos-editingText.length());
						}
						else if (!(EditingPosL==EditingPosR)) 
						{
							Point oldPos=EditingPosL;//It's very ugly =_=|| 
							_DeleteText(EditingPosL,EditingPosR,1);
							SetCursorPos(oldPos);
							EditingPosL=EditingPosR=ZERO_POINT;
						}
						SetCurrentIMEWindowPos(LastCursorPos,Posize(gPS.x,LastCursorPos.y,gPS.w,EachLineHeight),Win);
						break;
					}
				}
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			virtual void CheckEvent()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				switch (event.type)
				{
					case SDL_KEYDOWN:
						if (StateInput)
						{
							if (!Editing)
							{
								bool flag=1;
								switch (event.key.keysym.sym)
								{
									case SDLK_BACKSPACE: _DeleteTextCursor(1);	break;
									case SDLK_LEFT:	MoveCursorPos(-1);	break;
									case SDLK_RIGHT: MoveCursorPos(1);	break;
									case SDLK_UP: MoveCursorPosUpDown(-1);	break;
									case SDLK_DOWN: MoveCursorPosUpDown(1);	break;
									case SDLK_PAGEUP: MoveCursorPosUpDown(-fa2->GetrPS().h/EachLineHeight);	break;
									case SDLK_PAGEDOWN:	MoveCursorPosUpDown(fa2->GetrPS().h/EachLineHeight);break;
									case SDLK_TAB: _AddTextCursor("\t",1);	break;
									case SDLK_RETURN:
										if (Pos.x>=StrLenWithoutSlashNR(Text[Pos.y]))
											_AddNewLine(Pos.y+1,"\r\n",1);
										else 
										{
											Point pt=Pos;
											_AddNewLine(Pos.y+1,Text[Pos.y].substr(Pos.x,StrLenWithoutSlashNR(Text[Pos.y])-Pos.x),1);
											_DeleteText(pt,Point(StrLenWithoutSlashNR(Text[pt.y]),pt.y),1);
											Pos_0=Pos={0,pt.y+1};
											Win->SetNeedFreshScreen();
											Win->SetPresentArea(gPS);
										}
										break;
									
									case SDLK_v:
										if (event.key.keysym.mod&KMOD_CTRL)
										{
											char *s=SDL_GetClipboardText();
											stringUTF8 str=s;
											SDL_free(s);
											if (str.empty())
												break;
											_AddTextCursor(str,1);
										}
										break;
									
									case SDLK_c:
										if (event.key.keysym.mod&KMOD_CTRL)
											if (!(Pos==Pos_0))
												SDL_SetClipboardText(GetSelectedText().c_str());
										break;
										
									case SDLK_x:
										if (event.key.keysym.mod&KMOD_CTRL)
											if (!(Pos==Pos_0))
											{
												SDL_SetClipboardText(GetSelectedText().c_str());
												_DeleteTextCursor(1);
											}
										break;
									
									case SDLK_z:
										if (event.key.keysym.mod&KMOD_CTRL)
										{
											PUI_DD[2]<<"TextEditBox: "<<ID<<" ctrl z cannot use yet"<<endl;
										}
										break;
									
									case SDLK_a:
										if (event.key.keysym.mod&KMOD_CTRL)
											SetCursorPos({StrLenWithoutSlashNR(Text[Text.size()-1]),Text.size()-1},{0,0});
										break;
									
									case SDLK_ESCAPE:
										StateInput=0;
										TurnOnOffIMEWindow(0,Win);
										Editing=0;
										Win->SetNeedFreshScreen();
										Win->SetPresentArea(gPS);
										Win->SetKeyboardInputWg(NULL);
										editingText.clear();
										PUI_DD[0]<<"TextEditBox:"<<ID<<" Stop input"<<endl;
										break;
									
									default:
										flag=0;
								}
								if (flag)
									Win->StopSolveEvent();
							}
							else Win->StopSolveEvent();
						}
						break;
					
					case SDL_MOUSEWHEEL:
						if (EnableScrollResize)
							if (SDL_GetModState()&KMOD_CTRL)
							{
								double lambda=1+event.wheel.y*0.1;
								int h=EnsureInRange(EachLineHeight*lambda,10,160),w=h/2;
								SetEachCharSize(w,h);
								SetFontSize(h*0.75);
								Win->StopSolveEvent();
							}
						break;
				}
			}
		
			virtual void CheckPos()
			{
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (!CoverLmt.In(Win->NowPos()))
						if (StateInput)
						{
							if (event.type==SDL_MOUSEBUTTONDOWN)
								if (event.button.button==SDL_BUTTON_LEFT)
								{
									stat=0;
									StateInput=0;
									TurnOnOffIMEWindow(0,Win);
									Editing=0;
									Win->SetNeedFreshScreen();
									Win->SetPresentArea(gPS);
									Win->SetKeyboardInputWg(NULL);
									editingText.clear();
									RemoveNeedLoseFocus();
									PUI_DD[0]<<"TextEditBox:"<<ID<<" Stop input"<<endl;
								}
						}
						else if (stat!=0)
						{
							stat=0;
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
							RemoveNeedLoseFocus();
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						PUI_DD[0]<<"TextEditBox "<<ID<<" click"<<endl;
						if (!Editing)
							if (event.button.button==SDL_BUTTON_LEFT)
							{
								stat=2;
								Pos=Pos_0=GetPos(Win->NowPos());
								SetShowPosFromPos();
								Win->SetOccupyPosWg(this);
								PUI_DD[0]<<"TextEditBox "<<ID<<": Start  input"<<endl;
							}
							else if (event.button.button==SDL_BUTTON_RIGHT)
							{
								stat=3;
								if (RightClickFunc!=NULL)
								{
									RightClickFunc(RightClickFuncData,GetPos(Win->NowPos()),this);
									PUI_DD[0]<<"TextEditBox "<<ID<<": RightClickFunc"<<endl;
								}
							}
						Win->StopSolvePosEvent();
						Win->SetNeedFreshScreen();
						Win->SetPresentArea(gPS);
						break;
						
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							PUI_DD[0]<<"TextEditBox "<<ID<<": up"<<endl;
							if (stat==2)
								Win->SetOccupyPosWg(NULL);
							stat=1;
							StateInput=1;
							TurnOnOffIMEWindow(1,Win);
							SDL_Rect rct=PosizeToSDLRect(gPS);
							SDL_SetTextInputRect(&rct);
							Win->SetKeyboardInputWg(this);
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						break;
						
					case SDL_MOUSEMOTION:
						if (stat>=2)
						{
							if (!Editing)
							{
								Pos=GetPos(Win->NowPos());
								SetShowPosFromPos();
								Win->SetNeedFreshScreen();
								Win->SetPresentArea(gPS);
							}
						}
						else if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
							Win->SetPresentArea(gPS);
						}
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
//				Point EditingPosL,EditingPosR;
//				
//				if (StateInput&&Editing)
//				{
//					EditingPosL=Pos;
//					AddText(Pos,editingText);
//					EditingPosR=Pos;
//					MoveCursorPos(EditingTextCursorPos-editingText.length());
//				}
				
				Win->RenderFillRect(lmt&gPS,BackgroundColor[0]?BackgroundColor[0]:ThemeColor.BackgroundColor[0]);
				if (ShowBorder)
					Win->RenderDrawRectWithLimit(fa2->GetgPS(),BorderColor[EnsureInRange(stat,0,2)]?BorderColor[EnsureInRange(stat,0,2)]:ThemeColor[EnsureInRange(stat,0,2)*2+1],lmt);
				
				int ForL=-fa->GetrPS().y/EachLineHeight,
					ForR=ForL+fa2->GetgPS().h/EachLineHeight+1;
				ForL=EnsureInRange(ForL,0,Text.size()-1);
				ForR=EnsureInRange(ForR,0,Text.size()-1);
				
				Posize CharPs=Posize(gPS.x+BorderWidth,gPS.y+BorderWidth+ForL*EachLineHeight,EachChWidth,EachLineHeight);
				for (int i=ForL,flag=0;i<=ForR;++i)
				{
					stringUTF8_WithData <EachChData> &strUtf8=Text[i];
					for (int j=0;j<strUtf8.length();++j)
					{
						string str=strUtf8[j];
						CharPs.w=GetChWidth(str);
						if ((CharPs&lmt).Size()!=0)
						{
							if (Editing)
							{
								if (Pos==Point(j,i)&&StateInput)
									Win->RenderFillRect(Posize(CharPs.x-1,CharPs.y,2,CharPs.h)&lmt,BackgroundColor[1+EnsureInRange(stat,0,2)]?BackgroundColor[1+EnsureInRange(stat,0,2)]:ThemeColor[2*EnsureInRange(stat,0,2)+1]);
								flag=InSelectedRange({j,i},EditingPosL,EditingPosR);
							}
							else if (Pos==Pos_0)
							{
								if (Pos==Point(j,i)&&StateInput)
									Win->RenderFillRect(Posize(LastCursorPos.x=CharPs.x-1,LastCursorPos.y=CharPs.y,2,CharPs.h)&lmt,BackgroundColor[1+EnsureInRange(stat,0,2)]?BackgroundColor[1+EnsureInRange(stat,0,2)]:ThemeColor[2*EnsureInRange(stat,0,2)+1]);
							}
							else flag=InSelectedRange({j,i},Pos,Pos_0);
							
							if (!Editing&&flag)
								Win->RenderFillRect(CharPs&lmt,BackgroundColor[1+EnsureInRange(stat,0,2)]?BackgroundColor[1+EnsureInRange(stat,0,2)]:ThemeColor[2*EnsureInRange(stat,0,2)+1]);
							
							SDL_Texture *tex=NULL;
							int chCol=flag?(Editing?2:1):0;
							if (chCol==2)
								Win->RenderDrawText(str,CharPs,lmt,0,TextColor[2]?TextColor[2]:ThemeColor[6],font);
							else
							{
								RGBA &co=TextColor[chCol]?TextColor[chCol]:ThemeColor.MainTextColor[chCol];
								if (str=="\r")
									if (ShowSpecialChar)
									{
										if (timeCodeOfASCIItex['\r'][chCol]!=NewestCode||!ASCIItex['\r'][chCol])
										{
											ASCIItex['\r'][chCol]=SharedTexturePtr(Win->CreateRGBATextTexture("\\r",co,font));
											timeCodeOfASCIItex['\r'][chCol]=NewestCode;
										}
										tex=ASCIItex['\r'][chCol]();
									}
									else tex=NULL;
								else if (str=="\n")
									if (ShowSpecialChar)
									{
										if (timeCodeOfASCIItex['\n'][chCol]!=NewestCode||!ASCIItex['\n'][chCol])
										{
											ASCIItex['\n'][chCol]=SharedTexturePtr(Win->CreateRGBATextTexture("\\n",co,font));
											timeCodeOfASCIItex['\n'][chCol]=NewestCode;
										}
										tex=ASCIItex['\n'][chCol]();
									}
									else tex=NULL;
								else if (str=="\t")
									if (ShowSpecialChar)
									{
										if (timeCodeOfASCIItex['\t'][chCol]!=NewestCode||!ASCIItex['\t'][chCol])
										{
											ASCIItex['\t'][chCol]=SharedTexturePtr(Win->CreateRGBATextTexture("\\t",co,font));
											timeCodeOfASCIItex['\t'][chCol]=NewestCode;
										}
										tex=ASCIItex['\t'][chCol]();
									}
									else tex=NULL;
								else
								{
									if (strUtf8(j).timeCode[chCol]!=NewestCode||!strUtf8(j).tex[chCol])
										if (str.length()==1&&str[0]>0)
										{
											if (timeCodeOfASCIItex[str[0]][chCol]!=NewestCode||!ASCIItex[str[0]][chCol])
											{
												ASCIItex[str[0]][chCol]=SharedTexturePtr(Win->CreateRGBATextTexture(str.c_str(),co,font));
												timeCodeOfASCIItex[str[0]][chCol]=NewestCode;
											}
											strUtf8(j).tex[chCol]=ASCIItex[str[0]][chCol];
											strUtf8(j).timeCode[chCol]=NewestCode;
										}
										else
										{
											strUtf8(j).tex[chCol]=SharedTexturePtr(Win->CreateRGBATextTexture(str.c_str(),co,font));
											strUtf8(j).timeCode[chCol]=NewestCode;
										}
									tex=strUtf8(j).tex[chCol]();
								}
								if (tex!=NULL)
									Win->RenderCopyWithLmt_Centre(tex,CharPs,lmt);
							}
						}
						CharPs.x+=CharPs.w;
					}
					
					if (Editing)
					{
						if (Pos==Point(strUtf8.length(),i))
							Win->RenderFillRect(Posize(CharPs.x-1,CharPs.y,2,CharPs.h)&lmt,BackgroundColor[1+EnsureInRange(stat,0,2)]?BackgroundColor[1+EnsureInRange(stat,0,2)]:ThemeColor[2*EnsureInRange(stat,0,2)+1]);
					}
					else if (Pos==Pos_0)
						if (Pos==Point(strUtf8.length(),i))
							Win->RenderFillRect(Posize(CharPs.x-1,CharPs.y,2,CharPs.h)&lmt,BackgroundColor[1+EnsureInRange(stat,0,2)]?BackgroundColor[1+EnsureInRange(stat,0,2)]:ThemeColor[2*EnsureInRange(stat,0,2)+1]);
					
					CharPs.x=gPS.x+BorderWidth;
					CharPs.y+=EachLineHeight;
				}
				
//				if (StateInput&&Editing)
//				{
//					MoveCursorPos(editingText.length()-EditingTextCursorPos+editingText.length());
//					DeleteTextBack(editingText.length());
//				}

				Win->Debug_DisplayBorder(gPS);
			}
		
		public:
			inline void SetTextColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					TextColor[p]=co;
					++NewestCode;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"TextEditBox "<<ID<<": SetTextColor p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			inline void SetBackgroundColor(int p,const RGBA &co)
			{
				if (InRange(p,0,3))
				{
					BackgroundColor[p]=co;
					++NewestCode;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"TextEditBox "<<ID<<": SetBackgroundColor p "<<p<<" is not in Range[0,3]"<<endl;
			}
			
			inline void SetBorderColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					BorderColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"TextEditBox "<<ID<<": SetBorderColor p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			inline void EnableShowBorder(bool en)
			{
				if (ShowBorder==en) return;
				ShowBorder=en;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void SetBorderWidth(int w)
			{
				BorderWidth=w;
				fa2->ResizeLL(MaxTextLength*EachChWidth+BorderWidth*2,EachLineHeight*Text.size()+2*BorderWidth);
				rPS.w=max(fa2->GetrPS().w,MaxTextLength*EachChWidth+BorderWidth*2);
				rPS.h=max(fa2->GetrPS().h,EachLineHeight*Text.size()+2*BorderWidth);
				SetShowPosFromPos();
			}
			
			inline void EnableShowSpecialChar(bool en)
			{
				if (en==ShowSpecialChar) return;
				ShowSpecialChar=en;
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			inline void SetEachChangeFunc(void (*_eachchangefunc)(T&,TextEditBox*,bool))
			{EachChangeFunc=_eachchangefunc;}
			
			inline void SetEachChangeFuncData(const T &eachchangefuncdata)
			{EachChangeFuncdata=eachchangefuncdata;}
			
			inline void SetRightClickFunc(void (*_rightClickFunc)(T&,const Point&,TextEditBox*))
			{RightClickFunc=_rightClickFunc;}
			
			inline void SetRightClickFuncData(const T &rightclickfuncdata)
			{RightClickFuncData=rightclickfuncdata;}
			
			inline void SetEnableScrollResize(bool enable)
			{EnableScrollResize=enable;}
			
			inline void SetEnableEdit(bool enable)
			{EnableEdit=enable;}
			
			void AddPsEx(PosizeEX *psex)
			{fa2->AddPsEx(psex);}
			
			virtual void SetrPS(const Posize &ps)
			{fa2->SetrPS(ps);}
			
			inline LargeLayerWithScrollBar* GetFa2()
			{return fa2;}
			
			TextEditBox(int _ID,Widgets *_fa,PosizeEX *psex)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TextEditBox: "<<ID<<endl;
				Type=WidgetType_TextEditBox;
				fa2=new LargeLayerWithScrollBar(0,_fa,psex,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS=ZERO_POSIZE;
				TextLengthCount.resize(10,0);
				memset(timeCodeOfASCIItex,0,sizeof timeCodeOfASCIItex);
				_Clear(1);
				InitDefualtColor();
				font.Size=18;
			}
			
			TextEditBox(int _ID,Widgets *_fa,const Posize &_rps)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create TextEditBox: "<<ID<<endl;
				Type=WidgetType_TextEditBox;
				fa2=new LargeLayerWithScrollBar(0,_fa,_rps,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS={0,0,_rps.w,_rps.h};
				TextLengthCount.resize(10,0);
				memset(timeCodeOfASCIItex,0,sizeof timeCodeOfASCIItex);
				_Clear(1);
				InitDefualtColor();
				font.Size=18;
			}
	};
	
	class TerminalTextBox:public Widgets
	{
		protected:
			
			
		public:
			
	};
	
	class LayerForListViewTemplate:public Widgets
	{
		template <class T> friend class ListViewTemplate;
		protected:
			Widgets *ActualFather=NULL;
			
			virtual void CalcPsEx()
			{
				Posize lastPs=gPS;
				gPS=rPS;
				CoverLmt=gPS&ActualFather->GetCoverLmt();
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}
		
			LayerForListViewTemplate(int _ID,Widgets *actFa):ActualFather(actFa)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create LayerForListViewTemplate "<<ID<<" ActualFather "<<actFa->GetID()<<endl;
				Win=actFa->GetWin();
				Depth=actFa->GetDepth()+1;
				Type=WidgetType_LayerForListViewTemplate;
			}
	};
	
	template <class T> class ListViewTemplate:public Widgets
	{
		protected:
			struct EachRowColor
			{
				RGBA co[3];
				
				RGBA& operator [] (int p)
				{return co[p];}
				
				EachRowColor()
				{co[0]=co[1]=co[2]=RGBA_NONE;}
			};
			
			int ListCnt=0;
			vector <LayerForListViewTemplate*> LineLayer;
			vector <T> FuncData;
			void (*func)(T&,int,int)=NULL;//int1:Pos(CountFrom 0)   int2: 0:None 1:Left_Click 2:Left_Double_Click 3:Right_Click
			T BackgroundFuncData;
			LargeLayerWithScrollBar *fa2=NULL;
			int stat=0,//0:Up_NoFocus 1:Up_Focus_Row 2:Down_Left_TwiceClick 3:Down_Right 4:Down_Left_OnceClick
				FocusChoose=-1,
				ClickChoose=-1;
			int EachHeight=30,
				Interval=2;
			bool EnableKeyboardEvent=0;
			vector <EachRowColor> RowColor;
			EachRowColor DefaultColor;
			
			inline Posize GetLinePosize(int pos)
			{
				if (pos==-1) return ZERO_POSIZE;
				else return Posize(gPS.x,gPS.y+pos*(EachHeight+Interval),gPS.w,EachHeight);
			}
			
			inline int GetLineFromPos(int y)
			{
				int re=(Win->NowPos().y-gPS.y)/(EachHeight+Interval);
				if ((y-gPS.y)%(EachHeight+Interval)<EachHeight&&InRange(re,0,ListCnt-1))
					return re;
				else return -1;
			}
			
				 
			virtual void CheckEvent()
			{
				if (EnableKeyboardEvent)
				{
					
				}
			}
			
			virtual void CheckPos()
			{
				if (ListCnt==0||!Win->IsNeedSolvePosEvent()) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							if (FocusChoose!=-1)
							{
								Win->SetPresentArea(GetLinePosize(FocusChoose));
								FocusChoose=-1;
								Win->SetNeedFreshScreen();
							}
							RemoveNeedLoseFocus();
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						if (ClickChoose!=-1)
							Win->SetPresentArea(GetLinePosize(ClickChoose));
						if (FocusChoose!=-1)
							Win->SetPresentArea(GetLinePosize(FocusChoose));
						ClickChoose=FocusChoose=GetLineFromPos(Win->NowPos().y);
						if (event.button.button==SDL_BUTTON_LEFT)
						{
							if (event.button.clicks==2)
								stat=2;
							else stat=4;
						}
						else if (event.button.button==SDL_BUTTON_RIGHT)
							stat=3;
						else stat=4,PUI_DD[1]<<"ListViewTemplate "<<ID<<": Unknown click button,use it as left click once"<<endl;
						Win->StopSolvePosEvent();
						Win->SetNeedFreshScreen();
						if (ClickChoose!=-1)
							Win->SetPresentArea(GetLinePosize(ClickChoose));
						if (FocusChoose!=-1)
							Win->SetPresentArea(GetLinePosize(FocusChoose));
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							PUI_DD[0]<<"ListViewTemplate "<<ID<<" func "<<ClickChoose<<endl;
							if (func!=NULL)
								func(ClickChoose!=-1?FuncData[ClickChoose]:BackgroundFuncData,ClickChoose,stat==4?1:stat);
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
						}
						break;
					
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							Win->SetNeedFreshScreen();
						}
						else
						{
							int pos=(Win->NowPos().y-gPS.y)/(EachHeight+Interval);
							if ((Win->NowPos().y-gPS.y)%(EachHeight+Interval)<EachHeight&&InRange(pos,0,ListCnt-1))
							{
								if (pos!=FocusChoose)
								{
									if (FocusChoose!=-1)
										Win->SetPresentArea(Posize(gPS.x,gPS.y+FocusChoose*(EachHeight+Interval),gPS.w,EachHeight));
									Win->SetPresentArea(Posize(gPS.x,gPS.y+pos*(EachHeight+Interval),gPS.w,EachHeight));
									FocusChoose=pos;
									Win->SetNeedFreshScreen();
								}
							}
							else
								if (FocusChoose!=-1)
								{
									Win->SetPresentArea(Posize(gPS.x,gPS.y+FocusChoose*(EachHeight+Interval),gPS.w,EachHeight));
									FocusChoose=-1;
									Win->SetNeedFreshScreen();
								}
						}
						Win->StopSolvePosEvent();
						break;	
				}
			}

			virtual void Show(Posize &lmt)
			{
				if (ListCnt==0) return;
				Range ForLR=GetShowRange();
				if (!InRange(ClickChoose,0,ListCnt-1)) ClickChoose=-1;
				if (!InRange(FocusChoose,0,ListCnt-1)) FocusChoose=-1;
				Posize RowPs=Posize(gPS.x,gPS.y+ForLR.l*(EachHeight+Interval),gPS.w,EachHeight);
				for (int i=ForLR.l;i<=ForLR.r;RowPs.y+=EachHeight+Interval,++i)
					Win->RenderFillRect(RowPs&lmt,RowColor[i][ClickChoose==i?2:FocusChoose==i]?RowColor[i][ClickChoose==i?2:FocusChoose==i]:(DefaultColor[ClickChoose==i?2:FocusChoose==i]?DefaultColor[ClickChoose==i?2:FocusChoose==i]:ThemeColor[ClickChoose==i?4:(FocusChoose==i?2:0)])),
					Win->Debug_DisplayBorder(RowPs);
				Win->Debug_DisplayBorder(gPS);
			}

			virtual void CalcPsEx()//??
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)	
					PsEx->GetrPS(rPS);
				rPS.w=fa2->GetrPS().w;
				rPS.h=EnsureInRange(ListCnt*(EachHeight+Interval)-Interval,fa2->GetrPS().h,1e9);
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				fa2->ResizeLL(rPS.w,rPS.h);
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}

			virtual void _SolveEvent()
			{
				if (!Win->IsNeedSolveEvent())
					return;
				if (nxtBrother!=NULL)
					SolveEventOf(nxtBrother);
				if (!Enabled) return;
				if (childWg!=NULL)
					SolveEventOf(childWg);
				if (ListCnt>0)
				{
					Range ForLR=GetShowRange();
					for (int i=ForLR.l;i<=ForLR.r;++i)
						SolveEventOf(LineLayer[i]);
				}
				if (Win->IsNeedSolveEvent())
					CheckEvent();
			}
			
			virtual void _SolvePosEvent()
			{
				if (!Win->IsNeedSolvePosEvent())
					return;
				if (Enabled)
					if (gPS.In(Win->NowPos()))
					{
						if (childWg!=NULL)
							SolvePosEventOf(childWg);
						if (ListCnt>0)
						{
							Range ForLR=GetShowRange();
							for (int i=ForLR.l;i<=ForLR.r;++i)
								SolvePosEventOf(LineLayer[i]);
						}
						if (Win->IsNeedSolvePosEvent())
							CheckPos();
					}
				if (nxtBrother!=NULL)
					SolvePosEventOf(nxtBrother);
			}
			
			virtual void _PresentWidgets(Posize lmt)
			{
				if (nxtBrother!=NULL)
					PresentWidgetsOf(nxtBrother,lmt);
				if (Enabled)
				{
					lmt=lmt&gPS;
					Show(lmt);
					if (DEBUG_EnableWidgetsShowInTurn)
						SDL_Delay(DEBUG_WidgetsShowInTurnDelayTime),
						SDL_RenderPresent(Win->GetSDLRenderer());
					if (ListCnt>0)
					{
						Range ForLR=GetShowRange();
						for (int i=ForLR.l;i<=ForLR.r;++i)
							PresentWidgetsOf(LineLayer[i],lmt);
					}
					if (childWg!=NULL)
						PresentWidgetsOf(childWg,lmt);
				}
			}

			virtual void _UpdateWidgetsPosize()
			{
				if (nxtBrother!=NULL)
					UpdateWidgetsPosizeOf(nxtBrother);
				if (!Enabled) return;
				CalcPsEx();
				if (childWg!=NULL)
					UpdateWidgetsPosizeOf(childWg);
				if (ListCnt>0)
				{
					Range ForLR=GetShowRange();
					Posize RowPs=Posize(gPS.x,gPS.y+ForLR.l*(EachHeight+Interval),gPS.w,EachHeight);
					for (int i=ForLR.l;i<=ForLR.r;RowPs.y+=EachHeight+Interval,++i)
					{
						LineLayer[i]->SetrPS(RowPs);
						UpdateWidgetsPosizeOf(LineLayer[i]);
					}
				}
			}
			
		public:
			inline void SetRowColor(int pos,int p,const RGBA &co)
			{
				if (InRange(pos,0,ListCnt-1)&&InRange(p,0,2))
				{
					RowColor[pos][p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(GetLinePosize(pos));
				}
				else PUI_DD[1]<<"ListViewTemplate "<<ID<<": SetRowColor: pos or p is not in range"<<endl;
			}
			
			inline void SetDefaultRowColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					DefaultColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"ListViewTemplate "<<ID<<": SetDefaultRowColor: p "<<p<<" is not in Range[0,2]"<<endl;
			}
			
			void SetBackGroundColor(const RGBA &co)
			{fa2->SetLargeAreaColor(co);}

			inline void SetRowHeightAndInterval(int _height,int _interval)
			{
				EachHeight=_height;
				Interval=_interval;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}
			
			inline void SetListFunc(void (*_func)(T&,int,int))
			{func=_func;}
			
			void SetBackgroundFuncData(const T &data)
			{BackgroundFuncData=data;}
			
			Widgets* SetListContent(int p,const T &_funcdata,const EachRowColor &co)
			{
				p=EnsureInRange(p,0,ListCnt);
				FuncData.insert(FuncData.begin()+p,_funcdata);
				RowColor.insert(RowColor.begin()+p,co);
				LayerForListViewTemplate *re=new LayerForListViewTemplate(0,this);
				LineLayer.insert(LineLayer.begin()+p,re);
				++ListCnt;
				if (ClickChoose>=p) ++ClickChoose;
				rPS.h=EnsureInRange(ListCnt*(EachHeight+Interval)-Interval,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);//?? It don't need fresh such big area
				return re;
			}
			
			Widgets* SetListContent(int p,const T &_funcdata)
			{return SetListContent(p,_funcdata,EachRowColor());}
			
			void DeleteListContent(int p)
			{
				if (!ListCnt) return;
				p=EnsureInRange(p,0,ListCnt-1);
				FuncData.erase(FuncData.begin()+p);
				RowColor.erase(RowColor.begin()+p);
				delete LineLayer[p];
				LineLayer.erase(LineLayer.begin()+p);
				--ListCnt;
				if (FocusChoose==ListCnt)
					FocusChoose=-1;
				if (ClickChoose>p) --ClickChoose;
				else if (ClickChoose==p) ClickChoose=-1;
				rPS.h=EnsureInRange(ListCnt*(EachHeight+Interval)-Interval,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void ClearListContent()
			{
				if (!ListCnt) return;
				FocusChoose=ClickChoose=-1;
				FuncData.clear();
				RowColor.clear();
				for (auto vp:LineLayer)
					delete vp;
				LineLayer.clear();
				ListCnt=0;
				rPS.h=fa2->GetrPS().h;
				fa2->ResizeLL(0,rPS.h);
				fa2->SetViewPort(1,0);
				fa2->SetViewPort(2,0);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			Widgets* PushbackContent(const T &_funcdata,const EachRowColor &co)
			{return SetListContent(1e9,_funcdata,co);}
			
			Widgets* PushbackContent(const T &_funcdata)
			{return SetListContent(1e9,_funcdata,EachRowColor());}
			
			void SwapContent(int pos1,int pos2)
			{
				if (!InRange(pos1,0,ListCnt-1)||!InRange(pos2,0,ListCnt-1)||pos1==pos2) return;
				swap(LineLayer[pos1],LineLayer[pos2]);
				swap(FuncData[pos1],FuncData[pos2]);
				swap(RowColor[pos1],RowColor[pos2]);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(GetLinePosize(pos1)|GetLinePosize(pos2));
			}
			
			int Find(const T &_funcdata)
			{
				for (int i=0;i<ListCnt;++i)
					if (FuncData[i]==_funcdata)
						return i;
				return -1;
			}
			
			void SetSelectLine(int p)
			{
				if (!InRange(p,-1,ListCnt-1))
					return;
				if (p>=0&&!InRange(p*(Interval+EachHeight)+fa->GetrPS().y,0,fa2->GetrPS().h-Interval-EachHeight))
					fa2->SetViewPort(2,p*(Interval+EachHeight));
				else Win->SetPresentArea((GetLinePosize(p)|GetLinePosize(ClickChoose))&CoverLmt);
				ClickChoose=p;
				Win->SetNeedFreshScreen();
			}
			
			inline int GetListCnt()
			{return ListCnt;}
			
			Range GetShowRange()
			{
				Range re;
				re.l=(-fa->GetrPS().y)/(EachHeight+Interval);
				re.r=re.l+fa2->GetgPS().h/(EachHeight+Interval)+1;
				re.l=EnsureInRange(re.l,0,ListCnt-1);
				re.r=EnsureInRange(re.r,0,ListCnt-1);
				return re;
			}
			
			T& GetFuncData(int p)
			{
				p=EnsureInRange(p,0,ListCnt-1);
				return FuncData[p];
			}
			
			T& operator [] (int p)
			{
				p=EnsureInRange(p,0,ListCnt-1);
				return FuncData[p];
			}
			
			Widgets* GetLineLayer(int p)
			{
				if (ListCnt==0) 
					return NULL;
				p=EnsureInRange(p,0,ListCnt-1);
				return LineLayer[p];
			}
			
			Widgets* operator () (int p)
			{return GetLineLayer(p);} 
			
			void AddPsEx(PosizeEX *psex)//?? Need virtual?
			{fa2->AddPsEx(psex);}
			
			virtual void SetrPS(const Posize &ps)
			{fa2->SetrPS(ps);}
			
			~ListViewTemplate()
			{ClearListContent();}
			
			ListViewTemplate(int _ID,Widgets *_fa,const Posize &_rps,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create ListViewTemplate "<<ID<<endl;
				Type=WidgetType_ListViewTemplate;
				fa2=new LargeLayerWithScrollBar(0,_fa,_rps,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS={0,0,_rps.w,0};
			}
			
			ListViewTemplate(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create ListViewTemplate "<<ID<<endl;
				Type=WidgetType_ListViewTemplate;
				fa2=new LargeLayerWithScrollBar(0,_fa,psex,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS=ZERO_POSIZE;
			}
	};
	
	class LayerForBlockViewTemplate:public Widgets
	{
		template <class T> friend class BlockViewTemplate;
		protected:
			Widgets *ActualFather=NULL;
			
			virtual void CalcPsEx()
			{
				Posize lastPs=gPS;
				gPS=rPS;
				CoverLmt=gPS&ActualFather->GetCoverLmt();
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}
		
			LayerForBlockViewTemplate(int _ID,Widgets *actFa):ActualFather(actFa)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create LayerForBlockViewTemplate "<<ID<<" ActualFather "<<actFa->GetID()<<endl;
				Win=actFa->GetWin();
				Depth=actFa->GetDepth()+1;
				Type=WidgetType_LayerForBlockViewTemplate;
			}
	};
	
	template <class T> class BlockViewTemplate:public Widgets
	{
		public:
			struct BlockViewData
			{
				friend class BlockViewTemplate;
				protected:
					LayerForBlockViewTemplate *lay=NULL;
					RGBA co[3];
					
				public:
					T FuncData;
					
					inline LayerForBlockViewTemplate* operator () ()
					{return lay;}
					
					inline RGBA operator [] (int p)
					{return co[p];}
					
					BlockViewData(const T &funcdata,const RGBA &co0,const RGBA &co1,const RGBA &co2)
					:FuncData(funcdata)
					{
						co[0]=co0;
						co[1]=co1;
						co[2]=co2;
					}
					
					BlockViewData(const T &funcdata):FuncData(funcdata)
					{co[0]=co[1]=co[2]=RGBA_NONE;}
					
					BlockViewData()
					{co[0]=co[1]=co[2]=RGBA_NONE;}
			};
		
		protected:
			vector <BlockViewData> BlockData; 
			int BlockCnt=0;
			void (*func)(T&,int,int)=NULL;//int1:Pos(CountFrom 0)   int2: 0:None 1:Left_Click 2:Left_Double_Click 3:Right_Click
			T BackgroundFuncData;
			LargeLayerWithScrollBar *fa2=NULL;
			int stat=0,//0:Up_NoFocus 1:Up_Focus_Block 2:Down_Left_TwiceClick 3:Down_Right 4:Down_Left_OnceClick
				FocusChoose=-1,
				ClickChoose=-1,
				EachLineBlocks=1;//update this after changing rPS or EachPs
			Posize EachPs={5,5,240,80};//min(w,h) is the pic edge length, if w>=h ,the text will be show on right,else on buttom
			RGBA BlockColor[3];//NoFocusRow,FocusBlock,ClickBlock
			bool EnableKeyboardEvent=0;
			RGBA DefaultBlockColor[3];
			
			void InitDefaultBlockColor()
			{
				for (int i=0;i<=2;++i)
					DefaultBlockColor[i]=RGBA_NONE;
			}
			
			Posize GetBlockPosize(int p)//get specific Block Posize
			{
				if (p==-1) return ZERO_POSIZE;
				return Posize(gPS.x+p%EachLineBlocks*(EachPs.x+EachPs.w),gPS.y+p/EachLineBlocks*(EachPs.y+EachPs.h),EachPs.w,EachPs.h);
			}

			int InBlockPosize(const Point &pt)//return In which Posize ,if not exist,return -1
			{
				int re=(pt.y-gPS.y+EachPs.y)/(EachPs.y+EachPs.h)*EachLineBlocks+(pt.x-gPS.x+EachPs.x)/(EachPs.x+EachPs.w);
				if (InRange(re,0,BlockCnt-1)&&(GetBlockPosize(re)&CoverLmt).In(pt))
					return re;
				else return -1;
			}

			virtual void CheckEvent()
			{
				
			}
			
			virtual void CheckPos()
			{
				if (BlockCnt==0||!Win->IsNeedSolvePosEvent()) return;
				const SDL_Event &event=*Win->GetNowSolvingEvent();
				if (Win->GetNowSolvingPosEventMode()==1)
				{
					if (stat!=0)
						if (!CoverLmt.In(Win->NowPos()))
						{
							stat=0;
							if (FocusChoose!=-1)
							{
								Win->SetPresentArea(GetBlockPosize(FocusChoose));
								FocusChoose=-1;
								Win->SetNeedFreshScreen();
							}
							RemoveNeedLoseFocus();
						}
					return;
				}
				
				switch (event.type)
				{
					case SDL_MOUSEBUTTONDOWN:
						Win->SetPresentArea(GetBlockPosize(ClickChoose));
						Win->SetPresentArea(GetBlockPosize(FocusChoose));
						ClickChoose=FocusChoose=InBlockPosize(Win->NowPos());
						Win->SetPresentArea(GetBlockPosize(ClickChoose));
						Win->SetPresentArea(GetBlockPosize(FocusChoose));
						if (event.button.button==SDL_BUTTON_LEFT)
							if (event.button.clicks==2)
								stat=2;
							else stat=4;
						else if (event.button.button==SDL_BUTTON_RIGHT)
							stat=3;
						else stat=4,PUI_DD[1]<<"BlockViewTemplate "<<ID<<": Unknown click button,use it as left click once"<<endl;
						Win->StopSolvePosEvent();
						Win->SetNeedFreshScreen();
						break;
					
					case SDL_MOUSEBUTTONUP:
						if (stat>=2)
						{
							PUI_DD[0]<<"BlockViewTemplate function "<<ClickChoose<<endl;
							if (func!=NULL)
								func(ClickChoose==-1?BackgroundFuncData:BlockData[ClickChoose].FuncData,ClickChoose,stat==4?1:stat);
							stat=1;
							Win->StopSolvePosEvent();
							Win->SetNeedFreshScreen();
						}
						break;
					
					case SDL_MOUSEMOTION:
						if (stat==0)
						{
							stat=1;
							SetNeedLoseFocus();
							FocusChoose=InBlockPosize(Win->NowPos());
							Win->SetPresentArea(GetBlockPosize(FocusChoose));
							Win->SetNeedFreshScreen();
						}
						else
						{
							int pos=InBlockPosize(Win->NowPos());
							if (pos!=FocusChoose)
							{
								Win->SetPresentArea(GetBlockPosize(FocusChoose));
								Win->SetPresentArea(GetBlockPosize(pos));
								FocusChoose=pos;
								Win->SetNeedFreshScreen();
							}	
						}
						Win->StopSolvePosEvent();
						break;
				}
			}
			
			virtual void Show(Posize &lmt)
			{
				if (BlockCnt==0) return;
				Range ForLR=GetShowRange();

				Posize BlockPs=GetBlockPosize(ForLR.l);
				for (int i=ForLR.l;i<=ForLR.r;++i)
				{
					int p=ClickChoose==i?2:FocusChoose==i;
					Win->RenderFillRect(BlockPs&lmt,BlockData[i].co[p]?BlockData[i].co[p]:(DefaultBlockColor[p]?DefaultBlockColor[p]:ThemeColor[p*2]));
 					Win->Debug_DisplayBorder(BlockPs);
					
					if ((i+1)%EachLineBlocks==0)
						BlockPs.y+=EachPs.y+EachPs.h,BlockPs.x=gPS.x;
					else BlockPs.x+=EachPs.x+EachPs.w;
				}
				
				Win->Debug_DisplayBorder(gPS);
			}
			
			virtual void CalcPsEx()//??
			{
				Posize lastPs=gPS;
				if (PsEx!=NULL)	
					PsEx->GetrPS(rPS);
				rPS.w=fa2->GetrPS().w;
				EachLineBlocks=max(1,(rPS.w+EachPs.x)/(EachPs.x+EachPs.w));
				rPS.h=EnsureInRange(ceil(BlockCnt*1.0/EachLineBlocks)*(EachPs.y+EachPs.h)-EachPs.y,fa2->GetrPS().h,1e9);
				if (fa!=NULL)
					gPS=rPS+fa->GetgPS();
				else gPS=rPS;
				CoverLmt=gPS&GetFaCoverLmt();
				fa2->ResizeLL(rPS.w,rPS.h);
				if (!(lastPs==gPS))
					Win->SetPresentArea(lastPs|gPS);
			}

			virtual void _SolveEvent()
			{
				if (!Win->IsNeedSolveEvent()) 
					return;
				if (nxtBrother!=NULL)
					SolveEventOf(nxtBrother);
				if (!Enabled) return;
				if (childWg!=NULL)
					SolveEventOf(childWg);
				if (BlockCnt>0)
					for (Range ForLR=GetShowRange();!ForLR.IsEmpty();++ForLR.l)
						SolveEventOf(BlockData[ForLR.l].lay);
				if (Win->IsNeedSolveEvent())
					CheckEvent();
			}
			
			virtual void _SolvePosEvent()
			{
				if (!Win->IsNeedSolvePosEvent())
					return;
				if (Enabled)
					if (gPS.In(Win->NowPos()))
					{
						if (childWg!=NULL)
							SolvePosEventOf(childWg);
						if (BlockCnt>0)
							for (Range ForLR=GetShowRange();!ForLR.IsEmpty();++ForLR.l)
								SolvePosEventOf(BlockData[ForLR.l].lay);
						if (Win->IsNeedSolvePosEvent())
							CheckPos();
					}
				if (nxtBrother!=NULL)
					SolvePosEventOf(nxtBrother);
			}
			
			virtual void _PresentWidgets(Posize lmt)
			{
				if (nxtBrother!=NULL)
					PresentWidgetsOf(nxtBrother,lmt);
				if (Enabled)
				{
					lmt=lmt&gPS;
					Show(lmt);
					if (DEBUG_EnableWidgetsShowInTurn)
						SDL_Delay(DEBUG_WidgetsShowInTurnDelayTime),
						SDL_RenderPresent(Win->GetSDLRenderer());
					if (BlockCnt>0)
						for (Range ForLR=GetShowRange();!ForLR.IsEmpty();++ForLR.l)
							PresentWidgetsOf(BlockData[ForLR.l].lay,lmt);
					if (childWg!=NULL)
						PresentWidgetsOf(childWg,lmt);
				}
			}

			virtual void _UpdateWidgetsPosize()
			{
				if (nxtBrother!=NULL)
					UpdateWidgetsPosizeOf(nxtBrother);
				if (!Enabled) return;
				CalcPsEx();
				if (childWg!=NULL)
					UpdateWidgetsPosizeOf(childWg);
				if (BlockCnt>0)
				{
					Range ForLR=GetShowRange();
					Posize BlockPs=GetBlockPosize(ForLR.l);
					for (int i=ForLR.l;i<=ForLR.r;++i)
					{
						BlockData[i].lay->SetrPS(BlockPs);
						UpdateWidgetsPosizeOf(BlockData[i].lay);
						if ((i+1)%EachLineBlocks==0)
							BlockPs.y+=EachPs.y+EachPs.h,BlockPs.x=gPS.x;
						else BlockPs.x+=EachPs.x+EachPs.w;
					}
				}
			}
			
		public:
			inline void SetBlockColor(int pos,int p,const RGBA &co)
			{
				if (InRange(pos,0,BlockCnt-1)&&InRange(p,0,2))
				{
					BlockData[pos].co[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(GetBlockPosize(pos));
				}
				else PUI_DD[1]<<"BlockViewTemplate "<<ID<<": SetBlockColor: pos or p is not in range"<<endl;
			}
			
			inline void SetDefaultBlockColor(int p,const RGBA &co)
			{
				if (InRange(p,0,2))
				{
					DefaultBlockColor[p]=co;
					Win->SetNeedFreshScreen();
					Win->SetPresentArea(gPS);
				}
				else PUI_DD[1]<<"BlockViewTemplate "<<ID<<": SetDefaultBlockColor: p "<<p<<" is not in range[0,2]"<<endl;
			}			
			
			void SetBackGroundColor(const RGBA &co)
			{fa2->SetLargeAreaColor(co);}

			inline void SetEachBlockPosize(const Posize &ps)
			{
				EachPs=ps;
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
			}

			inline void SetBlockFunc(void (*_func)(T&,int,int))
			{func=_func;}
			
			void SetBackgroundFuncData(const T &data)
			{BackgroundFuncData=data;}
			
			Widgets* SetBlockContent(int p,const BlockViewData &data)
			{
				p=EnsureInRange(p,0,BlockCnt);
				BlockData.insert(BlockData.begin()+p,data);
				BlockData[p].lay=new LayerForBlockViewTemplate(0,this);
				++BlockCnt;
				if (ClickChoose>=p) ++ClickChoose;
				rPS.h=EnsureInRange(ceil(BlockCnt*1.0/EachLineBlocks)*(EachPs.y+EachPs.h)-EachPs.y,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
				return BlockData[p].lay;
			}
			
			Widgets* SetBlockContent(int p,const T &funcdata)
			{return SetBlockContent(p,BlockViewData(funcdata));}
			
			Widgets* SetBlockContent(int p,const T &funcdata,const RGBA &co0,const RGBA &co1,const RGBA &co2)
			{return SetBlockContent(p,BlockViewData(funcdata,co0,co1,co2));}

			void DeleteBlockContent(int p)//p: 0<=p<ListCnt:SetInP >=ListCnt:DeleteLast <0:DeleteFirst
			{
				if (!BlockCnt) return;
				p=EnsureInRange(p,0,BlockCnt-1);
				delete BlockData[p].lay;
				BlockData.erase(BlockData.begin()+p);
				--BlockCnt;
				if (FocusChoose==BlockCnt) FocusChoose=-1;
				if (ClickChoose>p) --ClickChoose;
				else if (ClickChoose==p) ClickChoose=-1;
				rPS.h=EnsureInRange(ceil(BlockCnt*1.0/EachLineBlocks)*(EachPs.y+EachPs.h)-EachPs.y,fa2->GetrPS().h,1e9);
				fa2->ResizeLL(0,rPS.h);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			void ClearBlockContent()
			{
				if (!BlockCnt) return;
				PUI_DD[0]<<"Clear BlockViewTemplate "<<ID<<" content: BlockCnt:"<<BlockCnt<<endl;
				FocusChoose=ClickChoose=-1;
				for (auto &vp:BlockData)
					delete vp.lay;
				BlockData.clear();
				BlockCnt=0;
				rPS.h=fa2->GetrPS().h;
				fa2->ResizeLL(0,rPS.h);
				fa2->SetViewPort(1,0);
				fa2->SetViewPort(2,0);
				Win->SetNeedUpdatePosize();
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}
			
			Widgets* PushbackContent(const BlockViewData &data)
			{return SetBlockContent(1e9,data);}
			
			Widgets* PushbackContent(const T &funcdata)
			{return SetBlockContent(1e9,funcdata);}
			
			Widgets* PushbackContent(const T &funcdata,const RGBA &co0,const RGBA &co1,const RGBA &co2)
			{return SetBlockContent(funcdata,co0,co1,co2);}

			void SetSelectBlock(int p)
			{
				if (!InRange(p,0,BlockCnt-1))
					return;
				ClickChoose=p;
				if (!InRange(p/EachLineBlocks*(EachPs.y+EachPs.h)+fa->GetrPS().y,0,fa2->GetrPS().h-EachPs.y-EachPs.h))
					fa2->SetViewPort(2,p/EachLineBlocks*(EachPs.y+EachPs.h));
				Win->SetNeedFreshScreen();
				Win->SetPresentArea(gPS);
			}

			inline int GetCurrentSelectBlock()//-1 means none
			{return ClickChoose;}
			
			inline int GetBlockCnt()
			{return BlockCnt;}
			
			inline bool Empty()
			{return BlockCnt==0;}
			
			int Find(const T &_funcdata)
			{
				for (int i=0;i<BlockCnt;++i)
					if (BlockData[i].FuncData==_funcdata)
						return i;
				return -1;
			}
			
			inline BlockViewData& GetBlockData(int p)
			{return BlockData[EnsureInRange(p,0,BlockCnt-1)];}
			
			T& GetFuncData(int p)
			{
				if (p==-1) return BackgroundFuncData;
				else return BlockData[EnsureInRange(p,0,BlockCnt-1)].FuncData;
			}
			
			inline Widgets* GetBlockLayer(int p)
			{return BlockData[EnsureInRange(p,0,BlockCnt-1)].lay;}
			
			inline T& GetBackgroundFuncData()
			{return BackgroundFuncData;}
			
			inline T& operator [] (int p)
			{return BlockData[EnsureInRange(p,0,BlockCnt-1)].FuncData;}
			
			inline Widgets* operator () (int p)
			{return BlockData[EnsureInRange(p,0,BlockCnt-1)].lay;}
			
			void SetUpdateBlock(int p)
			{
				if (!InRange(p,0,BlockCnt-1))
					return;
				Posize bloPs=GetBlockPosize(p);
				if ((bloPs&CoverLmt).Size()!=0)
				{
					Win->SetPresentArea(bloPs);
					Win->SetNeedFreshScreen();
				}
			}
			
			inline void SetEnableKeyboardEvent(bool en)
			{EnableKeyboardEvent=en;}

			Range GetShowRange()
			{
				Range re;
				re.l=-fa->GetrPS().y/(EachPs.y+EachPs.h)*EachLineBlocks;
				re.r=((-fa->GetrPS().y+fa2->GetgPS().h)/(EachPs.y+EachPs.h)+1)*EachLineBlocks-1;
				re.l=EnsureInRange(re.l,0,BlockCnt-1);
				re.r=EnsureInRange(re.r,0,BlockCnt-1);
				return re;
			}
			
			void AddPsEx(PosizeEX *psex)//?? Need virtual?
			{fa2->AddPsEx(psex);}
			
			virtual void SetrPS(const Posize &ps)
			{fa2->SetrPS(ps);}
			
			~BlockViewTemplate()
			{ClearBlockContent();}
			
			BlockViewTemplate(int _ID,Widgets *_fa,const Posize &_rps,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create BlockViewTemplate "<<ID<<endl;
				Type=WidgetType_BlockViewTemplate;
				fa2=new LargeLayerWithScrollBar(0,_fa,_rps,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS={0,0,_rps.w,0};
				InitDefaultBlockColor();
			}
			
			BlockViewTemplate(int _ID,Widgets *_fa,PosizeEX *psex,void (*_func)(T&,int,int)=NULL)
			:func(_func)
			{
				SetID(_ID);
				PUI_DD[0]<<"Create BlockViewTemplate "<<ID<<endl;
				Type=WidgetType_BlockViewTemplate;
				fa2=new LargeLayerWithScrollBar(0,_fa,psex,ZERO_POSIZE);
				SetFa(fa2->LargeArea());
				rPS=ZERO_POSIZE;
				InitDefaultBlockColor();
			}
	};
	
	template <class T> class TreeViewTemplate:public Widgets
	{
		protected:
			
			
		public:
			
			
			
	};
	
	template <class T> class StackViewLayer:public Widgets
	{
		protected:
			
			
		public:
			
			
			
	};
	
	//Special:
	//..
	
	class MessageBox2:public Widgets
	{
		protected:
			
			
		public:
			
			
	};
	
	class SelectFileBox:public Widgets
	{
		protected:
			
			
		public:
			
			
			
	};
	
	
	//Driver:
	//.. 
	
//	void UpdateWidgetsPosize(Widgets *wg)
//	{
//		if (wg==NULL) return;
//		UpdateWidgetsPosize(wg->nxtBrother);
//		if (!wg->Enabled) return;
//		wg->CalcPsEx();
//		UpdateWidgetsPosize(wg->childWg);
//	}
	
	void UpdateWidgetsPosize(PUI_Window *win=NULL)
	{
		if (win!=NULL)
		{
			CurrentWindow=win;
			if (!(win->_BackGroundLayer->gPS==win->WinPS.ToOrigin()))
			{
				win->PresentLimit=win->WinPS.ToOrigin();
				win->_BackGroundLayer->gPS=win->_BackGroundLayer->rPS=win->_BackGroundLayer->CoverLmt=win->WinPS.ToOrigin();
				win->_MenuLayer->gPS=win->_MenuLayer->rPS=win->_MenuLayer->CoverLmt=win->WinPS.ToOrigin();
			}
			if (win->_BackGroundLayer->childWg!=NULL)
				win->_BackGroundLayer->childWg->_UpdateWidgetsPosize();
			if (win->_MenuLayer->childWg!=NULL)
				win->_MenuLayer->childWg->_UpdateWidgetsPosize();
			win->NeedUpdatePosize=0;
			win->NeedFreshScreen=1;
		}
		else
			for (auto sp:PUI_Window::AllWindow)
				if (sp->NeedUpdatePosize)
					UpdateWidgetsPosize(sp);
	}
	
	bool PresentWidgets()//RecommendThis
	{
//		static Uint64 LastPresentTime=0;
//		Uint64 CurrentPresentTime=SDL_GetPerformanceCounter();
		bool re=0;
		for (auto sp:PUI_Window::AllWindow)
			if ((sp->NeedFreshScreen||PUI_Window::NeedFreshScreenAll)&&!sp->Hidden)
			{
				CurrentWindow=sp;
				if (PUI_Window::NeedFreshScreenAll)
					sp->PresentLimit=sp->WinPS.ToOrigin();
				if (DEBUG_DisplayPresentLimitFlag)
				{
					sp->_BackGroundLayer->_PresentWidgets(sp->WinPS.ToOrigin());
					sp->_MenuLayer->_PresentWidgets(sp->WinPS.ToOrigin());
				}
				else
				{
					sp->_BackGroundLayer->_PresentWidgets(sp->PresentLimit);
					sp->_MenuLayer->_PresentWidgets(sp->PresentLimit);
				}
				sp->NeedFreshScreen=0;
				if (DEBUG_DisplayPresentLimitFlag)
					sp->DEBUG_DisplayPresentLimit(),
					PUI_DD[3]<<"PresentLimit "<<sp->PresentLimit.x<<" "<<sp->PresentLimit.y<<" "<<sp->PresentLimit.w<<" "<<sp->PresentLimit.h<<endl;
				SDL_RenderPresent(sp->ren);
//				sp->PresentLimit=sp->WinPS.ToOrigin();
				sp->PresentLimit=ZERO_POSIZE;
				re=1;
			}
		PUI_Window::NeedFreshScreenAll=0;
		return re;
	}
	
	void PresentWidgets(PUI_Window *win,Posize lmt=ZERO_POSIZE)
	{
		CurrentWindow=win;
		if (lmt==ZERO_POSIZE)
			lmt=win->PresentLimit;
		win->_BackGroundLayer->_PresentWidgets(lmt);
		win->_MenuLayer->_PresentWidgets(lmt);
		SDL_RenderPresent(win->ren);
		win->NeedFreshScreen=0;
		win->PresentLimit=win->WinPS.ToOrigin();
	}
	
	void PresentWidgets(Widgets *tar)//Update specific widgets tree;ignore PresentLimit and lmt
	{
		CurrentWindow=tar->Win;
		tar->_PresentWidgets(tar->gPS);
		tar->Win->_MenuLayer->_PresentWidgets(tar->gPS);
		SDL_RenderPresent(tar->Win->ren);
		tar->Win->NeedFreshScreen=0;
	}
	
	int SolveEvent(SDL_Event &event)//0:Solve event successfully 1:Unknown event to solve 2:Known event but not solved
	{
		int re=1;
		switch (event.type)
		{
			case SDL_WINDOWEVENT:
				if (PUI_Window::WindowCnt==1)
					CurrentWindow=MainWindow;
				else CurrentWindow=PUI_Window::WinOfSDLWinID[event.window.windowID];
				CurrentWindow->NowSolvingEvent=&event;
				CurrentWindow->NeedSolveEvent=1;
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						PUI_DD[0]<<"Window Resize to "<<event.window.data1<<" "<<event.window.data2<<endl;
						CurrentWindow->WinPS.w=event.window.data1;
						CurrentWindow->WinPS.h=event.window.data2;
						CurrentWindow->NeedUpdatePosize=1;
						CurrentWindow->NeedSolveEvent=0;
						re=0;
						break;
						
			        case SDL_WINDOWEVENT_SHOWN:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" shown"<<endl;
			            break;
			        case SDL_WINDOWEVENT_HIDDEN:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" hidden"<<endl;
			            break;
			        case SDL_WINDOWEVENT_EXPOSED:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" exposed"<<endl;
			            break;
			        case SDL_WINDOWEVENT_MOVED:
			            PUI_DD[0]<<"Window "<<CurrentWindow->WindowTitle<<" moved to "<<event.window.data1<<","<<event.window.data2<<endl;
			            CurrentWindow->WinPS.x=event.window.data1;
			            CurrentWindow->WinPS.y=event.window.data2;
			            re=0;
			            break;
			        case SDL_WINDOWEVENT_SIZE_CHANGED:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" size changed to "<<event.window.data1<<"x"<<event.window.data2<<endl;
			            break;
			        case SDL_WINDOWEVENT_MINIMIZED:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" minimized"<<endl;
			            break;
			        case SDL_WINDOWEVENT_MAXIMIZED:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" maximized"<<endl;
			            break;
			        case SDL_WINDOWEVENT_RESTORED:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" restored"<<endl;
			            CurrentWindow->SetNeedFreshScreen();
			            CurrentWindow->SetPresentArea(CurrentWindow->GetWinPS().ToOrigin());
			            break;
			        case SDL_WINDOWEVENT_ENTER:
			            PUI_DD[0]<<"Mouse entered window "<<event.window.windowID<<endl;
			            CurrentWindow->PosFocused=1;
			            break;
			        case SDL_WINDOWEVENT_LEAVE:
			            PUI_DD[0]<<"Mouse left window "<<CurrentWindow->WindowTitle<<endl;
			            CurrentWindow->PosFocused=0;
//			            CurrentWindow->NowPos=GetGlobalMousePoint()-CurrentWindow->WinPS.GetLU();
//			            PUI_DD[3]<<"# "<<CurrentWindow->NowPos<<endl;
//			            goto StartSolvePosEventFlag;
			            break;
			        case SDL_WINDOWEVENT_FOCUS_GAINED:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" gained keyboard focus"<<endl;
			            CurrentWindow->KeyboardFocused=1;
			            break;
			        case SDL_WINDOWEVENT_FOCUS_LOST:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" lost keyboard focus"<<endl;
			            CurrentWindow->KeyboardFocused=0;
			            break;
			        case SDL_WINDOWEVENT_CLOSE:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" closed"<<endl;
			            if (CurrentWindow->CloseFunc!=NULL)
			           		CurrentWindow->CloseFunc->CallFunc(0);
			            break;
			        case SDL_WINDOWEVENT_TAKE_FOCUS:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" is offered a focus"<<endl;
			            break;
			        case SDL_WINDOWEVENT_HIT_TEST:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" has a special hit test"<<endl;
			            break;
			        default:
			            PUI_DD[0]<<"Window "<<event.window.windowID<<" got unknown event "<<event.window.event<<endl;
			            break;
				}
				break;
			
			case SDL_TEXTINPUT:
				if (PUI_Window::WindowCnt==1)
					CurrentWindow=MainWindow;
				else CurrentWindow=PUI_Window::WinOfSDLWinID[event.text.windowID];
			case SDL_TEXTEDITING:
				if (event.type==SDL_TEXTEDITING)
				{
					if (PUI_Window::WindowCnt==1)
						CurrentWindow=MainWindow;
					else CurrentWindow=PUI_Window::WinOfSDLWinID[event.edit.windowID];
				}
				CurrentWindow->NowSolvingEvent=&event;
				CurrentWindow->NeedSolveEvent=1;
				if (CurrentWindow->KeyboardInputWg!=NULL)
					CurrentWindow->KeyboardInputWg->ReceiveKeyboardInput();
				if (CurrentWindow->NeedSolveEvent)
				{
					CurrentWindow->_MenuLayer->_SolveEvent();
					CurrentWindow->_BackGroundLayer->_SolveEvent();
				}
				re=CurrentWindow->NeedSolveEvent?2:0;
				break;
			
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if (PUI_Window::WindowCnt==1)
					CurrentWindow=MainWindow;
				else CurrentWindow=PUI_Window::WinOfSDLWinID[event.key.windowID];
				CurrentWindow->NowSolvingEvent=&event;
				CurrentWindow->NeedSolveEvent=1;
				CurrentWindow->_MenuLayer->_SolveEvent();
				CurrentWindow->_BackGroundLayer->_SolveEvent();
				re=CurrentWindow->NeedSolveEvent?2:0;
				break;
			
//			case SDL_FINGERMOTION://Testing now
//				if (!PosEventGetFlag)
//				{
//	//				if (PUI_Window::WindowCnt==1)
//						CurrentWindow=MainWindow;
//	//				else CurrentWindow=PUI_Window::WinOfSDLWinID[event.tfinger.windowID];
//					CurrentWindow->NowPos={event.tfinger.x*CurrentWindow->WinPS.w,event.tfinger.y*CurrentWindow->WinPS.h};
//					PosEventGetFlag=1;
//				}		
		
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				if (PUI_Window::WindowCnt==1)
					CurrentWindow=MainWindow;
				else CurrentWindow=PUI_Window::WinOfSDLWinID[event.button.windowID];
				CurrentWindow->_NowPos={event.button.x,event.button.y};
				goto StartSolvePosEventFlag;
				
			case SDL_MOUSEMOTION:
				if (PUI_Window::WindowCnt==1)
					CurrentWindow=MainWindow;
				else CurrentWindow=PUI_Window::WinOfSDLWinID[event.motion.windowID];
				CurrentWindow->_NowPos={event.motion.x,event.motion.y};
				goto StartSolvePosEventFlag;
				
			case SDL_FINGERMOTION:
				if (PUI_Window::WindowCnt==1)
					CurrentWindow=MainWindow;
				else CurrentWindow=PUI_Window::WinOfSDLWinID[event.motion.windowID];
				CurrentWindow->_NowPos={event.tfinger.x*CurrentWindow->WinPS.w,event.tfinger.y*CurrentWindow->WinPS.h};
				goto StartSolvePosEventFlag;
				
			StartSolvePosEventFlag:
				CurrentWindow->NowSolvingEvent=&event;
				CurrentWindow->NeedSolvePosEvent=1;	
				
				if (CurrentWindow->OccupyPosWg!=NULL)
					if (CurrentWindow->OccupyPosWg->Enabled)
					{
						CurrentWindow->NowSolvingPosEventMode=2;
						CurrentWindow->OccupyPosWg->CheckPos();
					}
					else PUI_DD[2]<<"OccupyPosWg "<<CurrentWindow->OccupyPosWg->ID<<" id disabled!"<<endl; 
				else
				{
					CurrentWindow->NowSolvingPosEventMode=1;
					for (PUI_Window::LoseFocusLinkTable *p=CurrentWindow->LoseFocusWgHead;p;p=p->nxt)
						if (p->wg!=NULL)
							if (p->wg->Enabled)
								p->wg->CheckPos();
							else PUI_DD[2]<<"NeedLoseFocusWg "<<p->wg->ID<<" is disabled!"<<endl;
						else PUI_DD[2]<<"NeedLoseFocusWidgets is NULL"<<endl;
					
					CurrentWindow->NowSolvingPosEventMode=0;
					CurrentWindow->_MenuLayer->_SolvePosEvent();
					CurrentWindow->_BackGroundLayer->_SolvePosEvent();
				}
				re=CurrentWindow->NeedSolvePosEvent?2:0;
				CurrentWindow->NeedSolvePosEvent=0;
				break;
			
			case SDL_MOUSEWHEEL:
				if (PUI_Window::WindowCnt==1)
					CurrentWindow=MainWindow;
				else CurrentWindow=PUI_Window::WinOfSDLWinID[event.wheel.windowID];
				CurrentWindow->NowSolvingEvent=&event;
				CurrentWindow->NeedSolveEvent=1;
				CurrentWindow->_MenuLayer->_SolveEvent();
				CurrentWindow->_BackGroundLayer->_SolveEvent();
				re=CurrentWindow->NeedSolveEvent?2:0;
				break;
				
			case SDL_USEREVENT:
				if (event.user.type==PUI_EVENT_UpdateTimer)
					((Widgets*)event.user.data1)->CheckEvent();
				re=0;
				break;
			//Maybe need fix 
			
			default:
//				PUI_DD[1]<<"Such event was not considered yet!"<<endl;
//				EventSolvedFlag=0;
//				MenusLayer->_SolveEvent(event);
//				BackGroundLayer->_SolveEvent(event);
				break;
		}
		while (!Widgets::WidgetsToDeleteAfterEvent.empty())
		{
			delete Widgets::WidgetsToDeleteAfterEvent.front();
			Widgets::WidgetsToDeleteAfterEvent.pop();
		}
		return re;
	}
	
	void DebugEventSolve(const SDL_Event &event)
	{
		if (event.type==SDL_KEYDOWN)
			if (event.key.keysym.mod&KMOD_CTRL)
				switch (event.key.keysym.sym)
				{
					case SDLK_F2:
						DEBUG_DisplayBorderFlag=!DEBUG_DisplayBorderFlag,PUI_Window::SetNeedFreshScreenAll();
						break;
					
					case SDLK_F3:
						DD.SwitchToContrary();
						break;
						
					case SDLK_F4:
						DEBUG_DisplayPresentLimitFlag=!DEBUG_DisplayPresentLimitFlag;
						break;
					
//					case SDLK_F5:
//						DEBUG_EnableWidgetsShowInTurn=!DEBUG_EnableWidgetsShowInTurn;
//						break;
				}
	}
	
	void EasyEventLoop()
	{
		bool QuitFlag=0;
		while (!QuitFlag)
		{
			SDL_Event mainEvent;
			SDL_WaitEvent(&mainEvent);
			
			if (SolveEvent(mainEvent)!=0)
			{
				if (mainEvent.type==SDL_QUIT)
					QuitFlag=1;
				if (mainEvent.type==SDL_KEYDOWN)
					if (mainEvent.key.keysym.sym==SDLK_ESCAPE)
						QuitFlag=1;
				DebugEventSolve(mainEvent);
			}
			UpdateWidgetsPosize();
			PresentWidgets();
		}
	}
	
	int PAL_GUI_Init(const Posize &winps,const string &title="",unsigned int flagWin=SDL_WINDOW_RESIZABLE,unsigned int flagRen=SDL_RENDERER_ACCELERATED)
	{
		PUI_DD[0]<<"PAL_GUI_Init"<<endl;
		SDL_Init(SDL_INIT_EVERYTHING);
		char *basepath=SDL_GetBasePath();//??
		string path(basepath);
		SDL_free(basepath);
		
		PUI_DD.SetLOGFile(path+"PAL_GUI_DebugLog.txt");
		PUI_DD%DebugOut_CERR_LOG;
		PUI_DD.SetDebugType(0,"PUI_Info");
		PUI_DD.SetDebugType(1,"PUI_Warning");
		PUI_DD.SetDebugType(2,"PUI_Error");
		PUI_DD.SetDebugType(3,"PUI_Debug");
		
		if (!PUI_DefaultFonts)
		{
			PUI_DefaultFonts.SetDefaultSize(14);
			PUI_DefaultFonts.SetFontFile(path+"msyhl.ttc");
		}
		
		PUI_EVENT_UpdateTimer=SDL_RegisterEvents(1);
		
		MainWindow=CurrentWindow=new PUI_Window(winps,title,flagWin,flagRen);
		PUI_DD[0]<<"PAL_GUI_Init: OK"<<endl;
		return 0;
	}
	
	int PAL_GUI_Quit()
	{
		PUI_DD[0]<<"PAL_GUI_Quit"<<endl;
		while (!PUI_Window::GetAllWindowSet().empty())
			delete *PUI_Window::GetAllWindowSet().begin();
		while (!Widgets::GetWidgetsIDmap().empty())
			delete Widgets::GetWidgetsIDmap().begin()->second;
		SDL_Quit();
		PUI_DD[0]<<"PAL_GUI_Quit OK"<<endl;
		return 0;
	}
}
#endif
