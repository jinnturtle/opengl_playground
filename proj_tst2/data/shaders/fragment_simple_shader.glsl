#version 330 core

// interpolated values form vertex shaders
in vec3 fragment_color;

out vec3 color;

void main()
{
    color = fragment_color;
}
