#version 330

layout(location = 0) in vec2 aPosition;


uniform sampler2D uGPosition;
uniform sampler2D uGNormal;
uniform sampler2D uGAmbient;
uniform sampler2D uGDiffuse;
uniform sampler2D uGlossyShininess;


void main()
{
    gl_Position =  vec4(aPosition, 0, 1);
}