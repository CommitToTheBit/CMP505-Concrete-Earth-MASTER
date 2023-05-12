#pragma once
#include "MarchingCubes.h"
#include "StoryEngine.h"

#include "Camera.h"

#include "Shader.h"

class Board
{
public:
	Board();
	~Board();

	bool Initialize(ID3D11Device*, int hexRadius, int cells);

	// Rendering...
	void Render(ID3D11DeviceContext*,
		Shader* shader,
		DirectX::SimpleMath::Vector3 boardPosition,
		float boardScale,
		float tileScale,
		Camera* camera, 
		float time,
		Light* light);

	void SetInterpolation(int north, int east);
	void Interpolate(float t);

	// DEBUG: Testing interactivity...
	void AddThorns(ID3D11Device*, int hex, int thorns);

	// Changing scene...
	void Choose(int choice);
	bool Paused();

private:
	void SetInterpolationPerimeter();
	void ApplyInterpolationPermutation();

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

	// Storylets...
	StoryEngine::Scene m_scene;

private:
	StoryEngine m_storyEngine;
};

