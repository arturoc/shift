/*
 * Renderer.h
 *
 *  Created on: 27/12/2010
 *      Author: arturo
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "ofMain.h"
#include "ofxVectorMath.h"

class Renderer {
public:
	Renderer():

		T_rgb( 1.9985242312092553e-02f, -7.4423738761617583e-04f, -1.0916736334336222e-02f ),
		//T_rgb(2.1354778990792557e-02, 2.5073334719943473e-03, -1.2922411623995907e-02),
		/*R_rgb(9.9984628826577793e-01, 1.2635359098409581e-03,
				-1.7487233004436643e-02, -1.4779096108364480e-03,
				9.9992385683542895e-01, -1.2251380107679535e-02,
				1.7470421412464927e-02, 1.2275341476520762e-02,
				9.9977202419716948e-01)*/
		R_rgb(9.9984628826577793e-01f, 1.2635359098409581e-03f, -1.7487233004436643e-02f, 0,
			 -1.4779096108364480e-03f, 9.9992385683542895e-01f, -1.2251380107679535e-02f, 0,
			  1.7470421412464927e-02f, 1.2275341476520762e-02f, 9.9977202419716948e-01f, 0,
			  0,0,0,1)
		/*R_rgb(9.9977321644139494e-01, 1.7292658422779497e-03, -2.1225581878346968e-02, 0,
			 -2.0032487074002391e-03, 9.9991486643051353e-01, -1.2893676196675344e-02, 0,
			  2.1201478274968936e-02, 1.2933272242365573e-02,  9.9969156632836553e-01, 0,
			  0.					, 0.					,  0.					 , 1)*/
	{
		depthThreshold 	= 1000;
		useDepthFactor  = true;
		width = 640;
		height = 480;
		halfW = (width*0.5);
		halfH = (height*0.5);

		R_rgb.preMultTranslate(-T_rgb);
		R_rgb = ofxMatrix4x4::getTransposedOf(R_rgb);
	}
	virtual ~Renderer(){};

	virtual void setup()=0;

	virtual void update(float * vertexes, int w,int h)=0;
	virtual void update(float * vertexes, unsigned char* rgb, int w,int h)=0;
	virtual void draw(ofTexture * tex=NULL)=0;

	inline ofPoint getRealWorldCoordinates(float x, float y, float z){
		ofPoint result;
		//const double depth = z/100.0;
		result.x = float((x - cx_d) * z * fx_d);
		result.y = float((y - cy_d) * z * fy_d);
		result.z = -z;

		return result;
	}

	inline ofPoint getCalibratedRGBCoords(ofPoint texcoord3d){
		//calibration method from:  http://nicolas.burrus.name/index.php/Research/KinectCalibration
		ofxVec2f texcoord2d;
		texcoord3d = R_rgb * texcoord3d;
		const float invZ = 1/ -texcoord3d.z;
		texcoord2d.x = ofClamp(round(texcoord3d.x * fx_rgb *invZ) + cx_rgb,0,639);
		texcoord2d.y = ofClamp(round(texcoord3d.y * fy_rgb *invZ) + cy_rgb,0,479);

		/*int pos = int(texcoord2d.y)*640*3+int(texcoord2d.x)*3;
		texcoord3d.set(x,y,0);
		texcoord3d = rgbDepthMatrix * texcoord3d;*/
		return texcoord2d;
	}


	float depthThreshold;
	bool  useDepthFactor;
	float width, height;
	bool depthToGray;
	int minimumGray;

protected:
	float halfW, halfH;
	static const double fx_d = 1.0 / 5.9421434211923247e+02;
	static const double fy_d = 1.0 / 5.9104053696870778e+02;
	static const double cx_d = 3.3930780975300314e+02;
	static const double cy_d = 2.4273913761751615e+02;
	static const double fx_rgb = 5.2921508098293293e+02;
	static const double fy_rgb = 5.2556393630057437e+02;
	static const double cx_rgb = 3.2894272028759258e+02;
	static const double cy_rgb = 2.6748068171871557e+02;
	ofxVec3f T_rgb;
	ofxMatrix4x4 R_rgb;
};

#endif /* RENDERER_H_ */
