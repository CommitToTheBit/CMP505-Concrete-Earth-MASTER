#include "pch.h"
#include "MarchingTerrain.h"

void MarchingTerrain::GenerateHorizontalField(DirectX::SimpleMath::Vector3 origin)
{
	SimplexNoise simplex = SimplexNoise();

	int fieldCoordinate;

	DirectX::SimpleMath::Vector3 position;

	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;

				position = 2.0f*(m_field[fieldCoordinate].position-origin);

				m_field[fieldCoordinate].scalar = position.y;
				m_field[fieldCoordinate].scalar += simplex.FBMNoise(m_field[fieldCoordinate].position.x, m_field[fieldCoordinate].position.y, m_field[fieldCoordinate].position.z, 6, 1.0f); // Will we just manually handle height, etc.?
				//m_field[fieldCoordinate].scalar += std::min(simplex.FBMNoise(m_field[fieldCoordinate].position.x, m_field[fieldCoordinate].position.y, m_field[fieldCoordinate].position.z, 6, 1.0f), 0.0f);
			}
		}
	}
}

void MarchingTerrain::GenerateSphericalField(DirectX::SimpleMath::Vector3 origin)
{
	int fieldCoordinate;
	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;
				m_field[fieldCoordinate].scalar = 2.0f*(m_field[fieldCoordinate].position-origin).Length();
			}
		}
	}
}

void MarchingTerrain::GenerateSinusoidalSphericalField(DirectX::SimpleMath::Vector3 origin)
{
	const float AMPLITUDE = 0.03f;

	int fieldCoordinate;

	DirectX::SimpleMath::Vector3 position;
	float r, theta, phi, psi;

	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;

				position = 2.0f*(m_field[fieldCoordinate].position-origin);
				r = position.Length();
				theta = atan2(position.z, position.x);
				phi = atan2(position.y, position.z); // NB: Not 'true' phi...
				psi = atan2(position.x, position.y);

				m_field[fieldCoordinate].scalar = r;
				m_field[fieldCoordinate].scalar += AMPLITUDE*sin(2.0f*theta);
				m_field[fieldCoordinate].scalar += AMPLITUDE*sin(3.0f*phi);
				m_field[fieldCoordinate].scalar += AMPLITUDE*sin(5.0f*psi);
				m_field[fieldCoordinate].scalar /= 1.0f-3.0f*AMPLITUDE;
			}
		}
	}
}

void MarchingTerrain::GenerateToroidalField(DirectX::SimpleMath::Vector3 origin)
{
	ClassicNoise perlin = ClassicNoise();

	const float R = 2.0f/3.0f;

	int fieldCoordinate;

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 ringPosition;
	float r, theta, phi;

	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;

				position = 2.0f*(m_field[fieldCoordinate].position-origin);
				r = position.Length();
				theta = atan2(position.y, position.x); // NB: Not 'true' theta...

				// NB: This orientation can - should? - be generalised...
				ringPosition = DirectX::SimpleMath::Vector3(R*cos(theta), R*sin(theta), 0.0f);

				m_field[fieldCoordinate].scalar = (position-ringPosition).Length();
				m_field[fieldCoordinate].scalar += 0.2f*perlin.FBMNoise(m_field[fieldCoordinate].position.x, m_field[fieldCoordinate].position.y, m_field[fieldCoordinate].position.z, 6, 1.0f);
				m_field[fieldCoordinate].scalar /= 1.0f-R;
			}
		}
	}
}

void MarchingTerrain::GenerateHex(ID3D11Device* device, float isolevel)
{
	int fieldCoordinate;

	DirectX::SimpleMath::Vector2 position;
	float r, theta;

	int q, quadrant;
	DirectX::SimpleMath::Vector2 quadrantDirection;

	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;

				position = 2.0f*DirectX::SimpleMath::Vector2(m_field[fieldCoordinate].position.x-0.5f, m_field[fieldCoordinate].position.z-0.5f);

				theta = atan2(position.y, position.x);
				if (theta < 0.0f)
					theta += XM_2PI;

				q = 6;
				for (quadrant = 0; theta >= ((float)quadrant+1.0f)*XM_2PI/(float)q; quadrant++) { }
				quadrantDirection = DirectX::SimpleMath::Vector2(cos(((float)quadrant+0.5f)*XM_2PI/(float)q), sin(((float)quadrant+0.5f)*XM_2PI/(float)q));
				r = position.Dot(quadrantDirection)/cos(XM_PI/(float(q)));

				//sextant = ((int)(atan2(position.y,position.x)/(XM_2PI/6.0f))+6)%6;
				//sextantDirection = DirectX::SimpleMath::Vector2(cos((float)sextant*XM_PI/3.0f+XM_PI/6.0f), sin((float)sextant*XM_PI/3.0f+XM_PI/6.0f))*cos(XM_PI/6.0f);

				//r = std::max(abs(position.x), abs(position.y)); // NB: Radius for square prism...
				//r = position.Length(); // NB: Radius for cylinder...

				// FIXME: Causing a 'Lego' effect!
				//if (m_field[fieldCoordinate].scalar < isolevel || r >= 1.0f)
				m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, isolevel*r);
				m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, isolevel*(1.0f-m_field[fieldCoordinate].position.y));

				//if (((i == 0) || (j == 0) || (k == 0) || (i == m_cells) || (j == m_cells) || (k == m_cells)) && m_field[fieldCoordinate].scalar < isolevel) // FIXME: && r < 1.0f)
				//	m_field[fieldCoordinate].scalar = isolevel;

				// FIXME: Work out circular, then hex case...
				//if (m_field[fieldCoordinate].scalar < isolevel && r > 1.0f)
				//	m_field[fieldCoordinate].scalar = isolevel-1.0f;
			}
		}
	}

	// STEP 2: Draw the hex...
	GenerateIsosurface(device, isolevel);
}