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
	// Direct3D ��ü ����
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
		return false;

	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// m_Camera ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// ī�޶� ������ ����
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// m_Model ��ü ����
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// m_Model ��ü �ʱ�ȭ
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../DirectX11_Study/Cube.txt","../DirectX11_Study/CheckBox.tga"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
		return false;
	// m_LightShader ��ü �ʱ�ȭ
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

	// m_Light ��ü ����
	m_Light = new LightClass;

	// m_Light ��ü �ʱ�ȭ
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

	// m_LightShader ��ü ��ȯ
	if (m_LightShader != nullptr)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
	}

	// m_Model ��ü ��ȯ
	if (m_Model != nullptr)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// m_Camera ��ü ��ȯ
	if (m_Camera != nullptr)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// m_Direct3D ��ü ��ȯ
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

	// �� �������� rotation ������ ������Ʈ �մϴ�.
	rotation += (float_t)XM_PI * 0.003f;
	if (rotation > 360.0f)
		rotation -= 360.0f;

	// �׷��� ������ ó��
	return Render(rotation);
}


bool GraphicsClass::Render(float rotation)
{
	// ���� �׸��� ���� ���۸� ����ϴ�.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// �ﰢ���� ȸ�� �� �� �ֵ��� ȸ�� ������ ���� ����� ȸ���մϴ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// ���� ���̴��� ����Ͽ� ���� �������մϴ�.
	if (!m_LightShader->Render(
									m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), 
									worldMatrix, viewMatrix, projectionMatrix, 
									m_Model->GetTexture(),
									m_Light->GetDirection(), m_Light->GetDiffuseColor()
		))
		return false;

	// ������ ������ ȭ�鿡 ����մϴ�.
	m_Direct3D->EndScene();
	return true;
}