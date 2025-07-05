#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor; // Can be used to tint the texture
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 TexCoord;

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(aPos.x, aPos.y, 0.0, 1.0);
    vertexColor = aColor;
    TexCoord = aTexCoord;
}