#include "pch.h"
#include "Architecture.h"

Architecture::Architecture()
{
	m_seed = 0;

	// DEBUG:
	m_storylet.progress = 0;
	m_storylet.beginning.axiom = "{*FULL NAME}, a {*ARCHETYPE}, offers you payment for passage across the steppe.";
	m_storylet.middles = std::vector<Storylet::Text>(2);
	//m_storylet.middles[0].axiom = 
}


Architecture::~Architecture()
{

}

void Architecture::Initialize(float seed)
{
	m_seed = seed;

	// Initialize storylet bank...
	std::ifstream f("Storylets.json");
	nlohmann::json corpus = nlohmann::json::parse(f);

	for (auto letter : corpus.items())
	{
		/*if (!corpus[letter.key()].contains("productions"))
			continue;

		for (std::string production : corpus[letter.key()]["productions"])
		{
			//m_sentence += production;
			ProductionRuleType productionRule;
			productionRule.production = letter.key();
			productionRule.weight = (corpus[letter.key()].contains("weight")) ? corpus[letter.key()]["weight"] : 1.0f;
			productionRule.recency = 0;

			AddProductionRule(production, productionRule);
		}*/
	}
}