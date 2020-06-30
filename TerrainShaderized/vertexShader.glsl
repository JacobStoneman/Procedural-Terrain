#version 420 core

layout(location=0) in vec4 Coords;
layout(location=1) in vec3 Normals;
layout(location=2) in vec2 TexCoords;

uniform vec4 globAmb;
uniform mat4 projMat;
uniform mat4 modelMat;
uniform mat4 ViewMat;
uniform mat3 normalMat;
uniform int objNum;
uniform float time;
uniform vec3 camPos;

out vec2 texCoordsExport;
out vec3 skyTexCoords;
out vec3 normalExport;
out vec4 coordsExport;
out vec3 camPosExport;

void main(void)
{
	vec4 pos = Coords;
	if(objNum == 1){
		pos.y += 0.2f * (sin(pos.x + time) + cos(pos.z + time));
	}
	coordsExport = pos;
   normalExport = Normals;
   normalExport = normalize(normalMat * normalExport);
   texCoordsExport = TexCoords;
   gl_Position = projMat * modelMat * ViewMat * pos;

   	if(objNum == 10){
		pos = projMat * ViewMat * vec4(Coords.x + camPos.x, Coords.y + camPos.y, Coords.z + camPos.z, 1.0f);
		gl_Position = pos.xyww;
		skyTexCoords = vec3(Coords.x,Coords.y,Coords.z);
	}
}