#version 330 core

in vec3 interpolatedColor;

out vec4 fCol;

void main()
{
    fCol = vec4(interpolatedColor, 1);
}
