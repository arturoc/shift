/*
 * SCRenderer.h
 *
 *  Created on: Jan 6, 2011
 *      Author: arturo
 */

#ifndef SCRENDERER_H_
#define SCRENDERER_H_

#include "Renderer.h"
#include "ofxOpenCv.h"
#include "VertexArray.h"
#include "ofxShader.h"
#include <deque>
#include "Poco/Mutex.h"

class SCRenderer: public Renderer {
public:
	SCRenderer();
	virtual ~SCRenderer();

	void setup();

	void update(unsigned char * depth, int w, int h);
	void update(float * vertexes, int w,int h);
	void update(float * vertexes, unsigned char* rgb, int w,int h);
	void update(float * vertexes, ofPoint * texcoords, int w,int h);
	void draw(ofTexture * tex=NULL);

	void audioReceived( float * input, int bufferSize, int nChannels );


	float 				nearThreshold;
	float				farThreshold;

	int		 			numSoundBuffers;
	float 				soundDepthFactor;

	int 				oneInX, oneInY;

	bool				dof;
	bool				mesh;
	float 				focusDistance;
	float 				aperture;
	float 				pointBrightness;
	float 				rgbBrightness;
	float 				maxPointSize;
	float 				pointSizeFactor;

	float 				objectDepthThreshold;

private:
	ofxCvGrayscaleImage 	grayImage;
	ofxCvGrayscaleImage 	grayThresh;
	ofxCvGrayscaleImage 	grayThreshFar;

	ofxCvContourFinder 	contourFinder;

	VertexArray			va;

	deque<float*> soundBuffers;
	int totalSoundLength;

	ofxShader shader;

	Poco::FastMutex mutex;
};

#endif /* SCRENDERER_H_ */
