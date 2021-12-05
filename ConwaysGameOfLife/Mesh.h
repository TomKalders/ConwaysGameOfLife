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

struct Vertex_Input
{
	Vertex_Input(const glm::fvec3& position,
		const glm::fvec3& color,
		const glm::fvec3& normal,
		const glm::fvec2& uv)
		: position(position)
		, color(color)
		, normal(normal)
		, uv(uv)
	{
	}

	glm::fvec3 position;
	glm::fvec3 color;
	glm::fvec3 normal;
	glm::fvec3 tangent;
	glm::fvec2 uv;
	float power;
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
	const std::vector<uint32_t>& GetIndexBuffer();
	const std::vector<Vertex_Input>& GetVertexBuffer();

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

