#include "ogl_demos.hpp"

OglDEMOS::
OglDEMOS(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    :width_{ width }, height_{ height }
{
    start_time_ = std::chrono::high_resolution_clock::now();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    pWindow_ = glfwCreateWindow(width, height, title, monitor, share);
    glfwSetWindowUserPointer(pWindow_, this);
    glfwSetFramebufferSizeCallback(pWindow_, framebuffer_size_callback);
    glfwSetMouseButtonCallback(pWindow_, mouse_button_callback);
    glfwSetCursorPosCallback(pWindow_, mouse_callback);
    glfwSetScrollCallback(pWindow_, scroll_callback);
    glfwSetKeyCallback(pWindow_, key_callback);
    glfwMakeContextCurrent(pWindow_);
    glfwSetInputMode(pWindow_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gl3wInit();
}

void OglDEMOS::
MainLoop(void)
{
    do
    {
        process_frame(pWindow_, getFrameInterval_Seconds());
        Display();
        glfwSwapBuffers(pWindow_);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(pWindow_));
}

void OglDEMOS::
Run(void) 
{
    Initialize();
    MainLoop();
    Finalize();
}

float OglDEMOS::
getFrameInterval_Seconds(void)
{
    static auto last_time = std::chrono::high_resolution_clock::now();
    auto cur_time = std::chrono::high_resolution_clock::now();
    auto delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>
        (cur_time - last_time).count();
    last_time = cur_time;
    return (float)delta_time / 1000'000'000;
}

void OglDEMOS::
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void OglDEMOS::
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        default:
            break;
        }
    }
}

void OglDEMOS::
mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    //记录是否是第一次或再一次使 cursor 状态变成 disable
    //  cursor通过左右键切换状态disable和normal之间，需要记录下来辨别当前情况
    static bool is_cursor_disable_first_again = true;
    static double lastX = xpos;
    static double lastY = ypos;

    if (GLFW_CURSOR_DISABLED == glfwGetInputMode(window, GLFW_CURSOR))
    {
        if (is_cursor_disable_first_again) {
            lastX = xpos;
            lastY = ypos;
            is_cursor_disable_first_again = false;
        }

        double xoffset = xpos - lastX;
        double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        auto pThis = (OglDEMOS*)glfwGetWindowUserPointer(window);
        pThis->camera_.ProcessMouseMovement(xoffset, yoffset);
    }
    else if (GLFW_CURSOR_NORMAL == glfwGetInputMode(window, GLFW_CURSOR)) {
        is_cursor_disable_first_again = true;
    }
}

void OglDEMOS::
scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto pThis = (OglDEMOS*)glfwGetWindowUserPointer(window);
    pThis->camera_.ProcessMouseScroll(yoffset);
}

void OglDEMOS::
key_callback(GLFWwindow* window, int key, int, int action, int) {
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_KEY_M:
        if (action == GLFW_PRESS)
        {
            static GLenum  mode = GL_FILL;
            mode = (mode == GL_FILL ? GL_LINE : GL_FILL);
            glPolygonMode(GL_FRONT_AND_BACK, mode);
        }
        break;
    default:
        break;
    }
}

void OglDEMOS::
process_frame(GLFWwindow* window, float frame_time_interval)
{
    auto pThis = (OglDEMOS*)glfwGetWindowUserPointer(window);
    auto& camera = pThis->camera_;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD_, frame_time_interval);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD_, frame_time_interval);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT_, frame_time_interval);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT_, frame_time_interval);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP_, frame_time_interval);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN_, frame_time_interval);
}

