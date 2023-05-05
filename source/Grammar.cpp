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

	m_productionRules = std::map<std::string, std::vector<ProductionRuleType>>();
	for (auto letter : alphabet.items())
	{
		if (!alphabet[letter.key()].contains("productions"))
			continue;

		for (std::string production : alphabet[letter.key()]["productions"])
		{
			//m_sentence += production;
			ProductionRuleType productionRule;
			productionRule.production = letter.key();
			productionRule.dryness = -1;
			productionRule.weight = 1.0f;

			AddProductionRule(production, productionRule);
		}
	}

	//m_sentence = alphabet["non-Euclidean"]["productions"][0];// .value("Ipsum", "Failed!"); // FIXME: Figure out further json functionality! See https://stackoverflow.com/questions/42887392/how-to-get-array-from-nlohmann-json...
}

void Grammar::GenerateSentence(std::string axiom)
{
	srand(m_seed); // FIXME: Hacky, but a good patch in lieu of a better rng implementation?
	m_seed = 2.0f*(std::rand()-RAND_MAX/2)+std::rand()/RAND_MAX;

	// FIXME: Generalise this to include nested brackets...
	m_sentence = axiom;
	std::string iteratedSentence;
	while (m_sentence.find("{") != -1)
	{
		iteratedSentence = "";
		while (m_sentence.find("{") != -1)
		{
			iteratedSentence += m_sentence.substr(0, m_sentence.find("{"));
			m_sentence.erase(m_sentence.begin(), m_sentence.begin() + m_sentence.find("{") + 1);

			if (m_sentence.find("}") != -1)
			{
				iteratedSentence += GetProductionRule(m_sentence.substr(0, m_sentence.find("}"))).production;
				m_sentence.erase(m_sentence.begin(), m_sentence.begin() + m_sentence.find("}") + 1);
			}
		}
		iteratedSentence += m_sentence;

		m_sentence = iteratedSentence;
	}

	// DEBUG:
	//if (axiom.length() == 0)
	//	m_sentence = axiom;

	//m_sentence = "You flip a coin... it wobbles in the air... you catch it on the back of your hand... " + (std::string)((GetRNGRange() > 0.0f) ? "heads!" : "tails!");
}

std::string Grammar::GetSentence()
{
	return m_sentence;
}

void Grammar::AddProductionRule(std::string letter, ProductionRuleType productionRule)
{
	if (!m_productionRules.count(letter))
		m_productionRules.insert({ letter, std::vector<ProductionRuleType>{ productionRule } });
	else
		m_productionRules[letter].push_back(productionRule);
}

Grammar::ProductionRuleType Grammar::GetProductionRule(std::string letter, int turn)
{
	if (!m_productionRules.count(letter))
	{
		ProductionRuleType identity;
		identity.production = letter; // DEBUG: "NULL("+letter+")";
		identity.dryness = -1;
		identity.weight = 1.0f;
		return identity;
	}

	// FIXME: Add dryness assessment here?
	float totalWeight = 0.0f;
	for each (ProductionRuleType productionRule in m_productionRules[letter])
	{
		totalWeight += productionRule.weight;
	}

	int index = 0;
	float weight = GetRNGRange(0.0f, totalWeight);
	float summedWeight = 0.0f;
	for each (ProductionRuleType productionRule in m_productionRules[letter])
	{
		if (weight > summedWeight+productionRule.weight)
		{
			summedWeight += productionRule.weight;
			index++;
		}
		else
		{
			break;
		}
	}

	// Remember this has been used...
	m_productionRules[letter][index].dryness = std::max(m_productionRules[letter][index].dryness, turn);

	return m_productionRules[letter][index];
}

float Grammar::GetRNGRange(float a, float b)
{
	return a+(b-a)*std::rand()/RAND_MAX;
}