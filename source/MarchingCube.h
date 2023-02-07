#pragma once

using namespace DirectX;

class MarchingCube
{
private:
	// General vertex information, used by shaders...
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector3 tangent;
		DirectX::SimpleMath::Vector3 binormal;
	};

public:
	MarchingCube();
	~MarchingCube();

	bool Initialize(ID3D11Device*);
	void Render(ID3D11DeviceContext*);
	void Shutdown();

	// 
	void GenerateIsosurface(ID3D11Device*, float init_scalars[8], float init_isolevel);

	bool Update();

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void ShutdownBuffers();

	// Post-processing direction vectors
	void CalculateNormalTangentBinormal(VertexType vertex1, VertexType vertex2, VertexType vertex3, DirectX::SimpleMath::Vector3& normal, DirectX::SimpleMath::Vector3& tangent, DirectX::SimpleMath::Vector3& binormal);


private:
	float scalars[8]; // Indexing: (0,0,0), (1,0,0), (1,0,1), (0,0,1), (0,1,0), (1,1,0), (1,1,1), (0,1,1); check against http://paulbourke.net/geometry/polygonise/!
	float isolevel;

	int isosurfaceIndex;
	DirectX::SimpleMath::Vector3 isosurfacePositions[16];

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	// Lookup tables used in the construction of the isosurface.
	static const unsigned int m_edgeTable[256];
	static const unsigned int m_triTable[256][16];

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
};