#pragma once
#include <vector>
#include "glm.hpp"
#include "BaseEffect.h"
#include "regex"

#pragma warning(push)
#pragma warning(disable:4616)
#pragma warning(disable:4201)
#include <gtc/type_ptr.hpp>
#pragma warning(pop)

struct float2
{
	float x, y;
};

struct float3
{
	float x, y, z;
};

struct float4
{
	float x, y, z, w;
};

struct Vertex_Input
{
	float3 position;
	float3 color;
	float3 normal;
	//float2 uv;
};

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices);
	Mesh(ID3D11Device* pDevice, const std::string& filepath);
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext, const float* worldViewProjMatrix, const float* inverseView);

	glm::mat4 GetWorldMatrix();

private:
	//			DirectX				//
	BaseEffect* m_pEffect;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	uint32_t m_AmountIndices;
	//////////////////////////////////

	ID3D11ShaderResourceView* m_pResourceView;

	glm::mat4 m_WorldMatrix;

	void LoadMeshFromOBJ(const std::string& pathName);
	std::vector<uint32_t> m_IndexBuffer;
	std::vector<Vertex_Input> m_VertexBuffer;

	HRESULT CreateDirectXResources(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices);
	void CreateEffect(ID3D11Device* pDevice/*, bool isTransparent*/);
};

