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
	m_hexDiameter = (2*hexRadius+1)* (2*hexRadius+1);
	m_hexes = 1+3*hexRadius*(hexRadius+1);

	m_hexCoordinates = new int[m_hexDiameter];

	m_hexIsolevels = new float[m_hexes];
	m_hexModels = new MarchingCubes[m_hexes];

	Field hexField;

	m_horizontalField.Initialise(cells);
	m_horizontalField.InitialiseHorizontalField(6);

	std::srand(0);
	int index = 0;
	for (int j = -m_hexRadius; j <= m_hexRadius; j++)
	{
		for (int i = -m_hexRadius; i <= m_hexRadius; i++)
		{
			if (abs(i-j) > m_hexRadius)
				continue;

			m_hexIsolevels[index] = 0.15f+(0.15f*std::rand())/RAND_MAX;

			hexField.Initialise(&m_horizontalField);
			hexField.DeriveHexPrism(device, m_hexIsolevels[index]);

			m_hexModels[index].Initialize(device, cells, hexField.m_field, m_hexIsolevels[index]);

			m_hexCoordinates[(2*m_hexRadius+1)*(j+m_hexRadius)+i+m_hexRadius] = index++;
		}
	}

	return true;
}

void HexBoard::Render(ID3D11DeviceContext* deviceContext)
{
	for (int i = 0; i < 1+3*m_hexRadius*(m_hexRadius+1); i++)
		m_hexModels[i].Render(deviceContext);

	return;
}

void HexBoard::AddThorn(ID3D11Device* device, int hex)
{
	hex = (hex%m_hexes+m_hexes)%m_hexes;

	Field hexField;
	hexField.Initialise(&m_horizontalField);
	hexField.IntegrateHorizontalThorn(DirectX::SimpleMath::Vector3(0.5f+0.25f*cos(2.0f*XM_PI/3.0f), 0.35f+m_hexIsolevels[hex], 0.5f-0.25f*sin(2.0f*XM_PI/3.0f)), DirectX::SimpleMath::Vector3(0.5f, 0.0f, 0.5f), XM_PIDIV2/8.0f, m_hexIsolevels[hex]);
	hexField.IntegrateHorizontalThorn(DirectX::SimpleMath::Vector3(0.75f+0.3f*cos(-2.0f*XM_PI/3.0f), 0.4f+m_hexIsolevels[hex], 0.6f-0.3f*sin(-2.0f*XM_PI/3.0f)), DirectX::SimpleMath::Vector3(0.75f, 0.0f, 0.6f), XM_PIDIV2/8.0f, m_hexIsolevels[hex]);
	hexField.IntegrateOrb(DirectX::SimpleMath::Vector3(0.25f, 0.45f+m_hexIsolevels[hex], 0.5f), 0.125f, m_hexIsolevels[hex]);
	hexField.DeriveHexPrism(device, m_hexIsolevels[hex]);

	m_hexModels[hex].Initialize(device, hexField.m_cells, hexField.m_field, m_hexIsolevels[hex]);
}