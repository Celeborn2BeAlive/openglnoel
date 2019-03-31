#version 330

uniform sampler2D uGPosition;
uniform vec3 uSceneSize; // Scene size in view space (coordinate of top right view frustum corner)

out vec3 fColor;

void main()
{
    fColor = vec3(texelFetch(uGPosition, ivec2(gl_FragCoord.xy), 0)) / uSceneSize; // Since the depth is between 0 and 1, pow it to darkness its value
}