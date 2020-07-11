#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Include GLM core
#include <glm/glm.hpp>

#define CURRENTLY_BOUND
#define DATA_RECORD_IN_ONE_CALL

static std::string parseShader(const std::string filePath) // gets string from shader file
{
    std::stringstream code;
    std::string line;
    std::ifstream file(filePath);

    while (getline(file, line))
    {
        code << line << '\n';
    }
    file.close();
    return code.str();
};

GLuint compile_shaders(void)
{
    struct shader
    {
        int use;
        GLenum type;
        std::string path;
    };
    shader pipeline[] =
    {
        {1, GL_VERTEX_SHADER,            "res/shaders/Vertex.glsl"},
        {0, GL_TESS_CONTROL_SHADER,      "res/shaders/TessellationControl.glsl"},
        {0, GL_TESS_EVALUATION_SHADER,   "res/shaders/TessellationEvaluation.glsl"},
        {0, GL_GEOMETRY_SHADER,          "res/shaders/Geometry.glsl"},
        {1, GL_FRAGMENT_SHADER,          "res/shaders/Fragment.glsl"},
        {0, GL_COMPUTE_SHADER,           "res/shaders/Compute.glsl"}
    };

    GLuint program = glCreateProgram();

    for (int i = 0; i < 6; i++)
    {
        if (pipeline[i].use)
        {
            GLuint shader_obj = glCreateShader(pipeline[i].type);
            std::string shader_source = parseShader(pipeline[i].path);
            const GLchar* shader_src = shader_source.c_str();
            glShaderSource(shader_obj, 1, &shader_src, NULL);
            glCompileShader(shader_obj);
            glAttachShader(program, shader_obj);
            glDeleteShader(shader_obj);
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

class my_application
{
public:
    int startup()
    {
        /* Initialize the library */
        if (!glfwInit())
            return -1;

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(16 * window_size, 9 * window_size, "Window name", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current and initialize glew */
        glfwMakeContextCurrent(window);
        if (glewInit() != GLEW_OK)
        {
            std::cout << "Error!" << std::endl;
        };
        glfwSwapInterval(1); //60 fpc

        /* Debug stuff */
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        /* Compiling and linking our program */
        program = compile_shaders();
        
        /* Data */
        const struct vertex
        {
            //Coord
            float x;
            float y;
            float z;
            float w;
            //Color
            float r;
            float g;
            float b;
            float a;
        };

        vertex vertices[] = {
            {-0.5f, -0.5f, 0.5f, 1.0f,
             1.0f,  0.0f, 0.0f, 1.0f},

            {0.0f,  0.5f, 0.5f, 1.0f,
             0.0f,  1.0f, 0.0f, 1.0f},

            {0.5f, -0.5f, 0.5f, 1.0f,
             0.0f,  0.0f, 1.0f, 1.0f}
        };

        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &buffer);

#ifdef DATA_RECORD_IN_ONE_CALL
        glNamedBufferStorage(buffer, sizeof(vertices), vertices, 0);
#else
        glNamedBufferStorage(buffer, 1024*1024, NULL, GL_MAP_WRITE_BIT);
        void* ptr = glMapNamedBufferRange(buffer, 0, sizeof(vertices), GL_MAP_WRITE_BIT);
        memcpy(ptr, vertices, sizeof(vertices));
        glUnmapNamedBuffer(buffer);
#endif // DATA_RECORD_IN_ONE_CALL


#ifdef CURRENTLY_BOUND
        glBindVertexArray(vao); //BIND HERE

        glVertexAttribBinding(0, 0);
        glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
        glEnableVertexAttribArray(0);

        glVertexAttribBinding(1, 0);
        glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
        glEnableVertexAttribArray(1);

        glBindVertexBuffer(0, buffer, 0, sizeof(vertex));
#else 
        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
        glEnableVertexArrayAttrib(vao, 0);

        glVertexArrayAttribBinding(vao, 1, 0);
        glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
        glEnableVertexArrayAttrib(vao, 1);

        glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(vertex));
        glBindVertexArray(vao); //BIND HERE
#endif // CURRENTLY_BOUND

        return 0;
    }

    void render()
    {
        /* Debug */
        printf("%s\n", glGetString(GL_VERSION));
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, 0);

        /* Updates */
        frame_counter = 0;
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(program);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(window);
            glfwPollEvents();
            frame_counter++;
        }
        glDisableVertexArrayAttrib(vao, 0);
        glDisableVertexArrayAttrib(vao, 1);
        frame_counter = 0;
    }

    void shutdown()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        glDeleteBuffers(1, &buffer);
        glfwTerminate();
    }

private:
    char            window_size = 100; //default
    GLFWwindow*     window = NULL;
    GLuint          program{};
    GLuint          vao{};
    GLuint          buffer{};
    unsigned int    frame_counter{};
};

int main(void)
{
    my_application Application;
    if (!Application.startup()) //returns -1 if error
    {
        Application.render();
        Application.shutdown();
    }
    return 0;
}
