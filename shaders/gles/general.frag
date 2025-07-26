#version 300 es
precision mediump float;
precision highp int;

flat in uint vertexType;
in vec4 vertexColor;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    // Calculate BOTH possible outcomes for every pixel
    vec4 shapeColor = vertexColor;
    vec4 textColor  = vec4(vertexColor.rgb, vertexColor.a * texture(uTexture, TexCoord).r);

    // Select the correct outcome using arithmetic instead of a branch.
    // float(vertexType) will be 0.0 for shapes and 1.0 for text.
    FragColor = mix(shapeColor, textColor, float(vertexType));
}