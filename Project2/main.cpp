#define WIN32_LEAN_AND_MEAN
#define WINDOW_CLASS_NAME "WINCLASS1"
#include <Windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <D3D11.h>
#include <D3DX11.h>
#define WINDOW_HEIGHT 400 //������� ����
#define WINDOW_WIDTH 400
#define BBP 16 //������� �����
#pragma comment(lib, "d3d11.lib")
HWND main_window_handle = NULL;  // ������������� ����
HINSTANCE main_instance = NULL;  // ������������� ����������
D3D_DRIVER_TYPE g_drivertype = D3D_DRIVER_TYPE_NULL; //
D3D_FEATURE_LEVEL g_featurelevel = D3D_FEATURE_LEVEL_11_0; // ������  ������� �������������� �����������
ID3D11Device *g_pd3device = NULL; // �������� �������� (�������� �������, ������ 3-� ������ ��������  � �.�.)
ID3D11DeviceContext *g_pImmediateContext = NULL; // ����� ����������� ����������
IDXGISwapChain *g_pSwapChain = NULL;  // ������ � ������� ��������� � ����� ������������� �� �����, ������ ��������� ��� ������ ������ � �������� (����� ) ��� ���������� �������� ��� �����������
ID3D11RenderTargetView *g_pRenderTargetView = NULL; // ���������� ������ �����

HRESULT InitDevice();//�������������� �������
void CleanUpDevice(); //�������� �������� Direct3D
void Render(); //��������� 

LRESULT CALLBACK WindowProc(HWND hwnd,
							UINT msg,
							WPARAM wparam,
							LPARAM lparam)
{
	HDC hdc; //���������� ��������� ����������
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			if (wparam == 16)
			{
				MessageBox(NULL, LPCSTR("�� �����, ������"), LPCSTR("MessageBox"), 0);
			}
		}break;
		case WM_PAINT:
		{

		}break;
		case WM_DESTROY:
		{ 
			PostQuitMessage(0);
			return 0;
		}break;
	default:break;
	}
	return (DefWindowProc(hwnd,msg,wparam,lparam));
}



int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hpprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	MSG msg;

	WNDCLASSEX winclass = { 0 };

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	winclass.hbrBackground - (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	int y;
	if (!(y = RegisterClassEx(&winclass)))
	{
		return 0;
	}
	HWND hwnd;
	if (!(hwnd = CreateWindowEx(NULL,
		WINDOW_CLASS_NAME,
		"only text",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL,
		NULL,
		hinstance,
		NULL)))
		return 0;

	main_window_handle = hwnd;
	main_instance = hinstance;
	if (FAILED(InitDevice()))
	{
		CleanUpDevice();
		return 0;
	}
	
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}

	}
	CleanUpDevice();

	return 0;
}

HRESULT InitDevice()
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(main_window_handle, &rc);
	UINT width = rc.right - rc.left; //�������� ������
	UINT height = rc.bottom - rc.top; // � ������ ����

	UINT createDeviceFlags = 0;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes); //������� ������ ��� �������� �� ���������� ���������
	//������� ������ �������������� ������ �������

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	//�������� ��������� �������. �������� ���������
	// ������� ��������� �������� ��������������� � ����������� ��� � ������ ����
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1; //one back buffer
	sd.BufferDesc.Width = width; //shirina bufera
	sd.BufferDesc.Height = height; //visota buffera
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ������ ������� � ������
	sd.BufferDesc.RefreshRate.Numerator = 75; // ������� ���������� ������
	sd.BufferDesc.RefreshRate.Denominator = 1;// 
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //���������� ������ - peredniu? �����
	sd.OutputWindow = main_window_handle; //�������� � ����
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE; // ������� �����
	//
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_drivertype = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_drivertype, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
			&sd, &g_pSwapChain, &g_pd3device, &g_featurelevel, &g_pImmediateContext);
		if SUCCEEDED(hr) break; //���� ���������� ������� �� ������� �� �����
	}
	if FAILED(hr) return hr;
	//������� ������ ����� � SDK
	//RenderTargetOutput - ��� �������� �����, RenderTargetView - ������

	ID3D11Texture2D* pBackBuffer = NULL;//������ �������, �.�. ������� ������ �������� ����� ����� ��� ��������� ��� ����� ������ � ��� ��������
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
	if FAILED(hr) return hr;
	hr = g_pd3device->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if FAILED(hr) return hr;
	//���������� ������ ������� ������ � ��������� ����������
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
	//��������� ��������
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	//���������� ������� � ��������� ���������
	g_pImmediateContext->RSSetViewports(1, &vp);
	return S_OK;
}

void CleanUpDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3device)  g_pd3device->Release();
}

void Render()
{
	float ClearColor[4] = { 0.5f,0.5f,0.5f,1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pSwapChain->Present(0, 0);
}