#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "D3DINIT.h"

#define WINDOW_CLASS_NAME "WINCLASS1"




class WININIT
{
	
public:
	static bool mousepress;
	WININIT(int WIDTH,int HEIGHT, const char* caption, HINSTANCE hinstance, HWND &hwnd); //окно
	~WININIT();
	void SetPlayer(OBJECT * obj); //задать цель для камеры
};

