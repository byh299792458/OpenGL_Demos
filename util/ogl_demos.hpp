
#ifndef __OGL_DEMOS_HPP__
#define __OGL_DEMOS_HPP__

// c/c++
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

// 3rdparty
#include <GL/gl3w.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// user-defined
#include <camera.hpp>
#include <shader.hpp>
#include <texture2d.hpp>
#include <texture_cube.hpp>

// macros
#define TOVOIDP(x) ((void*)(x))


class OglDEMOS
{
protected:
    OglDEMOS(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
    OglDEMOS(const OglDEMOS&) = delete;
    OglDEMOS& operator=(const OglDEMOS&) = delete;

    static OglDEMOS* s_demo_;
    GLFWwindow* pWindow_;
    int width_;
    int height_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
    //Camera m_camera{ glm::vec3(0.0f, 0.0f, 1000.0f) };
    Camera camera_{ glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.f, 0.f, 0.f) };
    std::vector<Shader> shaders_;
    std::vector<Texture2D> texture_2ds_;
    std::vector<TextureCube> texture_cubes_;

public:
    virtual ~OglDEMOS() { glfwTerminate(); }

    // 返回程序从开始运行到当前经过的时间
    template<typename Duration = std::chrono::milliseconds>
    int64_t TimeElapsed_Milliseconds(void) {
        return std::chrono::duration_cast<Duration>(
            std::chrono::high_resolution_clock::now() - start_time_).count();
    }
    virtual void Initialize() {}
    virtual void Display() {}
    virtual void Finalize() {}
    void MainLoop(void);
    void Run(void);
    float getFrameInterval_Seconds(void);

    glm::mat4 getProjectionMatrix(double zNear, double zFar) {
        return glm::perspective(
            glm::radians(camera_.zoom_), (double)width_ / (double)height_, zNear, zFar);
    }
    glm::mat4 getViewMatrix() {
        return camera_.getViewMatrix();
    }

    template<typename ... Args>
    void EmplaceShader(Args&& ... args)
    {
        shaders_.emplace_back(args...);
    }

    template<typename ... Args>
    void EmplaceTexture2D(Args&& ... args)
    {
        texture_2ds_.emplace_back(args...);
    }

    template<typename ... Args>
    void EmplaceTextureCube(Args&& ... args)
    {
        texture_cubes_.emplace_back(args...);
    }

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void process_frame(GLFWwindow* window, float delta_time_seconds);
    static void key_callback(GLFWwindow*, int, int, int, int);
};

#define BEGIN_DEMO_DECLARATION(Demo)                                      \
class Demo : public OglDEMOS                                              \
{                                                                         \
public:                                                                   \
    typedef class OglDEMOS Base;                                          \
    template<typename ... Args>                                           \
    static OglDEMOS* Create(Args&& ... args)                              \
    {                                                                     \
        return s_demo_ = new Demo(args...);                                \
    }                                                                     \
protected:                                                                \
    Demo(int width, int height, const char* title,                        \
        GLFWmonitor* monitor, GLFWwindow* share)                          \
        :OglDEMOS(width, height, title, monitor, share)                   \
    {}

#define END_DEMO_DECLARATION(Demo)          \
};                                          \
OglDEMOS * OglDEMOS::s_demo_=nullptr;       \

#endif // !__OGL_DEMOS_HPP__








