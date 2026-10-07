#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <fstream>
#include <initializer_list>
#include <iostream>
#include <string>
#include <unordered_set>

#include "Application.h"
#include "config/Env.h"
#include "config/connection/Database.h"
#include "services/auth/AuthService.h"
#include "session/Session.h"
#include "views/LoginWindow.h"
#include "views/VMSWindow.h"
#include "core/media/VideoPlayer.h"

static bool connectDatabase()
{
    env::load(".env");
    env::load("../.env");

    const std::string host = env::get("DB_HOST", "localhost");
    const std::string user = env::get("DB_USER");
    const std::string password = env::get("DB_PASSWORD");
    const std::string name = env::get("DB_NAME", "vms_db");
    const unsigned int port = env::getInt("DB_PORT", 3306);

    if (user.empty())
    {
        std::cerr << "Falta DB_USER. Revisa el archivo .env\n";
        return false;
    }

    Database *db = Database::getInstance();

    if (!db->connect(host, user, password, name, port))
    {
        std::cerr << "No se pudo conectar a la base de datos '"
                  << name << "' en "
                  << host << ":" << port << "\n";
        return false;
    }

    return true;
}

static void loadUiFont()
{
    const char *candidates[] = {
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    };

    for (const char *path : candidates)
    {
        if (std::ifstream(path).good())
        {
            ImGui::GetIO().Fonts->AddFontFromFileTTF(path, 17.0f);
            return;
        }
    }
}

static std::string firstExisting(std::initializer_list<const char *> paths)
{
    for (const char *path : paths)
    {
        if (std::ifstream(path).good())
            return path;
    }
    return "";
}

Application::Application()
    : window(nullptr)
{
}

bool Application::init()
{
    if (!connectDatabase())
        return false;

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    if (!glfwInit())
    {
        std::cerr << "No se pudo iniciar GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 800, "Cyclops VMS", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "No se pudo crear la ventana\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if (err != GLEW_OK)
    {
        std::cerr << "Error en GLEW: " << glewGetErrorString(err) << "\n";
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    loadUiFont();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

void Application::run()
{
    AuthService authService;

    auto loginCallback = [&authService](const std::string &user,
                                        const std::string &password) -> bool
    {
        return authService.login(user, password);
    };

    LoginWindow login(loginCallback);
    VideoPlayer video;

    const std::string videoPath = firstExisting({
        "src/assets/video/BG_Login.mp4",
        "../src/assets/video/BG_Login.mp4",
    });

    if (videoPath.empty() || !video.load(videoPath))
    {
        std::cerr << "No se pudo cargar BG_Login.mp4\n";
    }

    login.setBackground(&video);

    VMSWindow vms;

    vms.setOnLogout([&authService, &login]()
                    {
        authService.logout();
        login.logout(); });

    vms.setAllowAllPermissions(true);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        video.update();

        const ImVec2 display = ImGui::GetIO().DisplaySize;

        if (!login.isLoggedIn() && video.isReady())
        {
            const VideoPlayer::UVRect uv = video.getCoverUV(display.x, display.y);

            ImDrawList *bg = ImGui::GetBackgroundDrawList();
            bg->AddImage(
                (ImTextureID)(intptr_t)video.getTexture(),
                ImVec2(0.0f, 0.0f),
                display,
                ImVec2(uv.u0, uv.v0),
                ImVec2(uv.u1, uv.v1));
            bg->AddRectFilled(ImVec2(0.0f, 0.0f), display, IM_COL32(6, 15, 30, 70));
        }

        if (!login.isLoggedIn())
        {
            login.render();
        }
        else
        {
            const User &user = Session::getInstance().currentUser;

            vms.setUser(
                user.username.empty() ? login.username() : user.username,
                user.role);

            if (!user.permissions.empty())
            {
                std::unordered_set<std::string> perms(
                    user.permissions.begin(),
                    user.permissions.end());
                vms.setPermissions(perms);
            }

            vms.render();
        }

        ImGui::Render();

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(0.10f, 0.12f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void Application::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
    Database::getInstance()->disconnect();
}