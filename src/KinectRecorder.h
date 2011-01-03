/*
 * KinectRecorder.h
 *
 *  Created on: 03/01/2011
 *      Author: arturo
 */

#ifndef KINECTRECORDER_H_
#define KINECTRECORDER_H_

#include "ofMain.h"

class KinectRecorder {
public:
	KinectRecorder();
	virtual ~KinectRecorder();

	void init(const string & filename);
	void newFrame(unsigned char* rgb, unsigned short * raw_depth);
	void close();

	bool isOpened();

private:
	FILE * f;
};

#endif /* KINECTRECORDER_H_ */
