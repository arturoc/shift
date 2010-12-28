/*
 * Renderer.h
 *
 *  Created on: 27/12/2010
 *      Author: arturo
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "ofMain.h"

class Renderer {
public:
	Renderer(){
		depthThreshold 	= 1000;
		minDistance 	= -10;
		scaleFactor 	= .0021;
		useDepthFactor  = true;
		width = 640;
		height = 480;
		halfW = (width*0.5);
		halfH = (height*0.5);
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


	float depthThreshold;
	float minDistance;
	float scaleFactor;
	bool  useDepthFactor;
	float width, height;

protected:
	float halfW, halfH;
	static const double fx_d = 1.0 / 5.9421434211923247e+02;
	static const double fy_d = 1.0 / 5.9104053696870778e+02;
	static const double cx_d = 3.3930780975300314e+02;
	static const double cy_d = 2.4273913761751615e+02;
};

#endif /* RENDERER_H_ */
