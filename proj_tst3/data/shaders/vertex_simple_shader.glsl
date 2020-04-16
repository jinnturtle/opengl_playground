#version 330 core

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec3 vert_color;

out vec3 fragment_color;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vert_pos, 1);

    fragment_color = vert_color;
}
