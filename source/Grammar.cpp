#include "pch.h"
#include "Grammar.h"

Grammar::Grammar()
{
	m_seed = 0;

	m_sentence = "";
	m_generations = 0;
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

	m_generations = 0;
}

void Grammar::GenerateSentence(std::string axiom, Storyworld::StoryCharacter* character)
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
				ProductionRuleType productionRule = GetProductionRule(m_sentence.substr(0, m_sentence.find("}")));
				iteratedSentence += productionRule.production;
				//iteratedSentence += " ("+std::to_string(productionRule.dryness)+")"; // DEBUG...
				m_sentence.erase(m_sentence.begin(), m_sentence.begin() + m_sentence.find("}") + 1);
			}
		}
		iteratedSentence += m_sentence;

		m_sentence = iteratedSentence+"; ";
	}

	// DEBUG:
	if (!character)
		m_sentence = "(DEBUG) NULL CHARACTER: " + m_sentence;

	m_generations++;
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

Grammar::ProductionRuleType Grammar::GetProductionRule(std::string letter, bool generation)
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
		totalWeight += GetWeight(productionRule, letter);
	}

	int index = 0;
	float weight = GetRNGRange(0.0f, totalWeight);
	float summedWeight = 0.0f;
	for each (ProductionRuleType productionRule in m_productionRules[letter])
	{
		if (weight > summedWeight + GetWeight(productionRule, letter))
		{
			summedWeight += GetWeight(productionRule, letter);
			index++;
		}
		else
		{
			//summedWeight += GetWeight(productionRule, letter);
			break;
		}
	}

	// Remember this has been used...
	if (generation)
		m_productionRules[letter][index].dryness = m_generations;

	return m_productionRules[letter][index];
}

float Grammar::GetWeight(ProductionRuleType productionRule, std::string letter)
{
	float weight = productionRule.weight;

	// STEP 1: Scale by dryness...
	// Order... lowest to highest?
	std::vector<int> generations = std::vector<int>();
	for (ProductionRuleType rule : m_productionRules[letter])
	{
		generations.push_back(rule.dryness);
	}
	std::sort(generations.begin(), generations.end());

	float minimumWeighting = 0.01f;// pow(0.01f, generations.size()); // NB: The most recently-used production rule will be minimumWeighting times as likely to be surfaced as the least recent...
	float exponent = ((float)std::distance(generations.begin(), std::find(generations.begin(), generations.end(), productionRule.dryness)))/generations.size();// /generations.size();
	weight *= (productionRule.dryness < m_generations) ? pow(minimumWeighting, exponent) : 0.0f; // NB: Explicitely blocks the uses of the same production rule (though not the same word!) in one generation...

	return weight;

}

float Grammar::GetRNGRange(float a, float b)
{
	return a+(b-a)*std::rand()/RAND_MAX;
}