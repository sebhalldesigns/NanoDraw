#version 330 core
flat in uint vertexType;
in vec4 vertexColor;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    switch (vertexType) 
    {
        case 1:
        {
            FragColor = vertexColor * texture(uTexture, TexCoord);
        } break;

        default:
        {
            FragColor = vertexColor;
        } break;
    }
}