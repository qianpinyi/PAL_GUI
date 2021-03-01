//2.cpp
#include <PAL_GUI/PAL_GUI_0.cpp>//include GUI库
using namespace PAL_GUI;//使用命名空间，也可以用::的方式访问，如使用此命名空间，则同时也使用了std命名空间

void ShowNewWindow(PUI_Window *&win)//按钮的回调函数，显示一个新窗口，也可以写成是lambda表达式 
{
	if (win==NULL)
		win=new PUI_Window({100,100,300,200},"NewWindow");//创建窗口，并保存指针到按钮中 
	else
	{
		delete win;//删除窗口 
		win=NULL;//将指针设为空 
	}
}

int main(int argc,char **argv)
{
	SDL_SetMainReady();//由于SDL2库定义了自己的main，要使用main函数需要SetMainReady，或者使用SDL_main
	system("chcp 65001");//切换控制台输出为UTF-8，非必要
	PAL_GUI_Init({PUI_Window::PUI_WINPS_CENTER,PUI_Window::PUI_WINPS_CENTER,640,360},"PAL_GUI Test");
	//创建居中的大小为640x360的名为PAL_GUI Test的窗口
	
	new Button <PUI_Window*> (0,MainWindow->BackGroundLayer(),{100,100,100,30},"ClickMe",ShowNewWindow,NULL);
	//在坐标为100,100的位置创建一个大小为100x30的按钮，名为ClickMe，按下时触发函数ShowWindow 
	
	EasyEventLoop();//主事件循环，也可以展开来自己写，当前没有必要 
	PAL_GUI_Quit();//退出，进行类的析构等 
	return 0;
}
