#include "pch.h"
#include "LSystem.h"

LSystem::LSystem()
{

}


LSystem::~LSystem()
{

}

bool LSystem::Initialize(ID3D11Device* device)
{
	InitializeBuffers(device);

	return true;
}

bool LSystem::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	DirectX::SimpleMath::Vector3 normal, tangent, binormal;
	float weight = 0.0f;

	// STEP 0: Generate graph...
	/*int graphVertices = 1;
	int graphEdges = 0;

	int graphVertex = 0;
	std::vector<int> pushedVertices;

	for each (std::string alpha in m_sentence)
	{
		if (alpha != "[" && alpha != "]" && alpha != "+" && alpha != "-")
		{
			graphEdges++;
			graphVertices++;
		}
		else if (alpha == "]" && graphVertices > 1) // NB: Popping means we start at an 'old' vertex..
		{
			graphVertices--;
		}
	}

	m_vertexCount = 0*graphVertices+4*graphEdges; // NB: Prototype starts with disconnected lines only...
	m_indexCount = 0*graphVertices+4*graphEdges;*/

	m_vertexCount = std::max((int)(4*m_treeVertices.size()),3);
	m_indexCount = std::max((int)(12*m_treeVertices.size()),3);

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	int index = 0;

	TreeVertexType treeVertex, parentVertex;
	DirectX::SimpleMath::Vector3 orthogonal;
	for (int i = 1; i < m_treeVertices.size(); i++)
	{
		treeVertex = m_treeVertices[i];
		parentVertex = m_treeVertices[treeVertex.parent];

		DirectX::SimpleMath::Vector3::Transform(treeVertex.position-parentVertex.position,DirectX::SimpleMath::Matrix::CreateRotationZ(DirectX::XM_PIDIV2),orthogonal);
		orthogonal.Normalize();

		// FIXME: Work out perpendicular component here...


		vertices[4*i].position = parentVertex.position+parentVertex.radius*orthogonal;
		vertices[4*i+1].position = parentVertex.position-parentVertex.radius*orthogonal;
		vertices[4*i+2].position = treeVertex.position-treeVertex.radius*orthogonal;
		vertices[4*i+3].position = treeVertex.position+treeVertex.radius*orthogonal;

		indices[12*i] = 4*i;
		indices[12*i+1] = 4*i+1;
		indices[12*i+2] = 4*i+2;

		indices[12*i+3] = 4*i;
		indices[12*i+4] = 4*i+2;
		indices[12*i+5] = 4*i+3;

		indices[12*i+6] = 4*i;
		indices[12*i+7] = 4*i+2;
		indices[12*i+8] = 4*i+1;

		indices[12*i+9] = 4*i;
		indices[12*i+10] = 4*i+3;
		indices[12*i+11] = 4*i+2;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	return true;
}

void LSystem::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);

	return;
}

void LSystem::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void LSystem::Shutdown()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void LSystem::ShutdownBuffers()
{
	return;
}

void LSystem::Update(ID3D11Device* device, float time)
{
	// STEP 1: Clear buffers...
	Shutdown();

	// STEP 2: Update tree topology using new parameters...
	UpdateTree(time);

	// STEP 3: Re-initialise buffers...
	Initialize(device);
}

void LSystem::UpdateTree(float time)
{
	float length = 0.5f*pow(2.0f, -6.0f); // NB: pow(2.0f,iterations)

	m_treeVertices = std::vector<TreeVertexType>();

	m_treeVertices.push_back(TreeVertexType());

	m_treeVertices[0].parent = 0;

	m_treeVertices[0].transform = DirectX::SimpleMath::Matrix::CreateRotationZ(DirectX::XM_PIDIV2)*DirectX::SimpleMath::Matrix::CreateTranslation(0.5f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_treeVertices[0].transform, m_treeVertices[0].position);

	m_treeVertices[0].radius = pow(2.0f, -4.0f);

	int parentIndex = 0;
	std::vector<int> parentIndices = std::vector<int>();

	int vertexDepth = 0;
	std::vector<int> vertexDepths = std::vector<int>();

	DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;

	for each (std::string alpha in m_sentence)
	{
		if (alpha == "[")
		{
			parentIndices.push_back(parentIndex);
			vertexDepths.push_back(vertexDepth);
		}
		else if (alpha == "]" && parentIndices.size() > 0)
		{
			parentIndex = parentIndices[parentIndices.size()-1];
			vertexDepth = vertexDepths[vertexDepths.size()-1];

			parentIndices.pop_back();
			vertexDepths.pop_back();
		}
		else if (alpha == "+")
		{
			vertexDepth++;
			localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ((45.0f)*DirectX::XM_PI/180.0f)*localTransform;
		}
		else if (alpha == "-")
		{
			vertexDepth++;
			localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ((-45.0f)*DirectX::XM_PI/180.0f)*localTransform;
		}
		else
		{
			// DEBUG:
			localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(1.0f*(1+vertexDepth)*sin(time/10.0f)*DirectX::XM_PI/180.0f)*localTransform;

			localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(length,0.0f,0.0f)*localTransform;

			m_treeVertices.push_back(TreeVertexType());

			m_treeVertices[m_treeVertices.size()-1].parent = parentIndex;
			m_treeVertices[m_treeVertices.size()-1].depth = vertexDepth;

			m_treeVertices[m_treeVertices.size()-1].transform = localTransform*m_treeVertices[parentIndex].transform;
			DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_treeVertices[m_treeVertices.size()-1].transform, m_treeVertices[m_treeVertices.size()-1].position);

			m_treeVertices[m_treeVertices.size()-1].radius = pow(2.0f,-(4.0f+m_treeVertices[m_treeVertices.size()-1].depth));

			parentIndex = m_treeVertices.size()-1;
			localTransform = DirectX::SimpleMath::Matrix::Identity;
		}
	}
}

void LSystem::InitializeProductionRule(std::string A, std::vector<std::string> alpha)
{
	if (!m_productionRules.count(A))
		m_productionRules.insert({ A, std::vector<std::vector<std::string>>{alpha} });
	else
		m_productionRules[A].push_back(alpha);
}

void LSystem::InitializeSentence(std::vector<std::string> startSentence, int iterations)
{
	m_sentence = startSentence;

	std::vector<std::string> iteratedSentence;
	for (int i = 0; i < iterations; i++)
	{
		iteratedSentence = std::vector<std::string>();
		for each (std::string A in m_sentence)
			for each (std::string alpha in GetProductionRule(A))
				iteratedSentence.push_back(alpha);

		m_sentence = iteratedSentence;
	}
}

std::vector<std::string> LSystem::GetProductionRule(std::string A)
{
	if (!m_productionRules.count(A))
		return std::vector<std::string>{A};

	// FIXME: Add stochastic components here... 
	return m_productionRules[A][0];
}

std::string LSystem::GetSentence()
{
	std::string text = "";

	int count = 0;
	for each (std::string alpha in m_sentence)
	{
		for each (char letter in alpha)
		{
			text += letter;
			if (++count >= 140)
			{
				text += "\n  ";
				count = 0;
			}
		}
	}

	return text;
}