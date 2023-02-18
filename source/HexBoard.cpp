#include "pch.h"
#include "HexBoard.h"

const DirectX::SimpleMath::Vector3 HexBoard::m_origin = DirectX::SimpleMath::Vector3(-0.5f, -0.5f, -0.5f);
const DirectX::SimpleMath::Vector3 HexBoard::m_p = DirectX::SimpleMath::Vector3(0.5f*(1.0f+cos(XM_PI/3.0f)), 0.0f, -0.5f*cos(XM_PI/6.0f));
const DirectX::SimpleMath::Vector3 HexBoard::m_q = DirectX::SimpleMath::Vector3(-0.5f*(1.0f+cos(XM_PI/3.0f)), 0.0f, -0.5f*cos(XM_PI/6.0f));

HexBoard::HexBoard()
{

}


HexBoard::~HexBoard()
{

}

bool HexBoard::Initialize(ID3D11Device* device, int hexRadius, int cells)
{
	m_hexRadius = hexRadius;

	m_hexCoordinates = new int[(2*m_hexRadius+1)*(2*m_hexRadius+1)];
	m_hexTiles = new MarchingTerrain[1+3*m_hexRadius*(m_hexRadius+1)];

	std::srand(0);
	int index = 0;
	for (int j = -m_hexRadius; j <= m_hexRadius; j++)
	{
		for (int i = -m_hexRadius; i <= m_hexRadius; i++)
		{
			if (abs(i-j) > m_hexRadius)
				continue;

			float h = (0.15f*std::rand())/RAND_MAX;

			m_hexTiles[index].Initialize(device, cells);
			m_hexTiles[index].InitialiseHorizontalField();
			m_hexTiles[index].IntegrateHorizontalThorn(DirectX::SimpleMath::Vector3(0.5f+0.25f*cos(2.0f*XM_PI/3.0f), 0.5f+h, 0.5f-0.25f*sin(2.0f*XM_PI/3.0f)), DirectX::SimpleMath::Vector3(0.5f, 0.0f, 0.5f), XM_PIDIV2/8.0f, 0.15f+h);
			m_hexTiles[index].IntegrateHorizontalThorn(DirectX::SimpleMath::Vector3(0.75f+0.3f*cos(-2.0f*XM_PI/3.0f), 0.55f+h, 0.6f-0.3f*sin(-2.0f*XM_PI/3.0f)), DirectX::SimpleMath::Vector3(0.75f, 0.0f, 0.6f), XM_PIDIV2/8.0f, 0.15f+h);
			m_hexTiles[index].IntegrateOrb(DirectX::SimpleMath::Vector3(0.5f, 0.45f+h, 0.5f), 0.125f, 0.15f+h);
			m_hexTiles[index].DeriveHexPrism(device, 0.15f+h);

			m_hexCoordinates[(2*m_hexRadius+1)*(j+m_hexRadius)+i+m_hexRadius] = index++;
		}
	}

	return true;
}

void HexBoard::Render(ID3D11DeviceContext* deviceContext)
{
	for (int i = 0; i < 1+3*m_hexRadius*(m_hexRadius+1); i++)
		m_hexTiles[i].Render(deviceContext);

	return;
}