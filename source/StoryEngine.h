#pragma once

#include "Architecture.h"
#include "Grammar.h"

#include "StoryWorld.h"
#include "Storylet.h"

class StoryEngine
{
public: 
	StoryWorld::StoryCharacter GenerateCharacter();

private:
	Architecture m_architecture;
	Grammar m_grammar;

	StoryWorld m_world;
	//Storylet m_storylet;
};

