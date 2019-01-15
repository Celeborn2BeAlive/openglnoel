#version 330

in vec2 vTexCoords;

out vec3 fColor;

uniform sampler2D uSampler;

void main()
{
   fColor = vec3(texture(uSampler, vTexCoords));
}