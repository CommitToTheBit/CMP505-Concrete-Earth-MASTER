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
using namespace ImGui;

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

	//setup imgui.  its up here cos we need the window handle too
	//pulled from imgui directx11 example
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); // FIXME: Is (void)io; necessary?
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window);		//tie to our window
	ImGui_ImplDX11_Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());	//tie to directx

	m_defaultFont = io.Fonts->AddFontFromFileTTF("beneg___.ttf", 54); // NB: pt-to-px conversion: px = (4.0f/3.0f)*pt
	m_choiceFont = io.Fonts->AddFontFromFileTTF("beneg___.ttf", 42);

	ImGuiStyle& style = ImGui::GetStyle();
	style.ButtonTextAlign = ImVec2(0.0f, 0.5f);
	style.FrameRounding = 12.0f;
	style.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.8f, 0.2f, 0.2f, 0.5f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.2f, 0.2f, 0.75f);

	//setup light
	m_Ambience = Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light.setAmbientColour(m_Ambience.x, m_Ambience.y, m_Ambience.z, m_Ambience.w);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(0.0f, 2.5f, 2.0f*m_Board.m_hexRadius-1);
	m_Light.setDirection(1.0f, 1.0f, 0.0f);
	m_Light.setStrength(100.0f);

	// FIXME: Refactor this, for 'cleaner' board set-up?
	float twist = XM_PI/12.0f;
	m_Camera.setPosition(7.0f*Vector3(cos(1.0f*XM_PI/8.0f)*sin(twist), sin(1.0f*XM_PI/8.0f)-0.15f, cos(1.0f*XM_PI/8.0f))*cos(twist));
	m_Camera.setRotation(Vector3(-90.0f-22.5f, -180.0f+180.0f*twist/XM_PI, 0.0f));
	
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

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"616516__justlaz__geiger_tick_low.wav");
    m_effect1 = m_soundEffect->CreateInstance();

    //m_effect1->Play(true); // DEBUG: Turned off temporarily...
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
	auto device = m_deviceResources->GetD3DDevice();

	m_time = m_timer.GetTotalSeconds();

	// CAMERA INPUTS:
	m_Camera.Update();

	// LIGHTING INPUTS:
	//m_Light.setPosition(4.0f*cos(XM_2PI*m_time/60.0f), 1.0f, 4.0f*sin(XM_2PI*m_time/60.0f)); // NB: Modelling a day/night cycle... so far, very limited...
	
	// Board INPUTS:
	if (m_Board.m_interpolating)
	{
		m_Board.Interpolate(2.0f*timer.GetElapsedSeconds());
	}
	if (!m_Board.m_interpolating && !m_Board.Paused()) // NB: Not an 'if/else', since this would waste a frame! 
	{
		if (m_gameInputCommands.forward)
			m_Board.SetInterpolation(1, 0);
		if (m_gameInputCommands.left)
			m_Board.SetInterpolation(1, -1);
		if (m_gameInputCommands.right)
			m_Board.SetInterpolation(1, 1);
		//if (m_gameInputCommands.back)
		//	m_Board.SetInterpolation(-1, 0);
		//	m_Board.SetInterpolation(-1, 1);
		//	m_Board.SetInterpolation(-1, -1);
		//	m_Board.AddThorns(device, m_add++, 3);
	}

	// VIGNETTE INPUTS:
	if (m_Board.m_interpolating) //m_gameInputCommands.clockwise || m_gameInputCommands.anticlockwise)
	{
		float deltaInterpolation = 1.0f;
		//if (m_gameInputCommands.clockwise)
		//	deltaInterpolation += 1.0f;
		//if (m_gameInputCommands.anticlockwise)
		//	deltaInterpolation -= 1.0f;
		//if (m_gameInputCommands.forward)
		//	deltaInterpolation += 1.0f;
		//if (m_gameInputCommands.back)
		//	deltaInterpolation -= 1.0f;
			
		for (int i = 0; i < m_BloodVesselCount; i++)
		{
			m_BloodVessels[i].Update(device, m_timer.GetElapsedSeconds(), 0.08f*deltaInterpolation*m_timer.GetElapsedSeconds());
		}
	}

	// WORLD MATRICES:
	m_view = m_Camera.getCameraMatrix();
	m_projection = m_Camera.getPerspective();
	m_world = Matrix::Identity;

	/*create our UI*/
	SetupGUI();

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
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();

	//Set Rendering states. 
	//context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	//context->OMSetBlendState(m_states->Additive(), nullptr, 0xFFFFFFFF); // NB: Which blend is best? Is it most efficient to just set this here?
	context->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
	//context->RSSetState(m_states->Wireframe());

	// If m_time == 0.0, then render all static textures (once only!)
	if (!m_preRendered)
	{
		m_preRendered = true;
	}

	// PHYSICAL RENDER
	m_PhysicalRenderPass->setRenderTarget(context);
	m_PhysicalRenderPass->clearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	DirectX::SimpleMath::Vector3 displacement = Vector3(0.0f, -0.4f, 0.0f);// DirectX::SimpleMath::Vector3(2.5f, 1.0f*sin(1.0f*XM_PI/5.0f), 0.0f);
	m_Board.Render(context, &m_TerrainShader, displacement, 1.0f, 0.95f, &m_Camera, m_time, &m_Light);
	m_Board.RenderUI(context, &m_LocationShader, displacement, 1.0f, &m_Camera, m_time, m_normalMap.Get());

	// DEBUG: Render three (normalised) torii, for voxel texturing...
	/*m_VoronoiShader.EnableShader(context);
	m_VoronoiShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, -0.5f)*Matrix::CreateRotationY(-XM_PIDIV4)*Matrix::CreateTranslation(-1.0f, 0.0f, -1.0f)*Matrix::CreateScale(1.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateOrthographic(2.0f*m_aspectRatio, 2.0f, 0.01f, 100.0f), true);// &m_Camera.getPerspective(), true);
	m_VoronoiShader.SetTimeBuffer(context, m_timer.GetTotalSeconds());
	m_Torus.Render(context);

	m_VoronoiShader.EnableShader(context);
	m_VoronoiShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, -0.5f)*Matrix::CreateRotationY(0.0f)*Matrix::CreateTranslation(0.0f, 0.0f, -1.0f)*Matrix::CreateScale(1.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateOrthographic(2.0f*m_aspectRatio, 2.0f, 0.01f, 100.0f), true);// &m_Camera.getPerspective(), true);
	m_VoronoiShader.SetTimeBuffer(context, m_timer.GetTotalSeconds());
	m_Torus.Render(context);

	m_VoronoiShader.EnableShader(context);
	m_VoronoiShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, -0.5f)*Matrix::CreateRotationY(XM_PIDIV4)*Matrix::CreateTranslation(1.0f, 0.0f, -1.0f)*Matrix::CreateScale(1.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateOrthographic(2.0f*m_aspectRatio, 2.0f, 0.01f, 100.0f), true);// &m_Camera.getPerspective(), true);
	m_VoronoiShader.SetTimeBuffer(context, m_timer.GetTotalSeconds());
	m_Torus.Render(context);*/

	// DEBUG: Render a dragon curve...
	/*m_NeutralShader.EnableShader(context);
	m_NeutralShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, 0.0f)*Matrix::CreateScale(1.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateScale(1.0f/m_aspectRatio, 1.0f, 1.0f), true);
	context->RSSetState(m_states->CullCounterClockwise());
	m_DragonCurve.Render(context);
	context->RSSetState(m_states->CullClockwise());
	m_DragonCurve.Render(context);*/

	// DEBUG: Render a sphinx tiling...
	/*m_NeutralShader.EnableShader(context);
	m_NeutralShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.375f, 0.0f)*Matrix::CreateScale(1.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateScale(1.0f/m_aspectRatio, 1.0f, 1.0f), true);
	context->RSSetState(m_states->CullCounterClockwise());
	m_SphinxTiling.Render(context);
	context->RSSetState(m_states->CullClockwise());
	m_SphinxTiling.Render(context);*/

	// DEBUG: Render a Penrose P3 tiling...
	/*m_NeutralShader.EnableShader(context);
	m_NeutralShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, 0.0f)*Matrix::CreateScale(1.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateScale(1.0f/m_aspectRatio, 1.0f, 1.0f), true);
	context->RSSetState(m_states->CullCounterClockwise());
	m_PenroseP3.Render(context);
	context->RSSetState(m_states->CullClockwise());
	m_PenroseP3.Render(context);*/

	// DEBUG: Render 14 fundamental partitions...
	/*m_NeutralShader.EnableShader(context);
	m_NeutralShader.SetMatrixBuffer(context, &(Matrix)Matrix::Identity, &(Matrix)Matrix::Identity, &(Matrix)Matrix::Identity, true);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	m_Screen.Render(context);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	for (int i = 0; i < 14; i++)
	{
		m_WireframeShader.EnableShader(context);
		m_WireframeShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, -0.5f)*Matrix::CreateScale(0.66f)*Matrix::CreateRotationY(-XM_PI/12.0f)*Matrix::CreateRotationX(XM_PI/9.0f)*Matrix::CreateTranslation(1.2f*(i%5-((i/5 < 2) ? 2.0f : 1.5f)), -((i/5)-1.0f), -1.0f)*Matrix::CreateScale(0.5f)), &(Matrix)Matrix::Identity, &Matrix::CreateOrthographic(2.0f*m_aspectRatio, 2.0f, 0.01f, 100.0f), true);// &m_Camera.getPerspective(), true);
		context->RSSetState(m_states->CullCounterClockwise());
		m_Wireframe.Render(context);

		m_TerrainShader.EnableShader(context);
		m_TerrainShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, -0.5f)*Matrix::CreateScale(0.66f)*Matrix::CreateRotationY(-XM_PI/12.0f)*Matrix::CreateRotationX(XM_PI/9.0f)*Matrix::CreateTranslation(1.2f*(i%5-((i/5 < 2) ? 2.0f : 1.5f)), -((i/5)-1.0f), -1.0f)*Matrix::CreateScale(0.5f)), &(Matrix)Matrix::Identity, &Matrix::CreateOrthographic(2.0f*m_aspectRatio, 2.0f, 0.01f, 100.0f), true);// &m_Camera.getPerspective(), true);
		m_TerrainShader.SetAlphaBuffer(context, 0.83f);
		m_TerrainShader.SetLightBuffer(context, &m_Light);
		context->RSSetState(m_states->CullCounterClockwise());
		m_Partitions[i].Render(context);
		context->RSSetState(m_states->CullClockwise());
		m_Partitions[i].Render(context);

		m_WireframeShader.EnableShader(context);
		m_WireframeShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, -0.5f)*Matrix::CreateScale(0.66f)*Matrix::CreateRotationY(-XM_PI/12.0f)*Matrix::CreateRotationX(XM_PI/9.0f)*Matrix::CreateTranslation(1.2f*(i%5-((i/5 < 2) ? 2.0f : 1.5f)), -((i/5)-1.0f), -1.0f)*Matrix::CreateScale(0.5f)), &(Matrix)Matrix::Identity, &Matrix::CreateOrthographic(2.0f*m_aspectRatio, 2.0f, 0.01f, 100.0f), true);// &m_Camera.getPerspective(), true);
		context->RSSetState(m_states->CullClockwise());
		m_Wireframe.Render(context);
	}*/

	// DEBUG: Render Zamir's model of arterial branching...
	/*for (int i = 0; i < m_DeterministicBloodVessels.size(); i++)
	{
		int upperRow = (m_DeterministicBloodVessels.size()-1)/2+1;
		float xOffset = (i < upperRow) ? i-0.5f*(upperRow-1) : (i-upperRow)-0.5f*((m_DeterministicBloodVessels.size()-upperRow)-1);
		float yOffset = (i < upperRow) ? 0.5f : -0.33f;

		m_NeutralShader.EnableShader(context);
		m_NeutralShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, 0.0f)*Matrix::CreateScale(0.9f)*Matrix::CreateTranslation(xOffset, yOffset, 0.0f)*Matrix::CreateScale(1.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateScale(1.0f/m_aspectRatio, 1.0f, 1.0f), true);
		context->RSSetState(m_states->CullCounterClockwise()); // NB: MatrixBuffer uses 'true' on both sides, since we aren't applying lighting to the model anyway!
		m_DeterministicBloodVessels[i].Render(context);
		context->RSSetState(m_states->CullClockwise());
		m_DeterministicBloodVessels[i].Render(context);
	}*/

	// DEBUG: Render exemplar, stochastic blood vessels...
	/*for (int i = 0; i < m_StochasticBloodVessels.size(); i++)
	{
		float xOffset = i-0.5f*(m_StochasticBloodVessels.size()-1);

		m_NeutralShader.EnableShader(context);
		m_NeutralShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, 0.0f)*Matrix::CreateScale(0.9f)*Matrix::CreateTranslation(0.25f*xOffset, 0.0f, 0.0f)*Matrix::CreateScale(2.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateScale(1.0f/m_aspectRatio, 1.0f, 1.0f), true);
		context->RSSetState(m_states->CullCounterClockwise()); // NB: MatrixBuffer uses 'true' on both sides, since we aren't applying lighting to the model anyway!
		m_StochasticBloodVessels[i].Render(context);
		context->RSSetState(m_states->CullClockwise());
		m_StochasticBloodVessels[i].Render(context);
	}*/

	// VEINS RENDER:
	m_VeinsRenderPass->setRenderTarget(context);
	m_VeinsRenderPass->clearRenderTarget(context, 0.0f, 0.0f, 0.0f, 0.0f);

	float theta;
	for (int i = 0; i < m_BloodVesselCount; i++)
	{
		theta = i*XM_2PI/m_BloodVesselCount+XM_PI/m_BloodVesselCount;

		m_NeutralShader.EnableShader(context);
		m_NeutralShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -0.5f, 0.0f)*Matrix::CreateScale(1.0f)*Matrix::CreateRotationZ(theta+XM_PIDIV2)*Matrix::CreateTranslation(0.85f*Vector3(pow(1.0f+pow(m_aspectRatio, 2.0f), 0.5f)*cos(theta), pow(1.0f/m_aspectRatio, 0.5f)*pow(1.0f+pow(m_aspectRatio, 2.0f), 0.5f)*sin(theta), 0.0f))*Matrix::CreateScale(1.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateScale(1.0f/m_aspectRatio, 1.0f, 1.0f), true);
		context->RSSetState(m_states->CullCounterClockwise()); // NB: MatrixBuffer uses 'true' on both sides, since we aren't applying lighting to the model anyway!
		m_BloodVessels[i].Render(context);
		context->RSSetState(m_states->CullClockwise());
		m_BloodVessels[i].Render(context);
	}

	// DEBUG: Render a sphinx tiling in the background...
	/*m_NeutralShader.EnableShader(context);
	m_NeutralShader.SetMatrixBuffer(context, &(Matrix::CreateTranslation(-0.5f, -3.0f/8.0f, 0.0f)*Matrix::CreateScale(8.0f)), &(Matrix)Matrix::Identity, &Matrix::CreateScale(1.0f/m_aspectRatio, 1.0f, 1.0f), true);
	context->RSSetState(m_states->CullCounterClockwise());
	m_SphinxTiling.Render(context)
	context->RSSetState(m_states->CullClockwise());
	m_SphinxTiling.Render(context);*/

	// COMPOSITE RENDER:
	context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);

	m_ScreenShader.EnableShader(context);
	m_ScreenShader.SetMatrixBuffer(context, &(Matrix)Matrix::Identity, &(Matrix)Matrix::Identity, &(Matrix)Matrix::Identity, true);
	m_ScreenShader.SetTimeBuffer(context, m_time);
	m_ScreenShader.SetAlphaBuffer(context, 2.0f/3.0f);
	m_ScreenShader.SetAspectRatioBuffer(context, m_aspectRatio);
	m_ScreenShader.SetStressBuffer(context, (m_BloodVesselCount > 0) ? *m_BloodVessels[0].GetIntensity() : 0.0f);
	m_ScreenShader.SetShaderTexture(context, m_PhysicalRenderPass->getShaderResourceView(), -1, 0);
	m_ScreenShader.SetShaderTexture(context, m_VeinsRenderPass->getShaderResourceView(), -1, 1);
	m_Screen.Render(context);

	//render our GUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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
	m_Board.Initialize(device, 4, 64);
	m_add = 0;

	// L-Systems
	/*m_DragonCurve.Initialize(device, 0.125f, 11);
	m_DragonCurve.Update(device, 0.0f, 1.0f); // NB: Static, so setting initial intensity here!*/

	/*m_SphinxTiling.Initialize(device, 0.01f, 5);
	m_SphinxTiling.Update(device, 0.0f, 1.0f);*/

	/*m_PenroseP3.Initialize(device, 0.01f, 1);
	m_PenroseP3.Update(device, 0.0f, 1.0f);*/

	/*for (int i = 0; i < 5; i++)
	{
		m_DeterministicBloodVessels.push_back(LDeterministicBloodVessel());
		m_DeterministicBloodVessels[i].Initialize(device, 0.1f, 1.0f-(1.0f/5.0f)*i, 9);
		m_DeterministicBloodVessels[i].Update(device, 0.0f, 1.0f);
	}*/

	/*for (int i = 0; i < 6; i++)
	{
		m_StochasticBloodVessels.push_back(LBloodVessel());
		m_StochasticBloodVessels[i].Initialize(device, 0.1f, 16, i);
		m_StochasticBloodVessels[i].Update(device, 0.0f, 1.0f);
	}*/

	m_BloodVesselCount = 16;
	for (int i = 0; i < m_BloodVesselCount; i++)
	{
		m_BloodVessels.push_back(LBloodVessel());
		m_BloodVessels[i].Initialize(device, 0.1f, 8, i);
	}

	// Models
	m_Screen.Initialize(device);
	m_Cube.InitializeModel(device, "cube.obj");

	/*Field toroidalField = Field();
	toroidalField.Initialise(64);
	toroidalField.InitialiseToroidalField(0.75f, 0);
	m_Torus.Initialize(device, 64, toroidalField.m_field, 1.0f);

	Field cubicField = Field();
	cubicField.Initialise(64);
	cubicField.InitialiseCubicField();
	m_Wireframe.Initialize(device, 64, cubicField.m_field, 1.0f);

	Field partitionField = Field();
	int configuration[14] = { 
		1, 9, 24, 22, 150,
		10, 26, 90, 14, 30,
		15, 27, 23, 0
	};
	for (int i = 0; i < 14; i++)
	{
		partitionField.InitialisePartition(configuration[i]);
		m_Partitions[i].Initialize(device, 1, partitionField.m_field, 0.0f);
	}*/

	// Shaders
	m_NeutralShader.InitShader(device, L"neutral_vs.cso", L"neutral_ps.cso");
	m_NeutralShader.InitMatrixBuffer(device);

	m_TerrainShader.InitShader(device, L"light_3vs.cso", L"light_3ps.cso");
	m_TerrainShader.InitMatrixBuffer(device);
	m_TerrainShader.InitAlphaBuffer(device);
	m_TerrainShader.InitLightBuffer(device);

	m_LocationShader.InitShader(device, L"texture_vs.cso", L"texture_ps.cso");
	m_LocationShader.InitMatrixBuffer(device);

	m_VoronoiShader.InitShader(device, L"texture_3vs.cso", L"euclidean_voronoi_3ps.cso");
	m_VoronoiShader.InitMatrixBuffer(device);
	m_VoronoiShader.InitTimeBuffer(device);

	m_WireframeShader.InitShader(device, L"texture_3vs.cso", L"wireframe_3ps.cso");
	m_WireframeShader.InitMatrixBuffer(device);

	m_ScreenShader.InitShader(device, L"vignette_vs.cso", L"vignette_ps.cso");
	m_ScreenShader.InitMatrixBuffer(device);
	m_ScreenShader.InitTimeBuffer(device);
	m_ScreenShader.InitAlphaBuffer(device);
	m_ScreenShader.InitAspectRatioBuffer(device);
	m_ScreenShader.InitStressBuffer(device);

	//load Textures
	CreateDDSTextureFromFile(device, L"LocationMarker_002.dds", nullptr,	m_normalMap.ReleaseAndGetAddressOf());

	m_preRendered = false;

	//Initialise Render to texture
	m_PhysicalRenderPass = new RenderTexture(device, 1920, 1080, 1, 2); // FIXME: How do I make this 2048x2048?
	m_VeinsRenderPass = new RenderTexture(device, 1920, 1080, 1, 2); // FIXME: How do I make this 2048x2048?
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	auto size = m_deviceResources->GetOutputSize();

	m_width = size.right;
	m_height = size.bottom;
	m_aspectRatio = (float)m_width/m_height;
	m_fov = 50.0f * XM_PI / 180.0f;

    // This sample makes use of a right-handed coordinate system using row-major matrices.
	m_Camera.setPerspective(m_fov, m_aspectRatio, 0.01f, 100.0f);
}

void Game::SetupGUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.825f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	float textWidth = std::max(720.0f, std::min(ImGui::CalcTextSize(m_Board.m_scene.premise.c_str()).x, 1440.0f));

	ImGui::Begin((m_Board.m_scene.premise.length() > 0) ? m_Board.m_scene.premise.c_str() : " ", (bool*)true, window_flags); // NB: Nice, robust failsafe here?

	// FIXME: How do we right align wrapped text? (without a ridiculous amount of code...)
	ImGui::PushTextWrapPos(1440.0f);
	ImGui::Text(m_Board.m_scene.premise.c_str(), ImVec2(textWidth, 0.0f));
	ImGui::PopTextWrapPos();

	// FIXME: StoryEngine buttons will be handled here...
	ImGui::PushFont(m_choiceFont);
	for (int i = 0; i < m_Board.m_scene.choices.size(); i++)
	{
		// FIXME: Needed to right align buttons...
		/*ImGui::Dummy(ImVec2(0.25f*textWidth, 0.0f));
		ImGui::SameLine();*/

		ImGui::PushID(i);
		if (ImGui::Button(("  "+std::to_string(i + 1)+". "+m_Board.m_scene.choices[i]).c_str(), ImVec2(0.9f*textWidth, 1.1f*m_choiceFont->FontSize)) || m_gameInputCommands.chooseReleased[i]) // NB: Input class has 'on released' logic in place, to stop the player skipping through storylets...
		{
			m_Board.Choose(i);
		}
		ImGui::PopID();
	}
	ImGui::PopFont();

	ImGui::End();

	// DEBUG:
	//ImGui::Wind
	//ImGui::EndFrame();
	//ImGui::NewFrame();
	//SetNextWindowPos(ImVec2(870, 520), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	//ImGui::Begin(" ", (bool*)true, window_flags); // NB: Nice, robust failsafe here?
	//ImGui::Text(m_Board.m_scene.premise.c_str(), ImVec2(textWidth, 0.0f));
	//ImGui::Image(m_normalMap.Get(), ImVec2(80, 80));
	//ImGui::End();

	ImGui::EndFrame();
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
