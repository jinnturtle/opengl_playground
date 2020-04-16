#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>

#include "FPS_manager.hpp"
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

    GLuint shader = load_shaders(
        "data/shaders/vertex_simple_shader.glsl",
        "data/shaders/fragment_simple_shader.glsl");
    if (shader == 0) {
        logs::err("errors while loading shaders");
        deinit(window);
        return -1;
    }
    glUseProgram(shader);

    static constexpr GLfloat vert_buf[] {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

    GLuint vert_buf_id;
    glGenBuffers(1, &vert_buf_id);
    glBindBuffer(GL_ARRAY_BUFFER, vert_buf_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_buf), vert_buf, GL_STATIC_DRAW);

    glm::mat4 projection {glm::perspective(
        glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f)};

    // camera matrix
    glm::mat4 view {glm::lookAt(
        glm::vec3 {4, 3, 3},
        glm::vec3 {0, 0, 0},
        glm::vec3 {0, 1, 0}
    )};

    glm::mat4 model {glm::mat4(1.0f)};

    glm::mat4 mvp {projection * view * model};
    constexpr char mvp_name[] {"MVP"};
    GLint mvp_matrix_id {glGetUniformLocation(shader, mvp_name)};
    if (glGetError() != GL_NO_ERROR) {
        logs::err("error while getting uniform '", mvp_name, "' location");
    }

    glm::vec3 view_pos {4.0f, 3.0f, 3.0f};
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

        // ----- render phase -----

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1st attribute buffer : vertices
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

        glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, &mvp[0][0]);
        if (glGetError() != GL_NO_ERROR) {
            logs::err("error while sending mvp");
        }

        glDrawArrays(GL_TRIANGLES, 0, 3);
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
