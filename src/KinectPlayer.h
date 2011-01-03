/*
 * KinectPlayer.h
 *
 *  Created on: 14/12/2010
 *      Author: arturo
 */

#ifndef KINECTPLAYER_H_
#define KINECTPLAYER_H_

#include "ofMain.h"
#include "of3DVideo.h"
#include "ofxVectorMath.h"

class KinectPlayer: public of3DVideo {
public:
	KinectPlayer();
	virtual ~KinectPlayer();

	void setup(const string & file, bool color=false);
	void update();
	void draw(float x, float y);
	void draw(float , float y, float w, float h);
	void draw(const ofPoint & point);
	void draw(const ofRectangle & rect);

	ofTexture & getTextureReference();
	ofTexture & getDepthTextureReference();
	void setUseTexture(bool bUseTexture);

	float getHeight();
	float getWidth();

	void setAnchorPercent(float xPct, float yPct){};
    void setAnchorPoint(float x, float y){};
	void resetAnchor(){};

	bool isFrameNew();
	void close();

	unsigned char * getPixels();
	unsigned char * getDepthPixels();
	float * getDistancePixels();
	unsigned char * getCalibratedRGBPixels();

	float getDistanceAt(int x, int y) ;
	ofxPoint3f getWorldCoordinateFor(int x, int y);
	ofxPoint3f getWorldCoordinateFor(int x, int y,float z);

	static double fx_d;
	static double fy_d;
	static float cx_d;
	static float cy_d;
	static double fx_rgb;
	static double fy_rgb;
	static float cx_rgb;
	static float cy_rgb;

	int fps;
private:
	void calculateLookups();
	static bool lookupsCalculated;
	static float distancePixelsLookup[2048];
	static unsigned char depthPixelsLookupNearWhite[2048];
	static unsigned char depthPixelsLookupFarWhite[2048];
	FILE * f;
	uint16_t *buf;
	float *distanceBuffer;
	unsigned char *depthBuffer;
	ofTexture tex;
	bool bUseTexture;
	unsigned char *rgb,*calibratedRGBPixels;

	bool readColor;

	ofxVec3f T_rgb;
	ofxMatrix4x4 R_rgb;

	int lastFrameTime;
};

#endif /* KINECTPLAYER_H_ */
