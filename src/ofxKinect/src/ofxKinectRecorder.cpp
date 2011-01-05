/*
 * KinectRecorder.cpp
 *
 *  Created on: 03/01/2011
 *      Author: arturo
 */

#include "ofxKinectRecorder.h"

ofxKinectRecorder::ofxKinectRecorder() {
	f=NULL;
	useCompression=true;
	img.setUseTexture(false);
}

ofxKinectRecorder::~ofxKinectRecorder() {
	// TODO Auto-generated destructor stub
}

void ofxKinectRecorder::init(const string & _filename){
	filename = _filename;
	f = fopen(ofToDataPath(filename).c_str(),"wb");
	if(useCompression) img.allocate(640,480,OF_IMAGE_COLOR);
	frameNum=0;
}

void ofxKinectRecorder::newFrame(unsigned char* rgb, unsigned short * raw_depth){
	if(!f) return;
	if(!useCompression){
		fwrite(rgb,640*480*3,1,f);
	}else{
		memcpy(img.getPixels(),rgb,640*480*3);
		char name[1024];
		sprintf(name,"%s%09i.jpg",filename.c_str(),frameNum);
		img.saveImage(name);
	}
	fwrite(raw_depth,640*480*sizeof(short),1,f);
	frameNum++;
}

void ofxKinectRecorder::close(){
	if(!f) return;
	fclose(f);
	f=0;
}

bool ofxKinectRecorder::isOpened(){
	return f;
}

void ofxKinectRecorder::setUseCompression(bool compress){
	useCompression = compress;
}

bool ofxKinectRecorder::isUsingCompression(){
	return useCompression;
}
