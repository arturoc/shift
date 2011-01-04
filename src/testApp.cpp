#include "testApp.h"
#include "homography.h"

//--------------------------------------------------------------
void testApp::setup(){
	//ofSetFrameRate(30);
	ofSetVerticalSync(true);
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

	kPlayer.setup("depth3-16-52-41.bin",true);
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

	ofEnableArbTex();

	texPoints = false;

	translateX=0;
	translateY=0;

	showRGB=false;

	gui.init("");
	gui.addTab("3d camera");
	//ofAddListener(gui.addSpinSlider("camera y:",640,-640,640).floatEvent,this,&testApp::cameraYChanged);
	//ofAddListener(gui.addSpinSlider("camera lookat y:",320,-640,640).floatEvent,this,&testApp::cameraLookAtYChanged);
	gui.addSpinSlider("render 1 in x:",&pc_renderer.oneInX,1,30);
	gui.addSpinSlider("render 1 in y:",&pc_renderer.oneInY,1,30);
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
	ofAddListener(gui.addToggle("video/live",false).boolEvent,this,&testApp::liveVideoChanged);
	ofAddListener(gui.addSpinSlider("tilt",0,-30,30,1).floatEvent,this,&testApp::tiltChanged);
	gui.addSpinSlider("player fps",&kPlayer.fps,0,60,1);
	ofAddListener(gui.addToggle("record",false).boolEvent,this,&testApp::recordChanged);

	gui.addTab("view");
	gui.addToggle("show contour",&showContour);
	gui.addToggle("show depth",&showDepth);
	gui.addToggle("show clipPlanes",&showClipPlanes);
	gui.addToggle("show rgb",&showRGB);


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
	gui.addToggle("real world coords",&useDepthFactor);
	gui.addToggle("depth of field",&pc_renderer.dof);
	gui.addToggle("color",&color);
	gui.addToggle("depth to gray",&pc_renderer.depthToGray);
	gui.addSpinSlider("min. gray", &pc_renderer.minimumGray, 0, 255);
	gui.addSpinSlider("mesh object thres.", &mesh_renderer.objectDepthThreshold, 0, 255);

	gui.addSpinSlider("focusDistance", &pc_renderer.focusDistance, 0, 2000);
	gui.addSpinSlider("aperture", &pc_renderer.aperture, 0, .1, .001);
	gui.addSpinSlider("pointBrightness", &pc_renderer.pointBrightness, 0, 1, .01);
	gui.addSpinSlider("rgbBrightness",  &pc_renderer.rgbBrightness, 0, 1, .01);
	gui.addSpinSlider("maxPointSize", &pc_renderer.maxPointSize, 0, 30);

	gui.loadFrom("settings.xml","settings");

	gui.addFps();

	texRGBCalibrated.allocate(640,480,GL_RGB);

	//gui.addSpinSlider("camera z",);
	//ofAddListener(gui.add2DSlider("camera xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f).floatEvent,this,&testApp::cameraPosChanged);
	//ofAddListener(gui.add2DSlider("lookat xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f).floatEvent,this,&testApp::cameraLookAtChanged);

	//ofAddListener(cameraPosSlider.floatEvent,this,&testApp::cameraPosChanged);
	//ofAddListener(cameraLookAtSlider.floatEvent,this,&testApp::cameraLookAtChanged);
	glDisable(GL_LIGHTING);
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
		mesh_renderer.useDepthFactor = useDepthFactor;
		//mesh_renderer.update(source->getDistancePixels(),source->getCalibratedRGBPixels(),640,480);
		mesh_renderer.update(source->getDistancePixels(),source->getCalibratedTexCoords(), 640,480);
	}else{
		pc_renderer.depthThreshold = depthThreshold;
		pc_renderer.useDepthFactor = useDepthFactor;
		if(color)
			pc_renderer.update(source->getDistancePixels(),source->getCalibratedRGBPixels(),640,480);
		else
			pc_renderer.update(source->getDistancePixels(),640,480);
	}
	if(showRGB && source==(ofxBase3DVideo*)&kinect){
		//texRGBCalibrated.loadData(kinect.getCalibratedRGBPixels(),640,480,GL_RGB);
	}
	if(recorder.isOpened() && source==(ofxBase3DVideo*)&kinect){
		recorder.newFrame(kinect.getPixels(),kinect.getRawDepthPixels());
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	ofNoFill();
	fbo.begin();
	//ofBackground(0,0,0);
	ofClear(0,0,0,255);
	ofPushMatrix();
		ofSetColor(gray,gray,gray,alpha);
		glPointSize(psize);

		if(fov>0)
			ofSetupScreenPerspective(640,480,false,fov);
		else
			ofSetupScreenOrtho(640,480,false);
		//ofViewport(0,0,640,480);
		//ofBackground(0, 0, 0);

		ofTranslate(0,0,postTranslateZ);

		ofTranslate(rot_axis);
		ofRotate(rot,0,1,0);
		if(showClipPlanes){
			ofSetColor(255,0,0);
			ofLine(0,0,0,ofGetHeight());
		}
		ofTranslate(-rot_axis);

		ofTranslate(0,0,translateZ);

		if(useDepthFactor || pc_renderer.dof){
			ofScale(2,2,2);
		}
		if(showClipPlanes){
		ofPushMatrix();
			ofSetColor(255,0,0);
			ofTranslate(0,0,-nearClip);
			ofRect(0,0,640,480);
			ofTranslate(0,0,nearClip-depthThreshold);
			ofRect(0,0,640,480);
		ofPopMatrix();
		}

		if(useDepthFactor || pc_renderer.dof){
			ofTranslate(translateX,translateY);
		}

		ofSetColor(255,255,255);
		if((texPoints || pc_renderer.dof) && !mesh)
			pc_renderer.draw(&pointTex.getTextureReference());
		else if(!mesh)// && !color)
			pc_renderer.draw();
		/*else if(!mesh)
			pc_renderer.draw(&source->getTextureReference());*/
		else if(color)
			mesh_renderer.draw(&source->getTextureReference());
		else if(pc_renderer.depthToGray)
			mesh_renderer.draw(&source->getDepthTextureReference());
		else
			mesh_renderer.draw();

	ofPopMatrix();
	fbo.end();

	ofSetColor(255,255,255);
	ofSetupScreen();
	//ofViewport(0,0,ofGetWidth(),ofGetHeight());
	fbo.draw(ofGetWidth()-fbo.getWidth(),0);

	ofNoFill();
	if(showDepth){
		cvdepth.draw(ofGetWidth()-320,ofGetHeight()-240,320,240);
		ofRect(ofGetWidth()-320,ofGetHeight()-240,320,240);
	}
	if(showRGB){
		kinect.draw(ofGetWidth()-320,ofGetHeight()-240,320,240);
		//texRGBCalibrated.draw(ofGetWidth()-320,ofGetHeight()-240,320,240);
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
		kinect.setCameraTiltAngle(tilt);
		source = &kinect;
	}else{
		kPlayer.setup("depth3-16-56-54.bin",true);
		source = &kPlayer;
	}
}

//--------------------------------------------------------------
void testApp::recordChanged(bool & pressed){
	if(pressed){
		recorder.init("depth"+ofToString(ofGetDay())+"-"+ofToString(ofGetHours())+"-"+ofToString(ofGetMinutes())+"-"+ofToString(ofGetSeconds())+".bin");
	}else{
		recorder.close();
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

