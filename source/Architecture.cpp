#include "pch.h"
#include "Architecture.h"

Architecture::Architecture()
{
	m_seed = 0;
}


Architecture::~Architecture()
{

}

void Architecture::Initialize(float seed)
{
	m_seed = seed;

	m_storylets = std::vector<Storylet>();

	// Initialize storylet bank...
	std::ifstream f("Storylets.json");
	nlohmann::json data = nlohmann::json::parse(f);

	for (auto datum : data.items())
	{
		if (!data[datum.key()].contains("Beginning"))
			continue;

		if (!data[datum.key()].contains("Middle") || !data[datum.key()]["Middle"].size() > 0)
			continue;

		if (!data[datum.key()].contains("End")) // FIXME: Add nuance here (and further down)...
			continue;

		Storylet storylet = Storylet();
		storylet.progressed = false;

		storylet.beginning = InitializeText(data[datum.key()]["Beginning"]);

		storylet.middle = std::vector<Storylet::Text>(data[datum.key()]["Middle"].size());
		for (int i = 0; i < data[datum.key()]["Middle"].size(); i++)
			storylet.middle[i] = InitializeText(data[datum.key()]["Middle"][i]);

		storylet.end = std::vector<std::vector<Storylet::Text>>(data[datum.key()]["Middle"].size());
		for (int i = 0; i < data[datum.key()]["Middle"].size(); i++)
		{
			int J = (data[datum.key()]["Middle"][i].contains("End")) ? data[datum.key()]["Middle"][i]["End"].size() : 0;

			storylet.end[i] = std::vector<Storylet::Text>(J + 1);
			for (int j = 0; j < J; j++)
				storylet.end[i][j] = InitializeText(data[datum.key()]["Middle"][i]["End"][j]);
			storylet.end[i][J] = InitializeText(data[datum.key()]["End"]); 
		}

		m_storylets.push_back(storylet);
	}
}

Storylet::Text Architecture::InitializeText(nlohmann::json data)
{
	Storylet::Text text;
	text.axiom = (data.contains("Axiom")) ? data["Axiom"] : "";

	// FIXME: Initialize causes, effects...

	return text;
}
