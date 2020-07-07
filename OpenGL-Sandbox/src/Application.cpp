#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Include .OBJ reader / GLM core / GLSL features
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

static std::string parseShader(const std::string filePath) // gets string from *.shader
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
    //Compile and create VertexShader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vs_source = parseShader("res/shaders/Vertex.shader");
    const GLchar* vs_src = vs_source.c_str();
    glShaderSource(vertex_shader, 1, &vs_src, NULL);
    glCompileShader(vertex_shader);

    //Compile and create TessellationControl Shader
    GLuint tessControl_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
    std::string tcs_source = parseShader("res/shaders/TessellationControl.shader");
    const GLchar* tcs_src = tcs_source.c_str();
    glShaderSource(tessControl_shader, 1, &tcs_src, NULL);
    glCompileShader(tessControl_shader);

    //Compile and create TessellationEvaluation Shader
    GLuint tessEvaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    std::string tes_source = parseShader("res/shaders/TessellationEvaluation.shader");
    const GLchar* tes_src = tes_source.c_str();
    glShaderSource(tessEvaluation_shader, 1, &tes_src, NULL);
    glCompileShader(tessEvaluation_shader);

    //Compile and create Geometry Shader
    GLuint geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    std::string gs_source = parseShader("res/shaders/Geometry.shader");
    const GLchar* gs_src = gs_source.c_str();
    glShaderSource(geometry_shader, 1, &gs_src, NULL);
    glCompileShader(geometry_shader);

    //Compile and create FragmentShader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fs_source = parseShader("res/shaders/Fragment.shader");
    const GLchar* fs_src = fs_source.c_str();
    glShaderSource(fragment_shader, 1, &fs_src, NULL);
    glCompileShader(fragment_shader);

    //Compile and create ComputeShader
    GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    std::string cs_source = parseShader("res/shaders/Compute.shader");
    const GLchar* cs_src = cs_source.c_str();
    glShaderSource(compute_shader, 1, &cs_src, NULL);
    glCompileShader(compute_shader);

    //Create Program & Link it with shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    //glAttachShader(program, tessControl_shader);
    //glAttachShader(program, tessEvaluation_shader);
    //glAttachShader(program, geometry_shader);
    glAttachShader(program, fragment_shader);
    //glAttachShader(program, compute_shader);
    glLinkProgram(program);

    //Delete all shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(tessControl_shader);
    glDeleteShader(tessEvaluation_shader);
    glDeleteShader(geometry_shader);
    glDeleteShader(fragment_shader);
    glDeleteShader(compute_shader);

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
        mv_location = glGetUniformLocation(program, "mv_matrix");
        proj_location = glGetUniformLocation(program, "proj_matrix");

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        static const GLfloat vertex_positions[] =
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

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

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
            

            static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
            static const GLfloat one = 1.0f;

            glViewport(0, 0, 16 * window_size, 9 * window_size);
            glClearBufferfv(GL_COLOR, 0, green);
            glClearBufferfv(GL_DEPTH, 0, &one);

            glUseProgram(program);

            glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

            float f = (float)frame_counter * 0.3f;
            glm::mat4 mv_matrix =   glm::translate(0.0f, 0.0f, -4.0f) *
                                    glm::translate(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
                                    glm::rotate((float)frame_counter * 45.0f, 0.0f, 1.0f, 0.0f) *
                                    glm::rotate((float)frame_counter * 81.0f, 1.0f, 0.0f, 0.0f);
            glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            glfwSwapBuffers(window);
            glfwPollEvents();
            frame_counter++;
        }
        glDisableVertexArrayAttrib(vao, 0);
        frame_counter = 0;
    }

    void shutdown()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        glDeleteBuffers(1, &buffer);
        glfwTerminate();
    }

    void onResize(int w, int h)
    {
        aspect = (float)w / (float)h;
        proj_matrix = glm::perspective(50.0f, aspect, 0.1f, 1000.0f);
    }

private:
    char            window_size = 100;
    GLFWwindow*     window;
    GLuint          program;
    GLuint          vao;
    GLuint          buffer;
    GLint           mv_location;
    GLint           proj_location;
    unsigned int    frame_counter;

    float           aspect;
    glm::mat4       proj_matrix;
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