#ifndef SRC_UTILS_HPP_
#define SRC_UTILS_HPP_

/*******************************************************************************
 * This file contains miscelanneaus helper utilities.
 *
 * Things that can be grouped under a common theme or functionality should be
 * moved to dedicated header/source files as this is meant for miscellaneous
 * things that are only one or two of a kind.
 ******************************************************************************/

#include <GL/glew.h>

struct Pos2 {
    int x;
    int y;
};

struct Pos2d {
    double x;
    double y;
};

struct Size2 {
    int w;
    int h;
};

auto load_shaders(
    const char* vertex_file_path,
    const char* fragment_file_path) -> GLuint;

#endif // SRC_UTILS_HPP_
