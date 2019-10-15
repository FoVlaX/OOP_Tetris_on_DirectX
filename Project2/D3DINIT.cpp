#include "D3DINIT.h"
using namespace std;

int OBJECT::global_ids[100] = { 0 };
int OBJECT::current_id = 0;
OBJECT* OBJECT::set_gg = NULL;
float D3DINIT::ViewDist = 9.f;
XMVECTOR D3DINIT::g_Eye = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
XMVECTOR D3DINIT::g_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
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
	if FAILED(hr)
		return hr;
	//������� ������ ����� � SDK
	//RenderTargetOutput - ��� �������� �����, RenderTargetView - ������

	ID3D11Texture2D* pBackBuffer = NULL;//������ �������, �.�. ������� ������ �������� ����� ����� ��� ��������� ��� ����� ������ � ��� ��������
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
	if FAILED(hr) 
		return hr;
	hr = g_pd3device->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if FAILED(hr)
		return hr;

	//������� ������ ������
	// ������� �������� �������� ������ ������

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
	//��� ������ ����������� ��������� �������� ��������� ������ �������
	hr = g_pd3device->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;
	//now ������� ��� ������ ������� ������ ������� ��� ������ ��������� � ���������
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3device->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;
	//������ ��������� � ����������� �������� ����� ��� ���� (������ � ������ ������)
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

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
	g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pConstantBufferLight); //1 - ����� ����� � �������� ������ � �������
	g_pImmediateContext->PSSetConstantBuffers(1, 1, &g_pConstantBufferLight);



}

void D3DINIT::SetGameSpeed(int spd) //spd  - ������ � ��������;
{
	GameSpeed = 1000 / spd;
}

void D3DINIT::RenderEnd()
{
	g_pSwapChain->Present(0, 0);
	Sleep(GameSpeed);
}

HRESULT D3DINIT::InitGeometry() //������� � ����������� ������
{
	HRESULT hr = S_OK;
	//���������� ���������� ������� �� �����
	ID3DBlob* pVSBlob = NULL; //��������������� ����� ������ ����� � ����������� ������
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
	//����������� ������� ������
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,20,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	UINT numElements = ARRAYSIZE(layout);
	//�������� ������� ������
	hr = g_pd3device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	ID3DBlob* pPSBlob = NULL; //��������������� ����� ������ ����� � ����������� ������
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

	//�������� ������ ������
	D3D11_BUFFER_DESC bd;
	

	//�������� ������������ ������
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3device->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr)) 
		return hr;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3device->CreateBuffer(&bd, NULL, &g_pConstantBufferLight);
	if (FAILED(hr))
		return hr;
	//�������� �������� �� �����


	if (FAILED(hr)) return hr;
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
			MessageBox(NULL, (const char*)pErrorBlob->GetBufferPointer(), "Error", NULL);
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
	g_World = XMMatrixIdentity(); //�������������� ������� ����
	// �������������� ������� ����
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -9.0f, 0.0f); //������ �������
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); //���� �������
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // ����������� �����
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	//������������� ������� ��������
	vLightDirs[0] = { -0.5f,0.5f,-0.5f,1.f };
	vLightDirs[1] = { -0.5f,0.5f,-0.5f,1.f };

	vLightColors[0] = { 0.8f,1.f,1.f,1.f };
	vLightColors[1] = {0.8f, 1.f, 1.f, 1.f};

	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);
	ConstantBufferLight cbl;
	cbl.vLightColor[0] = vLightColors[0];
	cbl.vLightColor[1] = vLightColors[1];
	cbl.vLightDir[0] = vLightDirs[0];
	cbl.vLightDir[1] = vLightDirs[1];
	cbl.vOutputColor = vOutputColor;
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
	g_pImmediateContext->UpdateSubresource(g_pConstantBufferLight, 0, NULL, &cbl, 0, 0);
	return S_OK;
}

void D3DINIT::SetView(float angleY, float angleX)
{
	g_At = XMVectorSet(OBJECT::set_gg->x, OBJECT::set_gg->y, OBJECT::set_gg->z,0.f);
	if (angleY != 0 || angleX !=0)
	{
		if (angleY != 0) {
			g_Eye = XMVector3Rotate(g_Eye, XMVectorSet(0.f, 1 * sin(angleY), 0.f, cos(angleY)));
			helpXas = XMVector3Rotate(helpXas, XMVectorSet(0.f, 1 * sin(angleY), 0.f, cos(angleY)));
			
		}
		g_Eye = XMVector3Rotate(g_Eye, XMVectorSet(XMVectorGetX(helpXas)*sin(angleX), XMVectorGetY(helpXas) * sin(angleX), XMVectorGetZ(helpXas) * sin(angleX), cos(angleX)));
		vertAng += angleX * 2;
		if (vertAng > 2 * XM_PI) vertAng -= 2 * XM_PI;
		if (vertAng < 0) vertAng += 2 * XM_PI;
	
	}
	if (vertAng > XM_PIDIV2&& vertAng < 3 * XM_PIDIV2)
	{
		g_Up = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	}
	else
	{
		g_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	}
	XMVECTOR Eye = D3DINIT::ViewDist*g_Eye+g_At;
	g_View = XMMatrixLookAtLH(Eye, g_At, g_Up);
}

OBJECT::OBJECT(char const* vertxt,  char const* texture, HRESULT &hr)
{

	FILE* vtxt;

	vtxt = fopen(vertxt, "r");// ��������� ���� ������

	XMFLOAT3* vert = new XMFLOAT3;
	XMFLOAT2* vtext = new XMFLOAT2;
	XMFLOAT3* vnorm = new XMFLOAT3;

	char s[40];

	while (strcmp(s, "o"))
		fscanf(vtxt, "%s", &s);

	char objName[40];
	fscanf(vtxt, "%s", &objName);

	fscanf(vtxt, "%s", &s);
	int i = 1;
	while (!strcmp(s, "v"))
	{
		vert = (XMFLOAT3*)realloc((void*)vert, sizeof(XMFLOAT3) * i);
		fscanf(vtxt, "%f%f%f", &vert[i - 1].x, &vert[i - 1].y, &vert[i - 1].z);
		i++;
		fscanf(vtxt, "%s", &s);
	}
	i = 1;
	while (!strcmp(s, "vt"))
	{
		vtext = (XMFLOAT2*)realloc((void*)vtext, sizeof(XMFLOAT2) * i);
		fscanf(vtxt, "%f%f", &vtext[i - 1].x, &vtext[i - 1].y);
		i++;
		fscanf(vtxt, "%s", &s);
	}
	i = 1;
	while (!strcmp(s, "vn"))
	{
		vnorm = (XMFLOAT3*)realloc((void*)vnorm, sizeof(XMFLOAT3) * i);
		fscanf(vtxt, "%f%f%f", &vnorm[i - 1].x, &vnorm[i - 1].y, &vnorm[i - 1].z);
		i++;
		fscanf(vtxt, "%s", &s);
	}
	while (strcmp(s,"off")) fscanf(vtxt, "%s", &s);
	i = 3;
	SimpleVertex* vercicles = new SimpleVertex;
	fscanf(vtxt, "%s", &s);
	while (!feof(vtxt) && !strcmp(s,"f"))
	{
		vercicles = (SimpleVertex*)realloc((void*)vercicles, sizeof(SimpleVertex) * i);
		int x1, x2, x3;
		int nx1, nx2, nx3;
		int tx1, tx2, tx3;
		
		fscanf(vtxt, "%i/%i/%i  %i/%i/%i  %i/%i/%i", &x1, &tx1, &nx1, &x2, &tx2, &nx2, &x3, &tx3, &nx3);
		vercicles[i - 3].Pos.x = vert[x1 - 1].x;
		vercicles[i - 3].Pos.y = vert[x1 - 1].y;
		vercicles[i - 3].Pos.z = vert[x1 - 1].z;
		vercicles[i - 2].Pos.x = vert[x2 - 1].x;
		vercicles[i - 2].Pos.y = vert[x2 - 1].y;
		vercicles[i - 2].Pos.z = vert[x2 - 1].z;
		vercicles[i - 1].Pos.x = vert[x3 - 1].x;
		vercicles[i - 1].Pos.y = vert[x3 - 1].y;
		vercicles[i - 1].Pos.z = vert[x3 - 1].z;

		vercicles[i - 3].Normal.x = vnorm[nx1 - 1].x;
		vercicles[i - 3].Normal.y = vnorm[nx1 - 1].y;
		vercicles[i - 3].Normal.z = vnorm[nx1 - 1].z;
		vercicles[i - 2].Normal.x = vnorm[nx2 - 1].x;
		vercicles[i - 2].Normal.y = vnorm[nx2 - 1].y;
		vercicles[i - 2].Normal.z = vnorm[nx2 - 1].z;
		vercicles[i - 1].Normal.x = vnorm[nx3 - 1].x;
		vercicles[i - 1].Normal.y = vnorm[nx3 - 1].y;
		vercicles[i - 1].Normal.z = vnorm[nx3 - 1].z;

		vercicles[i - 3].Tex.x = vtext[tx1 - 1].x;
		vercicles[i - 3].Tex.y = vtext[tx1 - 1].y;

		vercicles[i - 2].Tex.x = vtext[tx2 - 1].x;
		vercicles[i - 2].Tex.y = vtext[tx2 - 1].y;

		vercicles[i - 1].Tex.x = vtext[tx3 - 1].x;
		vercicles[i - 1].Tex.y = vtext[tx3 - 1].y;
		i += 3;
		fscanf(vtxt, "%s", &s);

	}
	m = i-3;
	/*int n = 0;
	fscanf(vtxt, "%i", &n);*/


	
	/*for (int i = 0; i < n; i++) //���������� �������� ���������
	{
		fscanf(vtxt, "%f", &vercicles[i].Pos.x);
		fscanf(vtxt, "%f", &vercicles[i].Pos.y);
		fscanf(vtxt, "%f", &vercicles[i].Pos.z);
		fscanf(vtxt, "%f", &vercicles[i].Tex.x);
		fscanf(vtxt, "%f", &vercicles[i].Tex.y);
		fscanf(vtxt, "%f", &vercicles[i].Normal.x);
		fscanf(vtxt, "%f", &vercicles[i].Normal.y);
		fscanf(vtxt, "%f", &vercicles[i].Normal.z);

	}

	fscanf(vtxt, "%i",&m);*/


	WORD* indexes = new WORD[m];
	//ZeroMemory(indexes, sizeof(WORD)*m);
	/*for (int i = 0; i < m; i++)
	{
		fscanf(vtxt, "%i", &indexes[i]);
	}*/
	for (int k = 0; k < m; k++)
	{
		indexes[k] =k;
	}


	D3D11_BUFFER_DESC bd; //c��������� ���������� ��� �����
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * m; //������ ������ = ������ ����� �������*�;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData; // ���������, ��������� ������ ������;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vercicles; // ��������� �� ���� �������;

								  //������� ������ ������� ������ ID3D11Buffer
	hr = g_pd3device->CreateBuffer(&bd, &InitData, &pVertexBuffer);
	if (FAILED(hr))
		return;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * m; //6 ������������� 18 ������ 6*3
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER; //��� ������ - ��������
	bd.CPUAccessFlags = 0;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indexes;
	//������� ���������� ������ ������ ���������
	hr = g_pd3device->CreateBuffer(&bd, &InitData, &pIndexBuffer);
	if (FAILED(hr))
		return;


	delete[]vercicles;
	indexes = NULL;
	delete[]indexes;
	delete[]vert;
	delete[]vnorm;
	delete[]vtext;
	id = OBJECT::current_id;
	OBJECT::current_id++;
	OBJECT::global_ids[id] = int(this);
	fclose(vtxt);

	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3device, texture, NULL, NULL, &pTextureRV, NULL);
	//
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;         // ������ ����������
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// ������� ��������� ������ ��������������
	hr = g_pd3device->CreateSamplerState(&sampDesc, &pSamplerLinear);

}
void OBJECT::step()
{
	/*
	x += vx;
	y += vy;
	z += vz;
	xang += vxang;
	yang += vyang;
	zang += vzang;
	*/
	XMVECTOR vpmg = XMVectorSet(XMVectorGetX(D3DINIT::g_Eye), 0, XMVectorGetZ(D3DINIT::g_Eye), 0);
	vpmg = XMVector3Normalize(vpmg);
	float ang = acos(XMVectorGetX(vpmg))+XM_PIDIV2;
	float k = XMVectorGetY(D3DINIT::g_Up);

	yang = XMVectorGetZ(D3DINIT::g_Eye) < 0 ? ang :  2*XM_PI -  ang;

	z += XMVectorGetZ(D3DINIT::g_Eye) < 0 ? speed * cos(ang - XM_PIDIV2): speed * cos(2*XM_PI-ang- XM_PIDIV2);
	x += XMVectorGetZ(D3DINIT::g_Eye) < 0 ? speed * sin(ang - XM_PIDIV2): speed * sin(2*XM_PI-ang- XM_PIDIV2);
	
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
	// ������� ������ � ��������� ������
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	g_pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset); //� ���������� ������� ������ ���
	//��������� ������ ��������
	g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//��������� ������� ��������� ������ � �����
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//����� ������ ������������ ��������� ������������� ��� ����������� ��������� ������� � ����
	XMMATRIX mSpinY = XMMatrixRotationY(yang); //������������ ��������� �� ������ ��������� �� ���� -� ++ �����
	XMMATRIX mSpinX = XMMatrixRotationX(xang);
	XMMATRIX mSpinZ = XMMatrixRotationZ(zang);// ������� �������� ������ ���� ���
	XMMATRIX mTrans = XMMatrixTranslation(x, y, z); //������ �������� �� -3 �� ��� �
	XMMATRIX mScale = XMMatrixScaling(xs,ys,zs); //������ ������ ��������
	g_World = mScale * mSpinY*mSpinX*mSpinZ*mTrans;
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
	
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer); // 0 - ����� ����� � �������� ������ � �������
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	g_pImmediateContext->PSSetShaderResources(0, 1, &pTextureRV);
	g_pImmediateContext->PSSetSamplers(0, 1, &pSamplerLinear);

	g_pImmediateContext->DrawIndexed(m, 0, 0);

}



OBJECT::~OBJECT()
{

}