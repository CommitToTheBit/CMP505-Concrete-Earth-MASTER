// CC: Simplex Noise Demystified...

#pragma once

using namespace DirectX;

class ClassicNoise
{
public:
	ClassicNoise();
	~ClassicNoise();

	float Noise(float x, float y, float z);

private:
	// Mathematical functions
	int FastFloor(float x);
	float Dot(int g[3], float x, float y, float z);
	float Mix(float a, float b, float t);
	float Fade(float t);

private:
	int m_grad3[12][3];
	int m_p[256];
	int m_perm[512];
};

