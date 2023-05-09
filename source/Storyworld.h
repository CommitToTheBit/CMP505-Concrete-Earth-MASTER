#pragma once

#include <map>
#include <string>

class Storyworld
{
public:
	struct StoryCharacter
	{
		std::map<std::string, std::string> m_traits;
	};
};

