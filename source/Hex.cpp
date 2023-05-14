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

void Hex::InitializeThorn(ID3D11Device* device, Field* heightField, float isolevel)
{
	m_suit = "thorn";

	Field* hexField = new Field();
	hexField->Initialise(heightField);

	hexField->DeriveHexPrism(isolevel);

	for (int i = 0; i < 3; i++)
	{
		float angle = XM_PIDIV2/10.0f;

		float rBaseRange = 0.5f*cos(XM_PI/6.0f);
		float rBase = rBaseRange*sqrt(pow(((float)std::rand()/RAND_MAX), 2.0f)+pow(((float)std::rand()/RAND_MAX), 2.0f));
		float thetaBase = XM_2PI*std::rand()/RAND_MAX;

		DirectX::SimpleMath::Vector3 base = DirectX::SimpleMath::Vector3(0.5f+0.5f*rBase*cos(thetaBase), 0.0f, 0.5f+0.5f*rBase*sin(thetaBase));

		float rOriginRange = cos(XM_PI/6.0f);
		float rOrigin = rOriginRange*(0.5f+0.5f*sqrt(pow(((float)std::rand()/RAND_MAX), 2.0f)+pow(((float)std::rand()/RAND_MAX), 2.0f)));
		float thetaOrigin = thetaBase + 2.0f*((XM_PI/3.0f)*std::rand()/RAND_MAX);

		DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3(0.5f+0.5f*rOrigin*cos(thetaOrigin), 0.4f+isolevel, 0.5f+0.5f*rBase*sin(thetaOrigin));

		hexField->IntegrateHorizontalThorn(origin, base, angle, isolevel);
	}

	__super::Initialize(device, hexField->m_cells, hexField->m_field, isolevel);

	delete hexField;
}

void Hex::InitializeShrapnel(ID3D11Device* device, Field* heightField, float isolevel)
{
	m_suit = "shrapnel";

	Field* hexField = new Field();
	hexField->Initialise(heightField);

	hexField->DeriveHexPrism(isolevel);

	hexField->IntegrateShrapnel(DirectX::SimpleMath::Vector3(0.5f, 0.4f, 0.5f), DirectX::SimpleMath::Vector3(2.0f*((float)std::rand()/RAND_MAX)-1.0f, 0.01f+((float)std::rand()/RAND_MAX), 2.0f*((float)std::rand()/RAND_MAX)-1.0f), 30.0f*(DirectX::XM_PI/180.0f)*((float)std::rand()/RAND_MAX), DirectX::SimpleMath::Vector3(0.3f, 0.4f, 0.2f), isolevel);
	hexField->IntegrateShrapnel(DirectX::SimpleMath::Vector3(0.5f, 0.4f, 0.5f), DirectX::SimpleMath::Vector3(2.0f*((float)std::rand()/RAND_MAX)-1.0f, 0.01f+((float)std::rand()/RAND_MAX), 2.0f*((float)std::rand()/RAND_MAX)-1.0f), 30.0f*(DirectX::XM_PI/180.0f)*((float)std::rand()/RAND_MAX), DirectX::SimpleMath::Vector3(0.3f, 0.4f, 0.2f), isolevel);
	hexField->IntegrateShrapnel(DirectX::SimpleMath::Vector3(0.5f, 0.4f, 0.5f), DirectX::SimpleMath::Vector3(2.0f*((float)std::rand()/RAND_MAX)-1.0f, 0.01f+((float)std::rand()/RAND_MAX), 2.0f*((float)std::rand()/RAND_MAX)-1.0f), 30.0f*(DirectX::XM_PI/180.0f)*((float)std::rand()/RAND_MAX), DirectX::SimpleMath::Vector3(0.3f, 0.4f, 0.2f), isolevel);

	//hexField->IntegrateShrapnel(DirectX::SimpleMath::Vector3(0.35f, 0.4f, 0.4f), DirectX::SimpleMath::Vector3(2.0f*((float)std::rand()/RAND_MAX)-1.0f, 0.01f+((float)std::rand()/RAND_MAX), 2.0f*((float)std::rand()/RAND_MAX)-1.0f), 60.0f*(DirectX::XM_PI/180.0f)*((float)std::rand()/RAND_MAX), DirectX::SimpleMath::Vector3(0.15f, 0.2f, 0.1f), isolevel);
	//hexField->IntegrateShrapnel(DirectX::SimpleMath::Vector3(0.65f, 0.6f, 0.6f), DirectX::SimpleMath::Vector3(2.0f*((float)std::rand()/RAND_MAX)-1.0f, 0.01f+((float)std::rand()/RAND_MAX), 2.0f*((float)std::rand()/RAND_MAX)-1.0f), 60.0f*(DirectX::XM_PI/180.0f)*((float)std::rand()/RAND_MAX), DirectX::SimpleMath::Vector3(0.15f, 0.2f, 0.1f), isolevel);


	__super::Initialize(device, hexField->m_cells, hexField->m_field, isolevel);

	delete hexField;
}