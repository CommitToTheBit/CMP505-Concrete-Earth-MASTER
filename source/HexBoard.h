#pragma once
#include "MarchingCubes.h"

class HexBoard
{
public:
	HexBoard();
	~HexBoard();

	bool Initialize(ID3D11Device*, int hexRadius, int cells);
	void Render(ID3D11DeviceContext*);

	// DEBUG: Testing interactivity...
	void AddThorn(ID3D11Device*, int hex);

private:


public: // FIXME: Left off while still accessed in Game.cpp...
	int m_hexRadius, m_hexDiameter, m_hexes; // total tiles: 1+3*m_hexRadius*(m_hexRadius+1)
	int* m_hexCoordinates;
	int* m_hexPermutation;

	// Modelling info...
	Field m_horizontalField;

	float* m_hexIsolevels;
	MarchingCubes* m_hexModels; // new MarchingTerrain[1+3*m_hexRadius*(m_hexRadius+1)]

	static const DirectX::SimpleMath::Vector3 m_origin, m_p, m_q;
};

