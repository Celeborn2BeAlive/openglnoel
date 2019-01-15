

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

out fcolor;

void main() {
	fcolor=normalize(vViewSpaceNormal);
}