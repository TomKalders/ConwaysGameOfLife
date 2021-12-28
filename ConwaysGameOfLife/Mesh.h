#pragma once
#include "glm.hpp"
#include "BaseEffect.h"

#include <set>
#include <map>
#include <vector>

#pragma warning(push)
#pragma warning(disable:4616)
#pragma warning(disable:4201)
#include <gtc/type_ptr.hpp>
#pragma warning(pop)

enum FileType
{
	OBJ,
	VTK
};

struct VertexInput
{
	VertexInput(const glm::fvec3& position,
		const glm::fvec3& color1,
		const glm::fvec3& color2,
		const glm::fvec3& normal,
		const glm::fvec2& uv,
		uint32_t index)
		: position(position)
		, color1(color1)
		, color2(color2)
		, normal(normal)
		, uv(uv)
		, tangent({0, 0, 0})
		, pulseStrength(0.f)
		, index(index)
		, propogationSpeed(1.f)
		, timeToTravel(0.f)
		, neighbourIndices({})
	{
	}

	VertexInput()
		: position{}
		, color1{1, 1, 1}
		, color2{0, 0, 0}
		, normal{1, 0, 0}
		, uv{}
		, tangent{1, 0, 0}
		, pulseStrength{}
		, index{}
		, propogationSpeed{}
		, timeToTravel{}
		, neighbourIndices{}
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

	//these member variables are not part of the input layout
	uint32_t index;
	float propogationSpeed;
	float timeToTravel;
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

	bool IsPulsed()
	{
		return pulseStrength >= 0.3f;
	}
};

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<VertexInput>& vertices, const std::vector<uint32_t>& indices);
	Mesh(ID3D11Device* pDevice, const std::string& filepath, bool skipOptimization = false, FileType fileType = FileType::OBJ, int nrOfThreads = 1);
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext, const float* worldViewProjMatrix, const float* inverseView);
	void UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext);

	void UpdateMesh(ID3D11DeviceContext* pDeviceContext, float deltaTime);
	void UpdateMeshV2(ID3D11DeviceContext* pDeviceContext, float deltaTime);

	void PulseVertex(uint32_t index, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);
	void PulseVertex(VertexInput* vertex, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);

	void PulseVertexV2(uint32_t index, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);
	void PulseVertexV2(VertexInput* vertex, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);

	void PulseMesh(ID3D11DeviceContext* pDeviceContext);
	void ClearPulse(ID3D11DeviceContext* pDeviceContext);

	glm::mat4 GetWorldMatrix();
	const std::vector<uint32_t>& GetIndexBuffer();
	const std::vector<VertexInput>& GetVertexBuffer();
	std::vector<VertexInput>& GetVertexBufferReference();
	const std::set<VertexInput*>& GetVerticesToUpdate();

	void SetVertexBuffer(ID3D11DeviceContext* pDeviceContext, const std::vector<VertexInput>& vertexBuffer);
	void SetWireframe(bool enabled);

	glm::fvec3 GetScale();
	void SetScale(const glm::fvec3& scale);
	void SetScale(float x, float y, float z);

private:
	Mesh();

	//----- DirectX -----
	BaseEffect* m_pEffect;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11RasterizerState* m_pRasterizerStateWireframe;
	ID3D11RasterizerState* m_pRasterizerStateSolid;
	bool m_WireFrameEnabled;

	uint32_t m_AmountIndices;
	//-------------------

	//Initialization of mesh
	void LoadMeshFromOBJ(const std::string& pathName, uint32_t nrOfThreads = 1);
	void LoadMeshFromVTK(const std::string& pathName);
	void CalculateTangents();
	void OptimizeIndexBuffer();
	void OptimizeVertexBuffer();
	void GetNeighbours(int nrOfThreads = 1);

	bool m_SkipOptimization;

	//Vertex Data
	void PulseNeighbours(const VertexInput& vertex);
	bool IsAnyNeighbourActive(const VertexInput& vertex);

	glm::mat4 m_WorldMatrix;
	std::vector<uint32_t> m_IndexBuffer;
	std::vector<VertexInput> m_VertexBuffer;
	std::set<VertexInput*> m_VerticesToUpdate;
	std::set<VertexInput*> m_NeighboursToUpdate;
	std::map<VertexInput*, float> m_VerticesToUpdateV2;

	HRESULT CreateDirectXResources(ID3D11Device* pDevice, const std::vector<VertexInput>& vertices, const std::vector<uint32_t>& indices);
	void CreateEffect(ID3D11Device* pDevice);
};

