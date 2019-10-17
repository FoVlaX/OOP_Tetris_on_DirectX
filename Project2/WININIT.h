#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "D3DINIT.h"

#define WINDOW_CLASS_NAME "WINCLASS1"




class WININIT
{
	
public:

	WININIT(int WIDTH,int HEIGHT, HINSTANCE hinstance, HWND &hwnd); //окно
	~WININIT();
	void SetPlayer(OBJECT * obj); //задать цель для камеры
};

