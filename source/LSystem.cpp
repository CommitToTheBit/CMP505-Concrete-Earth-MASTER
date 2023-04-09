#include "pch.h"
#include "LSystem.h"

LSystem::LSystem()
{
	m_time = 0.0f;
	m_intensity = 0.0f;

	m_length = 1.0f;
}


LSystem::~LSystem()
{

}

bool LSystem::Initialize(ID3D11Device* device)
{
	UpdateTree(0.0f, 0.0f);
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

		indices[24*i] = 4*treeVertex.parent+2;
		indices[24*i+1] = 4*treeVertex.parent+3;
		indices[24*i+2] = 4*i;

		indices[24*i+3] = 4*treeVertex.parent+2;
		indices[24*i+4] = 4*i;
		indices[24*i+5] = 4*i+1;

		indices[24*i+6] = 4*treeVertex.parent+2;
		indices[24*i+7] = 4*i;
		indices[24*i+8] = 4*treeVertex.parent+3;

		indices[24*i+9] = 4*treeVertex.parent+2;
		indices[24*i+10] = 4*i+1;
		indices[24*i+11] = 4*i;

		indices[24*i+12] = 4*i;
		indices[24*i+13] = 4*i+1;
		indices[24*i+14] = 4*i+2;

		indices[24*i+15] = 4*i;
		indices[24*i+16] = 4*i+2;
		indices[24*i+17] = 4*i+3;

		indices[24*i+18] = 4*i;
		indices[24*i+19] = 4*i+2;
		indices[24*i+20] = 4*i+1;

		indices[24*i+21] = 4*i;
		indices[24*i+22] = 4*i+3;
		indices[24*i+23] = 4*i+2;
	}

	/*if (m_treeVertices.size() > 0)
	{
		for (int i = 0; i < 16; i++)
			vertices[4*m_treeVertices.size()+i].position = m_treeVertices[0].position+DirectX::SimpleMath::Vector3(m_treeVertices[0].radius*cos(((float)i/16.0f)*DirectX::XM_2PI), m_treeVertices[0].radius*sin(((float)i/16.0f)*DirectX::XM_2PI), 0.0f);
		vertices[4*m_treeVertices.size()+16].position = m_treeVertices[0].position;

		for (int i = 0; i < 16; i++)
		{
			indices[24*m_treeVertices.size()+6*i] = 4*m_treeVertices.size()+16;
			indices[24*m_treeVertices.size()+6*i+1] = 4*m_treeVertices.size()+i;
			indices[24*m_treeVertices.size()+6*i+2] = 4*m_treeVertices.size()+(i+1)%16;

			indices[24*m_treeVertices.size()+6*i+3] = 4*m_treeVertices.size()+16;
			indices[24*m_treeVertices.size()+6*i+4] = 4*m_treeVertices.size()+(i+1)%16;
			indices[24*m_treeVertices.size()+6*i+5] = 4*m_treeVertices.size()+i;
		}
	}*/

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

void LSystem::Update(ID3D11Device* device, float deltaTime, float intensity)
{
	// STEP 1: Clear buffers...
	Shutdown();

	// STEP 2: Update tree topology using new parameters...
	UpdateTree(deltaTime, intensity);

	// STEP 3: Re-initialise buffers...
	Initialize(device);
}

void LSystem::UpdateTree(float deltaTime, float deltaIntensity)
{
	std::srand(m_seed);

	m_time += deltaTime;

	m_intensity += deltaIntensity;
	m_intensity = std::max(0.0f, std::min(m_intensity, 1.0f));

	float length = m_length;// 1.5*pow(2.0f, -5.0f); // NB: pow(2.0f,iterations)
	float radius = m_width;

	float radiusBase = 1.5f;

	m_treeVertices = std::vector<TreeVertexType>();

	m_treeVertices.push_back(TreeVertexType());
	m_treeVertices[0].parent = 0;
	m_treeVertices[0].depth = 0;
	m_treeVertices[0].degree = 0;
	m_treeVertices[0].transform = DirectX::SimpleMath::Matrix::Identity;// DirectX::SimpleMath::Matrix::CreateRotationZ(m_rotation)*DirectX::SimpleMath::Matrix::CreateTranslation(m_scaledVertices[0].position); // NB: Assumes we've initialised m_scaledVertices!
	DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_treeVertices[0].transform, m_treeVertices[0].position);

	int parentIndex = 0;
	std::vector<int> parentIndices = std::vector<int>();

	DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;

	// STEP 1: Create branching structure...
	int childIndex = 1;
	for each (ModuleType LModule in m_sentence)
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
			// DEBUG:
			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(0.25f*cos(time/5.0f)*DirectX::XM_PI/180.0f)*localTransform;
			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(pow(2.0f,vertexDepth)*(-1.0f+2.0f*std::rand()/RAND_MAX)*DirectX::XM_PI/180.0f)*localTransform;
			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(1.0f*pow(2.0f, vertexDepth)*simplex.FBMNoise(0.1f*m_time, 0.1f*m_treeVertices[parentIndex].position.x, m_treeVertices[parentIndex].position.y, 8)*DirectX::XM_PI/180.0f)*localTransform;
			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(2.0f*(-1.0f+2.0f*std::rand()/RAND_MAX)*cos(m_time/(3.0f+(m_treeVertices.size()-1)%5)+m_treeVertices.size()-1)*DirectX::XM_PI/180.0f)* localTransform;

			//localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(std::max(0.0f, (float)pow(2.0f, vertexDepth)*(m_intensity-1.0f)+1.0f)*(1.0f+0.25f*(-1.0f+2.0f*std::rand()/RAND_MAX))*length, 0.0f, 0.0f)*localTransform;
			localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(m_length*LModule.length*cos(LModule.rotation), m_length*LModule.length*sin(LModule.rotation), 0.0f) * localTransform;

			m_treeVertices.push_back(TreeVertexType());

			m_treeVertices[childIndex].parent = parentIndex;
			m_treeVertices[childIndex].degree = 1;
			m_treeVertices[childIndex].radius = m_length*LModule.width;
			m_treeVertices[childIndex].transform = localTransform*m_treeVertices[parentIndex].transform;
			m_treeVertices[childIndex].position = m_treeVertices[parentIndex].position+m_length*LModule.length*DirectX::SimpleMath::Vector3(cos(LModule.rotation), sin(LModule.rotation), 0.0f);
			//DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_treeVertices[childIndex].transform, m_treeVertices[childIndex].position);

			m_treeVertices[parentIndex].degree++;

			childIndex = m_scaledVertices.size();
			parentIndex = m_treeVertices.size()-1;
			localTransform = DirectX::SimpleMath::Matrix::Identity;
		}
	}

	/*for each (std::string alpha in m_sentence)
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
		else if (alpha == "^")
		{
			vertexDepth++;

			depthReached = std::max(vertexDepth, depthReached);
		}
		else if (m_rotationRules.count(alpha))
		{
			//vertexDepth++;
			localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(m_rotationRules[alpha]+(-1.0f+2.0f*std::rand()/RAND_MAX)*m_rotationRandomness[alpha])*localTransform;

			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ((120.0f)*DirectX::XM_PI/180.0f)*localTransform;
		}
		else
		{
			// DEBUG:
			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(0.25f*cos(time/5.0f)*DirectX::XM_PI/180.0f)*localTransform;
			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(pow(2.0f,vertexDepth)*(-1.0f+2.0f*std::rand()/RAND_MAX)*DirectX::XM_PI/180.0f)*localTransform;
			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(1.0f*pow(2.0f, vertexDepth)*simplex.FBMNoise(0.1f*m_time, 0.1f*m_treeVertices[parentIndex].position.x, m_treeVertices[parentIndex].position.y, 8)*DirectX::XM_PI/180.0f)*localTransform;
			//localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(2.0f*(-1.0f+2.0f*std::rand()/RAND_MAX)*cos(m_time/(3.0f+(m_treeVertices.size()-1)%5)+m_treeVertices.size()-1)*DirectX::XM_PI/180.0f)* localTransform;

			//localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(std::max(0.0f, (float)pow(2.0f, vertexDepth)*(m_intensity-1.0f)+1.0f)*(1.0f+0.25f*(-1.0f+2.0f*std::rand()/RAND_MAX))*length, 0.0f, 0.0f)*localTransform;
			localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(std::max(0.0f, (float)pow(2.0f, vertexDepth)*(m_intensity-1.0f)+1.0f)*length, 0.0f, 0.0f)*localTransform;

			m_treeVertices.push_back(TreeVertexType());

			m_treeVertices[m_treeVertices.size()-1].parent = parentIndex;
			m_treeVertices[m_treeVertices.size()-1].depth = vertexDepth;
			m_treeVertices[m_treeVertices.size()-1].degree = 1;
			m_treeVertices[m_treeVertices.size()-1].childDepth = maxDepth;
			m_treeVertices[m_treeVertices.size()-1].transform = localTransform*m_treeVertices[parentIndex].transform;
			DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_treeVertices[m_treeVertices.size()-1].transform, m_treeVertices[m_treeVertices.size()-1].position);

			m_treeVertices[parentIndex].degree++;
			m_treeVertices[parentIndex].childDepth = std::min(vertexDepth,m_treeVertices[parentIndex].childDepth);

			childIndex = m_scaledVertices.size();
			parentIndex = m_treeVertices.size()-1;
			localTransform = DirectX::SimpleMath::Matrix::Identity;
		}
	}

	// STEP 2: Assign widths based on depth...
	float branchLength, branch;
	float a, b;
	int vertexIndex;

	m_treeVertices[0].radius = m_intensity*radius; // NB: Set as an 'anchor'...
	for (int d = 0; d <= depthReached; d++)
	{
		for (int i = 1; i < m_treeVertices.size(); i++)
		{
			m_treeVertices[i].radius = m_intensity*radius; // DEBUG: Keeps a consistent width...
			continue; // DEBUG: Keeps a consistent width...

			if (m_treeVertices[i].depth == m_treeVertices[i].childDepth || m_treeVertices[i].depth != d)
				continue;

			a = m_intensity*radius*pow(radiusBase, -m_treeVertices[i].childDepth);

			branchLength = 0.0f;
			vertexIndex = i;
			while (m_treeVertices[vertexIndex].depth == m_treeVertices[i].depth && vertexIndex != m_treeVertices[vertexIndex].parent)
			{
				branchLength += (m_treeVertices[m_treeVertices[vertexIndex].parent].position-m_treeVertices[vertexIndex].position).Length();
				vertexIndex = m_treeVertices[vertexIndex].parent;
			}

			b = (m_treeVertices[vertexIndex].childDepth > m_treeVertices[vertexIndex].depth) ? m_intensity*radius*(float)pow(radiusBase, -m_treeVertices[i].depth) : std::min(m_intensity*radius*(float)pow(radiusBase, -m_treeVertices[i].depth), m_treeVertices[vertexIndex].radius);

			branch = 0.0f;
			vertexIndex = i;
			while (m_treeVertices[vertexIndex].depth == m_treeVertices[i].depth && vertexIndex != m_treeVertices[vertexIndex].parent)
			{
				branch += (m_treeVertices[m_treeVertices[vertexIndex].parent].position-m_treeVertices[vertexIndex].position).Length()/branchLength;
				m_treeVertices[vertexIndex].radius = std::max((1.0f-branch)*a+branch*b, m_treeVertices[vertexIndex].radius);
				vertexIndex = m_treeVertices[vertexIndex].parent;
			}
			//m_treeVertices[vertexIndex].radius = std::max(b, m_treeVertices[vertexIndex].radius);
		}
	}

	// If (degree 1 or all children have greater depth)
	// ...set radius to function of child depth...
	// ...count steps backwards up until parent has less depth...
	// ...and linearly interpolate width over that path!*/
}

void LSystem::InitializeProductionRule(std::string letter, ProductionRuleType productionRule)
{
	if (!m_productionRules.count(letter))
		m_productionRules.insert({ letter, std::vector<ProductionRuleType>{ productionRule } });
	else
		m_productionRules[letter].push_back(productionRule);
}

void LSystem::InitializeSentence(std::vector<ModuleType> axiom, int iterations)
{
	m_sentence = axiom;

	std::vector<ModuleType> iteratedSentence;
	for (int i = 0; i < iterations; i++)
	{
		iteratedSentence = std::vector<ModuleType>();

		ModuleType productionModule = m_sentence[0];

		for each (ModuleType LModule in m_sentence)
		{
			for each (std::function<ModuleType(ModuleType)> production in GetProductionRule(LModule.letter).productions)
			{
				productionModule = production(productionModule);
				iteratedSentence.push_back(productionModule);
			}
		}

		m_sentence = iteratedSentence;
	}
}

LSystem::ProductionRuleType LSystem::GetProductionRule(std::string letter)
{
	if (!m_productionRules.count(letter))
	{
		ProductionRuleType identity;
		identity.productions.push_back([](LSystem::ModuleType LModule) { return LModule; });
		identity.weight = 1.0f;
		return identity;
	}

	// FIXME: Add stochastic components here... 
	return m_productionRules[letter][0];
}

/*void LSystem::InitializeRotationRule(std::string A, float theta, float randomness)
{
	if (!m_rotationRules.count(A))
	{
		m_rotationRules.insert({ A, theta });
		m_rotationRandomness.insert({ A, randomness });
	}
}

void LSystem::InitializeScale(float seed, float width, float rotation) // NB: Add alignment options?
{
	m_seed = seed;
	m_length = 1.0f;
	m_width = width;
	m_rotation = rotation;

	m_scaledVertices = std::vector<ScaleVertexType>();

	m_scaledVertices.push_back(ScaleVertexType());
	m_scaledVertices[0].parent = 0;
	m_scaledVertices[0].transform = DirectX::SimpleMath::Matrix::CreateRotationZ(m_rotation); // FIXME: This value can be exposed!
	m_scaledVertices[0].position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

	int parentIndex = 0;
	std::vector<int> parentIndices = std::vector<int>();

	SimplexNoise simplex = SimplexNoise();
	DirectX::SimpleMath::Matrix localTransform = DirectX::SimpleMath::Matrix::Identity;

	// STEP 1: Create branching structure...
	int childIndex = 1;
	for each (std::string alpha in m_sentence)
	{
		if (alpha == "[")
		{
			parentIndices.push_back(parentIndex);
		}
		else if (alpha == "]" && parentIndices.size() > 0)
		{
			parentIndex = parentIndices[parentIndices.size()-1];

			parentIndices.pop_back();
		}
		else if (alpha == "^")
		{

		}
		else if (m_rotationRules.count(alpha))
		{
			localTransform = DirectX::SimpleMath::Matrix::CreateRotationZ(m_rotationRules[alpha])*localTransform;
		}
		else
		{
			localTransform = DirectX::SimpleMath::Matrix::CreateTranslation(m_length, 0.0f, 0.0f)*localTransform;

			m_scaledVertices.push_back(ScaleVertexType());
			m_scaledVertices[childIndex].parent = parentIndex;
			m_scaledVertices[childIndex].transform = localTransform*m_scaledVertices[parentIndex].transform;
			DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), m_scaledVertices[childIndex].transform, m_scaledVertices[childIndex].position);

			childIndex = m_scaledVertices.size();
			parentIndex = m_scaledVertices.size()-1;
			localTransform = DirectX::SimpleMath::Matrix::Identity;
		}
	}

	// STEP 2: Calculate bounds...
	float xMin = 0.0f;
	float yMin = 0.0f;
	float xMax = 0.0f; 
	float yMax = 0.0f;

	for (int i = 0; i < m_scaledVertices.size(); i++)
	{
		if (m_scaledVertices[i].position.x < xMin)
			xMin = m_scaledVertices[i].position.x;
		else if (m_scaledVertices[i].position.x > xMax)
			xMax = m_scaledVertices[i].position.x;

		if (m_scaledVertices[i].position.y < yMin)
			yMin = m_scaledVertices[i].position.y;
		else if (m_scaledVertices[i].position.y > yMax)
			yMax = m_scaledVertices[i].position.y;
	}

	float xDelta = xMax-xMin;
	float yDelta = yMax-yMin;
	float maxDelta = std::max(xDelta, yDelta);

	// NB: In the case of a point, scaling won't matter
	if (maxDelta == 0.0f)
		return;

	m_length /= maxDelta;

	// STEP 3: Rescale... 
	// NB: We can now ignore the transforms, as these won't be used again...
	float xAnchoring = 0.5f; // NB: Expose this...
	float yAnchoring = 0.5f; // NB: Expose this...

	float xBorder = xAnchoring*(1.0f-xDelta/maxDelta);
	float yBorder = yAnchoring*(1.0f-yDelta/maxDelta);

	for (int i = 0; i < m_scaledVertices.size(); i++)
	{
		m_scaledVertices[i].position.x = xBorder+(m_scaledVertices[i].position.x-xMin)/maxDelta;
		m_scaledVertices[i].position.y = yBorder+(m_scaledVertices[i].position.y-yMin)/maxDelta;

		// FIXME: Simplex noise... still not working?
		m_scaledVertices[i].simplex = simplex.FBMNoise(m_seed, m_scaledVertices[i].position.x, m_scaledVertices[i].position.y, 8);
	}
}*/

std::string LSystem::GetSentence()
{
	std::string text = "";

	int count = 0;
	for each (ModuleType LModule in m_sentence)
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

		for each (char character in std::to_string((int)(180.0f*LModule.rotation/XM_PI)))
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