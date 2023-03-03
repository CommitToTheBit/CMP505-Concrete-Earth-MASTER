//
// Game.h
//
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "RenderTexture.h"

#include "HexBoard.h"

#include "Camera.h"
#include "EnvironmentCamera.h"
#include "SpecimenShader.h"

#include "Shader.h"
#include "LightShader.h"
#include "SkyboxShader.h"
#include "RefractionShader.h"
#include "GlassShader.h"
#include "AlphaShader.h"
#include "OverlayShader.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);
#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    // Properties
    void GetDefaultSize( int& width, int& height ) const;
	
private:

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	}; 

    void Update(DX::StepTimer const& timer);

    void Render();

    // Rendering models
    void RenderSkyboxOnto(Camera* camera);

    // Render passes
    void RenderStaticTextures();
    void RenderDynamicTextures();
    void RenderShaderTexture(RenderTexture* renderPass, Shader rendering);

    void Clear();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
    float                                   m_time;
    bool                                    m_preRendered;

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;	
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	// Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//lights
	Light																	m_Light;
    DirectX::SimpleMath::Vector4                                            m_Ambience;

	//Cameras
	Camera																	m_Camera;
    EnvironmentCamera                                                       m_environmentCamera;

	//textures
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_normalMap;

	//Shaders
	LightShader																m_LightShaderPair;
    SkyboxShader                                                            m_SkyboxShaderPair;
    SpecimenShader                                                          m_SpecimenShaderPair;
    RefractionShader                                                        m_RefractionShaderPair;
    GlassShader                                                             m_GlassShaderPair;
    AlphaShader                                                             m_AlphaShaderPair;
    OverlayShader                                                           m_OverlayShaderPair;

    Shader                                                                  m_FieldRendering;

    // Models
    HexBoard                                                                m_HexBoard;
    int                                                                     m_add; // DEBUG...

    ModelClass																m_Cube;

	// Generated Textures
    RenderTexture*                                                          m_SkyboxRenderPass[6];
    Shader                                                                  m_SkyboxRendering[6];

    RenderTexture*                                                          m_NeutralRenderPass;
    Shader                                                                  m_NeutralRendering;

    RenderTexture*                                                          m_NeutralNMRenderPass;
    Shader                                                                  m_NeutralNMRendering;

    RenderTexture*                                                          m_DemoRenderPass;
    Shader                                                                  m_DemoRendering;

    RenderTexture*                                                          m_DemoNMRenderPass;
    Shader                                                                  m_DemoNMRendering;

    RenderTexture*                                                          m_SphericalPoresRenderPass;
    Shader                                                                  m_SphericalPoresRendering;

    RenderTexture*                                                          m_SphericalPoresNMRenderPass;
    Shader                                                                  m_SphericalPoresNMRendering;


#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif
    

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;
};