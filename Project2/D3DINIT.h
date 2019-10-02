#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <D3Dcompiler.h>
#include <D3DX11async.h>
#include <memory>
#define WINDOW_HEIGHT 800 //������� ����
#define WINDOW_WIDTH 800
#define BBP 16 //������� �����
#define MX_SETWORLD 0x101;
#pragma comment(lib,"D3DX11.lib") // ��� ��� �� ��� ���� ���� ��������tb
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")

static float time = 0.0f;
static const int vCount = 60; // ������������� ����
static HINSTANCE main_instance = NULL;  // ������������� ����������
static D3D_DRIVER_TYPE g_drivertype = D3D_DRIVER_TYPE_NULL; //
static D3D_FEATURE_LEVEL g_featurelevel = D3D_FEATURE_LEVEL_11_0; // ������  ������� �������������� �����������
static ID3D11Device *g_pd3device = NULL; // �������� �������� (�������� �������, ������ 3-� ������ ��������  � �.�.)
static ID3D11DeviceContext *g_pImmediateContext = NULL; // ����� ����������� ����������
static IDXGISwapChain *g_pSwapChain = NULL;  // ������ � ������� ��������� � ����� ������������� �� �����, ������ ��������� ��� ������ ������ � �������� (����� ) ��� ���������� �������� ��� �����������
static ID3D11RenderTargetView *g_pRenderTargetView = NULL; // ���������� ������ �����
static ID3D11VertexShader *g_pVertexShader = NULL; //��������� ������
static ID3D11PixelShader *g_pPixelShader = NULL; // ���������� ������
static ID3D11InputLayout *g_pVertexLayout = NULL; // �������� ������� ������
static ID3D11Buffer *g_pVertexBuffer = NULL; // ����� ������
static ID3D11Buffer *g_pIndexBuffer = NULL; //����� �������� ������ � ����� ������� ������������
static ID3D11Buffer *g_pConstantBuffer = NULL; // ���������� �����
static ID3D11Texture2D* g_pDepthStencil = NULL;
static ID3D11DepthStencilView* g_pDepthStencilView = NULL;

static XMMATRIX g_World; //������� ����00
static XMMATRIX g_View; //������� ����
static XMMATRIX g_Projection; //������� ��������
static float pulse = 0.0f;
static bool Bpulse = true;
static FLOAT t = 0.0f;

static XMFLOAT4 vLightDirs[2];
static XMFLOAT4 vLightColors[2];
static XMFLOAT4 vOutputColor = { 0.9f,0.8f,0.9f,1.f};

static int setid = 0;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal; //������ ������ �������� ���� � �����
};

//��������� ������������ ������

struct ConstantBuffer
{
	XMMATRIX mWorld; //������� ����
	XMMATRIX mView; //������ ����
	XMMATRIX mProjection; //������� ��������
	XMFLOAT4 vLightDir[2];
	XMFLOAT4 vLightColor[2];
	XMFLOAT4 vOutputColor;
};


class D3DINIT
{
	
public:

	D3DINIT(HWND mwh);
	~D3DINIT();
	HRESULT InitDevice();//�������������� �������
	void CleanUpDevice(); //�������� �������� Direct3D
	void RenderStart();
	void RenderEnd();//��������� 
	float ViewAngle = 0;
	HRESULT InitMatrixes(); //������������� ������
	void SetView(float angleY,float angleX); // ��������� ������� ����
	HRESULT InitGeometry();
	HRESULT CompileShaderFromFile(LPCSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HWND main_window_handle;
	
	void SetGameSpeed(int spd);
private:
	int GameSpeed = 1000 / 30;
	XMVECTOR g_Eye = XMVectorSet(0.0f, 0.0f, -9.0f, 0.0f); //������ �������
	XMVECTOR g_At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); //���� �������
	XMVECTOR g_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // ����������� �����
	XMVECTOR helpXas = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	float vertAng = 0;
};



class OBJECT
{

	public:
		static OBJECT* set_gg;
		static int global_ids[100];
		static int current_id;
		OBJECT(char const* vertxt,HRESULT &hr); // �������� �� ����� ������ � ������� �� ���������
		///-----------------------------------------------------------------------------------------------------------
		float x = 0;
		float y = 0;
		float z = 0;
		float xs = 1;
		float ys = 1;   // ---------- ��������� �������, ��� ��������� � ������� ������������, ������, ���� ��������
		float zs = 1;
		float xang= 0;
		float yang= 0;
		float zang=0;
		int m = 0;
		int id = 0;
		const char *name = "unnamed";
		float vx = 0;
		float vy = 0;
		float vz = 0;
		///-----------------------------------------------------------------------------------------------------------
		void draw(); // ������� ������� �������� ������� ���������� ��������, �������������� ���������� ������, ��������� ����������� ��������� ������������� � ������������ ������ ������ � ������ �������;
		void step();
		void setname(const char* nm);
		ID3D11Buffer *pVertexBuffer = NULL; // ����� ������
		ID3D11Buffer *pIndexBuffer = NULL; // ����� ��������
		~OBJECT();
		OBJECT* getadress();
	private:
		
};

