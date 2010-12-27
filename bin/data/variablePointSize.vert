uniform float focusDistance;
uniform float aperture;
uniform float pointBrightness;
uniform float rgbBrightness;
uniform float maxPointSize;
uniform float minDistance;
uniform float scaleFactor;

const float PI = 3.14159265;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;
	// get the homogeneous 2d position
	float depthFactor = (-gl_Vertex.z + minDistance)*scaleFactor;
	gl_Position = gl_ModelViewProjectionMatrix * vec4((gl_Vertex.x-320.)*depthFactor,(gl_Vertex.y-240.)*depthFactor,gl_Vertex.z,gl_Vertex.w);
	
	// use the distance from the camera and aperture to determine the radius
	// the +1. is because point sizes <1 are rendered differently than those >1
	
	float size = min(abs(gl_Position.z - focusDistance) * aperture + 1.,maxPointSize);
	gl_PointSize = size*4.;
	
	
	gl_FrontColor = gl_Color;
	//float radius = size / 2.;
	// divide the color alpha by the area
	//gl_FrontColor.a /= PI * radius * radius;
	gl_FrontColor.a /= size*size;
	gl_FrontColor.a *= pointBrightness;
	gl_FrontColor.rgb *= rgbBrightness;
}