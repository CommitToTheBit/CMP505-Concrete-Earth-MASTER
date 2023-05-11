#pragma once

#include "Architecture.h"
#include "Grammar.h"

#include "StoryWorld.h"
#include "Storylet.h"

class StoryEngine
{
public:
	struct Scene
	{
		std::string premise;
		std::vector<std::string> choices;
	};

public: 
	StoryEngine();
	~StoryEngine();

	void Initialize(float seed = 0.0f);

	Scene StartScene(std::string landmark);
	Scene ContinueScene(int choice);

	// DEBUG:
	std::string GenerateSentence(std::string landmark); // NB: Make this into an interactive storylet!

private:
	StoryWorld::StoryCharacter GenerateCharacter();
	//void UpdateStoryworld()

private:
	Architecture m_architecture;
	Grammar m_grammar;

	StoryWorld m_world;

	Storylet m_storylet;
};

