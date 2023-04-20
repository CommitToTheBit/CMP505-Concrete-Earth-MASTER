#include "pch.h"
#include "Grammar.h"

Grammar::Grammar()
{
	m_sentence = "";
	m_seed = 0;
}


Grammar::~Grammar()
{

}

void Grammar::Initialize(std::string jsonPath, float seed)
{
	m_seed = seed;

	std::ifstream f("Alphabet.json");
	nlohmann::json alphabet = nlohmann::json::parse(f);

	m_sentence = alphabet["Lorem"].value("Ipsum", "Failed!"); // FIXME: Figure out further json functionality! See https://stackoverflow.com/questions/42887392/how-to-get-array-from-nlohmann-json...
}

void Grammar::GenerateSentence(std::string axiom)
{
	srand(m_seed); // FIXME: Hacky, but a good patch in lieu of a better rng implementation?
	m_seed = 2.0f*(std::rand()-RAND_MAX/2)+std::rand()/RAND_MAX;

	// DEBUG:
	if (axiom.length() == 0)
		m_sentence = axiom;

	m_sentence = "You flip a coin... it wobbles in the air... you catch it on the back of your hand... " + (std::string)((GetRNGRange() > 0.0f) ? "heads!" : "tails!");
}

std::string Grammar::GetSentence()
{
	return m_sentence;
}

float Grammar::GetRNGRange(float a, float b)
{
	return a+(b-a)*std::rand()/RAND_MAX;
}