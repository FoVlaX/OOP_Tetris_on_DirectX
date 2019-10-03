#include "WININIT.h"



void WININIT::SetPlayer(OBJECT *obj)
{
	OBJECT::set_gg = obj;
	int h = 0;
}

 LRESULT CALLBACK WindowProc(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc; //дескриптор контекста устройтсва
	
	switch (msg)
	{
	case WM_KEYUP:
	{
	
		if (wparam == 38) //up
		{
			OBJECT::set_gg->vz = 0.f;
			OBJECT::set_gg->vx = 0.f;
		}
		if (wparam == 40) //down
		{
			OBJECT::set_gg->vz = 0.f;
			OBJECT::set_gg->vx = 0.f;
		}
	}break;
	case WM_KEYDOWN:
	{
		if (wparam == 16) //shift
		{
			AllocConsole();
			PostQuitMessage(0);
		}
		if (wparam == 37) //left
		{
			OBJECT::set_gg->direction -=0.015f;
			OBJECT::set_gg->yang = OBJECT::set_gg->direction;
			if (OBJECT::set_gg->direction < 0) OBJECT::set_gg->direction += XM_PI * 2;
		}
		if (wparam == 39) //right
		{
			OBJECT::set_gg->direction += 0.015f;
			OBJECT::set_gg->yang = OBJECT::set_gg->direction;
			if (OBJECT::set_gg->direction > XM_PI * 2) OBJECT::set_gg->direction -= XM_PI * 2;
		}
		if (wparam == 38) //up
		{
			OBJECT::set_gg->vz = 0.05f*cos(OBJECT::set_gg->direction);
			OBJECT::set_gg->vx = 0.05f * sin(OBJECT::set_gg->direction);
		}
		if (wparam == 40) //down
		{
			OBJECT::set_gg->vz = -0.05f * cos(OBJECT::set_gg->direction);
			OBJECT::set_gg->vx = -0.05f * sin(OBJECT::set_gg->direction);
		}
	}break;
	case WM_MOUSEWHEEL:
	{
		if (wparam == 4287102976)
		{
			D3DINIT::ViewDist += 0.4f;
		}
		else
		{
			D3DINIT::ViewDist -= 0.4f;
			
		}

	}break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);

	}break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}break;
	default:break;
	}
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}
 


WININIT::WININIT(int WIDTH, int HEIGHT, HINSTANCE hinstance, HWND &hwnd)
{

	WNDCLASSEX winclass = { 0 };

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	int y;
	if (!(y = RegisterClassEx(&winclass)))
	{
		hwnd = NULL;
	}
	if (!(hwnd = CreateWindowEx(NULL,
		WINDOW_CLASS_NAME,
		"DirectX",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0,
		WIDTH, HEIGHT,
		NULL,
		NULL,
		hinstance,
		NULL)))
	{
		hwnd = NULL;
	}

}


WININIT::~WININIT()
{
}
