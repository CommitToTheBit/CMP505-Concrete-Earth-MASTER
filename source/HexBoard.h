#pragma once
#include "MarchingCubes.h"

#include "Camera.h"

#include "LightShader.h"

class HexBoard
{
public:
	HexBoard();
	~HexBoard();

	bool Initialize(ID3D11Device*, int hexRadius, int cells);

	// Rendering...
	void Render(ID3D11DeviceContext*, 
		LightShader* lightShader,
		DirectX::SimpleMath::Vector3 boardPosition,
		Camera* camera, 
		float time,
		Light* light,  
		ID3D11ShaderResourceView* texture, 
		ID3D11ShaderResourceView* normalTexture);

	void SetInterpolation(int north, int east);
	void Interpolate(float t);

	// DEBUG: Testing interactivity...
	void AddThorn(ID3D11Device*, int hex);

private:
	void Permute();
	void SetPerimeter();

public: // FIXME: Left off while still accessed in Game.cpp...
	int m_hexRadius, m_hexDiameter, m_hexes; // total tiles: 1+3*m_hexRadius*(m_hexRadius+1)
	int* m_hexCoordinates;
	int* m_hexPermutation;

	// Modelling info...
	Field m_horizontalField;

	float* m_hexIsolevels;
	MarchingCubes* m_hexModels; // new MarchingTerrain[1+3*m_hexRadius*(m_hexRadius+1)]

	static const DirectX::SimpleMath::Vector3 m_origin, m_p, m_q;

	// Linear interpolation...
	int m_north, m_east;

	bool m_interpolating;
	float m_t;
	DirectX::SimpleMath::Vector3 m_direction;
};

