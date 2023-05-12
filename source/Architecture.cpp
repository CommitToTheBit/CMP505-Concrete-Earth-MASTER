#include "pch.h"
#include "Architecture.h"

Architecture::Architecture()
{
	m_seed = 0;
}


Architecture::~Architecture()
{

}

void Architecture::Initialize(StoryWorld* world, float seed)
{
	m_world = world;
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

Storylet Architecture::SelectBeginning()
{
	srand(m_seed); // FIXME: Hacky, but a good patch in lieu of a better rng implementation?
	m_seed = 2.0f*(std::rand()-RAND_MAX/2)+std::rand()/RAND_MAX;

	if (m_storylets.size() == 0)
		return Storylet();

	// FIXME: Add selection criteria here...

	return m_storylets[rand()%m_storylets.size()];
}

void Architecture::SelectMiddle(Storylet* storylet)
{
	srand(m_seed); // FIXME: Hacky, but a good patch in lieu of a better rng implementation?
	m_seed = 2.0f*(std::rand()-RAND_MAX/2)+std::rand()/RAND_MAX;

	// FIXME: Add selection criteria here...

	while (storylet->middle.size() > 3) // NB: Would this maximum ever be a variable?
		storylet->middle.erase(storylet->middle.begin() + rand()%storylet->middle.size());
}

void Architecture::SelectEnd(Storylet* storylet, int choice)
{
	srand(m_seed); // FIXME: Hacky, but a good patch in lieu of a better rng implementation?
	m_seed = 2.0f*(std::rand()-RAND_MAX/2)+std::rand()/RAND_MAX;

	// FIXME: Add selection criteria here...

	while (storylet->end[choice].size() > 1) // NB: Would this maximum ever be a variable?
		storylet->end[choice].pop_back();
}