#define WIN32_LEAN_AND_MEAN
#include "D3DINIT.h"
#include "WININIT.h"

int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hpprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	MSG msg;


	HWND hwnd;
	WININIT win(WINDOW_WIDTH, WINDOW_HEIGHT, hinstance, hwnd);

	if (hwnd == NULL)
	{
		return 0;
	}

	D3DINIT d3d(hwnd);
	main_instance = hinstance;

	if (FAILED(d3d.InitDevice()))
	{
		d3d.CleanUpDevice();
		return 0;
	}

	if (FAILED(d3d.InitGeometry()))
	{
		d3d.CleanUpDevice();
		return 0;
	}

	if (FAILED(d3d.InitMatrixes()))
	{
		d3d.CleanUpDevice();
		return 0;
	}

	bool dir = true;
	HRESULT hr = S_OK;
	OBJECT piramide("ver.txt",hr);
	OBJECT piramide1("ver.txt", hr);
	piramide1.x = 1.f;
	if (FAILED(hr))
	{
		return 0;
	}
	piramide.setname("piramide");

	win.SetPlayer(&piramide);
	OBJECT *test = (OBJECT*)D3DINIT::global_ids[0];
	d3d.SetGameSpeed(60);
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{	
			test->xang += 0.03f;
			test->zang += 0.03f;
			d3d.RenderStart();
			test->step();
			piramide.draw();
			piramide1.draw();
			d3d.RenderEnd();
		}

	}
	d3d.CleanUpDevice();
	return 0;
}

