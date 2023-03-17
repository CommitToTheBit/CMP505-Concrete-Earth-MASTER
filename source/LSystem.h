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

public:
	LSystem();
	~LSystem();

	bool Initialize(ID3D11Device*);
	void InitializeProductionRule(std::string A, std::vector<std::string> alpha);
	void InitializeSentence(std::vector<std::string> S, int iterations);

	void Render(ID3D11DeviceContext*);
	void Shutdown();

	// DEBUG:
	std::string GetSentence();

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void ShutdownBuffers();

	std::vector<std::string> GetProductionRule(std::string A);

private:
	std::map<std::string, std::vector<std::vector<std::string>>> productionRules;
	std::map<std::string, std::vector<std::vector<std::string>>> productionWeights;
	std::vector<std::string> sentence;

protected:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

