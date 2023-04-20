#include "pch.h"
#include "Grammar.h"

Grammar::Grammar()
{
	m_sentence = "";
}


Grammar::~Grammar()
{

}

void Grammar::Initialize(std::string jsonPath, float seed)
{
	std::srand(seed);
}

void Grammar::GenerateSentence(std::string axiom)
{
	// DEBUG:
	if (axiom.length() == 0)
		m_sentence = axiom;

	m_sentence = std::to_string(GetRNGRange());// "You flip a coin... it wobbles in the air... you catch it on the back of your hand... " + (std::string)((GetRNGRange() > 0.0f) ? "heads!" : "tails!");
}

std::string Grammar::GetSentence()
{
	return m_sentence;
}

float Grammar::GetRNGRange(float a, float b)
{
	return a+(b-a)*std::rand()/RAND_MAX;
}