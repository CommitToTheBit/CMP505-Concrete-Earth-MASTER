#include "pch.h"
#include "Hex.h"

Hex::Hex()
{

}

Hex::~Hex()
{

}

void Hex::InitializeSalt(ID3D11Device* device, Field* heightField, float isolevel)
{
	m_suit = "salt";

	Field* hexField = new Field();
	hexField->Initialise(heightField);

	hexField->DeriveHexPrism(isolevel);

	__super::Initialize(device, hexField->m_cells, hexField->m_field, isolevel);

	delete hexField;
}