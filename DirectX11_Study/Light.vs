/////////////////////////////////////////////////////////////
// Filename : color.vs
//////////////////////////////////////////////

//////////////
/// GLOBALS //
//////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

//////////////////////////////////
// Vertex Shader
///////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;

	// 올바르게 행렬 연산을 하기 위하여 position 벡터를  w까지 있는 4성분이 있는 것으로 사용합니다.
	input.position.w = 1.0f;

	// 정점의 위치를 월드, 뷰, 투영의 순으로 계산합니다.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// 픽셀 쉐이더의 텍스쳐 좌표를 저장한다.
	output.tex = input.tex;

	// 월드 행렬에 대해서만 법선 벡터를 계산합니다.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// 법선 벡터를 정규화합니다.
	output.normal = normalize(output.normal);

	// 정점 위치의 월드 포지션 계산.
	float4 worldPosition = mul(input.position, worldMatrix);

	// 월드 포지션에서 카메라를 향하는 벡터.
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// 뷰 방향 벡터를 정규화
	output.viewDirection = normalize(output.viewDirection);

	return output;
}