#include "stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass()
{

}
D3DClass::D3DClass(const D3DClass& other)
{

}
D3DClass::~D3DClass()
{

}
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hWnd, bool fullscreen,
						  float screenDepth, float screenNear)
{
	//vsync 여부
	m_vsync_enabled = vsync;

	#pragma region Display Component Connect
	//DXGI DirectX Graphic Infrastructure
//그래픽 하부조직
//유저단계의 부분 중 가장 로우 레벨을 다루는 인터페이스
//연결된 장치를 관리하고 출력한다.

//연결된 장치들의 셋, 대충 메인 보드 쪽
	IDXGIFactory* factory = nullptr;
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
		return false;

	//그래픽 카드들의 셋
	IDXGIAdapter *adapter = nullptr;
	if (FAILED(factory->EnumAdapters(0, &adapter)))
		return false;

	//디스플레이들의 셋
	IDXGIOutput *adapterOutput = nullptr;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
		return false;

	//모든 디스플레이 장치들을 가져온다
	UINT numModes = 0;
	if (FAILED(adapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
		return false;

	//디스플레이 종류들
	DXGI_MODE_DESC *displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	//displayModeList라는 변수안에 가져온다.
	if (FAILED(adapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
		return false;

	//분자, 분모
	UINT numerator = 0, denominator = 0;
	//연결되어 있는 디스플레이 순회
	for (UINT i = 0; i < numModes; i++)
	{
		auto& now = displayModeList[i];
		//현재 맞는 크기가 있을 경우
		if (now.Width == (UINT)screenWidth &&
			now.Height == (UINT)screenHeight)
		{
			//대충 Hz 헤르츠
			numerator = now.RefreshRate.Numerator;
			denominator = now.RefreshRate.Denominator;
		}
	}

	//그래픽카드 정보
	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
		return false;

	//비디오카드 메모리, 단위는 MB
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024.0 / 1024.0);

	size_t stringLength = 0;
	//m_videoCardDescription에 현재 연결된 그래픽카드의 설명을 복사
	if (wcstombs_s(&stringLength, m_videoCardDescription, 128,
				   adapterDesc.Description, 128) != 0)
		return false;

	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;
	#pragma endregion

	#pragma region SwapChain
	//swapchain? 대충 화면을 그릴 때 그려지는 버퍼들을 swap해주는 장치
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//체인 버퍼 개수
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (m_vsync_enabled)
	{
		//vsync라면 아까 구한 헤르츠에 맞게 설정
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		//아니라면 0/1 = 무한이므로, 프레임 제한이 없음
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//백버퍼를 설명할 값
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//내보낼 윈도우 핸들
	swapChainDesc.OutputWindow = hWnd;

	//다중 샘플링 갯수
	swapChainDesc.SampleDesc.Count = 1;
	//다중 샘플링 품질
	swapChainDesc.SampleDesc.Quality = 0;

	//창
	swapChainDesc.Windowed = !fullscreen;

	//스캔 라인 모드
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//스케일링 모드
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//버퍼를 내용을 처리하는 옵션
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//스왑 체인의 동작 옵션
	swapChainDesc.Flags = 0;

	//directX 기능 수준
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	//swpachain만들기
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
											 NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION,
											 &swapChainDesc, &m_swapChain, &m_device,
											 NULL, &m_deviceContext)))
	{
		return false;
	}
	#pragma endregion

	#pragma region Back Buffer
	//백버퍼 텍스쳐
	ID3D11Texture2D* backBufferPtr = nullptr;
	//백버퍼 가져오기
	if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
		return false;

	//렌더링 타겟을 볼 수 있는 뷰를 만듭니다.
	if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView)))
		return false;

	backBufferPtr->Release();
	backBufferPtr = 0;
	#pragma endregion

	#pragma region Depth Buffer And Stencil
	//텍스쳐 2D 설명
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//depth buffer는 픽셀의 깊이를 나타낸다
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	//MipMap 거리가 멀수록 낮은 해상도로 렌더링 속도를 향상 시킨다.
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//stencil을 어떤 픽셀에 마스킹
	if (FAILED(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))
		return false;

	//stencil 정보
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//깊이 사용
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//압축 방법
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//앞면
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//앞으로 갈수록 연산량 증가
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//압축 방법
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//뒤로 갈수록 연산량 감소
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//압축 방법
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//detph stencil 만들기
	if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
		return false;

	//현재 deviceContext에 depthStencil 적용
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//renderTargetView랑 차이점
	//renderTargetView : color
	//depthStencilView : detpth, stencilValue
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//DepthStencilView 만들기
	if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc,
												&m_depthStencilView)))
		return false;

	//RenderTarget으로 depthStencilView 설정
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	#pragma endregion

	#pragma region Raster
	//래스터 정보
	D3D11_RASTERIZER_DESC rasterDesc;
	//안티에일러징
	rasterDesc.AntialiasedLineEnable = false;
	//컬링 모드
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;

	//채우기 모드
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//Raster State 만들기
	if (FAILED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState)))
		return false;

	//Raster State 셋팅
	m_deviceContext->RSSetState(m_rasterState);
	#pragma endregion

	#pragma region Viewport
	//뷰포트 만들기
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	//깊이 설정
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//피벗?
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//Viewport 설정
	m_deviceContext->RSSetViewports(1, &viewport);

	float fieldOfView = 3.141592654f / 4.0f;
	//화면 비율
	float screenAspect = (float)screenWidth / (float)screenHeight;

	//각 Matrix 셋팅
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	m_worldMatrix = XMMatrixIdentity();
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);
	#pragma endregion

	return true;
}

void D3DClass::Shutdown()
{
	if (m_swapChain)
		m_swapChain->SetFullscreenState(false, NULL);
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}
	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}
	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}
	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}
	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}
}
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4] = { red, green, blue, alpha };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void D3DClass::EndScene()
{
	if (m_vsync_enabled) m_swapChain->Present(1, 0);
	else m_swapChain->Present(0, 0);
}
ID3D11Device* D3DClass::GetDevice() { return m_device; }
ID3D11DeviceContext* D3DClass::GetDeviceContext() { return m_deviceContext; }

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}
void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}
void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(std::string& cardName, int& memory)
{
	cardName = std::string(m_videoCardDescription, 128);
	memory = m_videoCardMemory;
}
