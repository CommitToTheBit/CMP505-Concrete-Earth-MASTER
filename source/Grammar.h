#pragma once

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <vector>
#include <functional>

class Grammar
{

private:
	struct ProductionRuleType
	{
		std::string production;
		std::function<float(std::string)> context; // FIXME: Expand arguments of context function!
		float weight;
	};

public:
	Grammar();
	~Grammar();

	void Initialize(std::string jsonPath);

private:
	//nlohmann::json m_alphabet;
	std::map<std::string, std::vector<ProductionRuleType>> m_productionRules;
};

