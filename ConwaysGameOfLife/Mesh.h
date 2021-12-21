#pragma once
#include "glm.hpp"
#include "BaseEffect.h"
#include "regex"

#include <set>
#include <vector>

#pragma warning(push)
#pragma warning(disable:4616)
#pragma warning(disable:4201)
#include <gtc/type_ptr.hpp>
#pragma warning(pop)

struct VertexInput
{
	VertexInput(const glm::fvec3& position,
		const glm::fvec3& color1,
		const glm::fvec3& color2,
		const glm::fvec3& normal,
		const glm::fvec2& uv)
		: position(position)
		, color1(color1)
		, color2(color2)
		, normal(normal)
		, uv(uv)
		, tangent({0, 0, 0})
		, pulseStrength(0.f)
		, PropogationSpeed(5.f)
		, timeBeforeActive(0.f)
		, neighbourIndices({})
	{
	}

	//Public member variables
	glm::fvec3 position;
	glm::fvec3 color1;
	glm::fvec3 color2;
	glm::fvec3 normal;
	glm::fvec3 tangent;
	glm::fvec2 uv;
	float pulseStrength;

	//this member variable not part of the input layout
	float PropogationSpeed;
	float timeBeforeActive;
	std::set<uint32_t> neighbourIndices;

	//Operator overloading
	bool operator==(const VertexInput& other)
	{
		return this->position == other.position;
	}

	friend bool operator==(const VertexInput& rhs, const VertexInput& lhs)
	{
		return rhs.position == lhs.position;
	}
};



class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<VertexInput>& vertices, const std::vector<uint32_t>& indices);
	Mesh(ID3D11Device* pDevice, const std::string& filepath);
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext, const float* worldViewProjMatrix, const float* inverseView);
	void UpdateMesh(ID3D11DeviceContext* pDeviceContext, float deltaTime);
	void PulseVertex(uint32_t index, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);
	void PulseVertex(VertexInput* vertex, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);
	void PulseMesh(ID3D11DeviceContext* pDeviceContext);

	glm::mat4 GetWorldMatrix();
	const std::vector<uint32_t>& GetIndexBuffer();
	const std::vector<VertexInput>& GetVertexBuffer();
	const std::set<VertexInput*>& GetVerticesToUpdate();

	void SetVertexBuffer(ID3D11DeviceContext* pDeviceContext, const std::vector<VertexInput>& vertexBuffer);
	void SetWireframe(bool enabled);

private:
	//			DirectX				//
	BaseEffect* m_pEffect;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11RasterizerState* m_pRasterizerStateWireframe;
	ID3D11RasterizerState* m_pRasterizerStateSolid;
	bool m_WireFrameEnabled;

	uint32_t m_AmountIndices;
	//////////////////////////////////

	glm::mat4 m_WorldMatrix;

	void LoadMeshFromOBJ(const std::string& pathName);
	void OptimizeIndexBuffer();
	void UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext);

	void PulseNeighbours(const VertexInput& vertex);
	std::vector<uint32_t> m_IndexBuffer;
	std::vector<VertexInput> m_VertexBuffer;
	std::set<VertexInput*> m_VerticesToUpdate;
	std::set<VertexInput*> m_NeighboursToUpdate;

	HRESULT CreateDirectXResources(ID3D11Device* pDevice, const std::vector<VertexInput>& vertices, const std::vector<uint32_t>& indices);
	void CreateEffect(ID3D11Device* pDevice);
};

