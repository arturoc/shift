#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofNoFill();
	ofBackground(0,0,0);
	currentPoint=-1;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	cameraPath.placeCamera(currentPoint);
	ofTranslate(ofGetWidth()/2,ofGetHeight()/2);
	cameraPath.drawPath();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key==OF_KEY_UP) currentZ--;
	if(key==OF_KEY_DOWN) currentZ++;
	if(key==OF_KEY_LEFT) angle++;
	if(key==OF_KEY_RIGHT) angle--;
	if(key=='r') angle=0;
	if(key=='>') currentPoint++;
	if(key=='<') currentPoint--;
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	cameraPath.addPoint(ofPoint(x,y,currentZ));
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

