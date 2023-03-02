#include "pch.h"
#include "HexBoard.h"

const DirectX::SimpleMath::Vector3 HexBoard::m_origin = DirectX::SimpleMath::Vector3(-0.5f, -0.5f, -0.5f);
const DirectX::SimpleMath::Vector3 HexBoard::m_p = DirectX::SimpleMath::Vector3(0.5f*(1.0f+cos(XM_PI/3.0f)), 0.0f, -0.5f*cos(XM_PI/6.0f)); // Points NE...
const DirectX::SimpleMath::Vector3 HexBoard::m_q = DirectX::SimpleMath::Vector3(-0.5f*(1.0f+cos(XM_PI/3.0f)), 0.0f, -0.5f*cos(XM_PI/6.0f)); // Points NW...

HexBoard::HexBoard()
{
	std::srand(0);
}


HexBoard::~HexBoard()
{

}

bool HexBoard::Initialize(ID3D11Device* device, int hexRadius, int cells)
{
	m_hexRadius = hexRadius;
	m_hexDiameter = (2*hexRadius+1);
	m_hexes = 1+3*hexRadius*(hexRadius+1);

	m_hexCoordinates = new int[m_hexDiameter*m_hexDiameter];
	m_hexPermutation = new int[m_hexes];

	m_hexIsolevels = new float[m_hexes];
	m_hexModels = new MarchingCubes[m_hexes];

	Field* hexField = new Field();

	m_horizontalField.Initialise(cells);
	m_horizontalField.InitialiseHorizontalField(6);

	int index = 0;
	for (int j = -m_hexRadius; j <= m_hexRadius; j++)
	{
		for (int i = -m_hexRadius; i <= m_hexRadius; i++)
		{
			if (abs(i-j) > m_hexRadius)
				continue;

			m_hexIsolevels[index] = 0.1f+0.02f*index;//0.15f*std::rand()/RAND_MAX;

			hexField->Initialise(&m_horizontalField);
			hexField->DeriveHexPrism(device, m_hexIsolevels[index]);

			m_hexModels[index].Initialize(device, cells, hexField->m_field, m_hexIsolevels[index]);

			m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i+m_hexRadius] = index;
			m_hexPermutation[index] = index;// (index+m_hexes/2)%m_hexes;
			index++;
		}
	}

	m_north = 0;
	m_east = 0;

	m_t = 0.0f;
	m_direction = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

	delete hexField;

	return true;
}

void HexBoard::Render(ID3D11DeviceContext* deviceContext, LightShader* lightShader, DirectX::SimpleMath::Vector3 boardPosition, Camera* camera, float time, Light* light, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalTexture)
{
	float ifrac = -m_t*(m_north+0.5f*m_east);
	float jfrac = -m_t*(m_north-0.5f*m_east);

	DirectX::SimpleMath::Vector3 relativePosition;
	for (int j = -m_hexRadius; j <= m_hexRadius; j++)
	{
		for (int i = -m_hexRadius; i <= m_hexRadius; i++)
		{
			if (abs(i-j) > m_hexRadius)
				continue;

			// FIXME: Refactor this, for 'cleaner' board set-up?
			float l = (camera->getPosition()-boardPosition).Length();
			DirectX::SimpleMath::Matrix ortho = DirectX::SimpleMath::Matrix::CreateOrthographic(l*1280.0f/720.0f, l*1.0f, 0.01f, 100.0f);

			relativePosition = m_origin+(i+ifrac)*m_p+(j+jfrac)*m_q;

			lightShader->EnableShader(deviceContext);
			lightShader->SetLightShaderParameters(deviceContext, &(DirectX::SimpleMath::Matrix::CreateScale(1.0f) * DirectX::SimpleMath::Matrix::CreateTranslation(boardPosition+relativePosition)), &camera->getCameraMatrix(), &ortho, true, time, light, texture, normalTexture);
			m_hexModels[m_hexPermutation[m_hexCoordinates[(2*m_hexRadius+1)*(j+m_hexRadius)+i+m_hexRadius]]].Render(deviceContext);
		}
	}

	return;
}

void HexBoard::SetInterpolation(int north, int east)
{
	// ERROR-HANDLING: 'Normalise' direction...
	m_north = (north != 0) ? north/abs(north) : 0;
	m_east = (east != 0) ? east/abs(east) : 0;

	if (m_north == 0)
		return;

	m_interpolating = true;
}

void HexBoard::Interpolate(float t)
{
	m_t += t;
	if (m_t < 1.0f)
		return;

	Permute();

	m_interpolating = false;
	m_t = 0.0f;
	m_north = 0;
	m_east = 0;
}

void HexBoard::Permute()
{
	if (m_north == 0)
		return;

	int* hexPermutation = new int[m_hexes];

	int ijColumn, ijColumnLength;
	int	iPermuted, jPermuted;
	for (int j = -m_hexRadius; j <= m_hexRadius; j++)
	{
		for (int i = -m_hexRadius; i <= m_hexRadius; i++)
		{
			if (abs(i-j) > m_hexRadius)
				continue;

			if (m_east == -1)
				ijColumn = i;
			else if (m_east == 0)
				ijColumn = i-j;
			else
				ijColumn = j;
			ijColumnLength = m_hexDiameter-abs(ijColumn);

			if (m_east == -1)
			{
				if (j-i == m_north*m_hexRadius || j == m_north*m_hexRadius)
					hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i+m_hexRadius]] = m_hexPermutation[m_hexCoordinates[m_hexDiameter*(j-m_north*(ijColumnLength-1)+m_hexRadius)+i+m_hexRadius]];
				else
					hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i+m_hexRadius]] = m_hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_north+m_hexRadius)+i+m_hexRadius]];
			}
			else if (m_east == 0)
			{
				if (i == m_north*m_hexRadius || j == m_north*m_hexRadius)
					hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i+m_hexRadius]] = m_hexPermutation[m_hexCoordinates[m_hexDiameter*(j-m_north*(ijColumnLength-1)+m_hexRadius)+i-m_north*(ijColumnLength-1)+m_hexRadius]];
				else
					hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i+m_hexRadius]] = m_hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_north+m_hexRadius)+i+m_north+m_hexRadius]];
			}
			else
			{
				if (i-j == m_north*m_hexRadius || i == m_north*m_hexRadius)
					hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i+m_hexRadius]] = m_hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i-m_north*(ijColumnLength-1)+m_hexRadius]];
				else
					hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i+m_hexRadius]] = m_hexPermutation[m_hexCoordinates[m_hexDiameter*(j+m_hexRadius)+i+m_north+m_hexRadius]];
			}
		}
	}

	m_hexPermutation = hexPermutation;
}

void HexBoard::AddThorn(ID3D11Device* device, int hex)
{
	hex = (hex%m_hexes+m_hexes)%m_hexes;

	// STEP 1: Initialise field...
	Field* hexField = new Field();
	hexField->Initialise(&m_horizontalField);

	// STEP 2: Set thorn parameters, integrate it into the field...
	// FIXME: A solid prototype, but highly lacking aesthetically...
	for (int i = 0; i < 2; i++)
	{
		float angle = XM_PIDIV2/10.0f;

		float rBaseRange = 0.5f*cos(XM_PI/6.0f);
		float rBase = rBaseRange*sqrt(pow(((float)std::rand()/RAND_MAX), 2.0f)+pow(((float)std::rand()/RAND_MAX), 2.0f));
		float thetaBase = XM_2PI*std::rand()/RAND_MAX;

		DirectX::SimpleMath::Vector3 base = DirectX::SimpleMath::Vector3(0.5f+0.5f*rBase*cos(thetaBase), 0.0f, 0.5f+0.5f*rBase*sin(thetaBase));

		float rOriginRange = cos(XM_PI/6.0f);
		float rOrigin = rOriginRange*(0.5f+0.5f*sqrt(pow(((float)std::rand()/RAND_MAX), 2.0f)+pow(((float)std::rand()/RAND_MAX), 2.0f)));
		float thetaOrigin = thetaBase + 2.0f*((XM_PI/3.0f)*std::rand()/RAND_MAX);

		DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3(0.5f+0.5f*rOrigin*cos(thetaOrigin), 0.4f+m_hexIsolevels[hex], 0.5f+0.5f*rBase*sin(thetaOrigin));

		hexField->IntegrateHorizontalThorn(origin, base, angle, m_hexIsolevels[hex]);
	}
	
	hexField->DeriveHexPrism(device, m_hexIsolevels[hex]);

	m_hexModels[hex].Initialize(device, hexField->m_cells, hexField->m_field, m_hexIsolevels[hex]);

	delete hexField;
}