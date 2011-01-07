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
#include "ofxKinectCalibration.h"

class Renderer {
public:
	Renderer()
	{
		depthThreshold 	= 1000;
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

	float depthThreshold;
	bool  useDepthFactor;
	float width, height;
	bool depthToGray;
	int minimumGray;

protected:
	float halfW, halfH;
	ofxKinectCalibration calibration;
};

#endif /* RENDERER_H_ */
