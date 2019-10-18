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
	OBJECT golp("sphere.obj", "textureforship.png", hr);
	OBJECT sphere("sphere.obj", "tex1.dds", hr);

	sphere.xs = 0.5;
	sphere.ys = 0.5;
	sphere.zs = 0.5;

	golp.x = 8;
	win.SetPlayer((OBJECT*)OBJECT::global_ids[0]);
	OBJECT *test = (OBJECT*)OBJECT::global_ids[0];
	d3d.SetGameSpeed(120);
	ShowCursor(FALSE);
	LIGHT pointL({ 2.f,3.f,0.f,17.f }, { 1.0f,1.f,0.9f,1.f }, pointLight);
	//LIGHT normL({ -0.5f,0.5f,-0.5f,1.f }, { 1.0f,1.f,0.9f,1.f }, normalLight);
	golp.lightOn = &pointL;
	float angle = 0;
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
			//if (normL.Dir.w > 0) normL.Dir.w -= 0.001;
			golp.x = 14*cos(angle);
			golp.z = 14*sin(angle);

			sphere.x = golp.x + 5 * cos(2*angle);
			sphere.z = golp.z + 5 * sin(2*angle);

			angle += XM_PIDIV4 / 40;
			if (angle > XM_PI * 2) angle -= XM_PI * 2;
			if (SRVR.recive(mes))
			{
				OBJECT::set_gg->x = mes.x;
				OBJECT::set_gg->y = mes.y;
				OBJECT::set_gg->z = mes.z;
			}
			piramide.yang += 0.04;
			//piramide.step();
			OBJECT::drawall();
			d3d.RenderEnd();
		}

	}
	d3d.CleanUpDevice();
	return 0;
}

