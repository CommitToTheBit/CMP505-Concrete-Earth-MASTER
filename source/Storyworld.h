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

	struct StoryWagon
	{
		std::vector<StoryCharacter> characters;
	};
};

