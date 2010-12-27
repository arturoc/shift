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

	inline float getDepthFactor(float depth){
		return (depth + minDistance) * scaleFactor;
	}


	float depthThreshold;
	float minDistance;
	float scaleFactor;
	bool  useDepthFactor;
	float width, height;

protected:
	float halfW, halfH;
};

#endif /* RENDERER_H_ */
