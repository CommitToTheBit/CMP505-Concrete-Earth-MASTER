#include "pch.h"
#include "Storylet.h"

Storylet::Text::Text()
{
	axiom = "";

	active = nullptr;
	passive = nullptr;
}

Storylet::Text::~Text()
{

}

Storylet::Storylet()
{
	beginning = Text();
}