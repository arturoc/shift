/*
 * KinectRecorder.cpp
 *
 *  Created on: 03/01/2011
 *      Author: arturo
 */

#include "KinectRecorder.h"

KinectRecorder::KinectRecorder() {
	f=NULL;

}

KinectRecorder::~KinectRecorder() {
	// TODO Auto-generated destructor stub
}

void KinectRecorder::init(const string & filename){
	f = fopen(ofToDataPath(filename).c_str(),"wb");
}

void KinectRecorder::newFrame(unsigned char* rgb, unsigned short * raw_depth){
	if(!f) return;
	fwrite(rgb,640*480*3,1,f);
	fwrite(raw_depth,640*480*sizeof(short),1,f);
}

void KinectRecorder::close(){
	if(!f) return;
	fclose(f);
	f=0;
}

bool KinectRecorder::isOpened(){
	return f;
}
