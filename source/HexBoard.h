#pragma once
#include "MarchingTerrain.h"

class HexBoard
{
public:
	HexBoard();
	~HexBoard();

	bool Initialize(ID3D11Device*, int hexRadius, int cells);
	void Render(ID3D11DeviceContext*);

//private: // FIXME: Left off while still accessed in Game.cpp...
	int m_hexRadius; // total tiles: 1+3*m_hexRadius*(m_hexRadius+1)
	int* m_hexCoordinates;
	MarchingTerrain* m_hexTiles; // new MarchingTerrain[1+3*m_hexRadius*(m_hexRadius+1)]

	static const DirectX::SimpleMath::Vector3 m_origin, m_p, m_q;
};

