#include "GUI.h"

#include "imgui.h"
#include "imgui_stdlib.h"

#include "networking.h"
#include <functional>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

namespace GUI
{
    inline static Networking::ProtocolV2 protocol;

	MainWindow::MainWindow()
	{
		glfwSetErrorCallback(glfw_error_callback);
	    if (!glfwInit())
	        throw "Could not initialise glfw";

	    // Decide GL+GLSL versions
		#if defined(IMGUI_IMPL_OPENGL_ES2)
		    // GL ES 2.0 + GLSL 100
		    const char* glsl_version = "#version 100";
		    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		#elif defined(__APPLE__)
		    // GL 3.2 + GLSL 150
		    const char* glsl_version = "#version 150";
		    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
		#else
		    // GL 3.0 + GLSL 130
		    const char* glsl_version = "#version 130";
		    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
		#endif

		    // Create window with graphics context
		   	window = glfwCreateWindow(1280, 720, "Gigachad Client", nullptr, nullptr);
		    if (window == NULL)
		    {
		        throw "Failed to create GLFW window";
		        glfwTerminate();
		    }

		    glfwMakeContextCurrent(window);

		    if (!gladLoadGL(glfwGetProcAddress))
		    {
		        throw "Failed to initialize GLAD";
		    }
		    glfwMakeContextCurrent(window);
		    glfwSwapInterval(1); // Enable vsync

		    // Setup Dear ImGui context
		    IMGUI_CHECKVERSION();
		    ImGui::CreateContext();
		    ImGuiIO& io = ImGui::GetIO(); (void)io;
		    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		    //io.ConfigViewportsNoAutoMerge = true;
		    //io.ConfigViewportsNoTaskBarIcon = true;
		    io.Fonts->AddFontFromFileTTF("../dependencies/imgui/misc/fonts/Roboto-Medium.ttf", 14);

		    // Setup Dear ImGui style
		    SetupImGuiStyle(true, 0.0f);
		    //ImGui::StyleColorsDark();
		    //ImGui::StyleColorsLight();

		    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		    ImGuiStyle& style = ImGui::GetStyle();
		    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		    {
		        style.WindowRounding = 0.5f;
		        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		    }

		    // Setup Platform/Renderer backends
		    ImGui_ImplGlfw_InitForOpenGL(window, true);
		    ImGui_ImplOpenGL3_Init(glsl_version);

		    // Load Fonts
		    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
		    // - Read 'docs/FONTS.md' for more instructions and details.
		    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
		    //io.Fonts->AddFontDefault();
		    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
		    //IM_ASSERT(font != nullptr);

		    loginWindow = new LoginWindow();
		    chatWindow = new ChatWindow();

	}

	void MainWindow::render()
	{
		// Runs once per frame

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
       
       	loginWindow->render();

        chatWindow->render();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

	}

	MainWindow::~MainWindow()
	{
		delete loginWindow;
		delete chatWindow;

	    // Cleanup
	    ImGui_ImplOpenGL3_Shutdown();
	    ImGui_ImplGlfw_Shutdown();
	    ImGui::DestroyContext();

	    glfwDestroyWindow(window);
	    glfwTerminate();
	}

	LoginWindow::LoginWindow()
	{
	}

	void LoginWindow::render()
	{
		if(visible)
		{
			ImGui::SetNextWindowSize(ImVec2(400, 400));
			ImGui::Begin("Log-In or Register", nullptr, ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoDocking
				| ImGuiWindowFlags_NoCollapse);

			ImVec2 contentRegionSize = ImGui::GetContentRegionAvail();

			if(loginResponse != 0)
			{
				ImGui::LabelText("Login Failed", "");
			}

			ImGui::Text("Username");
			ImGui::InputText("##username", usernameBuffer, IM_ARRAYSIZE(usernameBuffer));
			ImGui::Text("Password");
			if(ImGui::InputText("##password", passwordBuffer, IM_ARRAYSIZE(passwordBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_Password))
			{
				login();
			}
	
			if(ImGui::Button("Login or Register"))
			{
				login();
			}

			ImGui::End();
		}
	}

	void LoginWindow::login()
	{
		loginResponse = protocol.login(usernameBuffer, passwordBuffer);
		memset(usernameBuffer, 0, sizeof(usernameBuffer));
		memset(passwordBuffer, 0, sizeof(passwordBuffer));
		if(loginResponse == 0)
		{
			visible = false;
			ChatWindow::visible = true;
		}
	}

	LoginWindow::~LoginWindow()
	{
	}

	ChatWindow::ChatWindow()
	{
		//messageEx = new Networking::MessageExchange();
		//messageEx->startReceivingMessages(chatHistory);
	}

	void ChatWindow::render()
	{
		if (visible)
		{
			// Chat Window showing chats
			{
				auto& io = ImGui::GetIO();

				ImGui::Begin("Chat History", NULL, ImGuiDockNodeFlags_AutoHideTabBar);

	            ImGui::TextWrapped(chatHistory.c_str());

	            ImGui::End();
	        }

	        // Send Chats Window
	        {
	            ImGui::Begin("Send Chats");
	            ImGui::InputText("Send", &chatInput, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoHorizontalScroll);

	            if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
	            {
	            	protocol.sendMessage(chatInput);

	                chatInput = "";
	                ImGui::SetKeyboardFocusHere(-1);
	            }

	            ImGui::End();
	        }
		}
	}

	ChatWindow::~ChatWindow()
	{

	}


}