#include "pch.h"
#include "Shader.h"


Shader::Shader()
{
}


Shader::~Shader()
{
}

bool Shader::InitShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename)
{
	//LOAD SHADER:	VERTEX
	auto vertexShaderBuffer = DX::ReadData(vsFilename);
	HRESULT result = device->CreateVertexShader(vertexShaderBuffer.data(), vertexShaderBuffer.size(), NULL, &m_vertexShader);
	if (result != S_OK)
	{
		//if loading failed.  
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the MeshClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Get a count of the elements in the layout.
	unsigned int numElements;
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer.data(), vertexShaderBuffer.size(), &m_layout);


	//LOAD SHADER:	PIXEL
	auto pixelShaderBuffer = DX::ReadData(psFilename);
	result = device->CreatePixelShader(pixelShaderBuffer.data(), pixelShaderBuffer.size(), NULL, &m_pixelShader);
	if (result != S_OK)
	{
		//if loading failed. 
		return false;
	}

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	device->CreateSamplerState(&samplerDesc, &m_sampleState);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC timeBufferDesc;
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	device->CreateBuffer(&timeBufferDesc, NULL, &m_timeBuffer);

	return true;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, bool culling, float time)
{ 
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = world->Transpose(); 	// Transpose the matrices to prepare them for the shader.
	dataPtr->view = view->Transpose();
	dataPtr->projection = projection->Transpose();
	dataPtr->culling = culling;

	context->Unmap(m_matrixBuffer, 0);
	context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);	//note the first variable is the mapped buffer ID.  Corresponding to what you set in the VS;

	context->Map(m_timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	TimeBufferType* timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = time;
	timePtr->padding = DirectX::SimpleMath::Vector3(0.0, 0.0, 0.0);
	context->Unmap(m_timeBuffer, 0);
	context->PSSetConstantBuffers(0, 1, &m_timeBuffer);	//note the first variable is the mapped buffer ID.  Corresponding to what you set in the PS

	timePtr->time = time;
	timePtr->padding = DirectX::SimpleMath::Vector3(0.0, 0.0, 0.0);

	return false;
}

void Shader::EnableShader(ID3D11DeviceContext* context)
{
	context->IASetInputLayout(m_layout);							//set the input layout for the shader to match out geometry
	context->VSSetShader(m_vertexShader.Get(), 0, 0);				//turn on vertex shader
	context->PSSetShader(m_pixelShader.Get(), 0, 0);				//turn on pixel shader
	context->PSSetSamplers(0, 1, &m_sampleState);					// Set the sampler state in the pixel shader.

}

bool Shader::InitAlphaBuffer(ID3D11Device* device)
{
	// Setup Alpha buffer
	// Setup the description of the Alpha dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC alphaBufferDesc;
	alphaBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	alphaBufferDesc.ByteWidth = sizeof(AlphaBufferType);
	alphaBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	alphaBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	alphaBufferDesc.MiscFlags = 0;
	alphaBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	device->CreateBuffer(&alphaBufferDesc, NULL, &m_alphaBuffer);

	return true;
}

bool Shader::SetAlphaBufferParameters(ID3D11DeviceContext* context, float alpha)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(m_alphaBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	AlphaBufferType* alphaPtr = (AlphaBufferType*)mappedResource.pData;
	alphaPtr->alpha = alpha;
	context->Unmap(m_alphaBuffer, 0);
	context->PSSetConstantBuffers(1, 1, &m_alphaBuffer);	//note the first variable is the mapped buffer ID.  Corresponding to what you set in the PS

	return false;
}

bool Shader::InitLightBuffer(ID3D11Device* device)
{
	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);

	return true;
}

bool Shader::SetLightBufferParameters(ID3D11DeviceContext* context, Light* light)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	LightBufferType* lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient = light->getAmbientColour();
	lightPtr->diffuse = light->getDiffuseColour();
	lightPtr->position = light->getPosition();
	lightPtr->strength = light->getStrength();
	context->Unmap(m_lightBuffer, 0);
	context->PSSetConstantBuffers(2, 1, &m_lightBuffer);	//note the first variable is the mapped buffer ID.  Corresponding to what you set in the PS

	return false;
}

bool Shader::SetShaderTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, int vsStartSlot, int psStartSlot) // FIXME: Is there a way of just setting to the next available slot?
{
	//pass the desired texture to the vertex shader.
	if (vsStartSlot >= 0)
		context->VSSetShaderResources(vsStartSlot, 1, &texture);

	//pass the desired texture to the pixel shader.
	if (psStartSlot >= 0)
		context->VSSetShaderResources(psStartSlot, 1, &texture);

	return false;
}