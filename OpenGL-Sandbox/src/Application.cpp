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
        window = glfwCreateWindow(9 * windowSize, 9 * windowSize, "Window name", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            assert(0 && "Window creation error" );
        }
        glfwMakeContextCurrent(window);
        assert(glewInit() == GLEW_OK);

        // GLFW hints
        glfwSwapInterval(1); //60 fpc
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


        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        return 0;
    }

    void render()
    {
        /* Data */
        GLint mv_location{ glGetUniformLocation(program, "mvMatrix") };
        GLint proj_location{ glGetUniformLocation(program, "projMatrix") };

        /* Debug */
        printf("%s\n", glGetString(GL_VERSION));
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, 0);

        /* Updates */
        while (!glfwWindowShouldClose(window))
        {
            glClearColor(0.7f * glm::sin(glfwGetTime()), 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(program);


            glUniformMatrix4fv(mv_location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(glm::value_ptr(mvMatrix)));

            glDrawArrays(GL_TRIANGLES, 0, 36);

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

private:
    char            windowSize = 50; //default
    GLFWwindow*     window = NULL;
    GLuint          program{};
    GLuint          vao{};
    GLuint          buffer{};
    glm::mat4       mvMatrix{ 1.0f };
    glm::mat4       projMatrix{ 1.0f };
};

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
