#include "pch.h"
#include "StoryEngine.h"

StoryEngine::StoryEngine()
{
	// DEBUG: Fixed storylet...
	/*m_storylet.beginning.axiom = "{*FULL NAME}, a {*ARCHETYPE}, offers you payment for passage across the steppe.";

	m_storylet.middle = std::vector<Storylet::Text>(2);
	m_storylet.middle[0].axiom = "accept {*{*GENDER} FORENAME}'s coin...";
	m_storylet.middle[1].axiom = "turn {{*GENDER} OBJECT PRONOUN} away...";

	m_storylet.end = std::vector<std::vector<Storylet::Text>>(2);
	m_storylet.end[0] = std::vector<Storylet::Text>(2);
	m_storylet.end[0][0].axiom = "you extend your talon, and welcome the {*ARCHETYPE} aboard.";
	m_storylet.end[0][1].axiom = "you carry on, leaving poor {*{*GENDER} FORENAME} {*{*GENDER} PATRONYMIC} by the wayside.";
	m_storylet.end[1] = std::vector<Storylet::Text>(1);
	m_storylet.end[1][0].axiom = "you carry on, leaving poor {*{*GENDER} FORENAME} {*{*GENDER} PATRONYMIC} by the wayside.";

	m_storylet.progressed = false;*/
}

StoryEngine::~StoryEngine()
{

}

void StoryEngine::Initialize(float seed)
{
	// STEP 1: Initialise generators...
	m_architecture.Initialize(&m_world, seed);
	m_grammar.Initialize(seed);
}

StoryEngine::Scene StoryEngine::StartScene(std::string landmark)
{
	if (landmark == "salt")
		return Scene();

	// RESET TEMPS...
	m_world.active = StoryWorld::StoryCharacter();
	m_world.passive = StoryWorld::StoryCharacter();

	m_storylet = m_architecture.SelectBeginning();
	m_architecture.SelectMiddle(&m_storylet);

	Scene scene;
	StoryWorld::StoryCharacter character; // FIXME: Contain within storyworld as a "local" character (as in, local to the hex...)
	scene.premise = m_grammar.GenerateSentence(m_storylet.beginning.axiom, m_storylet.beginning.active, m_storylet.beginning.passive);
	scene.choices = std::vector<std::string>(); 
	for (Storylet::Text middle : m_storylet.middle)
		scene.choices.push_back(m_grammar.GenerateSentence(middle.axiom, middle.active, middle.passive));

	return scene;
}

StoryEngine::Scene StoryEngine::ContinueScene(int choice)
{
	if (m_storylet.progressed)
	{
		// FIXME: Could add a descriptor of the journey (w/o a choice) here, but might get tiresome...
		return Scene();
	}
	m_storylet.progressed = true;

	m_architecture.SelectEnd(&m_storylet, choice);

	Scene scene;
	scene.premise = m_grammar.GenerateSentence(m_storylet.end[choice][0].axiom, m_storylet.end[choice][0].active, m_storylet.end[choice][0].passive);
	
	// NB: Does forcing the player to hit "Continue" kill the pacing?
	/*scene.choices = std::vector<std::string>();
	scene.choices.push_back(m_grammar.GenerateSentence("Continue..."));*/

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