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

	// �ùٸ��� ��� ������ �ϱ� ���Ͽ� position ���͸�  w���� �ִ� 4������ �ִ� ������ ����մϴ�.
	input.position.w = 1.0f;

	// ������ ��ġ�� ����, ��, ������ ������ ����մϴ�.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// �ȼ� ���̴��� �ؽ��� ��ǥ�� �����Ѵ�.
	output.tex = input.tex;

	// ���� ��Ŀ� ���ؼ��� ���� ���͸� ����մϴ�.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// ���� ���͸� ����ȭ�մϴ�.
	output.normal = normalize(output.normal);

	// ���� ��ġ�� ���� ������ ���.
	float4 worldPosition = mul(input.position, worldMatrix);

	// ���� �����ǿ��� ī�޶� ���ϴ� ����.
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// �� ���� ���͸� ����ȭ
	output.viewDirection = normalize(output.viewDirection);

	return output;
}