#include "SoftwareRenderer.h"
#define INSIDE_TRIANGLE 1

void SandBoxLayer::StartDockSpace()
{
	static bool DockSpace_Open = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &DockSpace_Open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	//style.WindowMinSize.x = 450.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Exit")) { SnapEngine::Application::Get().Close(); }
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void SandBoxLayer::EndDockSpace()
{
	ImGui::End();
}

void SandBoxLayer::PutPixel(const FrameBufferPixels& FrameBuffer, const Math::vec2& PixelCoords, uint32_t Color)
{
	int PixelIndex = (int)(PixelCoords.x + PixelCoords.y * FrameBuffer.Width);
	m_FrameBuffer->m_Pixels[PixelIndex] = 0xFF00FF00;
}

static bool is_top_left_edge(const Math::vec2& EdgeVector)
{
	bool is_top_edge = EdgeVector.x > 0.0f && EdgeVector.y == 0.0f;
	bool is_left_edge = EdgeVector.y < 0;

	return is_top_edge || is_left_edge;
}

void SandBoxLayer::FillTriangle(const FrameBufferPixels& FrameBuffer, const Math::vec2& P0, const Math::vec2& P1, const Math::vec2& P2, const Color& Color0, const Color& Color1, const Color& Color2)
{
	// Get Triangle Bounding Box
	float minx = Math::Mathf::min(Math::Mathf::min(P0.x, P1.x), P2.x);
	float miny = Math::Mathf::min(Math::Mathf::min(P0.y, P1.y), P2.y);
	float maxx = Math::Mathf::max(Math::Mathf::max(P0.x, P1.x), P2.x);
	float maxy = Math::Mathf::max(Math::Mathf::max(P0.y, P1.y), P2.y);


	// Edge Vectors of Triangle
	Math::vec2 Edge1 = Math::vec2::Sub(P1, P0);
	Math::vec2 Edge2 = Math::vec2::Sub(P2, P1);
	Math::vec2 Edge3 = Math::vec2::Sub(P0, P2);

	// Check top left edges (Fill Convension)
	int l0 = is_top_left_edge(Edge1) ? 0 : -1;
	int l1 = is_top_left_edge(Edge2) ? 0 : -1;
	int l2 = is_top_left_edge(Edge3) ? 0 : -1;

	int TriangleArea = Math::vec2::Cross(Math::vec2::Sub(P1, P0), Math::vec2::Sub(P2, P0));

	for (size_t y = miny; y < maxy; y++)
	{
		for (size_t x = minx; x < maxx; x++)
		{
			{ // Point In Triangle Algorithm
				
				Math::vec2 Point = { x, y };


				// Vector From Triangle Point To Point
				Math::vec2 Pointv1 = Math::vec2::Sub(Point, P0);
				Math::vec2 Pointv2 = Math::vec2::Sub(Point, P1);
				Math::vec2 Pointv3 = Math::vec2::Sub(Point, P2);

				int w0 = (int)(Math::vec2::Cross(Edge1, Pointv1) + l0);
				int w1 = (int)(Math::vec2::Cross(Edge2, Pointv2) + l1);
				int w2 = (int)(Math::vec2::Cross(Edge3, Pointv3) + l2);

				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					float alpha = w0 / (float)TriangleArea;
					float beta = w1  / (float)TriangleArea;
					float gama = w2  / (float)TriangleArea;

					int a = 0xFF;
					int r = (alpha)*Color0.r + (beta)*Color1.r + (gama)*Color2.r;
					int g = (alpha)*Color0.g + (beta)*Color1.g + (gama)*Color2.g;
					int b = (alpha)*Color0.b + (beta)*Color1.b + (gama)*Color2.b;

					uint32_t InterpolationColor = 0x00000000;
					InterpolationColor = (InterpolationColor | a) << 8;
					InterpolationColor = (InterpolationColor | b) << 8;
					InterpolationColor = (InterpolationColor | g) << 8;
					InterpolationColor = (InterpolationColor | r);

					FrameBuffer.m_Pixels[x + y * FrameBuffer.Width] = InterpolationColor;
				}
			}
		}
	}
}

SnapEngine::Application* SnapEngine::CreatApplication()
{
	return new SandBoxApplication();
}