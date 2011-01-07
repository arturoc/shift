/*
 * CameraPath.h
 *
 *  Created on: Jan 7, 2011
 *      Author: arturo
 */

#ifndef CAMERAPATH_H_
#define CAMERAPATH_H_

#include "ofMain.h"
#include "VertexArray.h"

class CameraPath {
public:
	CameraPath();
	virtual ~CameraPath();

	void addPoint(const ofPoint & point);
	void drawPath();

	void placeCamera(int index=-1);
	int getNumPoints();

private:
	vector<ofPoint> points;
	VertexArray va;
};

#endif /* CAMERAPATH_H_ */
