#include "Mesh.h"
#include "fstream"

//External Header
#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4701)
#include "OBJ_Loader.h"
#undef OBJL_CONSOLE_OUTPUT
#pragma warning(pop)


Mesh::Mesh(ID3D11Device* pDevice, const std::vector<VertexInput>& vertices, const std::vector<uint32_t>& indices)
	: m_pEffect{}
	, m_pVertexLayout{nullptr}
	, m_pVertexBuffer{nullptr}
	, m_pIndexBuffer{nullptr}
	, m_pRasterizerStateWireframe{nullptr}
	, m_pRasterizerStateSolid{nullptr}
	, m_AmountIndices{}
	, m_WorldMatrix{ glm::mat4{1.f} }
{
	CreateEffect(pDevice);
	CreateDirectXResources(pDevice, vertices, indices);
}

Mesh::Mesh(ID3D11Device* pDevice, const std::string& filepath)
	: m_pEffect{}
	, m_pVertexLayout{ nullptr }
	, m_pVertexBuffer{ nullptr }
	, m_pIndexBuffer{ nullptr }
	, m_AmountIndices{}
	, m_WorldMatrix{ glm::mat4{1.f} }
{
	CreateEffect(pDevice);
	LoadMeshFromOBJ(filepath);
	CreateDirectXResources(pDevice, m_VertexBuffer, m_IndexBuffer);
}

Mesh::~Mesh()
{
	if (m_pRasterizerStateSolid)
		m_pRasterizerStateSolid->Release();

	if (m_pRasterizerStateWireframe)
		m_pRasterizerStateWireframe->Release();

	if (m_pIndexBuffer)
		m_pIndexBuffer->Release();

	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if (m_pVertexLayout)
		m_pVertexLayout->Release();

	if (m_pEffect)
		delete m_pEffect;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const float* worldViewProjMatrix, const float* inverseView)
{
	//Set vertex buffer
	UINT stride = sizeof(VertexInput);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//Set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set rasterizer state
	if (m_WireFrameEnabled)
		pDeviceContext->RSSetState(m_pRasterizerStateWireframe);
	else
		pDeviceContext->RSSetState(m_pRasterizerStateSolid);

	//Set input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	//Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the worldviewprojectionMatrix
	m_pEffect->GetWorldViewProjMatrix()->SetMatrix(worldViewProjMatrix);

	//Set the worldMatrix
	glm::mat4 world = glm::transpose(m_WorldMatrix);
	//glm::mat4 world = m_WorldMatrix;
	
	//float* data = (float*)(world[0].x);
	float* data = (float*)glm::value_ptr(world);
	m_pEffect->GetWorldMatrix()->SetMatrix(data);

	//Set the InverseViewMatrix
	m_pEffect->GetViewInverseMatrix()->SetMatrix(inverseView);

	//Render triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}

glm::mat4 Mesh::GetWorldMatrix()
{
	return m_WorldMatrix;
}

const std::vector<uint32_t>& Mesh::GetIndexBuffer()
{
	return m_IndexBuffer;
}

const std::vector<VertexInput>& Mesh::GetVertexBuffer()
{
	return m_VertexBuffer;
}

const std::set<VertexInput*>& Mesh::GetVerticesToUpdate()
{
	return m_VerticesToUpdate;
}

void Mesh::SetVertexBuffer(ID3D11DeviceContext* pDeviceContext, const std::vector<VertexInput>& vertexBuffer)
{
	m_VertexBuffer = vertexBuffer;
	UpdateVertexBuffer(pDeviceContext);
}

void Mesh::SetWireframe(bool enabled)
{
	m_WireFrameEnabled = enabled;
}


void Mesh::UpdateMesh(ID3D11DeviceContext* pDeviceContext, float deltaTime)
{
	//Loop over all the vertices that have a pulse going through and update them
	//If the pulse zeros out, mark them to remove them from the list.
	std::vector<VertexInput*> verticesToRemove{};
	for (VertexInput* vertex : m_VerticesToUpdate)
	{
		if (vertex->pulseStrength > 0.f)
			vertex->pulseStrength -= deltaTime;
		else
		{
			vertex->pulseStrength = 0.f;
			verticesToRemove.push_back(vertex);
		}
	}

	//Remove the vertices with no pulse from the list.
	for (VertexInput* vertex : verticesToRemove)
	{
		m_VerticesToUpdate.erase(vertex);
	}

	//Clear the vector to be reused and update all the neighbour vertices.
	verticesToRemove.clear();
	for (VertexInput* vertex : m_NeighboursToUpdate)
	{
		vertex->timeBeforeActive -= deltaTime;
		if (vertex->timeBeforeActive <= 0.f)
		{
			verticesToRemove.push_back(vertex);
			PulseVertex(vertex, pDeviceContext, false);
		}
	}

	//Remove the neighbour vertices with from the list.
	for (VertexInput* vertex : verticesToRemove)
	{
		m_NeighboursToUpdate.erase(vertex);
	}

	UpdateVertexBuffer(pDeviceContext);
}

void Mesh::PulseVertex(uint32_t index, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer)
{
	if (!m_VertexBuffer.empty() && index >= 0 && index < m_VertexBuffer.size())
	{
		PulseVertex(&m_VertexBuffer[index], pDeviceContext, updateVertexBuffer);
	}
}

void Mesh::PulseVertex(VertexInput* vertex, ID3D11DeviceContext* pDeviceContext, bool updateVertexBuffer)
{
	if (vertex)
	{
		if (vertex->pulseStrength <= 0.3f)
		{
			vertex->pulseStrength = 1;
			m_VerticesToUpdate.insert(vertex);
			PulseNeighbours(*vertex);
		}
	}

	if (updateVertexBuffer)
		UpdateVertexBuffer(pDeviceContext);
}

void Mesh::PulseMesh(ID3D11DeviceContext* pDeviceContext)
{
	for (int i{}; i < m_VertexBuffer.size(); i++)
	{
		PulseVertex(i, pDeviceContext, false);
	}
	UpdateVertexBuffer(pDeviceContext);
}

HRESULT Mesh::CreateDirectXResources(ID3D11Device* pDevice, const std::vector<VertexInput>& vertices, const std::vector<uint32_t>& indices)
{
	HRESULT result = S_OK;

	if (m_pIndexBuffer)
		m_pIndexBuffer->Release();

	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if (m_pVertexLayout)
		m_pVertexLayout->Release();

	//Create vertex layout
	static const uint32_t numElements{ 7 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "SECCOLOR";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 36;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 48;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[5].SemanticName = "TEXCOORD";
	vertexDesc[5].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[5].AlignedByteOffset = 60;
	vertexDesc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[6].SemanticName = "POWER";
	vertexDesc[6].Format = DXGI_FORMAT_R32_FLOAT;
	vertexDesc[6].AlignedByteOffset = 68;
	vertexDesc[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;



	//Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout
	);
	
	//Create vertex buffers
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexInput) * (uint32_t)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = vertices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return result;

	//Create index buffer
	m_AmountIndices = (uint32_t)indices.size();
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return result;

	//Create wireframe rasterizer state
	D3D11_RASTERIZER_DESC rswf{};
	rswf.FillMode = D3D11_FILL_WIREFRAME;
	rswf.CullMode = D3D11_CULL_NONE;
	rswf.DepthClipEnable = true;
	result = pDevice->CreateRasterizerState(&rswf, &m_pRasterizerStateWireframe);
	if (FAILED(result))
		return result;

	//Create solid rasterizer state
	D3D11_RASTERIZER_DESC rss{};
	rss.FillMode = D3D11_FILL_SOLID;
	rss.CullMode = D3D11_CULL_BACK;
	rss.FrontCounterClockwise = true;
	result = pDevice->CreateRasterizerState(&rss, &m_pRasterizerStateSolid);
	if (FAILED(result))
		return result;

	return result;
}

void Mesh::LoadMeshFromOBJ(const std::string& pathName)
{
	std::cout << "[Started Loading Mesh]\n";
	std::cout << "Started Reading Mesh File\n";
	objl::Loader loader;

	bool loadout = loader.LoadFile(pathName);
	if (loadout)
	{
		if (loader.LoadedMeshes.size() > 0)
		{
			glm::fvec3 color1 = {  50 / 255.f, 151 / 255.f, 142 / 255.f };
			glm::fvec3 color2 = { 225 / 255.f,  73 / 255.f,  80 / 255.f } ;

			for (const objl::Vertex& vertex : loader.LoadedVertices)
			{
				m_VertexBuffer.push_back({
					{ vertex.Position.X, vertex.Position.Y, vertex.Position.Z },
					color1,
					color2,
					{ vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z },
					{ vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y }
				});
			}

			for (unsigned int index : loader.LoadedIndices)
			{
				m_IndexBuffer.push_back(index);
			}
			std::cout << "Finished Reading Mesh File\n";

			OptimizeIndexBuffer();

			for (int i = 0; i < m_IndexBuffer.size(); i++)
			{
				if (i % 3 == 2)
				{
					//Change handedness
					std::swap(m_IndexBuffer[i - 1], m_IndexBuffer[i]);
				}

				if (i % 3 == 0)
				{
					//Calculate Tangent
					int index0 = m_IndexBuffer[i];
					int index1 = m_IndexBuffer[i + 1];
					int index2 = m_IndexBuffer[i + 2];
					VertexInput& vertex0 = m_VertexBuffer[index0];
					VertexInput& vertex1 = m_VertexBuffer[index1];
					VertexInput& vertex2 = m_VertexBuffer[index2];

					const glm::fvec3 edge0 = (vertex1.position - vertex0.position);
					const glm::fvec3 edge1 = (vertex2.position - vertex0.position);

					const glm::fvec2 diffX = glm::fvec2(vertex1.uv.x - vertex0.uv.x, vertex2.uv.x - vertex0.uv.x);
					const glm::fvec2 diffY = glm::fvec2(vertex1.uv.y - vertex0.uv.y, vertex2.uv.y - vertex0.uv.y);
					const float r = 1.f / (diffX.x * diffY.y - diffX.y * diffY.x);

					glm::vec3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
					vertex0.tangent = tangent;
					vertex1.tangent = tangent;
					vertex2.tangent = tangent;
				}
			}
		}
	}
	std::cout << "Finished Loading Mesh\n";
}

void Mesh::OptimizeIndexBuffer()
{
	std::cout << "Started Optimizing Index Buffer\n";
	//Get rid of out of bounds indices
	std::vector<uint32_t>::iterator removeIt = std::remove_if(m_IndexBuffer.begin(), m_IndexBuffer.end(), [this](uint32_t index)
		{
			return index >= m_VertexBuffer.size();
		});

	if (removeIt != m_IndexBuffer.end())
		m_IndexBuffer.erase(removeIt);

	//Loop over all the indices and check if they're pointing to duplicate vertices
	std::set<uint32_t> seenIndices{};
	for (size_t i{}; i < m_IndexBuffer.size(); i++)
	{
		uint32_t index = m_IndexBuffer[i];
		if (index >= m_VertexBuffer.size())
			continue;

		std::set<uint32_t>::iterator itFind = std::find(seenIndices.begin(), seenIndices.end(), index);
		if (itFind != seenIndices.end())
		{
			int division = int(m_IndexBuffer.size()) / 10;
			int result = i % division;
			if (result == 1)
			{
				std::cout << "Optimizing Index Buffer: " << std::to_string(int(float(i) / m_IndexBuffer.size() * 100)) << "%\n";
			}
			continue;
		}

		VertexInput vertex = m_VertexBuffer[index];

		auto CompareIndexVertex = [this, &vertex](uint32_t index)
		{
			return vertex == m_VertexBuffer[index];
		};

		std::vector<uint32_t> duplicates{};
		std::vector<uint32_t>::iterator it = std::find_if(m_IndexBuffer.begin() + i, m_IndexBuffer.end(), CompareIndexVertex);

		while (it != m_IndexBuffer.end())
		{
			duplicates.push_back(*it);
			++it;
			it = std::find_if(it, m_IndexBuffer.end(), CompareIndexVertex);
		}

		//Replace indices that are pointing to duplicates with one index
		for (uint32_t duplicate : duplicates)
		{
			std::replace(m_IndexBuffer.begin() + i, m_IndexBuffer.end(), duplicate, index);
		}
		seenIndices.insert(index);

		int division = int(m_IndexBuffer.size()) / 10;
		int result = i % division;
		if (result == 1)
		{
			std::cout << "Optimizing Index Buffer: " << std::to_string(int(float(i) / m_IndexBuffer.size() * 100)) << "%\n";
		}
	}
	std::cout << "Finished Optimizing Index Buffer\n";
}

void Mesh::UpdateVertexBuffer(ID3D11DeviceContext* pDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, m_VertexBuffer.data(), m_VertexBuffer.size() * sizeof(VertexInput));
	pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void Mesh::PulseNeighbours(const VertexInput& vertex)
{
	for (uint32_t neighbourIndex : vertex.neighbourIndices)
	{
		VertexInput& neighbourVertex = m_VertexBuffer[neighbourIndex];
		float distance = glm::distance(vertex.position, neighbourVertex.position);
		neighbourVertex.timeBeforeActive = distance / neighbourVertex.PropogationSpeed;

		m_NeighboursToUpdate.insert(&neighbourVertex);
	}
}

void Mesh::CreateEffect(ID3D11Device* pDevice)
{
	m_pEffect = new BaseEffect(pDevice, L"Resources/Shader/PosCol.fx");
}
