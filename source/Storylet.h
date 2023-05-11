#pragma once

#include "StoryWorld.h"

#include <map>
#include <string>
#include <vector>

struct Storylet
{
	struct Conditions
	{

	};

	struct Effects
	{

	};

	struct Text
	{
		std::string axiom;
		Conditions conditions;
		Effects effects;
	};

	Text beginning;
	std::vector<Text> middle;
	std::vector<std::vector<Text>> end;
	bool progressed;
};

