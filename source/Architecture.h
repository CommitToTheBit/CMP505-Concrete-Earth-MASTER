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

	// DEBUG:
	Storylet SelectBeginning();
	void SelectMiddle(Storylet* storylet);
	void SelectEnd(Storylet* storylet);

private:
	float m_seed;

	StoryWorld* m_world;

	// DEBUG:
	Storylet m_storylet;
};

