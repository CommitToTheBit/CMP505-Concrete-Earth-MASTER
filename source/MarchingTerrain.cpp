#include "pch.h"
#include "MarchingTerrain.h"

void MarchingTerrain::InitialiseHorizontalField(int octaves, float amplitude)
{
	SimplexNoise simplex = SimplexNoise();

	DirectX::SimpleMath::Vector3 position;

	int fieldCoordinate;
	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;
				m_field[fieldCoordinate].scalar = m_field[fieldCoordinate].position.y;
				m_field[fieldCoordinate].scalar += simplex.FBMNoise(m_field[fieldCoordinate].position.x, 0.0f, m_field[fieldCoordinate].position.z, octaves, amplitude);
				m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, 0.0f); 
			}
		}
	}
}

void MarchingTerrain::InitialiseSphericalField(int octaves, float amplitude)
{
	SimplexNoise simplex = SimplexNoise();

	DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f);

	int fieldCoordinate;
	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;
				m_field[fieldCoordinate].scalar = 2.0f*(m_field[fieldCoordinate].position-origin).Length();
				m_field[fieldCoordinate].scalar += simplex.FBMNoise(m_field[fieldCoordinate].position.x, m_field[fieldCoordinate].position.y, m_field[fieldCoordinate].position.z, octaves, amplitude);
				m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, 0.0f);
			}
		}
	}
}

void MarchingTerrain::InitialiseToroidalField(float R, int octaves, float amplitude)
{
	SimplexNoise simplex = SimplexNoise();

	DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f);

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 ringPosition;
	float r, theta, phi;

	int fieldCoordinate;
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
				ringPosition = DirectX::SimpleMath::Vector3(R*cos(theta), R*sin(theta), 0.0f);

				m_field[fieldCoordinate].scalar = (position-ringPosition).Length();
				m_field[fieldCoordinate].scalar += simplex.FBMNoise(m_field[fieldCoordinate].position.x, m_field[fieldCoordinate].position.y, m_field[fieldCoordinate].position.z, octaves, std::min(R, 1.0f-R)*amplitude);
				m_field[fieldCoordinate].scalar /= std::min(R, 1.0f-R);
				m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, 0.0f);
			}
		}
	}
}

void MarchingTerrain::AttachHorizontalThorn(float isolevel)
{
	SimplexNoise simplex = SimplexNoise();

	DirectX::SimpleMath::Vector3 peak = DirectX::SimpleMath::Vector3(0.5f+0.25f*cos(2.0f*XM_PI/3.0f), 0.5f, 0.5f-0.25f*sin(2.0f*XM_PI/3.0f));
	DirectX::SimpleMath::Vector3 base = DirectX::SimpleMath::Vector3(0.5f, 0.0f, 0.5f);
	float angle = XM_PIDIV2/8.0f;

	DirectX::SimpleMath::Vector3 position;
	float theta, thorn;

	int fieldCoordinate;
	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;

				theta = acos((m_field[fieldCoordinate].position-peak).Dot(base-peak)/((m_field[fieldCoordinate].position-peak).Length()*(base-peak).Length()));
				thorn = theta/angle;
				
				// FIXME: Procedural coarseness is not convincing...
				//thorn -= simplex.FBMNoise(m_field[fieldCoordinate].position.x, m_field[fieldCoordinate].position.y, m_field[fieldCoordinate].position.z, 8, 0.5f);

				m_field[fieldCoordinate].scalar = std::min(m_field[fieldCoordinate].scalar, isolevel*thorn); // NB: Use of min, since this points 'out' from isosurface...

				// FIXME: Procedural coarseness is not convincing...
				if ((position-peak).Length() < 4.0f*sqrt(3)/m_cells) // NB: Not rigourous, but seems to protect against 'floating islands' at the tip?
					m_field[fieldCoordinate].scalar = 2.0*isolevel;
			}
		}
	}
}

void MarchingTerrain::GenerateHexPrism(ID3D11Device* device, float isolevel, bool lowerBound, bool upperBound)
{
	DirectX::SimpleMath::Vector2 position;
	float r, theta, z;

	int q, quadrant;
	DirectX::SimpleMath::Vector2 quadrantDirection;

	int fieldCoordinate;
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
				m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, isolevel*r);

				if (lowerBound)
				{
					z = 1.0f-m_field[fieldCoordinate].position.y;
					m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, isolevel*z);
				}

				if (upperBound)
				{
					z = m_field[fieldCoordinate].position.y;
					m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, isolevel*z);
				}
			}
		}
	}

	// STEP 2: Draw the hex...
	GenerateIsosurface(device, isolevel);
}

void MarchingTerrain::GenerateCylindricalPrism(ID3D11Device* device, float isolevel, bool lowerBound, bool upperBound)
{
	DirectX::SimpleMath::Vector2 position;
	float r, theta, z;

	int q, quadrant;
	DirectX::SimpleMath::Vector2 quadrantDirection;

	int fieldCoordinate;
	for (int k = 0; k <= m_cells; k++)
	{
		for (int j = 0; j <= m_cells; j++)
		{
			for (int i = 0; i <= m_cells; i++)
			{
				fieldCoordinate = (m_cells+1)*(m_cells+1)*k+(m_cells+1)*j+i;

				position = 2.0f*DirectX::SimpleMath::Vector2(m_field[fieldCoordinate].position.x-0.5f, m_field[fieldCoordinate].position.z-0.5f);
				r = position.Length();
				m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, isolevel*r);

				if (lowerBound)
				{
					z = 1.0f-m_field[fieldCoordinate].position.y;
					m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, isolevel*z);
				}
				
				if (upperBound)
				{
					z = m_field[fieldCoordinate].position.y;
					m_field[fieldCoordinate].scalar = std::max(m_field[fieldCoordinate].scalar, isolevel*z);
				}
			}
		}
	}

	// STEP 2: Draw the hex...
	GenerateIsosurface(device, isolevel);
}