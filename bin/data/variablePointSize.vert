uniform float focusDistance;
uniform float aperture;
uniform float pointBrightness;
uniform float rgbBrightness;
uniform float maxPointSize;
uniform int depthToGray;
uniform float invDepthThres;
uniform float minimumGray;
uniform float pointSizeFactor;

const float PI = 3.14159265;
const float fx_d = 1.0 / 5.9421434211923247e+02;
const float fy_d = 1.0 / 5.9104053696870778e+02;
const float cx_d = 3.3930780975300314e+02;
const float cy_d = 2.4273913761751615e+02;


void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;
	// get the homogeneous 2d position
	
	
	vec4 result;
	//float depth = -gl_Vertex.z/100.0;
	result.x = float((gl_Vertex.x - cx_d) * -gl_Vertex.z * fx_d);
	result.y = float((gl_Vertex.y - cy_d) * -gl_Vertex.z * fy_d);
	result.z = gl_Vertex.z;
	result.w = gl_Vertex.w;
	//float depthFactor = (-gl_Vertex.z + minDistance)*scaleFactor;
	gl_Position = gl_ModelViewProjectionMatrix * result;//vec4((gl_Vertex.x-320.)*depthFactor,(gl_Vertex.y-240.)*depthFactor,gl_Vertex.z,gl_Vertex.w);
	
	// use the distance from the camera and aperture to determine the radius
	// the +1. is because point sizes <1 are rendered differently than those >1
	
	float size = min(abs(gl_Position.z - focusDistance) * aperture + 1.,maxPointSize);
	gl_PointSize = size*pointSizeFactor;
	
	if(depthToGray){
		gl_FrontColor = gl_Color * minimumGray + 1.-clamp(abs(gl_Position.z*invDepthThres),0,1);
	}else{
		gl_FrontColor = gl_Color;
	}
	//float radius = size / 2.;
	// divide the color alpha by the area
	//gl_FrontColor.a /= PI * radius * radius;
	gl_FrontColor.a /= size*size;
	gl_FrontColor.a *= pointBrightness;
	gl_FrontColor.rgb *= rgbBrightness;
}