/*
 * CameraEasing.cpp
 *
 *  Created on: Jan 8, 2011
 *      Author: arturo
 */

#include "CameraEasing.h"
#include "ofMain.h"

CameraEasing::CameraEasing() {
	easing_animated_rot_x.setFrameBasedAnimation();
	easing_animated_rot_y.setFrameBasedAnimation();
	easing_animated_rot_z.setFrameBasedAnimation();
	easing_animated_trans_x.setFrameBasedAnimation();
	easing_animated_trans_y.setFrameBasedAnimation();
	easing_animated_trans_z.setFrameBasedAnimation();
}

CameraEasing::~CameraEasing() {
	// TODO Auto-generated destructor stub
}

void CameraEasing::start(){
	easing_animated_rot_x.setParameters(easing,ofxTween::easeInOut,begin_animated_rot_x,end_animated_rot_x,duration_animated_rot_x,0);
	easing_animated_rot_y.setParameters(easing,ofxTween::easeInOut,begin_animated_rot_y,end_animated_rot_y,duration_animated_rot_y,0);
	easing_animated_rot_z.setParameters(easing,ofxTween::easeInOut,begin_animated_rot_z,end_animated_rot_z,duration_animated_rot_z,0);
	easing_animated_trans_x.setParameters(easing,ofxTween::easeInOut,begin_animated_trans_x,end_animated_trans_x,duration_animated_trans_x,0);
	easing_animated_trans_y.setParameters(easing,ofxTween::easeInOut,begin_animated_trans_y,end_animated_trans_y,duration_animated_trans_y,0);
	easing_animated_trans_z.setParameters(easing,ofxTween::easeInOut,begin_animated_trans_z,end_animated_trans_z,duration_animated_trans_z,0);

	easing_animated_rot_x.start();
	easing_animated_rot_y.start();
	easing_animated_rot_z.start();
	easing_animated_trans_x.start();
	easing_animated_trans_y.start();
	easing_animated_trans_z.start();

}

void CameraEasing::update(){
	animated_rot_x = easing_animated_rot_x.update();
	animated_rot_y = easing_animated_rot_y.update();
	animated_rot_z = easing_animated_rot_z.update();
	animated_trans_x = easing_animated_trans_x.update();
	animated_trans_y = easing_animated_trans_y.update();
	animated_trans_z = easing_animated_trans_z.update();
}

void CameraEasing::rotate(){
	ofRotate(animated_rot_x,1,0,0);
	ofRotate(animated_rot_y,0,1,0);
	ofRotate(animated_rot_z,0,0,1);
}

void CameraEasing::translate(){
	ofTranslate(animated_trans_x,animated_trans_y,animated_trans_z);
}
