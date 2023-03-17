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

	// STEP 2: Count *unique* vertices...
	// FIXME: Major refactor needed for this to work - but it seems necessary for 'balanced' normals!

	// Set the vertex count to the same as the index count.
	//m_vertexCount = m_indexCount;

	m_vertexCount = 3;
	m_indexCount = 3;

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

void LSystem::InitializeProductionRule(std::string A, std::vector<std::string> alpha)
{
	if (!productionRules.count(A))
		productionRules.insert({ A, std::vector<std::vector<std::string>>{alpha} });
	else
		productionRules[A].push_back(alpha);
}

void LSystem::InitializeSentence(std::vector<std::string> startSentence, int iterations)
{
	sentence = startSentence;

	std::vector<std::string> iteratedSentence;
	for (int i = 0; i < iterations; i++)
	{
		iteratedSentence = std::vector<std::string>();
		for each (std::string A in sentence)
			for each (std::string alpha in GetProductionRule(A))
				iteratedSentence.push_back(alpha);

		sentence = iteratedSentence;
	}
}

std::vector<std::string> LSystem::GetProductionRule(std::string A)
{
	if (!productionRules.count(A))
		return std::vector<std::string>{A};

	// FIXME: Add stochastic components here... 
	return productionRules[A][0];
}

std::string LSystem::GetSentence()
{
	std::string text = "";

	int count = 0;
	for each (std::string alpha in sentence)
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