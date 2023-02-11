#pragma once

#include "ClassicNoise.h"
#include "SimplexNoise.h"

using namespace DirectX;

class MarchingCubes
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

	struct FieldVertexType
	{
		DirectX::SimpleMath::Vector3 position;
		float scalar;
	};

public:
	MarchingCubes();
	~MarchingCubes();

	bool Initialize(ID3D11Device*, int cells);
	void Render(ID3D11DeviceContext*);
	void Shutdown();

	//
	void GenerateHorizontalField(DirectX::SimpleMath::Vector3 origin);
	void GenerateSphericalField(DirectX::SimpleMath::Vector3 origin);
	void GenerateSinusoidalSphericalField(DirectX::SimpleMath::Vector3 origin);
	void GenerateToroidalField(DirectX::SimpleMath::Vector3 origin);

	// 
	bool GenerateIsosurface(ID3D11Device*, float isolevel);
	DirectX::SimpleMath::Vector3 InterpolateIsosurface(FieldVertexType a, FieldVertexType b, float isolevel);

	bool Update();

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void ShutdownBuffers();

	// Post-processing direction vectors
	void CalculateNormalTangentBinormal(VertexType vertex1, VertexType vertex2, VertexType vertex3, DirectX::SimpleMath::Vector3& normal, DirectX::SimpleMath::Vector3& tangent, DirectX::SimpleMath::Vector3& binormal);


private:
	int m_cells;

	FieldVertexType* m_field;

	int* m_isosurfaceIndices;
	DirectX::SimpleMath::Vector3* m_isosurfacePositions;
	float m_isolevel;

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	// Lookup tables used in the construction of the isosurface.
	static const int m_edgeTable[256];
	static const int m_triTable[256][16];

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
};