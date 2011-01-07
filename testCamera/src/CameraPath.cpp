/*
 * CameraPath.cpp
 *
 *  Created on: Jan 7, 2011
 *      Author: arturo
 */

#include "CameraPath.h"

CameraPath::CameraPath() {
	// TODO Auto-generated constructor stub

}

CameraPath::~CameraPath() {
	// TODO Auto-generated destructor stub
}

void CameraPath::addPoint(const ofPoint & point){
	va.addVertex(point);
}

void CameraPath::drawPath(){
	glPointSize(5);
	ofBeginShape();
	for(int i=0; i<va.getVertexes().size(); i++){
		ofCurveVertex(va.getVertexes()[i].x,va.getVertexes()[i].y,va.getVertexes()[i].z);
	}
	ofEndShape(false);

	va.draw(GL_POINTS);
}

void CameraPath::placeCamera(int index){
	if(index==-1){
		if(va.getVertexes().size()==0) return;
		index = va.getVertexes().size()-1;
	}
	if(index>0)
		gluLookAt(va.getVertexes()[index].x,va.getVertexes()[index].y,va.getVertexes()[index].z,
					va.getVertexes()[index-1].x,va.getVertexes()[index-1].y,va.getVertexes()[index-1].z,
					0,1,0);
	else
		gluLookAt(va.getVertexes()[index].x,va.getVertexes()[index].y,va.getVertexes()[index].z,
					va.getVertexes()[index].x,va.getVertexes()[index].y,va.getVertexes()[index].z-100,
					0,1,0);

}

int CameraPath::getNumPoints(){
	return va.getVertexes().size();
}
