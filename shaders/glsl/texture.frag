#version 330 core
in vec4 vertexColor;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    // Combine texture color with vertex color for tinting
    FragColor = vertexColor * texture(uTexture, TexCoord);
}