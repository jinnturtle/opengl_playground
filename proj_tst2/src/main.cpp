#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#include "FPS_manager.hpp"
#include "Randomizer.hpp"
#include "utils.hpp"
#include "logs.hpp"

auto process_args(int argc, char** argv) -> void;
auto init() -> GLFWwindow*;
auto deinit(GLFWwindow* window) -> void;

auto main(int argc, char** argv) -> int
{
    if (argc > 1) {
        process_args(argc, argv);
    }

    GLFWwindow* window{init()};
    if (window == nullptr) {
        deinit(window);
        return -1;
    }

    glClearColor(0.2f, 0.0f, 0.4f, 0.0f);

    GLuint vert_array_id;
    glGenVertexArrays(1, &vert_array_id);
    glBindVertexArray(vert_array_id);

    GLuint shader {load_shaders(
        "data/shaders/vertex_simple_shader.glsl",
        "data/shaders/fragment_simple_shader.glsl")};
    if (shader == 0) {
        logs::err("errors while loading shaders");
        deinit(window);
        return -1;
    }

    // cube (made of tris - 2 per face)
    constexpr GLfloat cube_verts[] {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    GLuint vert_buf_id;
    glGenBuffers(1, &vert_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, vert_buf_id);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);

    /* some random generated colors (so it is easier to see the tris that make
     * up the cube) */
    std::array<GLfloat, 6*2*3*3> cube_vert_colors{};
    {
        Randomizer random;
        for (auto& vert : cube_vert_colors) {
            vert = random.get(0.0f, 1.0f);
        }
    }

    GLuint vert_color_buf_id;
    glGenBuffers(1, &vert_color_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, vert_color_buf_id);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cube_vert_colors),
        &cube_vert_colors[0],
        GL_STATIC_DRAW);

    glm::mat4 projection {glm::perspective(
        glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f)};

    // camera matrix
    glm::vec3 view_pos {4.0f, 3.0f, 3.0f};
    glm::mat4 view {glm::lookAt(
        view_pos,
        glm::vec3 {0.0f, 0.0f, 0.0f},
        glm::vec3 {0.0f, 1.0f, 0.0f}
    )};

    glm::mat4 model {glm::mat4(1.0f)};

    glm::mat4 mvp {projection * view * model};
    constexpr char mvp_name[] {"MVP"};
    GLint mvp_matrix_id {glGetUniformLocation(shader, mvp_name)};
    if (glGetError() != GL_NO_ERROR) {
        logs::err("error while getting uniform '", mvp_name, "' location");
    }


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    FPS_manager fps_man;
    while (glfwWindowShouldClose(window) == 0) {
        // ----- input phase -----

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        } else if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            view_pos.x -= 0.05f;
            view = glm::lookAt(
                view_pos,
                glm::vec3 {0, 0, 0},
                glm::vec3 {0, 1, 0}
            );
            mvp = projection * view * model;
        } else if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            view_pos.x += 0.05f;
            view = glm::lookAt(
                view_pos,
                glm::vec3 {0, 0, 0},
                glm::vec3 {0, 1, 0}
            );
            mvp = projection * view * model;
        }

        // ----- update phase -----

        for (auto& vert : cube_vert_colors) {
            vert += 0.001f;
            if (vert > 1.0f) {
                vert -= 1.0f;
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, vert_color_buf_id);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(cube_vert_colors),
            &cube_vert_colors[0],
            GL_STATIC_DRAW);

        // ----- render phase -----

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader);

        // attribute buffer 1 : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vert_buf_id);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // attribute buffer 2 : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vert_color_buf_id);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );

        glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, &mvp[0][0]);
        if (glGetError() != GL_NO_ERROR) {
            logs::err("error while sending mvp");
        }

        glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_verts));
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);

        fps_man.end_frame();
    }

    deinit(window);
    return 0;
}

auto process_args(int argc, char** argv) -> void
{
    std::cout << "args processing is not implemented yet" << std::endl;

    for (int i {0}; i < argc; ++i) {
        std::cout << "arg #" << i << ": " << argv[i] << std::endl;
    }
}

auto init() -> GLFWwindow*
{
    constexpr int opengl_v_maj {3};
    constexpr int opengl_v_min {3};

    std::cout << "init glfw" << std::endl;
    if(!glfwInit()) {
        logs::err("ERROR: glfw init failed!");
        return nullptr;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_v_maj); // we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_v_min); // we want OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MacOS; otherwise should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // no old OpenGL

    Size2d win_res {.w = 1366, .h = 768};

    GLFWwindow* window {
        glfwCreateWindow(win_res.w, win_res.h, "OpenGL Test", NULL, NULL)};
    if (window == nullptr) {
        logs::err(
            "could not create glfw window, is your GPU OpenGL ",
            opengl_v_maj, ".", opengl_v_min, " compatible?");
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true; // needed for core profile
    if (glewInit() != GLEW_OK) {
        logs::err("ERROR: could not initialise glew!");
        return nullptr;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    return window;
}

auto deinit(GLFWwindow* window) -> void
{
    std::cout << "terminating" << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
}
