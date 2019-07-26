#include "stdafx.h"
#include "D3DClass.h"
#include "GraphicsClass.h"
#include "CameraClass.h"	
#include "BitmapClass.h"	
#include "LightClass.h"
#include "TextureShaderClass.h"


GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Bitmap = nullptr;
	m_textureShader = nullptr;
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
	m_Camera->SetPosition(0.0f, 0.0f, -1000.0f);

	// m_Model ��ü ����
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
		return false;

	// m_Model ��ü �ʱ�ȭ
	if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, 
		"../DirectX11_Study/CheckBox.tga", 256, 256))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	m_textureShader = new TextureShaderClass;
	if (!m_textureShader)
		return false;
	// m_LightShader ��ü �ʱ�ȭ
	if (!m_textureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

	// m_Light ��ü ����
	m_Light = new LightClass;

	// m_Light ��ü �ʱ�ȭ
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 0.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

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
	if (m_textureShader != nullptr)
	{
		m_textureShader->Shutdown();
		delete m_textureShader;
		m_textureShader = nullptr;
	}

	// m_Model ��ü ��ȯ
	if (m_Bitmap != nullptr)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = nullptr;
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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
	if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100))
		return false;

	// ���� ���̴��� ����Ͽ� ���� �������մϴ�.
	if (!m_textureShader->Render(
									m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
									worldMatrix, viewMatrix, orthoMatrix,
									m_Bitmap->GetTexture()
		))
		return false;

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOff();

	// ������ ������ ȭ�鿡 ����մϴ�.
	m_Direct3D->EndScene();
	return true;
}