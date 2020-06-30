#version 430 core

in vec3 normalExport;
in vec2 texCoordsExport;
in vec3 skyTexCoords;
in vec4 coordsExport;
out vec4 colorsExport;

struct Light
{
   vec4 ambCols;
   vec4 difCols;
   vec4 specCols;
   vec4 coords;
};
uniform Light light0;

uniform vec4 globAmb;
  
struct Material
{
   vec4 ambRefl;
   vec4 difRefl;
   vec4 specRefl;
   vec4 emitCols;
   float shine;
};
uniform float time;
uniform Material terrainFandB;
uniform sampler2D grassTex;
uniform sampler2D waterTex;
uniform sampler2D rockTex;
uniform sampler2D sandTex;
uniform sampler2D snowTex;
uniform samplerCube skybox;

uniform sampler2D frontTex;
uniform sampler2D backTex;
uniform sampler2D leftTex;
uniform sampler2D rightTex;
uniform sampler2D upTex;
uniform sampler2D downTex;
in vec3 camPosExport;

uniform int objNum;
uniform sampler2D tex;

const float partOf1 = 0.5f;

vec3 normal, lightDirection;
vec4 fAndBDif, texColour, grassCol, waterCol, rockCol,sandCol,snowCol;

void main(void)
{
	texColour = texture(tex, texCoordsExport);
	grassCol = texture(grassTex, texCoordsExport);
	waterCol = texture(waterTex, texCoordsExport);
	rockCol = texture(rockTex, texCoordsExport);
	sandCol = texture(sandTex, texCoordsExport);
	snowCol = texture(snowTex, texCoordsExport);

   normal = normalize(normalExport);
   lightDirection = normalize(vec3(light0.coords));
   fAndBDif = max(dot(normal, lightDirection), 0.0f) * (light0.difCols * terrainFandB.difRefl); 
   colorsExport = vec4(vec3(min(fAndBDif * texColour, vec4(1.0))), 1.0); 
   if(objNum == 0){

		if(coordsExport.y <= 0){
			colorsExport = vec4(vec3(min(fAndBDif * sandCol, vec4(1.0))), 1.0);
		}
		if(coordsExport.y < 1 && coordsExport.y > 0){
			vec4 newCol = mix(sandCol, grassCol, coordsExport.y);
			colorsExport =  vec4(vec3(min(fAndBDif * newCol, vec4(1.0))), 1.0);
		}
		if (coordsExport.y >= 1 && coordsExport.y <= 20){
			vec4 newCol = mix(grassCol, rockCol, coordsExport.y / 20);
			colorsExport =  vec4(vec3(min(fAndBDif * newCol, vec4(1.0))), 1.0);
		} else if (coordsExport.y > 20 && coordsExport.y < 35){
		colorsExport =  vec4(vec3(min(fAndBDif * rockCol, vec4(1.0))), 1.0);
		} else if (coordsExport.y >= 35 && coordsExport.y <= 40){
		float num = (coordsExport.y - 35)/5;
		vec4 newCol = mix(rockCol, snowCol, num);
		colorsExport =  vec4(vec3(min(fAndBDif * newCol, vec4(1.0))), 1.0);
		} else if (coordsExport.y > 40){
		colorsExport = vec4(vec3(min(fAndBDif * snowCol, vec4(1.0))), 1.0);
		}

	}
  else if(objNum == 1){
		colorsExport = vec4(vec3(min(fAndBDif * waterCol, vec4(1.0))), 0.7f); 
   }
   else if (objNum == 2){
		colorsExport = vec4(0.54f,0.27f,0.07,1.0f);
   } else if (objNum == 3){
		colorsExport = vec4(0,1,0,1);
	} else if (objNum == 4){
		colorsExport  = texture(frontTex, texCoordsExport);
		} else if (objNum == 5){
		colorsExport  = texture(leftTex, texCoordsExport);
		} else if (objNum == 6){
		colorsExport  = texture(backTex, texCoordsExport);
		} else if (objNum == 7){
		colorsExport  = texture(rightTex, texCoordsExport);
		} else if (objNum == 8){
		colorsExport  = texture(upTex, texCoordsExport);
		} else if (objNum == 9){
		colorsExport  = texture(downTex, texCoordsExport);
	} else if(objNum == 10){
		colorsExport = texture(skybox, skyTexCoords);
	}
}