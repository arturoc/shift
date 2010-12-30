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

class KinectPlayer: public of3DVideo {
public:
	KinectPlayer();
	virtual ~KinectPlayer();

	void setup(const string & file);
	void update();
	void draw(float x, float y);
	void draw(float , float y, float w, float h);
	void draw(const ofPoint & point);
	void draw(const ofRectangle & rect);

	ofTexture & getTextureReference();
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

private:
	FILE * f;
	uint16_t *buf;
	float *distanceBuffer;
	unsigned char *depthBuffer;
	ofTexture tex;
	bool bUseTexture;
	unsigned char *rgb;
};

#endif /* KINECTPLAYER_H_ */
