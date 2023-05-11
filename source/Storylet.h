#pragma once

#include <map>
#include <string>
#include <vector>

struct Storylet
{
	struct Ifs
	{

	};

	struct Thens
	{

	};

	struct Beat
	{
		std::string axiom;
		Ifs ifs;
		Thens thens;
	};
};

