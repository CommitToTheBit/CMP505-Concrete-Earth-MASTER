//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	//setup light
	m_Ambience = Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light.setAmbientColour(m_Ambience.x, m_Ambience.y, m_Ambience.z, m_Ambience.w);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(0.0f, 2.5f, 2.0f*m_HexBoard.m_hexRadius-1);
	m_Light.setDirection(1.0f, 1.0f, 0.0f);
	m_Light.setStrength(100.0f);

	//setup camera
	//m_Camera.setPosition(Vector3(2.4f+0.75*cos(atan(-1.8/2.4)), 0.0f, 1.8f+0.75*sin(atan(-1.8/2.4))));
	//m_Camera.setRotation(Vector3(-90.0f, -180+(180.0/3.14159265)*atan(2.4/1.8), 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up.

	// FIXME: Refactor this, for 'cleaner' board set-up?
	float twist = XM_PI/12.0f;
	m_Camera.setPosition(7.0f*Vector3(cos(1.0f*XM_PI/5.0f)*sin(twist), sin(1.0f*XM_PI/5.0f), cos(1.0f*XM_PI/5.0f))*cos(twist));
	m_Camera.setRotation(Vector3(-90.0f-36.0f, -180.0f+180.0f*twist/XM_PI, 0.0f));

	
#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

	
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	m_time = m_timer.GetTotalSeconds();

	//note that currently.  Delta-time is not considered in the game object movement. 
	Vector3 inputPosition = Vector3(0.0f, 0.0f, 0.0f);

	// STEP 1: Read camera translation inputs (from keyboard) // FIXME: Refactor this, for 'cleaner' board set-up?
	/*if (m_gameInputCommands.forward)
		inputPosition.z += 1.0f;
	if (m_gameInputCommands.back)
		inputPosition.z -= 1.0f;
	if (m_gameInputCommands.right)
		inputPosition.x += 1.0f;
	if (m_gameInputCommands.left)
		inputPosition.x -= 1.0f;
	if (m_gameInputCommands.up)
		inputPosition.y += 1.0f;
	if (m_gameInputCommands.down)
		inputPosition.y -= 1.0f;

	if (inputPosition.x != 0.0f || inputPosition.y != 0.0f || inputPosition.z != 0.0f)
	{
		inputPosition.Normalize();
		inputPosition.x *= 0.8f;
		inputPosition.y *= 0.8f;

		// NB: forward/right directions are relative to the camera, but up remains relative to the space to avoid confusion...
		Vector3 deltaPosition = inputPosition.z*m_Camera.getForward() + inputPosition.x*m_Camera.getRight() + inputPosition.y*Vector3::UnitY;
		deltaPosition *= m_timer.GetElapsedSeconds()*m_Camera.getMoveSpeed();

		Vector3 position = m_Camera.getPosition()+deltaPosition;
		m_Camera.setPosition(position);
	}

	// STEP 2: Read camera rotation inputs (from mouse)
	Vector2 inputRotation = m_gameInputCommands.rotation;
	if (inputRotation.x != 0.0f || inputRotation.y != 0.0f)
	{
		Vector3 rotation = m_Camera.getRotation();
		inputRotation.x *= sin(XM_PIDIV4+0.5*XM_PI*(-rotation.x/180.0f));

		Vector3 deltaRotation = (-inputRotation.y)*Vector3::UnitX + (-inputRotation.x)*Vector3::UnitY;
		deltaRotation *= m_timer.GetElapsedSeconds()*m_Camera.getRotationSpeed();

		rotation += deltaRotation;
		rotation.x = std::min(-0.001f, std::max(rotation.x, -179.999f)); // NB: Prevents gimbal lock
		m_Camera.setRotation(rotation);
	}*/

	// STEP 3: Process inputs
	m_Camera.Update();
	//m_Light.setPosition(4.0f*cos(XM_2PI*m_time/60.0f), 1.0f, 4.0f*sin(XM_2PI*m_time/60.0f)); // NB: Modelling a day/night cycle... so far, very limited...
	
	// DEBUG:
	auto device = m_deviceResources->GetD3DDevice();
	//m_HexBoard.m_hexModels[0].GenerateIsosurface(device, 0.5f+0.25f*sin(m_time/(XM_2PI*5.0f)));
	//m_HexBoard.m_hexModels[0].InitialiseHorizontalField();
	//m_HexBoard.m_hexModels[0].DeriveHexPrism(device, 0.5f+0.25f*sin(XM_2PI*m_time/5.0f));
	if (m_HexBoard.m_interpolating)
	{
		m_HexBoard.Interpolate(2.0f*timer.GetElapsedSeconds());
	}
	if (!m_HexBoard.m_interpolating) // NB: Not an 'if/else', since this would waste a frame! 
	{
		if (m_gameInputCommands.forward)
			m_HexBoard.SetInterpolation(1, 0);
		if (m_gameInputCommands.left)
			m_HexBoard.SetInterpolation(1, -1);
		if (m_gameInputCommands.right)
			m_HexBoard.SetInterpolation(1, 1);
		if (m_gameInputCommands.back)
		//	m_HexBoard.SetInterpolation(-1, 0);
		//	m_HexBoard.SetInterpolation(-1, 1);
		//	m_HexBoard.SetInterpolation(-1, -1);
			m_HexBoard.AddThorns(device, m_add++, 3);
	}

	m_view = m_Camera.getCameraMatrix();
	m_projection = m_Camera.getPerspective();
	m_world = Matrix::Identity;

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

  
	if (m_input.Quit())
	{
		ExitGame();
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{	
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();

    // Draw Text to the screen
    m_sprites->Begin();
		m_font->DrawString(m_sprites.get(), L"", XMFLOAT2(10, 10), Colors::White);
    m_sprites->End();

	//Set Rendering states. 
	//context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	//context->OMSetBlendState(m_states->Additive(), nullptr, 0xFFFFFFFF); // NB: Which blend is best? Is it most efficient to just set this here?
	context->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
//	context->RSSetState(m_states->Wireframe());

	// If m_time == 0.0, then render all static textures (once only!)
	if (!m_preRendered)
	{
		RenderStaticTextures();

		m_preRendered = true;
	}

	// STEP 1: Run render to textures...
	// First render pass: Rendering any textures (including normal maps, etc...)
	//RenderDynamicTextures();

	// STEP 2: Render 'real' scene...
	// Draw Skybox
	RenderSkyboxOnto(&m_Camera);

	// Drawn DEBUG Cube...
	m_NeutralShader.EnableShader(context);
	m_NeutralShader.SetMatrixBuffer(context, &(Matrix)Matrix::Identity, &(Matrix)Matrix::Identity, &Matrix::CreateScale(1080.0f/1920.0f,1.0f,1.0f), true);
	m_lSystem.Render(device,context, m_time);

	// Draw board
	DirectX::SimpleMath::Vector3 displacement = Vector3(0.0f, -0.5f, 0.0f);// DirectX::SimpleMath::Vector3(2.5f, 1.0f*sin(1.0f*XM_PI/5.0f), 0.0f);
	m_HexBoard.Render(context, &m_FieldRendering, displacement, &m_Camera, m_time, &m_Light);

	// Draw Text to the screen
	m_sprites->Begin();
	m_font->DrawString(m_sprites.get(), m_lSystem.GetSentence().c_str(), XMFLOAT2(10, 10), Colors::White);
	m_sprites->End();

    // Show the new frame.
    m_deviceResources->Present();
}

// Rendering Models
void Game::RenderSkyboxOnto(Camera* camera)
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();

	//ID3D11ShaderResourceView* environmentMap[6];
	//for (int j = 0; j < 6; j++)
	//	environmentMap[j] = m_SkyboxRenderPass[j]->getShaderResourceView();
		
	context->OMSetDepthStencilState(m_states->DepthNone(), 0); // NB: Note use of DepthNone()
	context->RSSetState(m_states->CullCounterClockwise());
	
	// FIXME: Skybox out of commission, for now... 
	//m_SkyboxShaderPair.EnableShader(context);
	//m_SkyboxShaderPair.SetSkyboxShaderParameters(context, &Matrix::CreateTranslation(camera->getPosition()), &camera->getCameraMatrix(), &camera->getPerspective(), false, m_time, environmentMap); // FIXME: Flat normal map here... but holes when viewed through glass??
	//m_Cube.Render(context);

	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
}

// Render Passes
void Game::RenderStaticTextures()
{

}

void Game::RenderDynamicTextures()
{

}

void Game::RenderShaderTexture(RenderTexture* renderPass, Shader rendering)
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();

	renderPass->setRenderTarget(context);
	renderPass->clearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);
	rendering.EnableShader(context);
	// FIXME: Out of commission...
	/*rendering.SetShaderParameters(
		context,
		&SimpleMath::Matrix::CreateScale(2.0f),
		&(Matrix)Matrix::Identity,
		&(Matrix)Matrix::Identity,
		true,
		m_time);
	m_Cube.Render(context);*/
	context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::Black);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 1920;
    height = 1080;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	// Board
	m_HexBoard.Initialize(device, 4, 32);
	m_add = 0;

	// L-Systems
	m_lSystem.InitializeProductionRule("A", std::vector<std::string>{"B", "[", "+", "A", "]", "-", "A"});
	m_lSystem.InitializeProductionRule("B", std::vector<std::string>{"B", "B"});
	m_lSystem.InitializeSentence(std::vector<std::string>{"A"}, 5);
	m_lSystem.Initialize(device);

	// Models
	m_Cube.InitializeModel(device, "cube.obj");

	// Shaders
	m_FieldRendering.InitShader(device, L"light3D_vs.cso", L"light3D_ps.cso");
	m_FieldRendering.InitMatrixBuffer(device);
	m_FieldRendering.InitAlphaBuffer(device);
	m_FieldRendering.InitLightBuffer(device);

	m_NeutralShader.InitShader(device, L"neutral_vs.cso", L"neutral_ps.cso");
	m_NeutralShader.InitMatrixBuffer(device);

	//load Textures
	CreateDDSTextureFromFile(device, L"sample_nm.dds", nullptr,	m_normalMap.ReleaseAndGetAddressOf());

	//Initialise Render to texture

	m_preRendered = false;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 50.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    //if (aspectRatio < 1.0f)
    //{
    //    fovAngleY *= 2.0f;
    //}

    // This sample makes use of a right-handed coordinate system using row-major matrices.
	m_Camera.setPerspective(fovAngleY, aspectRatio, 0.01f, 100.0f);
}


void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
