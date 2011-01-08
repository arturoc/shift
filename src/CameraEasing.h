/*
 * CameraEasing.h
 *
 *  Created on: Jan 8, 2011
 *      Author: arturo
 */

#ifndef CAMERAEASING_H_
#define CAMERAEASING_H_

#include "ofxTween.h"

class CameraEasing {
public:
	CameraEasing();
	virtual ~CameraEasing();

	void start();
	void update();
	void rotate();
	void translate();


	float begin_animated_rot_x;
	float begin_animated_rot_y;
	float begin_animated_rot_z;
	float begin_animated_trans_x;
	float begin_animated_trans_y;
	float begin_animated_trans_z;

	float end_animated_rot_x;
	float end_animated_rot_y;
	float end_animated_rot_z;
	float end_animated_trans_x;
	float end_animated_trans_y;
	float end_animated_trans_z;

	float duration_animated_rot_x;
	float duration_animated_rot_y;
	float duration_animated_rot_z;
	float duration_animated_trans_x;
	float duration_animated_trans_y;
	float duration_animated_trans_z;

private:
	ofxTween easing_animated_rot_x;
	ofxTween easing_animated_rot_y;
	ofxTween easing_animated_rot_z;
	ofxTween easing_animated_trans_x;
	ofxTween easing_animated_trans_y;
	ofxTween easing_animated_trans_z;

	ofxEasingSine easing;

	float animated_rot_x;
	float animated_rot_y;
	float animated_rot_z;
	float animated_trans_x;
	float animated_trans_y;
	float animated_trans_z;

};

#endif /* CAMERAEASING_H_ */
