/*
 * ControlsWindow.cpp
 *
 *  Created on: 08/01/2011
 *      Author: arturo
 */

#include "ControlsWindow.h"

ControlsWindow::ControlsWindow(testApp * app) {
	this->app = app;
}

ControlsWindow::~ControlsWindow() {
	// TODO Auto-generated destructor stub
}


void ControlsWindow::fensterSetup(){
	fenster->setFPS(10);
	ofxWidget::setEventsObject(&ofxFensterEvents);
	min_blob = 640*480/5;
	max_blob = 640*480;

	showContour = false;

	texPoints = false;

	translateX=0;
	translateY=0;

	showRGB=false;
	twoScreens=false;
	showLive=false;
	//showPlayer[0]=true;
	//showPlayer[1]=true;
	for(int i=0;i<10;i++)
		showPlayer[i]=false;

	fps=30;

	nearClip=0;farClip=0;
	fov = 60;
	postTranslateZ=0;

	gui.init("");
	gui.addTab("3d camera");
	//ofAddListener(gui.addSpinSlider("camera y:",640,-640,640).floatEvent,this,&testApp::cameraYChanged);
	//ofAddListener(gui.addSpinSlider("camera lookat y:",320,-640,640).floatEvent,this,&testApp::cameraLookAtYChanged);
	gui.addSpinSlider("render 1 in x:",&oneInX,1,30);
	gui.addSpinSlider("render 1 in y:",&oneInY,1,30);
	//gui.addSpinSlider("depth threshold: ",&renderer.depthThreshold,0,1000);
	gui.addSpinSlider("near clip:",&nearClip,0,255);
	gui.addSpinSlider("far clip:",&farClip,0,255);
	gui.addSpinSlider("depth thres:",&depthThreshold,0,10000);
	gui.addSpinSlider("fov:",&fov,0,200);
	gui.addSpinSlider("min blob:",&min_blob,0,640*480);
	gui.addSpinSlider("max blob:",&max_blob,0,640*480);
	gui.addSpinSlider("axis x",&rot_axis.x,0,1000);
	gui.addSpinSlider("axis y",&rot_axis.y,0,1000);
	gui.addSpinSlider("axis z",&rot_axis.z,0,-1000);
	gui.addSpinSlider("rot y",&rot,-180,180);
	gui.addSpinSlider("translate z",&translateZ,-1000,1000);
	gui.addSpinSlider("translate x",&translateX,-1000,1000);
	gui.addSpinSlider("translate y",&translateY,-1000,1000);
	gui.addSpinSlider("postranslate z",&postTranslateZ,-1000,1000);
	gui.addLoadButton("settings.xml","settings");
	gui.addSaveButton("settings.xml","settings");

	gui.addTab("kinect");
	ofAddListener(gui.addToggle("video/live",false).boolEvent,app,&testApp::liveVideoChanged);
	ofAddListener(gui.addSpinSlider("tilt",0,-30,30,1).floatEvent,app,&testApp::tiltChanged);
	gui.addSpinSlider("player fps",&fps,0,60,1);
	gui.addToggle("no render",&noRender);
	ofAddListener(gui.addToggle("record",false).boolEvent,app,&testApp::recordChanged);

	gui.addTab("view");
	gui.addToggle("show contour",&showContour);
	gui.addToggle("show depth",&showDepth);
	gui.addToggle("show clipPlanes",&showClipPlanes);
	gui.addToggle("show rgb",&showRGB);
	gui.addToggle("show stats",&showStats);
	gui.addToggle("show live",&showLive);
	gui.addToggle("show player1",&(showPlayer[0]));
	gui.addToggle("show player2",&(showPlayer[1]));
	gui.addToggle("show player3",&(showPlayer[2]));
	gui.addToggle("show player4",&(showPlayer[3]));
	gui.addToggle("show player5",&(showPlayer[4]));
	gui.addToggle("only live",&onlyLive);
	ofAddListener(gui.addToggle("show warp",&showWarp).boolEvent,app,&testApp::showWarpChanged);


	gray = 255;
	alpha = 255;
	psize = 1;
	gui.addTab("render");
	gui.addSpinSlider("gray",&gray,0,255,1);
	gui.addSpinSlider("alpha",&alpha,0,255,1);
	gui.addSpinSlider("p. size",&psize,0,64,1);
	gui.addToggle("textured points",&texPoints);
	gui.addSpinSlider("minDistance",&minDistance,0,-100,1);
	gui.addSpinSlider("scaleFactor",&scaleFactor,0,.01,.001);
	gui.addToggle("draw mesh", &mesh);
	gui.addToggle("draw sound", &soundCloud);
	gui.addSpinSlider("sound buffers",&app->sc_renderer.numSoundBuffers,1,64);
	gui.addSpinSlider("sound depth",&app->sc_renderer.soundDepthFactor,0,2000);
	gui.addToggle("real world coords",&useDepthFactor);
	gui.addToggle("depth of field",&dof);
	gui.addToggle("color",&color);
	gui.addToggle("depth to gray",&depthToGray);
	gui.addSpinSlider("min. gray", &minimumGray, 0, 255);
	gui.addSpinSlider("mesh object thres.", &objectDepthThreshold, 0, 255);

	gui.addSpinSlider("focusDistance", &focusDistance, 0, 2000);
	gui.addSpinSlider("aperture", &aperture, 0, .1, .001);
	gui.addSpinSlider("pointBrightness", &pointBrightness, 0, 1, .01);
	gui.addSpinSlider("rgbBrightness",  &rgbBrightness, 0, 1, .01);
	gui.addSpinSlider("maxPointSize", &maxPointSize, 0, 30);
	gui.addGroupedToggle("circle",true,"DOF_TEX","DOF_TEX_CIRCLE");
	gui.addGroupedToggle("bokeh",false,"DOF_TEX","DOF_TEX_BOKEH");
	gui.addGroupedToggle("gauss",false,"DOF_TEX","DOF_TEX_GAUSS");
	gui.addSpinSlider("pSize factor", &pointSizeFactor, 1, 30);
	gui.addToggle("white screen",&whiteScreen);
	gui.addToggle("move warp",&app->warp.moveOrigin);
	ofAddListener(gui.addButton("two screens").boolEvent,app,&testApp::toggleTwoScreens);

	gui.loadFrom("settings.xml","settings");



	dramaticZoomFactor = 10;

	comp_gui.init("composite params");
	ofAddListener(comp_gui.addSpinSlider("fake dof",1.f,1.f,19.f).floatEvent,app,&testApp::fakeDOFChanged);
	ofAddListener(comp_gui.addSpinSlider("dramatic zoom",0.f,0.f,200.f).floatEvent,app,&testApp::dramaticZoomChanged);
	comp_gui.addSpinSlider("d. zoom fac.",&dramaticZoomFactor,1.f,100.f);

}

void ControlsWindow::fensterUpdate(){

}

void ControlsWindow::fensterDraw(){

}

void ControlsWindow::fensterKeyPressed  (int key){

}

void ControlsWindow::fensterKeyReleased(int key){

}

void ControlsWindow::fensterMouseMoved(int x, int y ){

}

void ControlsWindow::fensterMouseDragged(int x, int y, int button){

}

void ControlsWindow::fensterMousePressed(int x, int y, int button){

}

void ControlsWindow::fensterMouseReleased(int x, int y, int button){

}

void ControlsWindow::fensterWindowResized(int w, int h){

}
