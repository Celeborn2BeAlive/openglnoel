#version 330

in vec3 vPosition_vs;
in vec3 vNormal_vs; 
in vec2 vTexCoords;

out vec3 fFragColor;

void main() {

 fFragColor = normalize(vNormal_vs);

};
