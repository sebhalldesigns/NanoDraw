#version 330 core
layout (location = 0) in uint aType;
layout (location = 1) in vec2 aPos;
layout (location = 2) in vec4 aColor; // Can be used to tint the texture
layout (location = 3) in vec2 aTexCoord;

flat out uint vertexType;
out vec4 vertexColor;
out vec2 TexCoord;

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(aPos.x, aPos.y, 0.0, 1.0);
    vertexColor = aColor;
    TexCoord = aTexCoord;
    vertexType = aType;
}