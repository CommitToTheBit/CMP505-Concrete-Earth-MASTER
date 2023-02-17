#pragma once
#include "MarchingCubes.h"

class MarchingTerrain : public MarchingCubes
{
public:

	// Planar surface generation...
	void InitialiseHorizontalField(int octaves = 8, float amplitude = 0.1f);
	void AttachHorizontalThorn(DirectX::SimpleMath::Vector3 prick, DirectX::SimpleMath::Vector3 root, float radius, float isolevel);

	// Spherical surface generation...
	void InitialiseSphericalField(int octaves = 8, float amplitude = 0.2f);

	// Toroidal surface generation...
	void InitialiseToroidalField(float R, int octaves = 8, float amplitude = 0.2f);

	// Generate *with hex*
	void GenerateHexPrism(ID3D11Device*, float isolevel, bool lowerBound = true, bool upperBound = false);
	void GenerateCylindricalPrism(ID3D11Device*, float isolevel, bool lowerBound = true, bool upperBound = false);

//private:
};

