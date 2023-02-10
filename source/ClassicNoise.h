/* -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/* This enclosed section has been adapted from: Stefan Gustavson (2005) Demystifying Simplex Noise. Available at https://weber.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf (Accessed: 10 February 2023) */

#pragma once

using namespace DirectX;

class ClassicNoise
{
public:
	ClassicNoise();
	~ClassicNoise();

	float Noise(float x, float y, float z);
	float FBMNoise(float x, float y, float z);

private:
	// Mathematical functions
	int FastFloor(float x);
	float Dot(int g[3], float x, float y, float z);
	float Mix(float a, float b, float t);
	float Fade(float t);

private:
	static const int m_grad3[12][3];
	static const int m_perm[512];
};

/* -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */