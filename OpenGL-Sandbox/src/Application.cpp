#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/trigonometric.hpp> //for glm::sin
#include <glm/gtc/type_ptr.hpp> //for glm::value_ptr

std::string parseShader(const std::string filePath) // gets string from shader file
{
    std::stringstream code{};
    std::string line{};
    std::ifstream file(filePath);

    while (getline(file, line))
    {
        code << line << '\n';
    }
    file.close();

    return code.str();
};

GLuint compileShaders(void)
{
    const int shadersCount = 6; // there is 6 shaders in OpenGL pipeline
    struct Shader
    {
        int enabled{};
        GLenum type{};
        std::string path{};
    };
    Shader pipeline[shadersCount] =
    {
        {1, GL_VERTEX_SHADER,            "res/shaders/Vertex.glsl"},
        {0, GL_TESS_CONTROL_SHADER,      "res/shaders/TessellationControl.glsl"},
        {0, GL_TESS_EVALUATION_SHADER,   "res/shaders/TessellationEvaluation.glsl"},
        {0, GL_GEOMETRY_SHADER,          "res/shaders/Geometry.glsl"},
        {1, GL_FRAGMENT_SHADER,          "res/shaders/Fragment.glsl"},
        {0, GL_COMPUTE_SHADER,           "res/shaders/Compute.glsl"}
    };

    GLuint program = glCreateProgram();

    for (int i = 0; i < shadersCount; i++)
    {
        if (pipeline[i].enabled)
        {
            GLuint shaderObj{ glCreateShader(pipeline[i].type) };
            std::string shaderSource{ parseShader(pipeline[i].path) };
            const GLchar* shaderSrc{ shaderSource.c_str() };
            glShaderSource(shaderObj, 1, &shaderSrc, NULL);
            glCompileShader(shaderObj);
            glAttachShader(program, shaderObj);
            glDeleteShader(shaderObj);
        }
    }
    glLinkProgram(program);

    return program;
};

void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
};

class Application
{
public:
    int startup()
    {
        assert( glfwInit() );

        // Create a windowed mode window and its OpenGL context
        window = glfwCreateWindow(16 * windowSize, 9 * windowSize, "CLOWN CUBE", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            assert(0 && "Window creation error" );
        }
        glfwMakeContextCurrent(window);
        assert(glewInit() == GLEW_OK);

        // GLFW hints
        glfwSwapInterval(1);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        // Compiling and linking our program
        program = compileShaders();
        
        // Data
        static const GLfloat vertexPositions[] =
        {
            -0.25f,  0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,
             0.25f,  0.25f, -0.25f,

             0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
             0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f, -0.25f,
            -0.25f,  0.25f, -0.25f,
            -0.25f,  0.25f,  0.25f,

            -0.25f, -0.25f,  0.25f,
             0.25f, -0.25f,  0.25f,
             0.25f, -0.25f, -0.25f,

             0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f, -0.25f,
            -0.25f, -0.25f,  0.25f,

            -0.25f,  0.25f, -0.25f,
             0.25f,  0.25f, -0.25f,
             0.25f,  0.25f,  0.25f,

             0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f,  0.25f,
            -0.25f,  0.25f, -0.25f
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        return 0;
    }

    void render()
    {
        /* Debug */
        printf("%s\n", glGetString(GL_VERSION));
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, 0);

        /* Data */
        GLint mvpLocation{ glGetUniformLocation(program, "mvpMatrix") };
        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f,  0.0f,  0.0f),
            glm::vec3(2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f,  2.0f, -2.5f),
            glm::vec3(1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glLineWidth(4);

       

        // Updates
        while (!glfwWindowShouldClose(window))
        {
            glClearColor(0.01f * glm::exp(glfwGetTime()), 0.2f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



            glUseProgram(program);

            xRay(this);

            for (glm::vec3 pos : cubePositions)
            {
                getKeysWASD(this);
                mvpMatrix = glm::translate(mvpMatrix, pos);
                glUniformMatrix4fv(mvpLocation, 1, GL_TRUE, glm::value_ptr(mvpMatrix));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                mvpMatrix = glm::translate(mvpMatrix, -pos);
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glDisableVertexArrayAttrib(vao, 0);
        glDisableVertexArrayAttrib(vao, 1);
    }


    void shutdown()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        glDeleteBuffers(1, &buffer);
        glfwTerminate();
    }

    friend void getKeysWASD(Application *app);
    friend void xRay(Application* app);

private:
    char            windowSize = 100; //default
    GLFWwindow*     window = NULL;
    GLuint          program{};
    GLuint          vao{};
    GLuint          buffer{};
    glm::mat4       mvpMatrix{ 1.0f };
};

void getKeysWASD(Application *app)
{
    float s{ 0.001f };
    if (glfwGetKey(app->window, GLFW_KEY_W) == GLFW_PRESS)
        app->mvpMatrix = glm::rotate(app->mvpMatrix,  s * static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 0.0f, 0.0f));
    if (glfwGetKey(app->window, GLFW_KEY_A) == GLFW_PRESS)
        app->mvpMatrix = glm::rotate(app->mvpMatrix, s * static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
    if (glfwGetKey(app->window, GLFW_KEY_S) == GLFW_PRESS)
        app->mvpMatrix = glm::rotate(app->mvpMatrix, s * static_cast<float>(glfwGetTime()), glm::vec3(-1.0f, 0.0f, 0.0f));
    if (glfwGetKey(app->window, GLFW_KEY_D) == GLFW_PRESS)
        app->mvpMatrix = glm::rotate(app->mvpMatrix, s * static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, -1.0f));
    if (glfwGetKey(app->window, GLFW_KEY_Z) == GLFW_PRESS)
        app->mvpMatrix = glm::scale(app->mvpMatrix, glm::vec3(0.99f));
    if (glfwGetKey(app->window, GLFW_KEY_C) == GLFW_PRESS)
        app->mvpMatrix = glm::scale( app->mvpMatrix, glm::vec3(1.01f));
    if (glfwGetKey(app->window, GLFW_KEY_W) != GLFW_PRESS && glfwGetKey(app->window, GLFW_KEY_A) != GLFW_PRESS &&
        glfwGetKey(app->window, GLFW_KEY_S) != GLFW_PRESS && glfwGetKey(app->window, GLFW_KEY_D) != GLFW_PRESS &&
        glfwGetKey(app->window, GLFW_KEY_Z) != GLFW_PRESS && glfwGetKey(app->window, GLFW_KEY_C) != GLFW_PRESS)
        glfwSetTime(0);
}

void xRay(Application* app)
{
    if (glfwGetKey(app->window, GLFW_KEY_X) == GLFW_PRESS)
    {
        glEnable(GL_POLYGON_SMOOTH);
        glDisable(GL_CULL_FACE);
        glEnable(GL_LINE_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

int main(void)
{
    Application app;
    if (!app.startup()) //returns -1 if error
    {
        app.render();
        app.shutdown();
    }
    return 0;
}
