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

void Grammar::Initialize(float seed)
{
	m_seed = seed;
	m_generations = 0;

	m_productionRules = std::map<std::string, std::vector<ProductionRuleType>>();
	InitializeCorpus("CorpusPronouns.json");
	InitializeCorpus("CorpusMisc.json");
}

void Grammar::InitializeCorpus(std::string jsonPath)
{
	std::ifstream f(jsonPath);
	nlohmann::json corpus = nlohmann::json::parse(f);

	for (auto letter : corpus.items())
	{
		if (!corpus[letter.key()].contains("productions"))
			continue;

		for (std::string production : corpus[letter.key()]["productions"])
		{
			//m_sentence += production;
			ProductionRuleType productionRule;
			productionRule.production = letter.key();
			productionRule.weight = (corpus[letter.key()].contains("weight")) ? corpus[letter.key()]["weight"] : 1.0f;
			productionRule.recency = 0;

			AddProductionRule(production, productionRule);
		}
	}
}

std::string Grammar::GenerateSentence(std::string axiom, Storyworld::StoryCharacter* active, Storyworld::StoryCharacter* passive, bool nested)
{
	srand(m_seed); // FIXME: Hacky, but a good patch in lieu of a better rng implementation?
	m_seed = 2.0f*(std::rand()-RAND_MAX/2)+std::rand()/RAND_MAX;

	if (!nested)
		m_generations++;

	// FIXME: Generalise this to include nested brackets...
	std::string sentence = axiom;
	std::string iteratedSentence, nestedSentence;
	while (sentence.find("{") != -1)
	{
		iteratedSentence = "";
		while (sentence.find("{") != -1)
		{
			// FIXME: HANDLE NESTED BRACKETS!
			// NB: Will need to capitalize anything nested...

			iteratedSentence += sentence.substr(0, sentence.find("{"));
			sentence.erase(sentence.begin(), sentence.begin() + sentence.find("{"));

			int index = FindClosingBracket(sentence);
			if (index < 1)
				continue;

			sentence.erase(sentence.begin(), sentence.begin() + 1); // NB: Must wait for FindClosingBracket call to erase opening bracket...
			index--;

			nestedSentence = GenerateSentence(sentence.substr(0, index), active, passive, true); // NB: Recursive calls like this aren't ideal, but there'll never be too many nested brackets at once...

			if (nestedSentence.find("*") == 0)
				iteratedSentence += (nestedSentence.find("*", 1) == 1) ? GetProductionRule(nestedSentence.substr(2), passive) : GetProductionRule(nestedSentence.substr(1), active);
			else
				iteratedSentence += GetProductionRule(nestedSentence); // NB: nullptr passed in as 'forgetfulness override'...

			sentence.erase(sentence.begin(), sentence.begin() + index + 1);
		}
		iteratedSentence += sentence; // NB: Adds the remainder of the sentence, which doesn't need parsed...
		sentence = iteratedSentence;
	}

	if (nested)
	{
		for (int i = 0; i < sentence.length(); i++)
			sentence[i] = std::toupper(sentence[i]);

		return sentence;
	}

	return PostProcessSentence(sentence);
}

std::string Grammar::PostProcessSentence(std::string sentence)
{
	// STEP 1: Capitalise sentence...
	// After the start of the sentence and after every full stop, ensure the next character that can be capitalised *is* capitalised...
	// NB: Is "\n" treated as a single character? If so, should be handled without any special case...
	int index = 0;
	while (index < sentence.length() && index != -1)
	{
		if (std::tolower(sentence[index]) == std::toupper(sentence[index]))
		{
			index++;
		}
		else
		{
			sentence[index] = std::toupper(sentence[index]);
			index = sentence.find(".", index + 1);
		}
	}

	// STEP N: Add line breaks...
	// FIXME: Handle this with -1/non-negative int case, with overflow allowed...

	return sentence;
}

void Grammar::AddProductionRule(std::string letter, ProductionRuleType productionRule)
{
	if (!m_productionRules.count(letter))
		m_productionRules.insert({ letter, std::vector<ProductionRuleType>{ productionRule } });
	else
		m_productionRules[letter].push_back(productionRule);
}

std::string Grammar::GetProductionRule(std::string letter, Storyworld::StoryCharacter* character, bool generation)
{
	if (!m_productionRules.count(letter))
		return letter; // DEBUG: "NULL("+letter+")";

	if (character && character->m_traits.find(letter) != character->m_traits.end())
		return character->m_traits[letter];

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

	// Add as consistent feature of world model...
	if (character)
		character->m_traits[letter] = m_productionRules[letter][index].production;

	// Remember this has been used...
	if (generation)
		m_productionRules[letter][index].recency = m_generations;

	return m_productionRules[letter][index].production;
}

float Grammar::GetWeight(ProductionRuleType productionRule, std::string letter)
{
	float weight = productionRule.weight;

	// STEP 1: Scale by recency...
	std::vector<int> generations = std::vector<int>();
	for (ProductionRuleType rule : m_productionRules[letter])
	{
		generations.push_back(rule.recency);
	}
	std::sort(generations.begin(), generations.end());

	float minimumWeighting = 0.01f; // 0.0f // pow(0.01f, generations.size()); // NB: The most recently-used production rule will be minimumWeighting times as likely to be surfaced as the least recent...
	float exponent = ((float)std::distance(generations.begin(), std::find(generations.begin(), generations.end(), productionRule.recency)))/generations.size();// /generations.size();
	weight *= (productionRule.recency < m_generations) ? pow(minimumWeighting, exponent) : 0.0f; // NB: Explicitely blocks the uses of the same production rule (though not the same word!) in one generation (unless there are no other options)...

	return weight;

}

float Grammar::GetRNGRange(float a, float b)
{
	return a+(b-a)*std::rand()/RAND_MAX;
}

int Grammar::FindClosingBracket(std::string sentence)
{
	if (sentence.find("{") != 0)
		return -1;

	int depth = 1;
	int index = 0;
	int openingIndex, closingIndex;
	while (depth > 0)
	{
		openingIndex = sentence.find("{", index+1);
		closingIndex = sentence.find("}", index+1);

		if (closingIndex == -1)
			return closingIndex;

		if (closingIndex < openingIndex || openingIndex == -1)
		{
			index = closingIndex;
			depth--;
		}
		else
		{
			index = openingIndex;
			depth++;
		}
	}

	return index;
}