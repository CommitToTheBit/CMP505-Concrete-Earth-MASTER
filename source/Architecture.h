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

	void Initialize(StoryWorld* world, float seed = 0.0f);

	// DEBUG:
	Storylet SelectBeginning();
	void SelectMiddle(Storylet* storylet);
	void SelectEnd(Storylet* storylet, int choice);

private:
	Storylet::Text InitializeText(nlohmann::json data);
	Storylet::Causes InitializeCauses(nlohmann::json data);
	Storylet::Effects InitializeEffects(nlohmann::json data);

private:
public: // DBEUG...
	std::vector<Storylet> m_storylets;

	float m_seed;
	StoryWorld* m_world;
};

