#pragma once
#include "ClassicNoise.h"
#include "SimplexNoise.h"

struct FieldVertexType
{
	DirectX::SimpleMath::Vector3 position;
	float scalar;
};

class Field
{
public:
	Field();
	~Field();

	// Initialisation...
	bool Initialise(int cells);
	bool Initialise(Field* field);

	// Planar surface generation...
	void InitialiseHorizontalField(int octaves = 8, float amplitude = 0.05f);
	void IntegrateHorizontalThorn(DirectX::SimpleMath::Vector3 prick, DirectX::SimpleMath::Vector3 root, float radius, float isolevel);
	void IntegrateOrb(DirectX::SimpleMath::Vector3 centre, float radius, float isolevel);

	// Spherical surface generation...
	void InitialiseSphericalField(int octaves = 8, float amplitude = 0.2f);

	// Toroidal surface generation...
	void InitialiseToroidalField(float R, int octaves = 8, float amplitude = 0.2f);

	// Generate *with hex*
	void DeriveHexPrism(ID3D11Device*, float isolevel, bool lowerBound = true, bool upperBound = false);
	void DeriveCylindricalPrism(ID3D11Device*, float isolevel, bool lowerBound = true, bool upperBound = false);

	int m_cells;
	FieldVertexType* m_field;
};