#pragma once

#include <map>
#include <string>
#include <vector>

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
		int depth;

		DirectX::SimpleMath::Matrix transform;
		DirectX::SimpleMath::Vector3 position;
		float radius;
	};

public:
	LSystem();
	~LSystem();

	bool Initialize(ID3D11Device*);
	void InitializeProductionRule(std::string A, std::vector<std::string> alpha);
	void InitializeSentence(std::vector<std::string> S, int iterations);

	void Render(ID3D11DeviceContext*);
	void Shutdown();

	void Update(ID3D11Device*, float time);

	// DEBUG:
	std::string GetSentence();

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void ShutdownBuffers();

	void UpdateTree(float time);
	void DrawTree();

	std::vector<std::string> GetProductionRule(std::string A);

private:
	std::map<std::string, std::vector<std::vector<std::string>>> m_productionRules;
	std::map<std::string, std::vector<std::vector<std::string>>> m_productionWeights;
	std::vector<std::string> m_sentence;

	std::vector<TreeVertexType> m_treeVertices;
	std::map<int, std::vector<int>> m_treeEdges;

protected:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

