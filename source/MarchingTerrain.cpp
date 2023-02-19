#include "pch.h"
#include "MarchingTerrain.h"

MarchingTerrain::MarchingTerrain()
{

}


MarchingTerrain::~MarchingTerrain()
{

}

void MarchingTerrain::InitialiseHorizontalField(int octaves, float amplitude)
{
	m_field = new FieldVertexType[(m_cells + 1) * (m_cells + 1) * (m_cells + 1)];
	m_isosurfaceIndices = new int[m_cells * m_cells * m_cells];
	m_isosurfaceVertices = new int[12 * m_cells * m_cells * m_cells];
	m_isosurfacePositions = new DirectX::SimpleMath::Vector3[12 * m_cells * m_cells * m_cells];

	SimplexNoise simplex = SimplexNoise();

	DirectX::SimpleMath::Vector3 position;

	for (int f = 0; f < (m_cells+1)*(m_cells+1)*(m_cells+1); f++)
	{
		m_field[f].position = DirectX::SimpleMath::Vector3(f%(m_cells+1), (f/(m_cells+1))%(m_cells+1), f/((m_cells+1)*(m_cells+1)))/m_cells;

		m_field[f].scalar = m_field[f].position.y;
		m_field[f].scalar += simplex.FBMNoise(m_field[f].position.x, 0.0f, m_field[f].position.z, octaves, amplitude);
		m_field[f].scalar = std::max(m_field[f].scalar, 0.0f);
	}
}

void MarchingTerrain::InitialiseSphericalField(int octaves, float amplitude)
{
	m_field = new FieldVertexType[(m_cells + 1) * (m_cells + 1) * (m_cells + 1)];

	m_isosurfaceIndices = new int[m_cells * m_cells * m_cells];
	m_isosurfaceVertices = new int[12 * m_cells * m_cells * m_cells];
	m_isosurfacePositions = new DirectX::SimpleMath::Vector3[12 * m_cells * m_cells * m_cells];

	SimplexNoise simplex = SimplexNoise();

	DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f);

	for (int f = 0; f < (m_cells+1)*(m_cells+1)*(m_cells+1); f++)
	{
		m_field[f].position = DirectX::SimpleMath::Vector3(f%(m_cells+1), (f/(m_cells+1))%(m_cells+1), f/((m_cells+1)*(m_cells+1)))/m_cells;

		m_field[f].scalar = 2.0f*(m_field[f].position-origin).Length();
		m_field[f].scalar += simplex.FBMNoise(m_field[f].position.x, m_field[f].position.y, m_field[f].position.z, octaves, amplitude);
		m_field[f].scalar = std::max(m_field[f].scalar, 0.0f);
	}
}

void MarchingTerrain::InitialiseToroidalField(float R, int octaves, float amplitude)
{
	m_field = new FieldVertexType[(m_cells + 1) * (m_cells + 1) * (m_cells + 1)];

	m_isosurfaceIndices = new int[m_cells * m_cells * m_cells];
	m_isosurfaceVertices = new int[12 * m_cells * m_cells * m_cells];
	m_isosurfacePositions = new DirectX::SimpleMath::Vector3[12 * m_cells * m_cells * m_cells];

	SimplexNoise simplex = SimplexNoise();

	DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f);

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 ringPosition;
	float r, theta, phi;

	for (int f = 0; f < (m_cells+1)*(m_cells+1)*(m_cells+1); f++)
	{
		m_field[f].position = DirectX::SimpleMath::Vector3(f%(m_cells+1), (f/(m_cells+1))%(m_cells+1), f/((m_cells+1)*(m_cells+1)))/m_cells;

		position = 2.0f*(m_field[f].position-origin);
		r = position.Length();
		theta = atan2(position.y, position.x); // NB: Not 'true' theta...
		ringPosition = DirectX::SimpleMath::Vector3(R*cos(theta), R*sin(theta), 0.0f);

		m_field[f].scalar = (position-ringPosition).Length();
		m_field[f].scalar += simplex.FBMNoise(m_field[f].position.x, m_field[f].position.y, m_field[f].position.z, octaves, std::min(R, 1.0f-R)*amplitude);
		m_field[f].scalar /= std::min(R, 1.0f-R);
		m_field[f].scalar = std::max(m_field[f].scalar, 0.0f);
	}
}

void MarchingTerrain::IntegrateHorizontalThorn(DirectX::SimpleMath::Vector3 origin, DirectX::SimpleMath::Vector3 base, float angle, float isolevel)
{
	SimplexNoise simplex = SimplexNoise();

	float theta, thorn;

	for (int f = 0; f < (m_cells+1)*(m_cells+1)*(m_cells+1); f++)
	{
		theta = acos((m_field[f].position-origin).Dot(base-origin)/((m_field[f].position-origin).Length()*(base-origin).Length()));
		thorn = theta/angle;
				
		// FIXME: Procedural coarseness is not convincing...
		//thorn -= simplex.FBMNoise(m_field[f].position.x, m_field[f].position.y, m_field[f].position.z, 8, 0.5f);

		m_field[f].scalar = std::min(m_field[f].scalar, isolevel*thorn); // NB: Use of min, since this points 'out' from isosurface...

		// FIXME: Procedural coarseness is not convincing...
		//if ((m_field[f].position-origin).Length() < 4.0f*sqrt(3)/m_cells) // NB: Not rigourous, but seems to protect against 'floating islands' at the tip?
		//	m_field[f].scalar = 2.0*isolevel;
	}
}

void MarchingTerrain::IntegrateOrb(DirectX::SimpleMath::Vector3 centre, float radius, float isolevel)
{
	SimplexNoise simplex = SimplexNoise();

	float orb;

	for (int f = 0; f < m_cells* m_cells* m_cells; f++)
	{
			orb = (m_field[f].position-centre).Length()/radius;

			// FIXME: Procedural coarseness is not convincing...
			//orb += simplex.FBMNoise(m_field[f].position.x, m_field[f].position.y, m_field[f].position.z, 8, 0.2f*radius);

			m_field[f].scalar = std::min(m_field[f].scalar, isolevel*orb);
	}
}

void MarchingTerrain::DeriveHexPrism(ID3D11Device* device, float isolevel, bool lowerBound, bool upperBound)
{
	DirectX::SimpleMath::Vector2 position;
	float r, theta, z;

	int q, quadrant;
	DirectX::SimpleMath::Vector2 quadrantDirection;

	for (int f = 0; f < (m_cells+1)*(m_cells+1)*(m_cells+1); f++)
	{
		position = 2.0f*DirectX::SimpleMath::Vector2(m_field[f].position.x-0.5f, m_field[f].position.z-0.5f);
		theta = atan2(position.y, position.x);
		if (theta < 0.0f)
			theta += XM_2PI;

		q = 6;
		for (quadrant = 0; theta >= ((float)quadrant+1.0f)*XM_2PI/(float)q; quadrant++) { }
		quadrantDirection = DirectX::SimpleMath::Vector2(cos(((float)quadrant+0.5f)*XM_2PI/(float)q), sin(((float)quadrant+0.5f)*XM_2PI/(float)q));

		r = position.Dot(quadrantDirection)/cos(XM_PI/(float(q)));
		m_field[f].scalar = std::max(m_field[f].scalar, isolevel*r);

		if (lowerBound)
		{
			z = 1.0f-m_field[f].position.y;
			m_field[f].scalar = std::max(m_field[f].scalar, isolevel*z);
		}

		if (upperBound)
		{
			z = m_field[f].position.y;
			m_field[f].scalar = std::max(m_field[f].scalar, isolevel*z);
		}
	}

	// STEP 2: Draw the hex...
	GenerateIsosurface(device, isolevel);
}

void MarchingTerrain::DeriveCylindricalPrism(ID3D11Device* device, float isolevel, bool lowerBound, bool upperBound)
{
	DirectX::SimpleMath::Vector2 position;
	float r, theta, z;

	int q, quadrant;
	DirectX::SimpleMath::Vector2 quadrantDirection;

	for (int f = 0; f < (m_cells+1)*(m_cells+1)*(m_cells+1); f++)
	{
		position = 2.0f*DirectX::SimpleMath::Vector2(m_field[f].position.x-0.5f, m_field[f].position.z-0.5f);
		r = position.Length();
		m_field[f].scalar = std::max(m_field[f].scalar, isolevel*r);

		if (lowerBound)
		{
			z = 1.0f-m_field[f].position.y;
			m_field[f].scalar = std::max(m_field[f].scalar, isolevel*z);
		}
				
		if (upperBound)
		{
			z = m_field[f].position.y;
			m_field[f].scalar = std::max(m_field[f].scalar, isolevel*z);
		}
	}

	// STEP 2: Draw the hex...
	GenerateIsosurface(device, isolevel);
}