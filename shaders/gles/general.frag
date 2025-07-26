#version 300 es
precision highp float;
precision highp int;
precision highp usampler2D;

flat in uint vertexType;
in vec4 vertexColor;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    // Calculate BOTH possible outcomes for every pixel
    vec4 shapeColor = vertexColor;
    vec4 textColor  = vertexColor * texture(uTexture, TexCoord);

    // Select the correct outcome using arithmetic instead of a branch.
    // float(vertexType) will be 0.0 for shapes and 1.0 for text.
    FragColor = mix(shapeColor, textColor, float(vertexType));
}