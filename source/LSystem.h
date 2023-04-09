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

	// Tree vertex information, used for 'structuring' turtle drawings...
	struct TreeVertexType
	{
		int parent;
		int degree;
		DirectX::SimpleMath::Matrix transform;
		DirectX::SimpleMath::Vector3 position;
		float radius;
	};

	// Scale vertex information, used for normalising turtle drawings...
	struct ScaleVertexType
	{
		int parent;
		DirectX::SimpleMath::Matrix transform;
		DirectX::SimpleMath::Vector3 position;
		float simplex;
	};

public:
	struct ModuleType
	{
		std::string letter;
		float length;
		float rotation;
		float width;
	};

	struct ProductionRuleType
	{
		std::vector<std::function<ModuleType(ModuleType)>> productions;
		float weight;
	};

public:
	LSystem();
	~LSystem();

	bool Initialize(ID3D11Device*);
	void Render(ID3D11DeviceContext*);
	void Shutdown();

	void InitializeProductionRule(std::string letter, ProductionRuleType productionRule);
	void InitializeSentence(std::vector<ModuleType> axiom, int iterations);
	void InitializeTree();

	void Update(ID3D11Device*, float deltaTime, float deltaIntensity); // NB: Needs edited!

	// DEPRECATED:
	//void InitializeScale(float seed, float width, float rotation);

	//void Update(ID3D11Device*, float deltaTime, float deltaIntensity);

	// DEBUG:
	float* GetIntensity();
	std::string GetSentence();

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void ShutdownBuffers();

	void UpdateTree(float deltaTime, float intensity);
	void DrawTree();

	ProductionRuleType GetProductionRule(std::string letter);

private:
	std::map<std::string, std::vector<ProductionRuleType>> m_productionRules;
	std::vector<ModuleType> m_sentence;

	std::function<ModuleType()> m_moduleTransform;

	std::map<std::string, float> m_rotationRules;
	std::map<std::string, float> m_rotationRandomness;

	float m_seed, m_length, m_width, m_rotation;
	std::vector<ScaleVertexType> m_scaledVertices;

	float m_time, m_intensity;
	std::vector<TreeVertexType> m_treeVertices;

protected:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

