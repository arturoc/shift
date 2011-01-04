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

	fbo.setup(1024,768,GL_RGBA);

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
	pointTex.loadImage("point_solid.png");
	bokehTex.loadImage("point_bokeh.png");
	gaussTex.loadImage("point.png");

	ofEnableArbTex();

	texPoints = false;

	translateX=0;
	translateY=0;

	showRGB=false;
	twoScreens=false;

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
	gui.addToggle("show stats",&showStats);


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
	gui.addGroupedToggle("circle",true,"DOF_TEX","DOF_TEX_CIRCLE");
	gui.addGroupedToggle("bokeh",false,"DOF_TEX","DOF_TEX_BOKEH");
	gui.addGroupedToggle("gauss",false,"DOF_TEX","DOF_TEX_GAUSS");
	gui.addSpinSlider("pSize factor", &pc_renderer.pointSizeFactor, 1, 30);
	gui.addToggle("white screen",&whiteScreen);
	gui.addToggle("move warp",&warp.moveOrigin);
	ofAddListener(gui.addButton("two screens").boolEvent,this,&testApp::toggleTwoScreens);

	gui.loadFrom("settings.xml","settings");



	dramaticZoomFactor = 10;

	comp_gui.init("composite params");
	ofAddListener(comp_gui.addSpinSlider("fake dof",1.f,1.f,19.f).floatEvent,this,&testApp::fakeDOFChanged);
	ofAddListener(comp_gui.addSpinSlider("dramatic zoom",0.f,0.f,200.f).floatEvent,this,&testApp::dramaticZoomChanged);
	comp_gui.addSpinSlider("d. zoom fac.",&dramaticZoomFactor,1.f,100.f);

	texRGBCalibrated.allocate(640,480,GL_RGB);

	warp.init(ofRectangle(ofGetWidth()-640,0,640,480));

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

	vaWarp.clear();
	vaWarp.addVertex(warp.dst[0],ofPoint(0,0));
	vaWarp.addVertex(warp.dst[1],ofPoint(640,0));
	vaWarp.addVertex(warp.dst[2],ofPoint(640,480));
	vaWarp.addVertex(warp.dst[3],ofPoint(0,480));

	for(int i=0;i<4;i++){
		src[i]=warp.src[i]-ofPoint(warp.origin.x,warp.origin.y);
		dst[i]=warp.dst[i]-ofPoint(warp.origin.x,warp.origin.y);
	}

	findOpenCvHomography(src,dst,homography.getPtr());

}

//--------------------------------------------------------------
void testApp::draw(){

	ofNoFill();
	fbo.begin();
	//ofBackground(0,0,0);
	if(whiteScreen){
		ofClear(255,255,255,255);
	}else{
		ofClear(0,0,0,255);
		ofPushMatrix();
			glPointSize(psize);

			if(fov>0)
				ofSetupScreenPerspective(640,480,false,fov);
			else
				ofSetupScreenOrtho(640,480,false);


			ofNoFill();
			ofSetColor(255,255,255,255);
			ofRect(1,1,639,479);
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


			ofSetColor(gray,gray,gray,alpha);
			if((texPoints || pc_renderer.dof) && !mesh){
				if(gui.getValueB("DOF_TEX_CIRCLE"))
					pc_renderer.draw(&pointTex.getTextureReference());
				else if(gui.getValueB("DOF_TEX_BOKEH"))
					pc_renderer.draw(&bokehTex.getTextureReference());
				else if(gui.getValueB("DOF_TEX_GAUSS"))
					pc_renderer.draw(&gaussTex.getTextureReference());

			}else if(!mesh)// && !color)
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
	}
	fbo.end();

	ofSetColor(255,255,255);
	ofSetupScreen();
	//ofViewport(0,0,ofGetWidth(),ofGetHeight());

	glPushMatrix();
	//ofSetupScreenOrtho(ofGetWidth(),ofGetHeight(),true);
	ofTranslate(ofGetWidth()-640,0);
	glMultMatrixf(homography.getPtr());
	fbo.draw(0,0,640,480);
	glPopMatrix();
	//vaWarp.draw(GL_QUADS,&fbo.getTexture(0));

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

	if(showStats){
		for(int i=0;i<4;i++)
			ofDrawBitmapString(ofToString((int)src[i].x) + "," + ofToString((int)src[i].y) + ":" + ofToString((int)dst[i].x) + "," + ofToString((int)dst[i].y),ofGetWidth()-200,ofGetHeight()-(i+1)*20);
		ofDrawBitmapString(ofToString(int(ofGetFrameRate())),ofGetWidth()-200,ofGetHeight()-5*20);
	}

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
		kPlayer.setup("depth4-19-4-50.bin",true);
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
void testApp::fakeDOFChanged(float & fakeDOF){
	psize = fakeDOF;
	pc_renderer.oneInX = 2+fakeDOF*(1/(20-fakeDOF));
	pc_renderer.oneInY = 2+fakeDOF*(1/(20-fakeDOF));
	alpha = ofClamp(255./(fakeDOF*0.5),100,255);
}


void testApp::dramaticZoomChanged(float & zoom){
	fov = 60+zoom;
	postTranslateZ = zoom*dramaticZoomFactor;
	pc_renderer.focusDistance = 800.-zoom*dramaticZoomFactor;
}

void testApp::toggleTwoScreens(bool & pressed){
	if(pressed){
		ofSetWindowShape(1280+200,800);
		ofSetWindowPosition(1280-200,-20);
	}
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
	warp.reshape(ofRectangle(ofGetWidth()-640,0,640,480));
}

