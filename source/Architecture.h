#pragma once

#include "StoryWorld.h"
#include "Storylet.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

class Architecture
{
public:
	Architecture();
	~Architecture();

	void Initialize(float seed = 0.0f);

private:
	float m_seed;
};

