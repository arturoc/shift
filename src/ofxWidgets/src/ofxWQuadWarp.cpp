/*
 * ofxWQuadWarp.cpp
 *
 *  Created on: 27/06/2010
 *      Author: arturo
 */

#include "ofxWQuadWarp.h"

ofxWQuadWarp::ofxWQuadWarp()
:ofxWButton("quad_warp")
{
	moveOrigin = false;
	resize = false;
	keepAspect = false;

	for(int i=0; i<4; i++){
		buttons[i] = new ofxWCircleButton("button" + ofToString(i));
	}
}

void ofxWQuadWarp::init(ofRectangle origin){
	ofxWButton::init("");
	this->origin = origin;

	src[0].x = origin.x;
	src[0].y = origin.y;

	src[1].x = origin.x + origin.width;
	src[1].y = origin.y;

	src[2].x = origin.x + origin.width;
	src[2].y = origin.y + origin.height;

	src[3].x = origin.x;
	src[3].y = origin.y + origin.height;

	for(int i=0; i<4; i++){
		dst[i] = src[i];
		buttons[i]->init("");
		ofAddListener(buttons[i]->boolEvent,this,&ofxWQuadWarp::buttonPressed);
	}

	aspectRatio = origin.width/origin.height;
	inited = true;
}


void ofxWQuadWarp::reshape(ofRectangle origin){
	ofxWButton::init("");
		this->origin = origin;

		src[0].x = origin.x;
		src[0].y = origin.y;

		src[1].x = origin.x + origin.width;
		src[1].y = origin.y;

		src[2].x = origin.x + origin.width;
		src[2].y = origin.y + origin.height;

		src[3].x = origin.x;
		src[3].y = origin.y + origin.height;

		for(int i=0; i<4; i++){
			dst[i] = src[i];
		}

		aspectRatio = origin.width/origin.height;
}

void ofxWQuadWarp::render(ofxWidgetsStyle & style){
	ofPushStyle();
	ofNoFill();
	ofSetColor(style.border.color.r,style.border.color.g,style.border.color.b);

	ofBeginShape();
	for(int i=0;i<4; i++){
		ofVertex(dst[i].x,dst[i].y);
	}
	ofEndShape(true);

	ofBeginShape();
	for(int i=0;i<4; i++){
		ofVertex(src[i].x,src[i].y);
	}
	ofEndShape(true);


	ofPopStyle();
}

ofRectangle ofxWQuadWarp::getActiveArea(ofxWidgetsStyle & style){
	return ofRectangle(dst[0].x,dst[0].y,dst[1].x - dst[0].x,dst[2].y - dst[1].y);
}

ofRectangle ofxWQuadWarp::getTotalArea(ofxWidgetsStyle & style){
	return getActiveArea(style);
}

void ofxWQuadWarp::update(){
	ofxWButton::update();
	for(int i=0; i<4; i++){
		if(buttons[i]==activeButton){
			if(moveOrigin){
				src[i] = mousePos;
				if(resize){
					switch(i){
					case 0:
						src[1].y=src[i].y;
						src[3].x=src[i].x;
						break;
					case 1:
						src[0].y=src[i].y;
						src[2].x=src[i].x;
						break;
					case 2:
						src[3].y=src[i].y;
						src[1].x=src[i].x;
						break;
					case 3:
						src[2].y=src[i].y;
						src[0].x=src[i].x;
						break;
					}
				}
			}

			dst[i] = mousePos;
			if(resize){
				switch(i){
				case 0:
					dst[1].y=dst[i].y;
					dst[3].x=dst[i].x;
					if(keepAspect){
						float xdist = fabs(dst[1].x-dst[i].x);
						float ydist = xdist / aspectRatio;
						dst[2].y = dst[i].y + ydist;
						dst[3].y = dst[i].y + ydist;
					}
					break;
				case 1:
					dst[0].y=dst[i].y;
					dst[2].x=dst[i].x;
					if(keepAspect){
						float xdist = fabs(dst[0].x-dst[i].x);
						float ydist = xdist / aspectRatio;
						dst[2].y = dst[i].y + ydist;
						dst[3].y = dst[i].y + ydist;
					}
					break;
				case 2:
					dst[3].y=dst[i].y;
					dst[1].x=dst[i].x;
					if(keepAspect){
						float xdist = fabs(dst[3].x-dst[i].x);
						float ydist = xdist / aspectRatio;
						dst[0].y = dst[i].y - ydist;
						dst[1].y = dst[i].y - ydist;
					}
					break;
				case 3:
					dst[2].y=dst[i].y;
					dst[0].x=dst[i].x;
					if(keepAspect){
						float xdist = fabs(dst[2].x-dst[i].x);
						float ydist = xdist / aspectRatio;
						dst[0].y = dst[i].y - ydist;
						dst[0].y = dst[i].y - ydist;
					}
					break;
				}
			}
		}
		buttons[i]->setPosition(dst[i]);
	}
	if(getValueB() && !activeButton){
		//cout << "changing pos" << endl;
		ofPoint mouseDiff = mousePos - prevMousePos;
		for(int i=0; i<4; i++){
			//src[i]+=mouseDiff;
			if(moveOrigin){
				src[i] += mouseDiff;
			}
			dst[i]+=mouseDiff;
		}
		if(moveOrigin){
			origin.x+=mouseDiff.x;
			origin.y+=mouseDiff.y;
		}
	}
	prevMousePos = mousePos;

}

void ofxWQuadWarp::buttonPressed(const void * sender, bool & pressed){
	if(pressed){
		activeButton = (ofxWCircleButton*)sender;
	}else{
		activeButton = 0;
	}
}


ofxWidgetsState ofxWQuadWarp::manageEvent(ofxWidgetsEvent event, ofxWidgetEventArgs & args, ofxWidgetsState currentState){

	//prevMousePos = mousePos;
	mousePos.x = mouse.x;
	mousePos.y = mouse.y;
	int mod = glutGetModifiers();
	if (mod == GLUT_ACTIVE_CTRL)
		resize=true;
	else
		resize=false;

	mod = glutGetModifiers();
	if (mod == GLUT_ACTIVE_SHIFT){
		resize=true;
		keepAspect=true;
	}else{
		keepAspect=false;
	}
	return ofxWButton::manageEvent(event,args,currentState);
}


void ofxWQuadWarp::setVisible(bool visible){
	if(!inited) return;
	ofxWidget::setVisible(visible);
	for(int i=0;i<4;i++){
		buttons[i]->setVisible(visible);
	}
}

void ofxWQuadWarp::enable(){
	if(!inited) return;
	ofxWidget::enable();
	for(int i=0;i<4;i++){
		buttons[i]->enable();
	}
}

void ofxWQuadWarp::disable(){
	if(!inited) return;
	ofxWidget::disable();
	for(int i=0;i<4;i++){
		buttons[i]->disable();
	}
}


void ofxWQuadWarp::setLocalEventsObject(ofCoreEvents * events){
	ofxWidget::setLocalEventsObject(events);
	for(int i=0;i<4;i++){
		buttons[i]->setLocalEventsObject(events);
	}
}
