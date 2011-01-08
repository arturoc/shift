/*
 *  ofxFenster.cpp
 *  ofxFensterExample
 *
 *  Created by Philip Whitfield on 5/19/10.
 *  Copyright 2010 undef.ch. All rights reserved.
 *
 */

#include "ofxFenster.h"

ofCoreEvents ofxFensterEvents;

ofxFenster* singleton;
static int fensterButtonInUse;

static ofAppBaseWindow * OFWin;

//GLUT CALLBACKS
void updateWindow(){
	singleton->update();
}

void displayWindow(){
	singleton->draw(true);
} 

void mouseCallback(int button, int state, int x, int y){
	static ofMouseEventArgs mouse;
	mouse.x = x;
	mouse.y = y;
	mouse.button = button;

	if (state == GLUT_DOWN){
		singleton->listener->fensterMousePressed(x,y,button);
		ofNotifyEvent(ofxFensterEvents.mousePressed,mouse,singleton);

		fensterButtonInUse = button;
	}else if (state == GLUT_UP){
		singleton->listener->fensterMouseReleased(x,y,button);
		ofNotifyEvent(ofxFensterEvents.mouseReleased,mouse,singleton);
	}
}

void winResize(int w, int h){
	singleton->listener->fensterWindowResized(w, h);
	singleton->getBounds();
	
	static ofResizeEventArgs efe;
	efe.width = w;
	efe.height = h;
	ofNotifyEvent(ofxFensterEvents.windowResized, efe, singleton);
}

void winKeyboardDown(unsigned char key, int x, int y) {
	singleton->listener->fensterKeyPressed(key);

	static ofKeyEventArgs args;
	args.key=key;
	ofNotifyEvent(ofxFensterEvents.keyPressed,args,singleton);
}

void winKeyboardDownSpecial(int key, int x, int y) {
	winKeyboardDown(key| OF_KEY_MODIFIER, x, y);

	static ofKeyEventArgs args;
	args.key=key | OF_KEY_MODIFIER;
	ofNotifyEvent(ofxFensterEvents.keyPressed,args,singleton);
}

void winKeyboardUp(unsigned char key, int x, int y) {
	singleton->listener->fensterKeyReleased(key);

	static ofKeyEventArgs args;
	args.key=key;
	ofNotifyEvent(ofxFensterEvents.keyReleased,args,singleton);
}

void winKeyboardUpSpecial(int key, int x, int y) {
	winKeyboardUp(key| OF_KEY_MODIFIER, x, y);

	static ofKeyEventArgs args;
	args.key=key | OF_KEY_MODIFIER;
	ofNotifyEvent(ofxFensterEvents.keyReleased,args,singleton);
}

void winMouseMove(int x, int y){
	singleton->listener->fensterMouseMoved(x, y);

	static ofMouseEventArgs mouse;
	mouse.x = x;
	mouse.y = y;
	ofNotifyEvent(ofxFensterEvents.mouseMoved,mouse,singleton);
}

void winMouseDragged(int x, int y){
	singleton->listener->fensterMouseMoved(x, y);

	static ofMouseEventArgs mouse;
	mouse.x = x;
	mouse.y = y;
	mouse.button = fensterButtonInUse;
	ofNotifyEvent(ofxFensterEvents.mouseDragged,mouse,singleton);
}

//OFX FENSTER 
ofxFenster::ofxFenster(){
	singleton = this;
	width = 800;
	height = 400;
	nextWinUpdate = 0;
	nextWinDraw = 0;
	setFPS(60);
	hasGlut = false;
	mainContextSkip = 0;
};

ofxFenster::~ofxFenster(){
	
};

void ofxFenster::init(ofxFensterListener* l, ofAppBaseWindow * mainWin ,string name){
	listener = l;
	listener->fenster = this;
	OFWin = mainWin;
	//ofAddListener(ofEvents.update, this, &ofxFenster::update);
	//ofAddListener(ofEvents.draw, this, &ofxFenster::draw);
	
	mainWinRef = glutGetWindow();
	glutInitWindowSize(width, height);
	winRef = glutCreateWindow(name.c_str());
	
	glutSetWindow(winRef);
	glutIdleFunc(updateWindow);
	glutDisplayFunc(displayWindow);
	glutMouseFunc(mouseCallback);
	glutReshapeFunc(winResize);
	glutKeyboardFunc(winKeyboardDown);
	glutKeyboardUpFunc(winKeyboardUp);
	glutSpecialUpFunc(winKeyboardUpSpecial);
	glutSpecialFunc(winKeyboardDownSpecial);
	glutMotionFunc(winMouseDragged);
	glutPassiveMotionFunc(winMouseMove);
	listener->fensterSetup();
	ofEventArgs args;
	ofNotifyEvent(ofxFensterEvents.setup,args,this);
	glutSetWindow(mainWinRef);

}

void ofxFenster::toContext(){
	mainContextSkip++;
	if(hasGlut)
		return;
	glutSetWindow(winRef);
	hasGlut = true;
}
void ofxFenster::toMainContext(){
	mainContextSkip--;
	if(mainContextSkip>0)
		return;

	glutSwapBuffers();
	glutSetWindow(mainWinRef);
	hasGlut = false;

}

void ofxFenster::toggleFullscreen(){
	toContext();
	if(isFullscreen){
		glutPositionWindow(origSize.x, origSize.y);
		glutReshapeWindow(origSize.width, origSize.height);
		isFullscreen = false;
	}else{
		origSize = getBounds();
		glutFullScreen();
		getBounds();
		isFullscreen = true;
	}
	toMainContext();
}

void ofxFenster::update(){
	int t= ofGetElapsedTimeMillis();
	if (t>nextWinUpdate) {
		nextWinUpdate = t+1000/fpsUpdate;
		listener->fensterUpdate();
	}
	ofAppGlutWindow::idle_cb();
};

void ofxFenster::update(ofEventArgs& e){
	update();
	static ofEventArgs efe;
	ofNotifyEvent(ofxFensterEvents.update, efe,this);
}

void ofxFenster::draw(bool force){
	int t= ofGetElapsedTimeMillis();
	toContext();
	if (t>nextWinDraw || force) {
		nextWinDraw = t+1000/fpsDraw;

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		int w, h;
		
		w = width;
		h = height;
		
		h = h > 0 ? h : 1;
		// set viewport, clear the screen
		glViewport( 0, 0, w, h );
		
		float halfFov, theTan, screenFov, aspect;
		screenFov 		= 60.0f;
		
		float eyeX 		= (float)w / 2.0;
		float eyeY 		= (float)h / 2.0;
		halfFov 		= PI * screenFov / 360.0;
		theTan 			= tanf(halfFov);
		float dist 		= eyeY / theTan;
		float nearDist 	= dist / 10.0;	// near / far clip plane
		float farDist 	= dist * 10.0;
		aspect 			= (float)w/(float)h;
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(screenFov, aspect, nearDist, farDist);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eyeX, eyeY, dist, eyeX, eyeY, 0.0, 0.0, 1.0, 0.0);
		
		glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
		glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.	
		
		listener->fensterDraw();
		static ofEventArgs e;
		ofNotifyEvent(ofxFensterEvents.draw, e, this);
	}
	toMainContext();
	ofAppGlutWindow::display();
};

void ofxFenster::draw(ofEventArgs& e){
	draw();
}

void ofxFenster::setFPS(int fps){
	setFPS(fps, fps);
};

void ofxFenster::setFPS(int fpsU, int fpsD){
	fpsUpdate = fpsU;
	fpsDraw = fpsD;
};

void ofxFenster::setBackground(int r, int g, int b, int a){
	toContext();
	glClearColor(r, g, b, a);
	toMainContext();
};

void ofxFenster::setBounds(ofRectangle r){
	setBounds(r.x, r.y, r.width, r.height);
};
void ofxFenster::setBounds(int x, int y, int w, int h){
	toContext();
	glutReshapeWindow(w, h);
	glutPositionWindow(x,y);
	toMainContext();
};

ofRectangle ofxFenster::getBounds(){
	toContext();
	x = getX();
	y = getY();
	width = getWidth();
	height = getHeight();
	toMainContext();
	return ofRectangle(x, y, width, height);
}

int ofxFenster::getX(){
	toContext();
	x = glutGet(GLUT_WINDOW_X);
	toMainContext();
	return x;
};

int ofxFenster::getY(){
	toContext();
	y = glutGet(GLUT_WINDOW_Y);
	toMainContext();
	return y;	
}

int ofxFenster::getWidth(){
	if(!hasGlut)toContext();
	width = glutGet(GLUT_WINDOW_WIDTH);
	if(!hasGlut)toMainContext();
	return width;
}

int ofxFenster::getHeight(){
	if(!hasGlut)toContext();
	height = glutGet(GLUT_WINDOW_HEIGHT);
	if(!hasGlut)toMainContext();
	return height;
}
