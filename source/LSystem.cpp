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
	int graphVertices = 1;
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
	m_indexCount = 0*graphVertices+4*graphEdges;

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

	vertices[0].position = DirectX::SimpleMath::Vector3(0.5f, 0.0f, 0.0f);
	vertices[1].position = DirectX::SimpleMath::Vector3(0.4f, 0.5f, 0.0f);
	vertices[2].position = DirectX::SimpleMath::Vector3(0.6f, 0.5f, 0.0f);

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;

	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].normal.Normalize();
		vertices[i].tangent.Normalize();
		vertices[i].binormal.Normalize();
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

void LSystem::Render(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float time)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(device, deviceContext, time);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);

	return;
}

void LSystem::RenderBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float time)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	DirectX::SimpleMath::Vector3 normal, tangent, binormal;

	unsigned int stride;
	unsigned int offset;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return;
	}

	// Initialize the index to the vertex buffer.
	int index = 0;

	vertices[0].position = DirectX::SimpleMath::Vector3(0.5f+0.5f*sin(2.0f*time), 0.0f, 0.0f);
	vertices[1].position = DirectX::SimpleMath::Vector3(0.4f, 0.5f, 0.0f);
	vertices[2].position = DirectX::SimpleMath::Vector3(0.6f, 0.5f, 0.0f);

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;

	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].normal.Normalize();
		vertices[i].tangent.Normalize();
		vertices[i].binormal.Normalize();
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
		return;
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
		return;
	}

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

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