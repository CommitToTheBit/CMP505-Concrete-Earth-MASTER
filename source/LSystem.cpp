#include "pch.h"
#include "LSystem.h"

LSystem::LSystem()
{

}


LSystem::~LSystem()
{

}

bool LSystem::Initialize(ID3D11Device* device, std::vector<LModuleType> axiom, int iterations, DirectX::SimpleMath::Vector2 anchoring, float seed)
{
	// STEP 1: Initialize sentence, using grammar...
	InitializeSentence(axiom, iterations);

	// STEP 2: Initialize tree, to scale...
	InitializeTree(anchoring, seed);

	// STEP 3:
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

	m_vertexCount = std::max((int)(4*m_treeVertices.size())/*+17*/, 3); // NB: Using 16-gon to approximate a circle!
	m_indexCount = std::max((int)(24*m_treeVertices.size())/*+6*16*/, 3);

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
	for (int i = 0; i < m_treeVertices.size(); i++)
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

		for (int j = 0; j < 2; j++) // NB: j loops over 'both sides' of culling...
		{
			indices[24*i+12*j] = 4*treeVertex.parent+2;
			indices[24*i+12*j+1+j] = 4*treeVertex.parent+3;
			indices[24*i+12*j+2-j] = 4*i;

			indices[24*i+12*j+3] = 4*treeVertex.parent+2;
			indices[24*i+12*j+4+j] = 4*i;
			indices[24*i+12*j+5-j] = 4*i+1;

			indices[24*i+12*j+6] = 4*i;
			indices[24*i+12*j+7+j] = 4*i+1;
			indices[24*i+12*j+8-j] = 4*i+2;

			indices[24*i+12*j+9] = 4*i;
			indices[24*i+12*j+10+j] = 4*i+2;
			indices[24*i+12*j+11-j] = 4*i+3;
		}
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
	ShutdownBuffers();

	return;
}

void LSystem::ShutdownBuffers()
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

void LSystem::Update(ID3D11Device* device, float deltaTime, float intensity)
{
	// STEP 1: Clear buffers...
	ShutdownBuffers();

	// STEP 2: Update tree topology using new parameters...
	UpdateTree(deltaTime, intensity);

	// STEP 3: Re-initialise buffers...
	InitializeBuffers(device);
}

void LSystem::AddProductionRule(std::string letter, ProductionRuleType productionRule)
{
	if (!m_productionRules.count(letter))
		m_productionRules.insert({ letter, std::vector<ProductionRuleType>{ productionRule } });
	else
		m_productionRules[letter].push_back(productionRule);
}

void LSystem::InitializeSentence(std::vector<LModuleType> axiom, int iterations)
{
	m_sentence = axiom;

	std::vector<LModuleType> iteratedSentence;
	for (int i = 0; i < iterations; i++)
	{
		iteratedSentence = std::vector<LModuleType>();

		for each (LModuleType LModule in m_sentence)
		{
			LModuleType productionModule = LModule;
			for each (std::function<LModuleType(LModuleType)> production in GetProductionRule(LModule.letter).productions)
			{
				iteratedSentence.push_back(production(LModule));
			}
		}

		m_sentence = iteratedSentence;
	}
}

void LSystem::InitializeTree(DirectX::SimpleMath::Vector2 anchoring, float seed) // NB: Add alignment options?
{
	m_seed = seed;
	m_scale = 1.0f;
	m_time = 0.0f;
	m_intensity = 0.0f;

	m_seedVertices = std::vector<SeedVertexType>();

	m_seedVertices.push_back(SeedVertexType());
	m_seedVertices[0].parent = 0;
	m_seedVertices[0].transform = DirectX::SimpleMath::Matrix::Identity;// DirectX::SimpleMath::Matrix::CreateRotationZ(m_rotation); // FIXME: This value can be exposed!
	m_seedVertices[0].position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

	int parentIndex = 0;
	std::vector<int> parentIndices = std::vector<int>();

	DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;

	// STEP 1: Create branching structure...
	int childIndex = 1;
	for each (LModuleType LModule in m_sentence)
	{
		if (LModule.letter == "[")
		{
			parentIndices.push_back(parentIndex);
		}
		else if (LModule.letter == "]" && parentIndices.size() > 0)
		{
			parentIndex = parentIndices[parentIndices.size()-1];
			parentIndices.pop_back();
		}
		else
		{
			localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(LModule.rotation) * localTransform;

			if (LModule.length == 0.0f)
				continue;

			localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(m_scale*LModule.length, 0.0f, 0.0f) * localTransform;

			m_seedVertices.push_back(SeedVertexType());

			m_seedVertices[childIndex].parent = parentIndex;
			m_seedVertices[childIndex].transform = localTransform * m_seedVertices[parentIndex].transform;
			DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_seedVertices[childIndex].transform, m_seedVertices[childIndex].position);

			childIndex = m_seedVertices.size();
			parentIndex = m_seedVertices.size()-1;
			localTransform = DirectX::SimpleMath::Matrix::Identity;
		}
	}

	// STEP 2: Calculate bounds...
	DirectX::SimpleMath::Vector2 minima = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	DirectX::SimpleMath::Vector2 maxima = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	for (int i = 0; i < m_seedVertices.size(); i++)
	{
		if (m_seedVertices[i].position.x < minima.x)
			minima.x = m_seedVertices[i].position.x;
		else if (m_seedVertices[i].position.x > maxima.x)
			maxima.x = m_seedVertices[i].position.x;

		if (m_seedVertices[i].position.y < minima.y)
			minima.y = m_seedVertices[i].position.y;
		else if (m_seedVertices[i].position.y > maxima.y)
			maxima.y = m_seedVertices[i].position.y;
	}

	DirectX::SimpleMath::Vector2 delta = maxima-minima;
	float maxDelta = std::max(delta.x,delta.y);

	// NB: In the case of a point, scaling won't matter
	if (maxDelta > 0.0f)
		m_scale /= maxDelta;

	// STEP 3: Rescale... 
	float xBorder = anchoring.x*(1.0f-delta.x/maxDelta);
	float yBorder = anchoring.y*(1.0f-delta.y/maxDelta);

	for (int i = 0; i < m_seedVertices.size(); i++)
	{
		m_seedVertices[i].position.x = xBorder+(m_seedVertices[i].position.x-minima.x)/maxDelta;
		m_seedVertices[i].position.y = yBorder+(m_seedVertices[i].position.y-minima.y)/maxDelta;

		// FIXME: Simplex noise... still not working?
		m_seedVertices[i].simplex = 0.0f;
	}

	// STEP 4: Instantaneously 'update' m_treeVertices...
	UpdateTree(0.0f, 0.0f);
}

void LSystem::UpdateTree(float deltaTime, float deltaIntensity)
{
	std::srand(m_seed);

	m_time += deltaTime;

	m_intensity += deltaIntensity;
	m_intensity = std::max(0.0f, std::min(m_intensity, 1.0f));

	m_treeVertices = std::vector<TreeVertexType>();

	m_treeVertices.push_back(TreeVertexType());
	m_treeVertices[0].parent = 0;
	m_treeVertices[0].transform = DirectX::SimpleMath::Matrix::CreateTranslation(m_seedVertices[0].position); // NB: Assumes we've initialised m_seedVertices!
	DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_treeVertices[0].transform, m_treeVertices[0].position);

	int parentIndex = 0;
	std::vector<int> parentIndices = std::vector<int>();

	DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;

	// STEP 1: Create branching structure...
	int childIndex = 1;
	float randomness;
	for each (LModuleType LModule in m_sentence)
	{
		if (LModule.letter == "[")
		{
			parentIndices.push_back(parentIndex);
		}
		else if (LModule.letter == "]" && parentIndices.size() > 0)
		{
			parentIndex = parentIndices[parentIndices.size()-1];
			parentIndices.pop_back();
		}
		else
		{
			randomness = (LModule.period > 0.0f) ? GetRNGRange()*cos(XM_2PI*(m_time/(LModule.period+GetRNGRange(0.0f, std::max(LModule.aperiodicity, 0.0f)))+(LModule.synchronisation+GetRNGRange(0.0f, LModule.asynchronicity)))) : 0.0f;
			localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(LModule.rotation+randomness*LModule.randomRotation) * localTransform;

			if (LModule.length == 0.0f)
				continue;

			randomness = (LModule.period > 0.0f) ? GetRNGRange()*cos(XM_2PI*(m_time/(LModule.period+GetRNGRange(0.0f, std::max(LModule.aperiodicity, 0.0f)))+(LModule.synchronisation+GetRNGRange(0.0f, LModule.asynchronicity)))) : 0.0f;
			localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(m_scale*(LModule.length+randomness*LModule.randomLength), 0.0f, 0.0f) * localTransform;

			m_treeVertices.push_back(TreeVertexType());

			m_treeVertices[childIndex].parent = parentIndex;
			m_treeVertices[childIndex].degree = 1;
			m_treeVertices[childIndex].radius = m_scale*LModule.width;
			m_treeVertices[childIndex].transform = localTransform * m_treeVertices[parentIndex].transform;
			DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_treeVertices[childIndex].transform, m_treeVertices[childIndex].position);

			m_treeVertices[parentIndex].radius = std::max(m_scale*LModule.width, m_treeVertices[parentIndex].radius);
			m_treeVertices[parentIndex].degree++;

			childIndex = m_treeVertices.size();
			parentIndex = m_treeVertices.size()-1;
			localTransform = DirectX::SimpleMath::Matrix::Identity;
		}
	}
}

LSystem::ProductionRuleType LSystem::GetProductionRule(std::string letter)
{
	if (!m_productionRules.count(letter))
	{
		ProductionRuleType identity;
		identity.productions.push_back([](LSystem::LModuleType LModule) { return LModule; });
		identity.weight = 1.0f;
		return identity;
	}

	// FIXME: Add stochastic components here... 
	return m_productionRules[letter][0];
}

float LSystem::GetRNGRange(float a, float b)
{
	// FIXME: Rework this around simplex noise in the future?
	return a+(b-a)*std::rand()/RAND_MAX;
}

std::string LSystem::GetSentence()
{
	std::string text = "";

	int count = 0;
	for each (LModuleType LModule in m_sentence)
	{
		for each (char character in LModule.letter)
		{
			text += character;
			if (++count >= 140)
			{
				text += "\n  ";
				count = 0;
			}
		}
	}

	return text;
}

float* LSystem::GetIntensity()
{
	return &m_intensity;
}