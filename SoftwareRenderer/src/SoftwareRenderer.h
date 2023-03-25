#pragma once
#include <SnapEngine.h>
#include "Math/Math.h"

using namespace SnapEngine;

class SandBoxLayer : public SnapEngine::Layer
{
	struct FrameBufferPixels
	{
		FrameBufferPixels() = default;
		FrameBufferPixels(const FrameBufferPixels&) = default;
		FrameBufferPixels(uint32_t Width, uint32_t Height)
			: Width(Width), Height(Height)
		{
			m_Pixels = new uint32_t[Width * Height];
		}

		~FrameBufferPixels() { delete m_Pixels; }

		void Resize(uint32_t Width, uint32_t Height)
		{
			this->Width = Width;
			this->Height = Height;
			delete m_Pixels;
			m_Pixels = new uint32_t[Width * Height];
		}

		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t* m_Pixels = nullptr;
	};

	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

public:
	SandBoxLayer()
		: Layer("SnadBoxLayer")
	{}

	virtual void Start() override
	{
		// Creat Frame Buffer Texture
		auto& Window = SnapEngine::Application::Get().GetWindow();
		uint32_t WindowWidth = Window.GetWidth();
		uint32_t WindowHeight = Window.GetHeight();

		m_FrameBufferTexture = SnapPtr<Texture2D>(Texture2D::Creat(WindowWidth, WindowHeight));
		m_FrameBuffer = CreatSnapPtr<FrameBufferPixels>(WindowWidth, WindowHeight);
		
		// Creat OrthoGraphic Camera
		m_OrthoGraphicCamera = CreatSnapPtr<OrthoGraphicCamera>(
			WindowWidth  * -0.5f,
			WindowWidth  *  0.5f,
			WindowHeight * -0.5f,
			WindowHeight *  0.5f);
	}

	virtual void Update(SnapEngine::TimeStep Time) override
	{
		m_Time += Time;

		{ // Start Frame
			RendererCommand::SetClearColor({ 0.3f, 0.3f, 0.3f, 1.0f });
			RendererCommand::Clear();
			Renderer2D::ResetStats();
		}

		// Update
		uint32_t PixelCount = m_FrameBufferTexture->getWidth() * m_FrameBufferTexture->getHeight();
		for (size_t i = 0; i < PixelCount; i++)
			m_FrameBuffer->m_Pixels[i] = 0x00;

		FillTriangle(*m_FrameBuffer,
			{ 200.0f, 200.0f },
			{ 600.0f, 200.0f },
			{ 400.0f, 500.0f },
			{ 0xFF, 0x00, 0x00 },
			{ 0x00, 0xFF, 0x00 },
			{ 0x00, 0x00, 0xFF });

		m_FrameBufferTexture->SetData(m_FrameBuffer->m_Pixels, PixelCount * sizeof(uint32_t));

		{// End Frame
			Renderer2D::Begin(m_OrthoGraphicCamera->GetRendererCamera());
			Renderer2D::DrawQuad(
				m_FrameBufferTexture,
				{ 0.0f, 0.0f, 0.0f },
				{ (float)m_FrameBufferTexture->getWidth(), (float)m_FrameBufferTexture->getHeight(), 1.0f }
			);
			Renderer2D::End();
		}
	}

	virtual void ImGuiRender() override
	{
		ImGui::Begin("Properties");
		ImGui::DragFloat("Time", &m_Time);
		ImGui::End();
	}

	virtual void ProcessEvent(SnapEngine::IEvent& e) override
	{
		SnapEngine::EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<SnapEngine::WindowResizeEvent>(SNAP_BIND_FUNCTION(SandBoxLayer::OnWindowResize));	
	}

	bool OnWindowResize(SnapEngine::WindowResizeEvent& e)
	{
		// Creat Frame Buffer Texture
		uint32_t WindowWidth = e.GetWidth();
		uint32_t WindowHeight = e.GetHeight();

		m_FrameBufferTexture = SnapPtr<Texture2D>(Texture2D::Creat(WindowWidth, WindowHeight));
		m_FrameBuffer->Resize(WindowWidth, WindowHeight);

		// Creat OrthoGraphic Camera
		m_OrthoGraphicCamera = CreatSnapPtr<OrthoGraphicCamera>(
			WindowWidth * -0.5f,
			WindowWidth * 0.5f,
			WindowHeight * -0.5f,
			WindowHeight * 0.5f);

		return false;
	}

private:
	void StartDockSpace();
	void EndDockSpace();

private:
	void PutPixel(const FrameBufferPixels& FrameBuffer, const Math::vec2& PixelCoords, uint32_t Color);
	void FillTriangle(
		const FrameBufferPixels& FrameBuffer,
		const Math::vec2& P0,
		const Math::vec2& P1,
		const Math::vec2& P2,
		const Color& Color0,
		const Color& Color1,
		const Color& Color2);

private:
	SnapPtr<OrthoGraphicCamera> m_OrthoGraphicCamera;
	SnapPtr<Texture2D> m_FrameBufferTexture;
	SnapPtr<FrameBufferPixels> m_FrameBuffer;

	float m_Time = 0.0f;
};

class SandBoxApplication : public SnapEngine::Application
{
public:
	SandBoxApplication()
		: SnapEngine::Application("SandBox Game", 800, 600)
	{
		PushLayer(new SandBoxLayer());
	}
};