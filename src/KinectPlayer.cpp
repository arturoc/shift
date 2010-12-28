/*
 * KinectPlayer.cpp
 *
 *  Created on: 14/12/2010
 *      Author: arturo
 */

#include "KinectPlayer.h"

KinectPlayer::KinectPlayer() {
	f = 0;
	buf = 0;
	depthBuffer = 0;
	distanceBuffer = 0;
	rgb = 0;
	bUseTexture = true;
}

KinectPlayer::~KinectPlayer() {
	close();

	if(buf) delete[] buf;
	if(distanceBuffer) delete[] distanceBuffer;
	if(depthBuffer) delete[] depthBuffer;
	if(rgb) delete[] rgb;
}

void KinectPlayer::setUseTexture(bool _bUseTexture){
	bUseTexture = _bUseTexture;
}

void KinectPlayer::setup(const string & file){
	f = fopen(ofToDataPath(file).c_str(), "rb");
	if(!buf) buf 		= new uint16_t[640*480];
	if(!distanceBuffer) distanceBuffer = new float[640*480];
	if(!depthBuffer) depthBuffer = new unsigned char[640*480];
	if(!rgb) rgb = new unsigned char[640*480*3];
	memset(rgb,255,640*480*3);
	if(!tex.bAllocated())
		tex.allocate(640,480,GL_LUMINANCE);
}

void KinectPlayer::update(){
	if(!f) return;
	fread(buf,640*480*sizeof(uint16_t),1,f);
	uint16_t *buf_ptr = buf;
	unsigned char* depth_ptr = depthBuffer;
	float * distance_ptr 	 = distanceBuffer;
	float depth;

	for ( int y = 0; y < 480; y++) {
		for ( int x = 0; x < 640; x++) {
			if(*buf_ptr == 2047) {
				depth = 0;
				*depth_ptr++ = 0;
			} else {
				// using equation from https://github.com/OpenKinect/openkinect/wiki/Imaging-Information
				depth = 100.f / (-0.00307f * *buf_ptr + 3.33f);
				*depth_ptr++ = (float) (2048 * 256) / (2048 - *buf_ptr);
			}

			buf_ptr++;
			*distance_ptr++ = depth;
		}
	}

	if(bUseTexture)
		tex.loadData(depthBuffer,640,480,GL_LUMINANCE);

}

void KinectPlayer::draw(float x, float y){
	tex.draw(x,y);
}

void KinectPlayer::draw(float x, float y, float w, float h){
	tex.draw(x,y,w,h);
}

unsigned char * KinectPlayer::getPixels(){
	return rgb;
}

unsigned char * KinectPlayer::getDepthPixels(){
	return depthBuffer;
}

float * KinectPlayer::getDistancePixels(){
	return distanceBuffer;
}

unsigned char * KinectPlayer::getCalibratedRGBPixels(){
	return rgb;
}


ofTexture & KinectPlayer::getTextureReference(){
	return tex;
}

float KinectPlayer::getHeight(){
	return 480;
}

float KinectPlayer::getWidth(){
	return 640;
}

bool KinectPlayer::isFrameNew(){
	return true;
}

void KinectPlayer::close(){
	if(f)
		fclose(f);
	f = 0;
}
