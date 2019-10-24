#pragma once
//#define WIN32_LEAN_AND_MEAN
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
#include <D3DX11tex.h>
#include <fstream>
#include <vector>
#define WINDOW_HEIGHT 800 //размеры окна
#define WINDOW_WIDTH 800
#define BBP 16 //глубина цвета
#define MX_SETWORLD 0x101;

#pragma comment(lib,"D3DX11.lib") // вот оно то что надо было подключиtb
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")

static float time = 0.0f;
static const int vCount = 60; // идентификатор окна
static HINSTANCE main_instance = NULL;  // идентификатор приложения
static D3D_DRIVER_TYPE g_drivertype = D3D_DRIVER_TYPE_NULL; //
static D3D_FEATURE_LEVEL g_featurelevel = D3D_FEATURE_LEVEL_11_0; // версия  директХ поддерживаемая видеокартой
static ID3D11Device *g_pd3device = NULL; // создание ресурсов (текстуры шейдеры, буферы 3-ч мерных объектов  и т.д.)
static ID3D11DeviceContext *g_pImmediateContext = NULL; // вывод графической информации
static IDXGISwapChain *g_pSwapChain = NULL;  // работа с буфером рисования и вывод нарисованного на экран, должен содержать два буфера задний и передний (экран ) для корректной отрисвки без мельтешений
static ID3D11RenderTargetView *g_pRenderTargetView = NULL; // собственно задний буфер
static ID3D11VertexShader *g_pVertexShader = NULL; //вершинный шейдер
static ID3D11PixelShader *g_pPixelShader = NULL; // пиксельынй шейдер
static ID3D11InputLayout *g_pVertexLayout = NULL; // описание формата вершин
static ID3D11Buffer *g_pVertexBuffer = NULL; // Буфер вершин
static ID3D11Buffer *g_pIndexBuffer = NULL; //Буфер индексов вершин в каком порядке отрисовывать
static ID3D11Buffer *g_pConstantBuffer = NULL; //matrixes
static ID3D11Buffer* g_pConstantBufferLight = NULL; // Констатный буфер light
static ID3D11Texture2D* g_pDepthStencil = NULL;
static ID3D11DepthStencilView* g_pDepthStencilView = NULL;
static ID3D11Buffer* g_pConstantBufferPointLight = NULL;
static ID3D11PixelShader* g_pPixelShaderLightO = NULL;

static XMMATRIX g_World; //матрица мира00
static XMMATRIX g_View; //матрциа вида
static XMMATRIX g_Projection; //матрица проекции
static float pulse = 0.0f;
static bool Bpulse = true;
static FLOAT t = 0.0f;

static XMFLOAT4 vLightDirs[2];
static XMFLOAT4 vLightColors[2];
static XMFLOAT4 vOutputColor = { 0.2f,0.8f,0.9f,1.f}; //цвет выходной 
static ID3D11ShaderResourceView* g_pTextureRV = NULL; //Объект текстуры
static ID3D11SamplerState* g_pSamplerLinear = NULL; //параметры nalozheniya текстуры obrazec
static enum typelight { normalLight, pointLight };

static int setid = 0;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Normal; //каждая вершна содержит инфу о цвете
};

//Структура константного буфера

struct ConstantBuffer
{
	XMMATRIX mWorld; //матрица мира
	XMMATRIX mView; //марица вида
	XMMATRIX mProjection; //матрица проекции
	XMFLOAT4 colorWhenNoLight;
};

static struct ConstantBufferLight
{
	XMFLOAT4 vLightDir[100]; // в альфа канале будем хранить коэффициент яркости
	XMFLOAT4 vLightColor[100];
	XMFLOAT4 vOutputColor; // здесь запишем инфу о кол-во света 0 - кол-во направленного света, 1 - кол-во точечных источников света, 2 - кол-во конусных
};

static struct ConstantBufferPointLight
{
	XMFLOAT4 vPos[100];
	XMFLOAT4 vLightPointColor[100];
};




class D3DINIT
{
	
public:
	static XMVECTOR g_Eye;
	static XMVECTOR g_Up; 
	static float ViewDist;
	
	D3DINIT(HWND mwh);
	~D3DINIT();
	HRESULT InitDevice();//инициализайция директХ
	void CleanUpDevice(); //удаление объектов Direct3D
	void RenderStart();
	void RenderEnd();//отрисовка 
	float ViewAngle = 0;
	HRESULT InitMatrixes(); //Инициализация матриц
	void SetView(float angleY,float angleX); // изменение матрицы мира
	HRESULT InitGeometry();
	HRESULT CompileShaderFromFile(LPCSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HWND main_window_handle;
	
	void SetGameSpeed(int spd);
private:
	int GameSpeed = 1000 / 30;
	 //откуда смотрим
	XMVECTOR g_At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); //Куда смотрим
	 // Направление верха
	XMVECTOR helpXas = XMVectorSet(1.f, 0.f, 0.f, 0.f); // Горизонтальная ось вокруг которой вектор направления на кмеру вращается вверх вниз
	float vertAng = 0; //вертикаьлный угол камеры
};


class LIGHT {

public:
	static int currentidP;
	static int currentidN;
	static int idsP[100];
	static int idsN[100];
	static void lightAll(); // перемещениие в структуры контантного буффер инфы о всех светах и обновление глобальных контантных буфферов
	LIGHT(XMFLOAT4 PosP, XMFLOAT4 Color, typelight typeL);

	XMFLOAT4 Pos; //позиция и Радиус в альфа канале для точечного
	XMFLOAT4 Dir; //направление и интенсивность в альфа канале ля направенного света
	XMFLOAT4 CurColor; //цвет света
	void setLight(ConstantBufferLight& cbl, ConstantBufferPointLight& cbpl); // перемещение текущих данный о конкретном источники света в струкутру по конкретному ай ди
	int id = 0; // ид текущего света
	~LIGHT();
	typelight tl;
private:

};


class OBJECT
{

	public:
		static OBJECT* set_gg;
		static int global_ids[1000];
		static int current_id;
		OBJECT(char const* vertxt, char const* texture,HRESULT &hr); // загрузка из файла вершин и порядка их отрисовки
		///-----------------------------------------------------------------------------------------------------------
		float x = 0;
		float y = 0;
		float z = 0;
		float xs = 1;
		float ys = 1;   // ---------- Состояние объекта, его положение в мировом пространстве, маштаб, углы вращения
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
		float vxang = 0;
		float vyang = 0;
		float vzang = 0;
		///-----------------------------------------------------------------------------------------------------------
		static void drawall();
		void draw(); // функция которая передает буфферы устройству рисвания, переопределяет констатный буффер, выполняет неоюходимые матричные трансформации и отрисовывает данный объект в заднем буффере;
		void step();
		void setname(const char* nm);
		ID3D11Buffer *pVertexBuffer = NULL; // Буфер вершин
		ID3D11Buffer *pIndexBuffer = NULL; // Буфер индексов
		~OBJECT();
		OBJECT* getadress();
		LIGHT *lightOn;
		float direction = 0;
		float speed = 0;
		XMFLOAT4 blend;
	private:
		ID3D11ShaderResourceView* pTextureRV = NULL; //Объект текстуры
		ID3D11SamplerState* pSamplerLinear = NULL; //параметры nalozheniya текстуры obrazec
		
};

