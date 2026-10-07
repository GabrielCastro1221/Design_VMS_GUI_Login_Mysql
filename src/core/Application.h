#pragma once

struct GLFWwindow;

class Application
{
private:
    GLFWwindow *window;

public:
    Application();

    bool init();
    void run();
    void shutdown();
};