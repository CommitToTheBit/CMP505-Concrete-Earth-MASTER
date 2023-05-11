#pragma once

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

	int progress;
	Text beginning;
	std::vector<Text> middles;
	std::vector<std::vector<Text>> ends;
};

