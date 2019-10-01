#include "D3DINIT.h"
using namespace std;

int OBJECT::global_ids[100] = { 0 };
int OBJECT::current_id = 0;
OBJECT* OBJECT::set_gg = NULL;
D3DINIT::D3DINIT(HWND mwh)
{
	main_window_handle = mwh;
}


D3DINIT::~D3DINIT()
{
}

HRESULT D3DINIT::InitDevice()
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(main_window_handle, &rc);
	UINT width = rc.right - rc.left; //получаем ширину
	UINT height = rc.bottom - rc.top; // и высоту окна

	UINT createDeviceFlags = 0;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes); //создаем массив для проверки на хардварную обработку
	//создаем список поддерживаемых версий директХ

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	//Создание устройств директХ. Заполним структуру
	// которая описывает свойства переднегобуфера и привязывает его к нашему окну
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1; //one back buffer
	sd.BufferDesc.Width = width; //shirina bufera
	sd.BufferDesc.Height = height; //visota buffera
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ФОРМАТ ПИКСЕЛЯ В БУФЕРЕ
	sd.BufferDesc.RefreshRate.Numerator = 75; // частота обновления экрана
	sd.BufferDesc.RefreshRate.Denominator = 1;// 
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //НАЗНАЧЕНИЕ БУФЕРА - peredniu? БУФЕР
	sd.OutputWindow = main_window_handle; //привязка к окну
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE; // оконный режим
	//
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_drivertype = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_drivertype, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
			&sd, &g_pSwapChain, &g_pd3device, &g_featurelevel, &g_pImmediateContext);
		if SUCCEEDED(hr) break; //если устройства созданы то выходим из цикла
	}
	if FAILED(hr)
		return hr;
	//создаем задний буфер в SDK
	//RenderTargetOutput - это передний буфер, RenderTargetView - задний

	ID3D11Texture2D* pBackBuffer = NULL;//объект текстур, т.е. область памяти которуюю можно юзать для расования как буфер глубин и как текстуру
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
	if FAILED(hr) 
		return hr;
	hr = g_pd3device->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if FAILED(hr)
		return hr;

	//Создаем буффер глубин
	// Создаем текстуру описание буфера глубин

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	//при помощи заполненной структуры описания созахдаем объект тектуры
	hr = g_pd3device->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;
	//now создаем сам объект буффера глубин сначало как обычно стурктура с описынием
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3device->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;
	//теперь подключим к устройствву рисоваия сразу оба вида (глубин и задний буффер)
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	//настройка вьюпорта
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	//подключаем вьюпорт к контексту устроства
	g_pImmediateContext->RSSetViewports(1, &vp);
	return S_OK;
}

void D3DINIT::CleanUpDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3device)  g_pd3device->Release();
}

void D3DINIT::RenderStart()
{
	float ClearColor[4] = { 0.5f,0.5f,0.6f,1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);


}

void D3DINIT::SetGameSpeed(int spd) //spd  - кадров в секкунду;
{
	GameSpeed = 1000 / spd;
}

void D3DINIT::RenderEnd()
{
	g_pSwapChain->Present(0, 0);
	Sleep(GameSpeed);
}

HRESULT D3DINIT::InitGeometry() //шейдеры и констынтный буффер
{
	HRESULT hr = S_OK;
	//компиляция вершинного шейдера из файла
	ID3DBlob* pVSBlob = NULL; //вспомогательный оъект просто место в оперативной памяти
	hr = CompileShaderFromFile("urok2.fx", "VS", "vs_5_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, "VS Don't compile file FX, Please, execute this porgram from directory with FX file", "ERROR", MB_OK);
		return hr;
	}

	//Create Vertex SHader
	hr = g_pd3device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}
	//определение шаблона вершин
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	UINT numElements = ARRAYSIZE(layout);
	//создание шаблона вершин
	hr = g_pd3device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	ID3DBlob* pPSBlob = NULL; //вспомогательный оъект просто место в оперативной памяти
	hr = CompileShaderFromFile("urok2.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, "PS Don't compile file FX, Please, execute this porgram from directory with FX file", "ERROR", MB_OK);
		return hr;
	}

	//Create Vertex SHader
	hr = g_pd3device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	if (FAILED(hr))
	{
		pPSBlob->Release();
		return hr;
	}

	//создание буфера вершин
	D3D11_BUFFER_DESC bd;
	

	//Создание константного буфера
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3device->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr)) 
		return hr;

	return S_OK;
}
HRESULT D3DINIT::CompileShaderFromFile(LPCSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel, dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
		{
			OutputDebugStringA((char *)pErrorBlob->GetBufferPointer());
		}
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();
	return S_OK;
}

HRESULT D3DINIT::InitMatrixes()
{
	RECT rc;
	GetClientRect(main_window_handle, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	g_World = XMMatrixIdentity(); //инициализируем матрицу мира
	// инициализируем матрицу вида
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -9.0f, 0.0f); //откуда смотрим
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); //Куда смотрим
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Направление верха
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	//инициализация матрицы проекции
	vLightDirs[0] = { -0.5f,0.5f,-0.5f,1.f };
	vLightDirs[1] = { -0.5f,0.5f,0.5f,1.f };

	vLightColors[0] = { 1.f,1.f,1.f,1.f };
	vLightColors[1] = {1.f, 1.f, 1.f, 1.f};

	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);
	cb.vLightColor[0] = vLightColors[0];
	cb.vLightColor[1] = vLightColors[1];
	cb.vLightDir[0] = vLightDirs[0];
	cb.vLightDir[1] = vLightDirs[1];
	cb.vOutputColor = vOutputColor;
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
	return S_OK;
}

void D3DINIT::SetView()
{
	g_At = XMVectorSet(OBJECT::set_gg->x, OBJECT::set_gg->y, OBJECT::set_gg->z,0.f);
	
	g_Eye = XMVector3Rotate(g_Eye, XMVectorSet(0.f, 1 * sin(0.003f),0.f, cos(0.003f)));
	XMVECTOR Eye = g_At + g_Eye;
	g_View = XMMatrixLookAtLH(Eye, g_At, g_Up);
}

OBJECT::OBJECT(char const* vertxt, HRESULT &hr)
{
	
	FILE *vtxt;
	
	vtxt = fopen(vertxt, "r");// открываем наши фалйлы


	int n = 0;
	fscanf(vtxt, "%i", &n);

	SimpleVertex *vercicles = new SimpleVertex[n];



	for (int i = 0; i < n; i++) //собственно заполняем структуру
	{
		fscanf(vtxt, "%f", &vercicles[i].Pos.x);
		fscanf(vtxt, "%f", &vercicles[i].Pos.y);
		fscanf(vtxt, "%f", &vercicles[i].Pos.z);
		fscanf(vtxt, "%f", &vercicles[i].Normal.x);
		fscanf(vtxt, "%f", &vercicles[i].Normal.y);
		fscanf(vtxt, "%f", &vercicles[i].Normal.z);

	}
	
	fscanf(vtxt, "%i",&m);

	
	WORD *indexes = new WORD[m];
	//ZeroMemory(indexes, sizeof(WORD)*m);
	for (int i = 0; i < m; i++)
	{
		fscanf(vtxt, "%i", &indexes[i]);
	}


	

	D3D11_BUFFER_DESC bd; //cтруктурка описывающая наш буфер
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * n; //размер буфера = размер одной вершины*т;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData; // структура, содержащая данные буфера;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem =vercicles; // указатель на наши вершины;

								  //Создаем объект буффера вершин ID3D11Buffer
	hr = g_pd3device->CreateBuffer(&bd, &InitData, &pVertexBuffer);
	if (FAILED(hr))
		return;
	
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * m; //6 треугольников 18 вершин 6*3
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER; //тип буфера - индексов
	bd.CPUAccessFlags = 0;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indexes;
	//Создаем глобальный объект буфера индекссов
	hr = g_pd3device->CreateBuffer(&bd, &InitData, &pIndexBuffer);
	if (FAILED(hr))
		return;

	
	delete[]vercicles;
	indexes = NULL;
	delete[]indexes;
	id = OBJECT::current_id;
	OBJECT::current_id++;
	OBJECT::global_ids[id] = int(this);
	fclose(vtxt);
}
void OBJECT::step()
{
	x += vx;
	y += vy;
	z += vz;
}


OBJECT* OBJECT::getadress()
{
	return this;
}

void OBJECT::setname(const char* nm)
{
	name = nm;
}

void OBJECT::draw()
{
	// начиная отсюда в отрисовку бъекта
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	g_pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset); //в отрисовкку объекта пойдет это
	//установка буфера индексов
	g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//установка способа отрисовки вершин в буфре
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//ДАЛЕЕ ДЕЛАЕМ НЕООБХОДИМЫЕ МАТРИЧНЫЕ ТРАНСФОРМАЦИИ ДЛЯ ОТОБРАЖЕНИЯ ПОЛОЖЕНИЯ ОБЪЕКТА В МИРЕ
	XMMATRIX mSpinY = XMMatrixRotationY(yang); //поворачиваем смещенную от центра пираамиду на угол -т ++ англе
	XMMATRIX mSpinX = XMMatrixRotationX(xang);
	XMMATRIX mSpinZ = XMMatrixRotationZ(zang);// вращаем пирамиду вокруг воей оси
	XMMATRIX mTrans = XMMatrixTranslation(x, y, z); //смщаем пирамиду на -3 по оси х
	XMMATRIX mScale = XMMatrixScaling(xs,ys,zs); //меняем маштаб пирамиды
	g_World = mScale * mSpinY*mSpinX*mSpinZ*mTrans;
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);
	cb.vLightColor[0] = vLightColors[0];
	cb.vLightColor[1] = vLightColors[1];
	cb.vLightDir[0] = vLightDirs[0];
	cb.vLightDir[1] = vLightDirs[1];
	cb.vOutputColor = vLightColors[0];
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);

	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer); // 0 - точка входа в констант буффер в шейдере
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->DrawIndexed(m, 0, 0);

}



OBJECT::~OBJECT()
{

}