#pragma once

#include "StoryWorld.h"

#include <map>
#include <string>
#include <vector>

struct Storylet
{
	struct Causes
	{

	};

	struct Effects
	{

	};

	struct Text
	{
		std::string axiom;
		Causes causes;
		Effects effects;

		StoryWorld::StoryCharacter* active;
		StoryWorld::StoryCharacter* passive;

		Text();
		~Text();
	};

	Text beginning;
	std::vector<Text> middle;
	std::vector<std::vector<Text>> end;
	bool progressed;

	Storylet();
};

