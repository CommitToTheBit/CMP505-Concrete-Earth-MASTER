#pragma once
#include "ClassicNoise.h"
#include "SimplexNoise.h"

#include <map>
#include <string>
#include <vector>
#include <functional>

class LSystem
{
protected:
	// General vertex information, used by shaders...
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector3 tangent;
		DirectX::SimpleMath::Vector3 binormal;
	};

	// Seed vertex information, used for normalising turtle drawings...
	struct SeedVertexType
	{
		int parent;
		DirectX::SimpleMath::Matrix transform;
		DirectX::SimpleMath::Vector3 position;
		float simplex;
	};

	// Tree vertex information, used for 'structuring' turtle drawings...
	struct TreeVertexType
	{
		int parent;
		int degree;
		DirectX::SimpleMath::Matrix transform;
		DirectX::SimpleMath::Vector3 position;
		float radius;
	};

public:
	struct LModuleType
	{
		std::string letter;

		float length;
		float rotation;
		float width;
		// FIXME: Add 'growth' terms!

		float period;
		float aperiodicity;

		float synchronisation;
		float asynchronicity;

		float randomLength;
		float randomRotation;
	};

	struct ProductionRuleType
	{
		std::vector<std::function<LModuleType(LModuleType)>> productions;
		float weight;
	};

public:
	LSystem();
	~LSystem();

	bool Initialize(ID3D11Device*, std::vector<LModuleType> axiom, int iterations, DirectX::SimpleMath::Vector2 anchoring = DirectX::SimpleMath::Vector2(0.5f, 0.5f), float seed = 0.0f);
	void Render(ID3D11DeviceContext*);
	void Shutdown();

	void Update(ID3D11Device*, float deltaTime, float deltaIntensity);

	void AddProductionRule(std::string letter, ProductionRuleType productionRule);

	// DEBUG:
	float* GetIntensity();
	std::string GetSentence();

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void ShutdownBuffers();

	void InitializeSentence(std::vector<LModuleType> axiom, int iterations);
	void InitializeTree(DirectX::SimpleMath::Vector2 anchoring, float seed);
	void UpdateTree(float deltaTime, float intensity);

	ProductionRuleType GetProductionRule(std::string letter);
	float GetRNGRange(float a = -1.0f, float b = 1.0f);

private:
	std::map<std::string, std::vector<ProductionRuleType>> m_productionRules;
	std::vector<LModuleType> m_sentence;

	float m_seed, m_scale;
	std::vector<SeedVertexType> m_seedVertices;

	float m_time, m_intensity;
	std::vector<TreeVertexType> m_treeVertices;

protected:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

