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
	piramide.y = -1.0f;
	OBJECT piramide1("ver.txt", hr);
	piramide1.x = 1.f;
	if (FAILED(hr))
	{
		return 0;
	}
	piramide.setname("piramide");

	win.SetPlayer((OBJECT*)OBJECT::global_ids[1]);
	OBJECT *test = (OBJECT*)OBJECT::global_ids[0];
	d3d.SetGameSpeed(120);
	test->xs = 2;
	ShowCursor(FALSE);

	bool pulse = false;

	float scale = 1;

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

			d3d.RenderStart();
			POINT p;
			GetCursorPos(&p);
			float ang = 0.3*(WINDOW_WIDTH / 2 - p.x)*XM_PI/180;
			float ang2 = 0.3*(WINDOW_HEIGHT / 2 - p.y) * XM_PI / 180;
			d3d.SetView(ang,ang2);
			SetCursorPos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
			if (!pulse && scale < 1.8)
			{
				OBJECT::set_gg->xs = scale;
				OBJECT::set_gg->ys = scale;
				OBJECT::set_gg->zs = scale;
				scale += 0.025;
			}
			if (scale >= 1.8 && !pulse) pulse = true;
			if (scale <= 1 && pulse) pulse = false;
			if (pulse && scale > 1)
			{
				OBJECT::set_gg->xs = scale;
				OBJECT::set_gg->ys = scale;
				OBJECT::set_gg->zs= scale;
				scale -= 0.025;
			}
			
			test->step();
			test->zang += 0.005;
			piramide1.step();
			piramide.draw();
			piramide1.draw();
			d3d.RenderEnd();
		}

	}
	d3d.CleanUpDevice();
	return 0;
}

