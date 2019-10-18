#define WIN32_LEAN_AND_MEAN
#include "D3DINIT.h"
#include "WININIT.h"
#include "server.h"
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
	OBJECT piramide("test6.obj", "test.png",hr);
	piramide.y = -1.0f;

	server SRVR(hr);
	if (FAILED(hr))
	{
		return 0;
	}


	piramide.setname("sword");
	OBJECT golp("test5.obj", "textureforship.png", hr);
	golp.x = 8;
	win.SetPlayer((OBJECT*)OBJECT::global_ids[0]);
	OBJECT *test = (OBJECT*)OBJECT::global_ids[0];
	d3d.SetGameSpeed(120);
	ShowCursor(FALSE);
	LIGHT pointL({ 2.f,3.f,0.f,7.f }, { 1.0f,1.f,0.9f,1.f }, pointLight);
	LIGHT normL({ -0.5f,0.5f,-0.5f,1.f }, { 1.0f,1.f,0.9f,1.f }, normalLight);
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
			state mes;
			d3d.RenderStart();
			POINT p;
			GetCursorPos(&p);
			float ang = 0.3*(WINDOW_WIDTH / 2 - p.x)*XM_PI/180;
			float ang2 = 0.3*(WINDOW_HEIGHT / 2 - p.y) * XM_PI / 180;
			d3d.SetView(ang,ang2);
			SetCursorPos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
			if (normL.Dir.w > 0) normL.Dir.w -= 0.001;
			if (SRVR.recive(mes))
			{
				OBJECT::set_gg->x = mes.x;
				OBJECT::set_gg->y = mes.y;
				OBJECT::set_gg->z = mes.z;
			}
			piramide.xang += 0.04;
			//piramide.step();
			piramide.draw();
			golp.draw();
			d3d.RenderEnd();
		}

	}
	d3d.CleanUpDevice();
	return 0;
}

