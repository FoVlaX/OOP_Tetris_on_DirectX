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
	float p = 0;
	float ang = 0;
	float koef = 3.5;
	OBJECT *stars[1000];
	
	
	int i = 0;

	float step = 0;
	bool show = true;
	while (i < 800)
	{
		p = koef * ang;
		stars[i] = new OBJECT("test.obj", "startex.dds", hr);
		stars[i]->setname("sword");
		stars[i]->x = p*cos(ang+step) + 0.001f * (rand() % 3000) - 1.5 ;
		stars[i]->z = p*sin(ang+step) + 0.001f * (rand() % 3000) - 1.5;
		stars[i]->y = 0.001f * (rand() % 3000) - 1.5;
		stars[i]->xs = 0.04;
		stars[i]->ys = 0.04;
		stars[i]->zs = 0.04;
		stars[i]->blend = { (FLOAT)(195 + (rand() % 60)) / 255,  (FLOAT)(195 + (rand() % 60)) / 255 , (FLOAT)(195 + (rand() % 60)) / 255 ,1.f };
		i++;
		if (i%4==0) ang += XM_PIDIV4/5.f;
		if (i % 200 == 0) { 
			ang = 0; 
			step +=  XM_PI/2;
		}
	}
	

	win.SetPlayer((OBJECT*)OBJECT::global_ids[0]);
	OBJECT *test = (OBJECT*)OBJECT::global_ids[0];
	d3d.SetGameSpeed(120);
	ShowCursor(TRUE);
	

	if (FAILED(hr))
	{
		return 0;
	}
	ang = 0;
	float ang2 = 0;
	float angle = 0;
	while (1)
	{
		if (WININIT::mousepress)
		{
			ShowCursor(FALSE);
			if (show)
			{
				
				SetCursorPos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
				show = !show;
				
			}
			
		}
		else
		{
			if (!show)
			{
				
				show = !show;
			}
			ShowCursor(TRUE);
		}
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
			if (WININIT::mousepress)
			{
				ang = 0.3 * (WINDOW_WIDTH / 2 - p.x) * XM_PI / 180;
				ang2 = 0.3 * (WINDOW_HEIGHT / 2 - p.y) * XM_PI / 180;
			}
			

			d3d.SetView(ang, ang2);

			if (WININIT::mousepress)
			{
				SetCursorPos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
			}
				//if (normL.Dir.w > 0) normL.Dir.w -= 0.001;
				
	
			//piramide.step();
			OBJECT::drawall();
			d3d.RenderEnd();
		}

	}
	d3d.CleanUpDevice();
	return 0;
}

