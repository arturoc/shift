/*
 * ControlsWindow.h
 *
 *  Created on: 08/01/2011
 *      Author: arturo
 */

#ifndef CONTROLSWINDOW_H_
#define CONTROLSWINDOW_H_

#include "ofxFenster/ofxFenster.h"
#include "ofxWidgets/src/ofxWidgets.h"
#include "testApp.h"

class ControlsWindow: public ofxFensterListener {
public:
	ControlsWindow(testApp * app);
	virtual ~ControlsWindow();

	void fensterSetup();
	void fensterUpdate();
	void fensterDraw();

	void fensterKeyPressed  (int key);
	void fensterKeyReleased(int key);
	void fensterMouseMoved(int x, int y );
	void fensterMouseDragged(int x, int y, int button);
	void fensterMousePressed(int x, int y, int button);
	void fensterMouseReleased(int x, int y, int button);
	void fensterWindowResized(int w, int h);



	bool whiteScreen;

	bool twoScreens;
	bool noRender;


	int fps;

	int oneInX, oneInY;
	bool dof;
	int minimumGray;
	int objectDepthThreshold;

	float focusDistance, aperture, pointBrightness, rgbBrightness, maxPointSize, pointSizeFactor;

	int numPlayers;
	bool showLive,showPlayer[10],onlyLive;
	bool showWarp;
	bool soundCloud;



	float dramaticZoomFactor;


	bool texPoints;
	bool mesh;
	bool color;
	bool depthToGray;

	float depthThreshold,minDistance,scaleFactor;
	bool useDepthFactor;


	float tilt;
	int gray,alpha,psize;


	int min_blob, max_blob;

	float nearClip,farClip;
	float fov;

	ofPoint rot_axis;
	float	rot;
	float   translateZ,translateX,translateY;
	float	postTranslateZ;

	bool showContour, showDepth, showClipPlanes, showRGB, showStats;
	bool doCameraEasing;

	ofxWFrame gui;
	ofxWFrame comp_gui;

	testApp * app;
};

#endif /* CONTROLSWINDOW_H_ */
