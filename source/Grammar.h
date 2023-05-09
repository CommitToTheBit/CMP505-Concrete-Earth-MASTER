#pragma once

#include "Storyworld.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

class Grammar
{

private:
	struct ProductionRuleType
	{
		std::string production;
		
		//std::function<float(std::string)> context; // FIXME: Expand arguments of context function!

		int dryness;
		float weight;
	};

public:
	Grammar();
	~Grammar();

	void Initialize(std::string jsonPath, float seed = 0.0f);

	// DEBUG:
	std::string GenerateSentence(std::string axiom, Storyworld::StoryCharacter* character = nullptr); // NB: Can equally apply this to character initialisation!

private:
	void AddProductionRule(std::string letter, ProductionRuleType productionRule);

	ProductionRuleType Grammar::GetProductionRule(std::string letter, bool generation = true);
	float GetWeight(ProductionRuleType productionRule, std::string letter);
	float GetRNGRange(float a = -1.0f, float b = 1.0f);

private:
	std::map<std::string, std::vector<ProductionRuleType>> m_productionRules;

	float m_seed;
	int m_generations;

// DEBUG: These variables will later be handled in other classes...
public:
	std::string m_sentence;
	Storyworld::StoryCharacter m_character;
};

