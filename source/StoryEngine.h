#pragma once

#include "Architecture.h"
#include "Grammar.h"

#include "StoryWorld.h"
#include "Storylet.h"

class StoryEngine
{
public: 
	StoryEngine();
	~StoryEngine();

	void Initialize(float seed = 0.0f);

	StoryWorld::StoryCharacter GenerateCharacter();

	// DEBUG:
	std::string GenerateSentence(std::string landmark); // NB: Make this into an interactive storylet!

private:
	Architecture m_architecture;
	Grammar m_grammar;

	StoryWorld m_world;
	//Storylet m_storylet;
};

