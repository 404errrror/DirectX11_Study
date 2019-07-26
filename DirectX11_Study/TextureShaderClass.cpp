#include "stdafx.h"
#include "TextureShaderClass.h"
#include <string>

TextureShaderClass::TextureShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
}


TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}


TextureShaderClass::~TextureShaderClass()
{
}


bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// ���� �� �ȼ� ���̴��� �ʱ�ȭ�մϴ�.
	return InitializeShader(device, hwnd, L"../DirectX11_Study/Texture.vs", L"../DirectX11_Study/Texture.ps");
}


void TextureShaderClass::Shutdown()
{
	ShutdownShader();
}


bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	// �������� ����� ���̴� �Ű� ������ �����մϴ�.
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture))
		return false;
	
	// ������ ���۸� ���̴��� �������մϴ�.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// ���ؽ� ���̴� �ڵ带 �������Ѵ�.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		// ���̴� ������ ���н� �����޽����� ����մϴ�.
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// �ȼ� ���̴� �ڵ带 �������մϴ�.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		// ���̴� ������ ���н� �����޽����� ����մϴ�.
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
		else
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}
	// ���۷κ��� ���� ���̴��� �����Ѵ�.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader)))
		return false;
	// ���۷κ��� �ȼ� ���̴��� �����մϴ�.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader)))
		return false;

	// ���� �Է� ���̾ƿ� ����ü�� �����մϴ�.
	// �� ������ ModelClass�� ���̴� VertexType ������ ��ġ�ؾ��մϴ�.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ��� ��� ���� �����ɴϴ�.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� ����ϴ�.
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
		return false;

	// �� �̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���� ���̴��� �ִ� ��� ��� ������ ����ü�� �ۼ��մϴ�.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �մϴ�.
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
		return false;

	// �ؽ��� ���÷� ���� ����ü�� ���� �� �����մϴ�.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// �ؽ�ó ���÷� ���¸� ����ϴ�.
	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState)))
		return false;

	return true;
}


#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;
void TextureShaderClass::ShutdownShader()
{

	SAFE_RELEASE(m_sampleState);			// ���÷� ���¸� �����մϴ�.
	SAFE_RELEASE(m_matrixBuffer);			// ��� ��� ���۸� �����մϴ�.
	SAFE_RELEASE(m_layout);					// ���̾ƿ��� �����մϴ�.
	SAFE_RELEASE(m_pixelShader);			// �ȼ� ���̴��� �����մϴ�.
	SAFE_RELEASE(m_vertexShader);			// ���ؽ� ���̴��� �����մϴ�.
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// ���� �޽����� ���â�� ǥ���մϴ�.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// ���� �޽����� ��ȯ�մϴ�.
	SAFE_RELEASE(errorMessage);

	// ������ ������ ������ �˾� �޽����� �˷��ݴϴ�.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}


bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�.
	// �� ��ġ��ķ� �ٲٴ��� �˻��غôµ�, DirectX�� �� ���� ����� ����ϰ� HLSL�� �� ���� ����� ����ϱ� ����.
	// matrix[0][0], matrix[0][1] ... �̷��� �޸𸮻� �ִٸ� GPU�ʿ��� matrix[0][0], matrix[1][0]... �̷��� �޸𸮿��� ������.
	// �� ���� ����� GPU���ɿ� �� ���� ������.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	// ��Ȯ���� GPU�� subresource�� �����ϴ� ���� �ź��ϰ�, subresource�� ���ԵǾ��� �������� �����͸� ������.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǳ�ϴ�.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	unsigned int bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}


void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����մϴ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ﰢ���� �׸��ϴ�.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}