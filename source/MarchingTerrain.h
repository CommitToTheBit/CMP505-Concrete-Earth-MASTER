#pragma once
#include "MarchingCubes.h"

class MarchingTerrain : public MarchingCubes
{
public:

	// *Basic* surface...
	void GenerateHorizontalField(DirectX::SimpleMath::Vector3 origin);
	void GenerateSphericalField(DirectX::SimpleMath::Vector3 origin);
	void GenerateSinusoidalSphericalField(DirectX::SimpleMath::Vector3 origin);
	void GenerateToroidalField(DirectX::SimpleMath::Vector3 origin);

	// Add to field...

	// Generate *with hex*
	void GenerateHex(ID3D11Device*, float isolevel);

//private:
};

