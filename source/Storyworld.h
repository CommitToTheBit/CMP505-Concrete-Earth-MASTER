#pragma once

#include <map>
#include <string>
#include <vector>

struct StoryWorld
{
	struct StoryCharacter
	{
		std::map<std::string, std::string> traits;
	};

	std::vector<StoryCharacter> passenger;
	StoryCharacter active, passive;
};

