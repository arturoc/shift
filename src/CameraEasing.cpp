/*
 * CameraEasing.cpp
 *
 *  Created on: Jan 8, 2011
 *      Author: arturo
 */

#include "CameraEasing.h"
#include "ofMain.h"

CameraEasing::CameraEasing() {




}

CameraEasing::~CameraEasing() {
	// TODO Auto-generated destructor stub
}

void CameraEasing::update(){
	if (begin_animated_rot_x != end_animated_rot_x)
	{
		begin_animated_rot_x = makeAnimation(begin_animated_rot_x, end_animated_rot_x, speed_animated_rot_x);
		speed_animated_rot_x *= easing_animated_rot_x;
		animated_rot_x = begin_animated_rot_x;
	}

	if (begin_animated_rot_y != end_animated_rot_y)
	{
		begin_animated_rot_y = makeAnimation(begin_animated_rot_y, end_animated_rot_y, speed_animated_rot_y);
		speed_animated_rot_y *= easing_animated_rot_y;
		animated_rot_y = begin_animated_rot_y;
	}

	if (begin_animated_rot_z != end_animated_rot_z)
	{
		begin_animated_rot_z = makeAnimation(begin_animated_rot_z, end_animated_rot_z, speed_animated_rot_z);
		speed_animated_rot_z *= easing_animated_rot_z;
		animated_rot_z = begin_animated_rot_z;
	}

	if (begin_animated_trans_x != end_animated_trans_x)
	{
		begin_animated_trans_x = makeAnimation(begin_animated_trans_x, end_animated_trans_x, speed_animated_trans_x);
		speed_animated_trans_x *= easing_animated_trans_x;
		animated_trans_x = begin_animated_trans_x;
	}

	if (begin_animated_trans_y != end_animated_trans_y)
	{
		begin_animated_trans_y = makeAnimation(begin_animated_trans_y, end_animated_trans_y, speed_animated_trans_y);
		speed_animated_trans_y *= easing_animated_trans_y;
		animated_trans_y = begin_animated_trans_y;
	}

	if (begin_animated_trans_z != end_animated_trans_z)
	{
		begin_animated_trans_z = makeAnimation(begin_animated_trans_z, end_animated_trans_z, speed_animated_trans_z);
		speed_animated_trans_z *= easing_animated_trans_z;
		animated_trans_z = begin_animated_trans_z;
	}

}

void CameraEasing::rotate(){
	ofRotate(animated_rot_x,1,0,0);
	ofRotate(animated_rot_y,0,1,0);
	ofRotate(animated_rot_z,0,0,1);
}

void CameraEasing::translate(){
	ofTranslate(animated_trans_x,animated_trans_y,animated_trans_z);
}
