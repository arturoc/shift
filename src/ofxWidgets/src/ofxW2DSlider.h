/*
 * ofxYAButton.h
 *
 *  Created on: 19-feb-2009
 *      Author: arturo castro
 */
#pragma once

#include "ofxWTypes.h"
#include "ofxWidget.h"
#include "ofxWStyleLoader.h"
#include "ofMain.h"


class ofxW2DSlider: public ofxWidget{
public:

	float  xvalue, yvalue;

	ofxW2DSlider(const string & name);

	void init(const string & _title, float * xvalue, float * yvalue, float _xmin, float _xmax, float _ymin, float _ymax, const string & style);
	void init(const string & _title, int * xvalue, int * yvalue, int _xmin, int _xmax, int _ymin, int _ymax, const string & style);
	void init(const string & _title, float _xvalue, float _yvalue, float _xmin, float _xmax, float _ymin, float _ymax, const string & style);
	void init(const string & _title, int _xvalue, int _yvalue, int _xmin, int _xmax, int _ymin, int _ymax, const string & style);

	void setValue(float x, float y);

	int getValueI();
	float getValueF();
	bool getValueB();

	void update();

	virtual void render(ofxWidgetsStyle & style);

	virtual ofRectangle getActiveArea(ofxWidgetsStyle & style);
	virtual ofRectangle getTotalArea(ofxWidgetsStyle & style);

	// UNFOCUSED -> over -> FOCUSED -> pressed -> PRESSED -> out -> OUT -> released -> UNFOCUSED
	// PRESSED -> released -> FOCUSSED
	// FOCUSED -> out ->  UNFOCUSED
	virtual ofxWidgetsState manageEvent(ofxWidgetsEvent event, ofxWidgetEventArgs & args, ofxWidgetsState currentState);

	void saveTo(ofxXmlSettings & xml, const string & tag);
	void loadFrom(ofxXmlSettings & xml, const string & tag);
	
	
	ofEvent<ofPoint> 	intEvent;
	ofEvent<ofPoint>  	floatEvent;

protected:
	bool intOnly;
	float xmin,xmax;
	float ymin,ymax;

	float * fxtargetValue,*fytargetValue;
	int   * ixtargetValue,*iytargetValue;
};

