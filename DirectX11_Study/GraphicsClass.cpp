#include "stdafx.h"
#include "D3DClass.h"
#include "GraphicsClass.h"
#include "CameraClass.h"	
#include "ModelClass.h"	
#include "LightClass.h"
#include "LightShaderClass.h"


GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
}


GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D 객체 생성
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
		return false;

	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// m_Camera 객체 생성
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// 카메라 포지션 설정
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// m_Model 객체 생성
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// m_Model 객체 초기화
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../DirectX11_Study/Cube.txt","../DirectX11_Study/CheckBox.tga"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
		return false;
	// m_LightShader 객체 초기화
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

	// m_Light 객체 생성
	m_Light = new LightClass;

	// m_Light 객체 초기화
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	return true;
}


void GraphicsClass::ShutDown()
{
	if (m_Light != nullptr)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	// m_LightShader 객체 반환
	if (m_LightShader != nullptr)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	// m_Model 객체 반환
	if (m_Model != nullptr)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// m_Camera 객체 반환
	if (m_Camera != nullptr)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// m_Direct3D 객체 반환
	if (m_Direct3D != nullptr)
	{
		m_Direct3D->Shutdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = nullptr;
	}
}


bool GraphicsClass::Frame()
{
	static float rotation = 0.0f;

	// 각 프레임의 rotation 변수를 업데이트 합니다.
	rotation += (float_t)XM_PI * 0.003f;
	if (rotation > 360.0f)
		rotation -= 360.0f;

	// 그래픽 랜더링 처리
	return Render(rotation);
}


bool GraphicsClass::Render(float rotation)
{
	// 씬을 그리기 위해 버퍼를 지웁니다.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 삼각형이 회전 할 수 있도록 회전 값으로 월드 행렬을 회전합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 색상 쉐이더를 사용하여 모델을 렌더링합니다.
	if (!m_LightShader->Render(
									m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), 
									worldMatrix, viewMatrix, projectionMatrix, 
									m_Model->GetTexture(),
									m_Light->GetDirection(), m_Light->GetDiffuseColor()
		))
		return false;

	// 버퍼의 내용을 화면에 출력합니다.
	m_Direct3D->EndScene();
	return true;
}