/*
 * ofxW2DSlider.cpp
 *
 *  Created on: 27/06/2010
 *      Author: arturo
 */

#include "ofxW2DSlider.h"

ofxW2DSlider::ofxW2DSlider(const string & name):ofxWidget(name){
	xvalue 			= 0;
	yvalue 			= 0;
	fxtargetValue 	= 0;
	ixtargetValue 	= 0;
	fytargetValue 	= 0;
	iytargetValue 	= 0;
	title			= "";
	intOnly 		= false;
}

void ofxW2DSlider::init(const string & _title, float * xvalue, float * yvalue, float _xmin, float _xmax, float _ymin, float _ymax, const string & style){
	ofxWidget::init();
	ixtargetValue	= 0;
	fxtargetValue	= xvalue;
	iytargetValue	= 0;
	fytargetValue	= yvalue;
	title			= _title;
	xmin			= _xmin;
	xmax			= _xmax;
	ymin			= _ymin;
	ymax			= _ymax;
	setStyles("slider2D",style);
	intOnly 		= false;
}

void ofxW2DSlider::init(const string & _title, int * xvalue, int * yvalue, int _xmin, int _xmax, int _ymin, int _ymax, const string & style){
	ofxWidget::init();
	fxtargetValue	= 0;
	ixtargetValue	= xvalue;
	fytargetValue	= 0;
	iytargetValue	= yvalue;
	title			= _title;
	xmin			= _xmin;
	xmax			= _xmax;
	ymin			= _ymin;
	ymax			= _ymax;
	setStyles("slider2D",style);
	intOnly = true;

}
void ofxW2DSlider::init(const string & _title, float _xvalue, float _yvalue, float _xmin, float _xmax, float _ymin, float _ymax, const string & style){
	ofxWidget::init();
	ixtargetValue	= 0;
	fxtargetValue	= 0;
	iytargetValue	= 0;
	fytargetValue	= 0;
	title			= _title;
	xmin			= _xmin;
	xmax			= _xmax;
	ymin			= _ymin;
	ymax			= _ymax;
	xvalue			= ofMap(_xvalue,_xmin,_xmax,0,1);
	yvalue			= ofMap(_yvalue,_ymin,_ymax,0,1);
	setStyles("slider2D",style);
	intOnly 		= false;
}

void ofxW2DSlider::init(const string & _title, int _xvalue, int _yvalue, int _xmin, int _xmax, int _ymin, int _ymax, const string & style){
	ofxWidget::init();
	fxtargetValue	= 0;
	ixtargetValue	= 0;
	fytargetValue	= 0;
	iytargetValue	= 0;
	title			= _title;
	xmin			= _xmin;
	xmax			= _xmax;
	ymin			= _ymin;
	ymax			= _ymax;
	xvalue			= ofMap(_xvalue,_xmin,_xmax,0,1);
	yvalue			= ofMap(_yvalue,_ymin,_ymax,0,1);
	setStyles("slider2D",style);
	intOnly 		= true;

}
int ofxW2DSlider::getValueI(){
	return ofMap(xvalue,0,1,xmin,xmax);
}

float ofxW2DSlider::getValueF(){
	return ofMap(xvalue,0,1,xmin,xmax);
}

bool ofxW2DSlider::getValueB(){
	return xvalue;
}

void ofxW2DSlider::update(){
	if(fxtargetValue)
		xvalue = ofClamp(ofMap(*fxtargetValue,xmin,xmax,0,1),0,1);
	if(fytargetValue)
		yvalue = ofClamp(ofMap(*fytargetValue,ymin,ymax,0,1),0,1);
	if(ixtargetValue)
		xvalue = ofClamp(ofMap(*ixtargetValue,xmin,xmax,0,1),0,1);
	if(iytargetValue)
		yvalue = ofClamp(ofMap(*iytargetValue,ymin,ymax,0,1),0,1);
}

void ofxW2DSlider::setValue(float x, float y){
	xvalue = ofClamp(x,0,1);
	yvalue = ofClamp(y,0,1);

	float floatxValue = ofMap(xvalue,0,1,xmin,xmax);
	int intxValue 	 = floatxValue;
	float floatyValue = ofMap(yvalue,0,1,ymin,ymax);
	int intyValue 	 = floatyValue;

	if(intOnly){
		xvalue=ofMap(intxValue,xmin,xmax,0,1);
		yvalue=ofMap(intxValue,ymin,ymax,0,1);
	}

	ofPoint ivalue(intxValue,intyValue);
	ofPoint fvalue(floatxValue,floatyValue);
	ofNotifyEvent(intEvent, ivalue, this);
	ofNotifyEvent(floatEvent, fvalue, this);

	if(fxtargetValue)
		*fxtargetValue = floatxValue;
	if(fytargetValue)
		*fytargetValue = floatyValue;

	if(ixtargetValue)
		*ixtargetValue = intxValue;
	if(iytargetValue)
		*iytargetValue = intyValue;
}

void ofxW2DSlider::render(ofxWidgetsStyle & style){
	ofPushMatrix();
	ofTranslate(style.position.x,style.position.y);

	//background
	ofFill();
	ofSetColor(style.background.color.r,style.background.color.g,style.background.color.b);
	float backgroundX= (style.border.width-style.background.width)/2;
	float backgroundY= (style.border.height-style.background.height)/2;
	ofRect(backgroundX,backgroundY,style.background.width,style.background.height);

	//foreground
	ofSetColor(style.foreground.color.r,style.foreground.color.g,style.foreground.color.b);
	float foregroundX, foregroundY, foregroundW, foregroundH;

	ofCircle((style.background.width-style.foreground.width)/2+style.foreground.width*xvalue,
			(style.background.height-style.foreground.height)/2+style.foreground.height-style.foreground.height*yvalue,
			3);

	ofLine((style.background.width-style.foreground.width)/2,
			(style.background.height-style.foreground.height)/2+style.foreground.height-style.foreground.height*yvalue,
			(style.background.width-style.foreground.width)/2+style.foreground.width,
			(style.background.height-style.foreground.height)/2+style.foreground.height-style.foreground.height*yvalue);

	ofLine((style.background.width-style.foreground.width)/2+style.foreground.width*xvalue,
				(style.background.height-style.foreground.height)/2,
				(style.background.width-style.foreground.width)/2+style.foreground.width*xvalue,
				(style.background.height-style.foreground.height)/2+style.foreground.height);

	/*if(max>0 && min<0){
		foregroundX = (style.background.width-style.foreground.width)/2+style.foreground.width*ofMap((max + min),min,max,1,0);
		foregroundY = (style.background.height-style.foreground.height)/2;
		foregroundW = (style.foreground.width*value-style.foreground.width*ofMap((max + min),min,max,1,0));
		foregroundH = style.foreground.height;
		ofRect(foregroundX,foregroundY,foregroundW,foregroundH);
		ofSetColor(style.border.color.r,style.border.color.g,style.border.color.b);
		ofSetLineWidth(style.border.lineWidth);
		ofLine(foregroundX,0,foregroundX,style.border.height);
	}else if(min>max || max <=0 ){
		foregroundX = (style.background.width-style.foreground.width)/2 + style.foreground.width;
		foregroundY = (style.background.height-style.foreground.height)/2;
		foregroundW = style.foreground.width*value-style.foreground.width;
		foregroundH = style.foreground.height;
		ofRect(foregroundX,foregroundY,foregroundW,foregroundH);
	}else{
		foregroundX = (style.background.width-style.foreground.width)/2;
		foregroundY = (style.background.height-style.foreground.height)/2;
		foregroundW = style.foreground.width*value;
		foregroundH = style.foreground.height;
		ofRect(foregroundX,foregroundY,foregroundW,foregroundH);
	}*/

	// border
	ofNoFill();
	ofSetColor(style.border.color.r,style.border.color.g,style.border.color.b);
	ofSetLineWidth(style.border.lineWidth);
	ofRect(0,0,style.border.width,style.border.height);

	char msg[1024];
	if(ixtargetValue)
		sprintf(msg,"%i,%i",*ixtargetValue,*iytargetValue);
	else if(fxtargetValue)
		sprintf(msg,"%.2f,%.2f",*fxtargetValue,*fytargetValue);
	else if(intOnly)
		sprintf(msg,"%i,%i",(int)ofMap(xvalue,0,1,xmin,xmax),(int)ofMap(yvalue,0,1,ymin,ymax));
	else
		sprintf(msg,"%.2f,%.2f",ofMap(xvalue,0,1,xmin,xmax),ofMap(yvalue,0,1,ymin,ymax));
	ofSetColor(style.text.color.r,style.text.color.g,style.text.color.b);
	if(style.text.ttf){
		style.text.font->drawString(title +" "+ msg,style.text.position.x,style.text.position.y);
	}else{
		ofDrawBitmapString(title +" "+ msg,style.text.position.x,style.text.position.y);
	}

	ofPopMatrix();
}

ofRectangle ofxW2DSlider::getActiveArea(ofxWidgetsStyle & style){
	ofRectangle area;
	area.x=style.position.x;
	area.y=style.position.y;
	area.width=style.foreground.width;
	area.height=style.foreground.height;
	return area;
}

ofRectangle ofxW2DSlider::getTotalArea(ofxWidgetsStyle & style){
	return getActiveArea(style);
}

// UNFOCUSED -> over -> FOCUSED -> pressed -> PRESSED -> out -> OUT -> released -> UNFOCUSED
// PRESSED -> released -> FOCUSSED
// FOCUSED -> out ->  UNFOCUSED
ofxWidgetsState ofxW2DSlider::manageEvent(ofxWidgetsEvent event, ofxWidgetEventArgs & args, ofxWidgetsState currentState){
	switch(event){

	case OFX_W_E_POINTER_OVER:
		if(currentState==OFX_WIDGET_PRESSED){
			setValue(args.relative_x,1-args.relative_y);
		}
		if(currentState==OFX_WIDGET_UNFOCUSED){
			return OFX_WIDGET_FOCUSED;
		}
		if(currentState==OFX_WIDGET_OUT){
			return OFX_WIDGET_PRESSED;
		}
		break;

	case OFX_W_E_POINTER_OUT:
		if(currentState==OFX_WIDGET_OUT){
			setValue(args.relative_x,1-args.relative_y);
		}
		if(currentState==OFX_WIDGET_FOCUSED){
			return OFX_WIDGET_UNFOCUSED;
		}
		if(currentState==OFX_WIDGET_PRESSED){
			return OFX_WIDGET_OUT;
		}
		break;

	case OFX_W_E_POINTER_PRESSED:
		if(currentState==OFX_WIDGET_FOCUSED){
			setValue(args.relative_x,1-args.relative_y);
			return OFX_WIDGET_PRESSED;
		}
		break;
	case OFX_W_E_POINTER_RELEASED:
		if(currentState==OFX_WIDGET_PRESSED){
			return OFX_WIDGET_FOCUSED;
		}
		if(currentState==OFX_WIDGET_OUT){
			return OFX_WIDGET_UNFOCUSED;
		}
		break;
	case OFX_W_E_ENABLED:
	case OFX_W_E_DISABLED:
	case OFX_W_E_VALUE_CHANGED:
	case OFX_W_E_POINTER_MOVED:
	case OFX_W_E_POINTER_DRAGGED:
	case OFX_W_E_KEY_PRESSED:
		break;
	case OFX_W_E_KEY_RELEASED:
		if(args.key==OF_KEY_LEFT){
			setValue(xvalue-ofMap(1,xmin,xmax,0,1),yvalue);
		}else if(args.key==OF_KEY_RIGHT){
			setValue(xvalue+ofMap(1,xmin,xmax,0,1),yvalue);
		}else if(args.key==OF_KEY_UP){
			setValue(xvalue,yvalue+ofMap(1,ymin,ymax,0,1));
		}else if(args.key==OF_KEY_DOWN){
			setValue(xvalue,yvalue+ofMap(-1,ymin,ymax,0,1));
		}
		break;

	default:
	break;
	}

	return currentState;
}

void ofxW2DSlider::saveTo(ofxXmlSettings & xml, const string & tag){
	if(fxtargetValue) xml.addValue(tag,*fxtargetValue);
	else if(ixtargetValue) xml.addValue(tag, *ixtargetValue);
	else xml.addValue(tag,ofMap(xvalue,0,1,xmin,xmax));
}

void ofxW2DSlider::loadFrom(ofxXmlSettings & xml, const string & tag){
	xvalue = ofMap(xml.getValue(tag,xmin),xmin,xmax,0,1);
	if(fxtargetValue) *fxtargetValue = xml.getValue(tag,*fxtargetValue);
	else if(ixtargetValue) *ixtargetValue = xml.getValue(tag,*ixtargetValue);
}
