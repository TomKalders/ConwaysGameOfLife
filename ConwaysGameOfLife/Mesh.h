#pragma once
#include "glm.hpp"
#include "BaseEffect.h"

#include <set>
#include <map>
#include <vector>
#include <chrono>

#pragma warning(push)
#pragma warning(disable:4616)
#pragma warning(disable:4201)
#include <gtc/type_ptr.hpp>
#pragma warning(pop)

enum class FileType
{
	OBJ,
	VTK,
	BIN
};

enum class State
{
	Waiting,	//The vertex does not have a pulse running throug it
	Receiving,	//The vertex has an incoming pulse.
	APD,		//The vertex is in it's Action Potential Duration(APD)
	DI,			//The vertex is in it's Diastolic Interval(DI)
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
		, apVisualization(0.f)
		, state{State::Waiting}
		, index(index)
		, propogationSpeed(1.f)
		, actionPotential{ 0.f }
		, timePassed(0.f)
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
		, apVisualization{}
		, state{State::Waiting}
		, index{}
		, propogationSpeed{1.f}
		, actionPotential{0.f}
		, timePassed(0.f)
		, timeToTravel{}
		, neighbourIndices{}
	{
	}

	//Members part of input layout
	glm::fvec3 position;					//World position
	glm::fvec3 color1;						//Non-pulsed color
	glm::fvec3 color2;						//Pulsed color
	glm::fvec3 normal;						//World normal
	glm::fvec3 tangent;						//World tangent
	glm::fvec2 uv;							//UV coordinate
	float apVisualization;					//[0, 1] value to visualize pulse

	//Members not part of input layout
	State state;							//Current state of the vertex
	uint32_t index;							//Index of the vertex (used in optimization)
	float propogationSpeed;					//Propogation speed of the pulse
	float actionPotential;					//Current action potential (in mV)
	float timePassed;						//Float to store passed time in different states
	float timeToTravel;						//The time before activating this vertex
	std::set<uint32_t> neighbourIndices;	//The indices of the neighbouring vertices

	//Operator overloading
	bool operator==(const VertexInput& other)
	{
		return this->position == other.position;
		//return (abs(this->position.x - other.position.x) < 0.01f &&
		//		abs(this->position.y - other.position.y) < 0.01f &&
		//		abs(this->position.z - other.position.z) < 0.01f);
	}

	friend bool operator==(const VertexInput& rhs, const VertexInput& lhs)
	{
		return rhs.position == lhs.position;
		//return (abs(rhs.position.x - lhs.position.x) < 0.01f &&
		//		abs(rhs.position.y - lhs.position.y) < 0.01f &&
		//		abs(rhs.position.z - lhs.position.z) < 0.01f);
	}

	bool IsPulsed()
	{
		return apVisualization >= 0.3f;
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
	void PulseVertex(uint32_t index, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);
	void PulseVertex(VertexInput* vertex, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);

	void UpdateMeshV2(ID3D11DeviceContext* pDeviceContext, float deltaTime);
	void PulseVertexV2(uint32_t index, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);
	void PulseVertexV2(VertexInput* vertex, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);

	void UpdateMeshV3(ID3D11DeviceContext* pDeviceContext, float deltaTime);
	void PulseVertexV3(uint32_t index, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);
	void PulseVertexV3(VertexInput* vertex, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer = true);

	void PulseMesh(ID3D11DeviceContext* pDeviceContext);
	void ClearPulse(ID3D11DeviceContext* pDeviceContext);
	void CalculateNeighbours(int nrOfThreads = 1);

	const glm::mat4& GetWorldMatrix() const;
	const std::vector<uint32_t>& GetIndexBuffer() const;
	const std::vector<VertexInput>& GetVertexBuffer() const;
	std::vector<VertexInput>& GetVertexBufferReference();

	const std::set<VertexInput*>& GetVerticesToUpdate() const;
	const std::vector<float>& GetAPPlot() const;
	glm::fvec2 GetMinMax() const;
	float GetAPD() const;
	std::chrono::milliseconds GetDiastolicInterval() const;

	void SetVertexBuffer(ID3D11DeviceContext* pDeviceContext, const std::vector<VertexInput>& vertexBuffer);
	void SetWireframe(bool enabled);

	glm::fvec3 GetScale();
	void SetScale(const glm::fvec3& scale);
	void SetScale(float x, float y, float z);

	void CreateCachedBinary();
	void CreatedChacedNeighbours();
	void LoadCachedNeighbours();

private:
	Mesh();

	//----- DirectX -----
	BaseEffect* m_pEffect;
	BaseEffect* m_pOptimizerEffect;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11RasterizerState* m_pRasterizerStateWireframe;
	ID3D11RasterizerState* m_pRasterizerStateSolid;
	bool m_WireFrameEnabled;

	uint32_t m_AmountIndices;
	//-------------------

	//Initialization of mesh
	void LoadMeshFromOBJ(uint32_t nrOfThreads = 1);	//Should be put in an AssetLoader Class
	void LoadMeshFromVTK();							//Should be put in an AssetLoader Class
	void LoadMeshFromBIN();							//Should be put in an AssetLoader Class
	void CalculateTangents();						//Should be put in an AssetLoader Class
	void OptimizeIndexBuffer();						//Should be put in an AssetLoader Class
	void OptimizeIndexBufferLib();					//Should be put in an AssetLoader Class
	void OptimizeVertexBuffer();					//Should be put in an AssetLoader Class
	bool m_SkipOptimization;						//Should be put in an AssetLoader Class

	void CreateEffect(ID3D11Device* pDevice);
	HRESULT CreateDirectXResources(ID3D11Device* pDevice, const std::vector<VertexInput>& vertices, const std::vector<uint32_t>& indices);


	//Vertex Data
	void PulseNeighbours(const VertexInput& vertex);
	bool IsAnyNeighbourActive(const VertexInput& vertex);

	glm::mat4 m_WorldMatrix;
	std::vector<uint32_t> m_IndexBuffer;
	std::vector<VertexInput> m_VertexBuffer;
	std::set<VertexInput*> m_VerticesToUpdate;
	std::set<VertexInput*> m_NeighboursToUpdate;
	std::map<VertexInput*, float> m_VerticesToUpdateV2;

	//Plot Data
	void LoadPlotData(int nrOfValuesAPD);
	
	std::chrono::milliseconds m_DiastolicInterval;
	float m_APThreshold;
	float m_APMaxValue;
	float m_APMinValue;
	float m_APD;
	std::vector<float> m_APPlot; // APD (mV) in function of time (ms)
	std::vector<std::chrono::milliseconds> m_APDPlot; // APD (ms) in function of Diastolic Interval (ms)

	//File Data
	std::string m_PathName;
};

