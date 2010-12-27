#include "testApp.h"
#include "homography.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(30);
	ofBackground(0,0,0);
	ofSetColor(255,255,255);

	ofEnableAlphaBlending();

	cvdepth.allocate(640,480);

	grayThresh.allocate(640,480);
	grayThreshFar.allocate(640,480);

	fbo.setup(640,480,GL_RGBA);

	nearClip=0;farClip=0;
	fov = 60;
	postTranslateZ=0;

	source = &kPlayer;

	kPlayer.setup("depth.bin");
	//camera.disableMouseEvents();
	camera.position(0,0,0);
	//cameraPosSlider.init("camera xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f,"default");
	//cameraLookAtSlider.init("lookat xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f,"default");
	//cameraLookAtSlider.setPosition(ofPoint(0,190));

	min_blob = 640*480/5;
	max_blob = 640*480;

	showContour = false;

	ofDisableArbTex();
	pointTex.loadImage("point.png");

	texPoints = false;

	translateX=0;
	translateY=0;

	gui.init("");
	gui.addTab("3d camera");
	ofAddListener(gui.addSpinSlider("camera y:",640,-640,640).floatEvent,this,&testApp::cameraYChanged);
	ofAddListener(gui.addSpinSlider("camera lookat y:",320,-640,640).floatEvent,this,&testApp::cameraLookAtYChanged);
	gui.addSpinSlider("render 1 in x: ",&pc_renderer.oneInX,1,30);
	gui.addSpinSlider("render 1 in y: ",&pc_renderer.oneInY,1,30);
	//gui.addSpinSlider("depth threshold: ",&renderer.depthThreshold,0,1000);
	gui.addSpinSlider("near clip: ",&nearClip,0,255);
	gui.addSpinSlider("far clip: ",&farClip,0,255);
	gui.addSpinSlider("depth thres:",&depthThreshold,0,10000);
	gui.addSpinSlider("fov: ",&fov,0,200);
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
	ofAddListener(gui.addToggle("video/live",false).boolEvent,this,&testApp::liveVideoChanged);
	ofAddListener(gui.addSpinSlider("tilt",0,-30,30,1).floatEvent,this,&testApp::tiltChanged);

	gui.addTab("view");
	gui.addToggle("show contour",&showContour);
	gui.addToggle("show depth",&showDepth);
	gui.addToggle("show clipPlanes",&showClipPlanes);


	gray = 255;
	alpha = 255;
	psize = 1;
	gui.addTab("render");
	gui.addToggle("draw mesh", &mesh);
	gui.addSpinSlider("gray",&gray,0,255,1);
	gui.addSpinSlider("alpha",&alpha,0,255,1);
	gui.addSpinSlider("p. size",&psize,0,64,1);
	gui.addToggle("textured points",&texPoints);
	gui.addSpinSlider("minDistance",&minDistance,0,-100,1);
	gui.addSpinSlider("scaleFactor",&scaleFactor,0,.01,.001);
	gui.addToggle("use depth factor",&useDepthFactor);

	gui.loadFrom("settings.xml","settings");

	gui.addFps();
	//gui.addSpinSlider("camera z",);
	//ofAddListener(gui.add2DSlider("camera xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f).floatEvent,this,&testApp::cameraPosChanged);
	//ofAddListener(gui.add2DSlider("lookat xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f).floatEvent,this,&testApp::cameraLookAtChanged);

	//ofAddListener(cameraPosSlider.floatEvent,this,&testApp::cameraPosChanged);
	//ofAddListener(cameraLookAtSlider.floatEvent,this,&testApp::cameraLookAtChanged);

}

//--------------------------------------------------------------
void testApp::update(){
	source->update();
	if(showContour){
		cvdepth.setFromPixels(source->getDepthPixels(),640,480);
		grayThreshFar = cvdepth;
		grayThresh = cvdepth;
		grayThreshFar.threshold(farClip, true);
		grayThresh.threshold(nearClip);
		cvAnd(grayThresh.getCvImage(), grayThreshFar.getCvImage(), cvdepth.getCvImage(), NULL);
		contourFinder.findContours(cvdepth,min_blob,max_blob,10,false,true);
	}

	if(mesh){
		mesh_renderer.depthThreshold = depthThreshold;
		mesh_renderer.minDistance = minDistance;
		mesh_renderer.scaleFactor = scaleFactor;
		mesh_renderer.useDepthFactor = useDepthFactor;
		mesh_renderer.update(source->getDistancePixels(),640,480);
	}else{
		pc_renderer.depthThreshold = depthThreshold;
		pc_renderer.minDistance = minDistance;
		pc_renderer.scaleFactor = scaleFactor;
		pc_renderer.useDepthFactor = useDepthFactor;
		pc_renderer.update(source->getDistancePixels(),640,480);
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	ofNoFill();
	//fbo.begin();
	ofBackground(0,0,0);
	ofPushMatrix();
		ofSetColor(gray,gray,gray,alpha);
		glPointSize(psize);

		if(fov>0)
			ofSetupScreenPerspective(640,480,true,fov);
		else
			ofSetupScreenOrtho(640,480,true);
		ofViewport(0,0,640,480);
		ofBackground(0, 0, 0);

		ofTranslate(0,0,postTranslateZ);

		ofTranslate(rot_axis);
		ofRotate(rot,0,1,0);
		ofTranslate(-rot_axis);

		ofTranslate(0,0,translateZ);

		if(useDepthFactor){
			ofScale(2,2,1);
			ofTranslate(translateX,translateY);
		}

		if(texPoints && !mesh)
			pc_renderer.draw(&pointTex.getTextureReference());
		else if(!mesh)
			pc_renderer.draw();
		else
			mesh_renderer.draw();

		if(showClipPlanes){
			ofSetColor(255,0,0);
			ofTranslate(0,0,-nearClip);
			ofRect(0,0,640,480);
			ofTranslate(0,0,nearClip-depthThreshold);
			ofRect(0,0,640,480);
		}
	ofPopMatrix();
	//fbo.end();

	ofSetColor(255,255,255);
	ofSetupScreen();
	ofViewport(0,0,ofGetWidth(),ofGetHeight());
	//fbo.draw(ofGetWidth()-640,0);

	if(showDepth){
		cvdepth.draw(ofGetWidth()-320,ofGetHeight()-240,320,240);
		ofRect(ofGetWidth()-320,ofGetHeight()-240,320,240);
	}

	if(showContour)
		contourFinder.draw(0,240);


}

//--------------------------------------------------------------
void testApp::cameraPosChanged(ofPoint & pos){
	camera.position(ofxVec3f(pos.x,camera.getPosition().y,pos.y));
}

//--------------------------------------------------------------
void testApp::cameraLookAtChanged(ofPoint & pos){
	camera.eye(ofxVec3f(pos.x,camera.getEye().y,pos.y));
}

//--------------------------------------------------------------
void testApp::cameraYChanged(float & pos){
	camera.position(ofxVec3f(camera.getPosition().x,pos,camera.getPosition().z));
}

//--------------------------------------------------------------
void testApp::cameraLookAtYChanged(float & pos){
	camera.eye(ofxVec3f(camera.getEye().x,pos,camera.getEye().z));
}

//--------------------------------------------------------------
void testApp::liveVideoChanged(bool & pressed){
	source->close();
	if(pressed){
		kinect.init();
		kinect.open();
		source = &kinect;
	}else{
		kPlayer.setup("depth.bin");
		source = &kPlayer;
	}
}

//--------------------------------------------------------------
void testApp::tiltChanged(float & tilt){
	kinect.setCameraTiltAngle(tilt);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
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

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

