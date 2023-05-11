#include "pch.h"
#include "StoryEngine.h"

StoryEngine::StoryEngine()
{

}

StoryEngine::~StoryEngine()
{

}

void StoryEngine::Initialize(float seed)
{
	// STEP 1: Initialise generators...
	m_architecture.Initialize(seed);
	m_grammar.Initialize(seed);
}

StoryWorld::StoryCharacter StoryEngine::GenerateCharacter()
{
	StoryWorld::StoryCharacter character;
	m_grammar.GenerateSentence("{*FULL NAME}", &character);

	return character;
}

std::string StoryEngine::GenerateSentence(std::string suit)
{
	if (suit == "salt")
		return "";

	return m_grammar.GenerateSentence("Might we meet {*FULL NAME}, the {*ARCHETYPE}, here?");
}