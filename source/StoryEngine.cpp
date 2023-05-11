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

StoryEngine::Scene StoryEngine::StartScene(std::string landmark)
{
	if (landmark == "salt")
		return Scene();

	// FIXME: Use architecture to choose a new storylet...
	Scene scene;
	StoryWorld::StoryCharacter character;
	scene.premise = m_grammar.GenerateSentence("Might we meet {*FULL NAME}, the {*ARCHETYPE}, here?", &character);
	scene.choices = std::vector<std::string>();
	scene.choices.push_back(m_grammar.GenerateSentence("{*{*GENDER} FORENAME} is known to frequent the area...", &character));
	scene.choices.push_back(m_grammar.GenerateSentence("It would be most unlikely to see {{*GENDER} OBJECT PRONOUN}...", &character));

	return scene;
}

StoryEngine::Scene StoryEngine::ContinueScene(int choice)
{
	// FIXME: Use architecture to narrow down storylet...
	Scene scene;
	scene.premise = m_grammar.GenerateSentence("{LANDMARK ADJECTIVE} and {LANDMARK ADJECTIVE} lands await...");

	return scene;
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