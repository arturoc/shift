/*
 * CameraEasing.h
 *
 *  Created on: Jan 8, 2011
 *      Author: arturo
 */

#ifndef CAMERAEASING_H_
#define CAMERAEASING_H_

class CameraEasing {
public:
	CameraEasing();
	virtual ~CameraEasing();

	void setup();
	void update();
	void rotate();
	void translate();


	//--------------------------------------------------------------
	float makeAnimation(float begin_value, float end_value, float speed){
		if(begin_value < end_value) begin_value += speed;
		else if(begin_value > end_value) begin_value -= speed;
		else if(begin_value == end_value) begin_value = begin_value;
		return begin_value;
	}

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

	float speed_animated_rot_x;
	float speed_animated_rot_y;
	float speed_animated_rot_z;
	float speed_animated_trans_x;
	float speed_animated_trans_y;
	float speed_animated_trans_z;

	float easing_animated_rot_x;
	float easing_animated_rot_y;
	float easing_animated_rot_z;
	float easing_animated_trans_x;
	float easing_animated_trans_y;
	float easing_animated_trans_z;

	float animated_rot_x;
	float animated_rot_y;
	float animated_rot_z;
	float animated_trans_x;
	float animated_trans_y;
	float animated_trans_z;

	float begin_value;
	float end_value;
	float speed;
};

#endif /* CAMERAEASING_H_ */
