#include "testApp.h"
#include "homography.h"
#include "ControlsWindow.h"

testApp::testApp(){

	source = &kPlayer[0];

	kPlayer[0].setup("depth.bin",true);
}

//--------------------------------------------------------------
void testApp::setup(){
	//ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofBackground(0,0,0);
	ofSetColor(255,255,255);

	ofEnableAlphaBlending();

	sc_renderer.setup();

	cvdepth.allocate(640,480);

	grayThresh.allocate(640,480);
	grayThreshFar.allocate(640,480);

	fbo.setup(1024,768,GL_RGBA);
	fbo2.setup(1024,768,GL_RGBA);


	//camera.disableMouseEvents();
	camera.position(0,0,0);
	//cameraPosSlider.init("camera xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f,"default");
	//cameraLookAtSlider.init("lookat xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f,"default");
	//cameraLookAtSlider.setPosition(ofPoint(0,190));


	ofDisableArbTex();
	pointTex.loadImage("point_solid.png");
	bokehTex.loadImage("point_bokeh.png");
	gaussTex.loadImage("point.png");

	ofEnableArbTex();


	texRGBCalibrated.allocate(640,480,GL_RGB);

	warp.setLocalEventsObject(&ofEvents);
	warp2.setLocalEventsObject(&ofEvents);
	warp.init(ofRectangle(ofGetWidth()-640,0,640,480));
	warp2.init(ofRectangle(ofGetWidth()-640*2-10,0,640,480));

	//gui.addSpinSlider("camera z",);
	//ofAddListener(gui.add2DSlider("camera xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f).floatEvent,this,&testApp::cameraPosChanged);
	//ofAddListener(gui.add2DSlider("lookat xz: ",0.f,0.f,-1000.f,1000.f,-1000.f,1000.f).floatEvent,this,&testApp::cameraLookAtChanged);

	//ofAddListener(cameraPosSlider.floatEvent,this,&testApp::cameraPosChanged);
	//ofAddListener(cameraLookAtSlider.floatEvent,this,&testApp::cameraLookAtChanged);
	glDisable(GL_LIGHTING);


	ofSoundStreamSetup(0,2,this,44100,1024,1);
}

//--------------------------------------------------------------
void testApp::update(){

	source->update();

	if(!controlsWindow->noRender){
		if(controlsWindow->showLive){
			//cout << "updating live "  << endl;
			if(controlsWindow->soundCloud){
				sc_renderer.oneInX = controlsWindow->oneInX;
				sc_renderer.oneInY = controlsWindow->oneInY;
				sc_renderer.useDepthFactor = controlsWindow->useDepthFactor;
				sc_renderer.dof=controlsWindow->dof;
				sc_renderer.depthToGray=controlsWindow->depthToGray;
				sc_renderer.minimumGray=controlsWindow->minimumGray;
				sc_renderer.focusDistance=controlsWindow->focusDistance;
				sc_renderer.aperture=controlsWindow->aperture;
				sc_renderer.pointBrightness=controlsWindow->pointBrightness;
				sc_renderer.rgbBrightness=controlsWindow->rgbBrightness;
				sc_renderer.maxPointSize=controlsWindow->maxPointSize;
				sc_renderer.pointSizeFactor=controlsWindow->pointSizeFactor;
				sc_renderer.useDepthFactor = controlsWindow->useDepthFactor;
				sc_renderer.objectDepthThreshold = controlsWindow->objectDepthThreshold;
				sc_renderer.mesh = controlsWindow->mesh;
				sc_renderer.update(source->getDepthPixels(), 640,480);
				if(controlsWindow->mesh){
					sc_renderer.update(source->getDistancePixels(),source->getCalibratedTexCoords(),640,480);
				}else if(controlsWindow->color){
					sc_renderer.update(source->getDistancePixels(),source->getCalibratedRGBPixels(),640,480);
				}else{
					sc_renderer.update(source->getDistancePixels(),640,480);
				}

			}else if(controlsWindow->mesh){
				mesh_renderer[0].objectDepthThreshold=controlsWindow->objectDepthThreshold;
				mesh_renderer[0].depthThreshold = controlsWindow->depthThreshold;
				mesh_renderer[0].useDepthFactor = controlsWindow->useDepthFactor;
				//mesh_renderer.update(source->getDistancePixels(),source->getCalibratedRGBPixels(),640,480);
				mesh_renderer[0].update(source->getDistancePixels(),source->getCalibratedTexCoords(), 640,480);
			}else{
				pc_renderer[0].oneInX=controlsWindow->oneInX;
				pc_renderer[0].oneInY=controlsWindow->oneInY;
				pc_renderer[0].dof=controlsWindow->dof;
				pc_renderer[0].depthToGray=controlsWindow->depthToGray;
				pc_renderer[0].minimumGray=controlsWindow->minimumGray;
				pc_renderer[0].focusDistance=controlsWindow->focusDistance;
				pc_renderer[0].aperture=controlsWindow->aperture;
				pc_renderer[0].pointBrightness=controlsWindow->pointBrightness;
				pc_renderer[0].rgbBrightness=controlsWindow->rgbBrightness;
				pc_renderer[0].maxPointSize=controlsWindow->maxPointSize;
				pc_renderer[0].pointSizeFactor=controlsWindow->pointSizeFactor;
				pc_renderer[0].depthThreshold = controlsWindow->depthThreshold;
				pc_renderer[0].useDepthFactor = controlsWindow->useDepthFactor;
				if(controlsWindow->color){
					pc_renderer[0].update(source->getDistancePixels(),source->getCalibratedRGBPixels(),640,480);
				}else{
					pc_renderer[0].update(source->getDistancePixels(),640,480);
				}
			}
		}

		if(!controlsWindow->onlyLive){
			for(int i=1;i<controlsWindow->numPlayers+1;i++){
				kPlayer[i-1].update();
				kPlayer[i-1].fps=controlsWindow->fps;
				if(!(controlsWindow->showPlayer[i-1])) continue;
				//cout << "updating player " << i-1 << endl;
				if(controlsWindow->mesh){
					mesh_renderer[i].objectDepthThreshold=controlsWindow->objectDepthThreshold;
					mesh_renderer[i].depthThreshold = controlsWindow->depthThreshold;
					mesh_renderer[i].useDepthFactor = controlsWindow->useDepthFactor;
					//mesh_renderer.update(kPlayer[i-1].getDistancePixels(),kPlayer[i-1].getCalibratedRGBPixels(),640,480);
					mesh_renderer[i].update(kPlayer[i-1].getDistancePixels(),kPlayer[i-1].getCalibratedTexCoords(), 640,480);
				}else{
					pc_renderer[i].oneInX=controlsWindow->oneInX;
					pc_renderer[i].oneInY=controlsWindow->oneInY;
					pc_renderer[i].dof=controlsWindow->dof;
					pc_renderer[i].depthToGray=controlsWindow->depthToGray;
					pc_renderer[i].minimumGray=controlsWindow->minimumGray;

					pc_renderer[i].focusDistance=controlsWindow->focusDistance,
					pc_renderer[i].aperture=controlsWindow->aperture,
					pc_renderer[i].pointBrightness=controlsWindow->pointBrightness,
					pc_renderer[i].rgbBrightness=controlsWindow->rgbBrightness,
					pc_renderer[i].maxPointSize=controlsWindow->maxPointSize,
					pc_renderer[i].pointSizeFactor=controlsWindow->pointSizeFactor;
					pc_renderer[i].depthThreshold = controlsWindow->depthThreshold;
					pc_renderer[i].useDepthFactor = controlsWindow->useDepthFactor;
					if(controlsWindow->color)
						pc_renderer[i].update(kPlayer[i-1].getDistancePixels(),kPlayer[i-1].getCalibratedRGBPixels(),640,480);
					else
						pc_renderer[i].update(kPlayer[i-1].getDistancePixels(),640,480);
				}
			}
		}

		if(controlsWindow->showContour){
			cvdepth.setFromPixels(source->getDepthPixels(),640,480);
			grayThreshFar = cvdepth;
			grayThresh = cvdepth;
			grayThreshFar.threshold(controlsWindow->farClip, true);
			grayThresh.threshold(controlsWindow->nearClip);
			cvAnd(grayThresh.getCvImage(), grayThreshFar.getCvImage(), cvdepth.getCvImage(), NULL);
			contourFinder.findContours(cvdepth,controlsWindow->min_blob,controlsWindow->max_blob,10,false,true);
		}

		for(int i=0;i<4;i++){
			src[i]=warp.src[i]-ofPoint(warp.origin.x,warp.origin.y);
			dst[i]=warp.dst[i]-ofPoint(warp.origin.x,warp.origin.y);
		}
		findOpenCvHomography(src,dst,homography.getPtr());

		for(int i=0;i<4;i++){
			src[i]=warp2.src[i]-ofPoint(warp2.origin.x,warp2.origin.y);
			dst[i]=warp2.dst[i]-ofPoint(warp2.origin.x,warp2.origin.y);
		}
		findOpenCvHomography(src,dst,homography2.getPtr());
	}
	if(recorder.isOpened() && source==(ofxBase3DVideo*)&kinect){
		recorder.newFrame(kinect.getPixels(),kinect.getRawDepthPixels());
	}

	if(controlsWindow->doCameraEasing){
		cameraEasing.update();
	}
}

void testApp::drawScene(){
	//ofBackground(0,0,0);
	if(controlsWindow->whiteScreen){
		ofClear(255,255,255,255);
	}else{
		ofClear(0,0,0,255);
		ofPushMatrix();
			glPointSize(controlsWindow->psize);

			if(controlsWindow->fov>0)
				ofSetupScreenPerspective(640,480,false,controlsWindow->fov);
			else
				ofSetupScreenOrtho(640,480,false);


			ofNoFill();
			ofSetColor(255,255,255,255);
			if(controlsWindow->showWarp)
				ofRect(1,1,639,479);
			//ofViewport(0,0,640,480);
			//ofBackground(0, 0, 0);

			ofTranslate(0,0,controlsWindow->postTranslateZ);

			ofTranslate(controlsWindow->rot_axis);
			ofRotate(controlsWindow->rot,0,1,0);



			if(controlsWindow->doCameraEasing){
				cameraEasing.rotate();
			}

			if(controlsWindow->showClipPlanes){
				ofSetColor(255,0,0);
				ofLine(0,0,0,ofGetHeight());
			}
			ofTranslate(-controlsWindow->rot_axis);

			ofTranslate(0,0,controlsWindow->translateZ);



			if(controlsWindow->doCameraEasing){
				cameraEasing.translate();
			}


			if(controlsWindow->useDepthFactor || controlsWindow->dof){
				ofScale(2,2,2);
			}

			if(controlsWindow->showClipPlanes){
				ofPushMatrix();
					ofSetColor(255,0,0);
					ofTranslate(0,0,-controlsWindow->nearClip);
					ofRect(0,0,640,480);
					ofTranslate(0,0,controlsWindow->nearClip-controlsWindow->depthThreshold);
					ofRect(0,0,640,480);
				ofPopMatrix();
			}

			if(controlsWindow->useDepthFactor || controlsWindow->dof){
				ofTranslate(controlsWindow->translateX,controlsWindow->translateY);
			}

			ofSetColor(controlsWindow->gray,controlsWindow->gray,controlsWindow->gray,controlsWindow->alpha);

			if(controlsWindow->soundCloud){
				if((controlsWindow->texPoints || controlsWindow->dof) && !controlsWindow->mesh){
					if(controlsWindow->gui.getValueB("DOF_TEX_CIRCLE")){
						sc_renderer.draw(&pointTex.getTextureReference());
					}else if(controlsWindow->gui.getValueB("DOF_TEX_BOKEH")){
						sc_renderer.draw(&bokehTex.getTextureReference());
					}else if(controlsWindow->gui.getValueB("DOF_TEX_GAUSS")){
						sc_renderer.draw(&gaussTex.getTextureReference());
					}
				}else if(controlsWindow->mesh){
					sc_renderer.draw(&source->getTextureReference());
				}else{
					sc_renderer.draw();
				}
			}else{
				for(int i=0;i<controlsWindow->numPlayers+1;i++){
					if(i==0 && !controlsWindow->showLive) continue;
					if(i>0 && !(controlsWindow->showPlayer[i-1])) continue;
					//cout << "drawing player" << i-1 << endl;
					if((controlsWindow->texPoints || controlsWindow->dof) && !controlsWindow->mesh){
						if(controlsWindow->gui.getValueB("DOF_TEX_CIRCLE")){
							pc_renderer[i].draw(&pointTex.getTextureReference());
						}else if(controlsWindow->gui.getValueB("DOF_TEX_BOKEH")){
							pc_renderer[i].draw(&bokehTex.getTextureReference());
						}else if(controlsWindow->gui.getValueB("DOF_TEX_GAUSS")){
							pc_renderer[i].draw(&gaussTex.getTextureReference());
						}

					}else if(!controlsWindow->mesh){// && !color){
						pc_renderer[i].draw();
					/*else if(!mesh)
						pc_renderer.draw(&source->getTextureReference());*/
					}else if(controlsWindow->color){
						if(i==0)
							mesh_renderer[i].draw(&source->getTextureReference());
						else
							mesh_renderer[i].draw(&kPlayer[i-1].getTextureReference());
					}else if(controlsWindow->depthToGray){
						if(i==0)
							mesh_renderer[i].draw(&source->getDepthTextureReference());
						else
							mesh_renderer[i].draw(&kPlayer[i-1].getDepthTextureReference());
					}else{
						mesh_renderer[i].draw();
					}

					if(controlsWindow->onlyLive) break;
				}
			}

		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if(!controlsWindow->noRender){
		ofNoFill();
		fbo.begin();
			drawScene();
		fbo.end();


		fbo2.begin();
			drawScene();
		fbo2.end();

		ofSetColor(255,255,255);
		ofSetupScreen();
		//ofViewport(0,0,ofGetWidth(),ofGetHeight());

		glPushMatrix();
		//ofSetupScreenOrtho(ofGetWidth(),ofGetHeight(),true);
		ofTranslate(warp.origin.x,warp.origin.y);
		glMultMatrixf(homography.getPtr());
		fbo.draw(0,0,640,480);
		glPopMatrix();


		glPushMatrix();
		//ofSetupScreenOrtho(ofGetWidth(),ofGetHeight(),true);
		ofTranslate(warp2.origin.x,warp2.origin.y);
		glMultMatrixf(homography2.getPtr());
		fbo2.draw(0,0,640,480);
		glPopMatrix();
		//vaWarp.draw(GL_QUADS,&fbo.getTexture(0));

		ofNoFill();
		if(controlsWindow->showDepth){
			cvdepth.draw(ofGetWidth()-320,ofGetHeight()-240,320,240);
			ofRect(ofGetWidth()-320,ofGetHeight()-240,320,240);
		}
		if(controlsWindow->showRGB){
			kinect.draw(ofGetWidth()-320,ofGetHeight()-240,320,240);
			//texRGBCalibrated.draw(ofGetWidth()-320,ofGetHeight()-240,320,240);
			ofRect(ofGetWidth()-320,ofGetHeight()-240,320,240);
		}

		if(controlsWindow->showContour)
			contourFinder.draw(0,240);

		if(controlsWindow->showStats){
			for(int i=0;i<4;i++)
				ofDrawBitmapString(ofToString((int)src[i].x) + "," + ofToString((int)src[i].y) + ":" + ofToString((int)dst[i].x) + "," + ofToString((int)dst[i].y),ofGetWidth()-200,ofGetHeight()-(i+1)*20);
			ofDrawBitmapString(ofToString(int(ofGetFrameRate())),ofGetWidth()-200,ofGetHeight()-5*20);
		}
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
		kinect.setCameraTiltAngle(controlsWindow->tilt);

		//women
		/*kPlayer[0].setup("6-13-9-48/depth.bin",true);
		kPlayer[1].setup("6-14-17-4/depth.bin",true);
		kPlayer[2].setup("6-14-38-34/depth.bin",true);
		kPlayer[3].setup("6-15-1-52/depth.bin",true);*/


		//dresses
		kPlayer[0].setup("dresses/5-16-52-46/depth.bin",true);
		kPlayer[1].setup("dresses/5-17-8-59/depth.bin",true);
		kPlayer[2].setup("dresses/5-18-59-54/depth.bin",true);
		kPlayer[3].setup("dresses/5-19-13-23/depth.bin",true);
		kPlayer[4].setup("dresses/5-19-33-10/depth.bin",true);

		source = &kinect;

		controlsWindow->numPlayers = 4;
	}else{
		kPlayer[0].setup("dresses/5-16-52-46/depth.bin",true);
		source = &kPlayer[0];

		controlsWindow->numPlayers = 1;
	}
}

//--------------------------------------------------------------
void testApp::recordChanged(bool & pressed){
	if(pressed){
		string folder = ofToString(ofGetDay())+"-"+ofToString(ofGetHours())+"-"+ofToString(ofGetMinutes())+"-"+ofToString(ofGetSeconds());

		system(("mkdir " + ofToDataPath(folder)).c_str());
		recorder.init(folder+"/depth.bin");
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
	controlsWindow->psize = fakeDOF;
	for(int i=0;i<controlsWindow->numPlayers+1;i++){
		pc_renderer[i].oneInX = 2+fakeDOF*(1/(20-fakeDOF));
		pc_renderer[i].oneInY = 2+fakeDOF*(1/(20-fakeDOF));
	}
	controlsWindow->alpha = ofClamp(255./(fakeDOF*0.5),100,255);
}


void testApp::dramaticZoomChanged(float & zoom){
	controlsWindow->fov = 60+zoom;
	controlsWindow->postTranslateZ = zoom*controlsWindow->dramaticZoomFactor;
	for(int i=0;i<controlsWindow->numPlayers+1;i++){
		pc_renderer[i].focusDistance = 800.-zoom*controlsWindow->dramaticZoomFactor;
	}
}

void testApp::toggleTwoScreens(bool & pressed){
	if(pressed){
		ofSetWindowShape(1280+200,800);
		ofSetWindowPosition(1280-200,-20);
	}
}


void testApp::showWarpChanged(bool & pressed){
	warp.setVisible(pressed);
	if(pressed){
		warp.enable();
	}else{
		warp.disable();
	}
	warp2.setVisible(pressed);
	if(pressed){
		warp2.enable();
	}else{
		warp2.disable();
	}
}

void testApp::activateCameraEasing(bool & pressed){

	// variables for animation
	// SETTING 2 : swooping out slightly to left
	 cameraEasing.begin_animated_rot_x = 50;
	 cameraEasing.end_animated_rot_x = -50;
	 cameraEasing.duration_animated_rot_x = 300;

	 cameraEasing.begin_animated_rot_y = 0;
	 cameraEasing.end_animated_rot_y = 0;
	 cameraEasing.duration_animated_rot_y = 300;

	 cameraEasing. begin_animated_rot_z = 0;
	 cameraEasing.end_animated_rot_z = 0;
	 cameraEasing.duration_animated_rot_z = 300;

	 cameraEasing.begin_animated_trans_x = 0;
	 cameraEasing.end_animated_trans_x = 0;
	 cameraEasing.duration_animated_trans_x = 300;

	 cameraEasing.begin_animated_trans_y = 0;
	 cameraEasing.end_animated_trans_y = 0;
	 cameraEasing.duration_animated_trans_y = 300;

	 cameraEasing.begin_animated_trans_z = 0;
	 cameraEasing.end_animated_trans_z = 0;
	 cameraEasing.duration_animated_trans_z = 300;

	 cameraEasing.start();

}

void testApp::activateCameraEasing2(bool & pressed){
	// variables for animation
	// SETTING 1 : swooping out slightly to left

	cameraEasing.begin_animated_rot_x = -25.0;
	cameraEasing.end_animated_rot_x = 0.0;
	cameraEasing.duration_animated_rot_x = 300;

	cameraEasing.begin_animated_rot_y = 90.0;
	cameraEasing.end_animated_rot_y = 0.0;
	cameraEasing.duration_animated_rot_y = 300;

	cameraEasing.begin_animated_rot_z = 0;
	cameraEasing.end_animated_rot_z = 0;
	cameraEasing.duration_animated_rot_z = 300;

	cameraEasing.begin_animated_trans_x = 0;
	cameraEasing.end_animated_trans_x = 0;
	cameraEasing.duration_animated_trans_x = 300;

	cameraEasing.begin_animated_trans_y = 200;
	cameraEasing.end_animated_trans_y = 0;
	cameraEasing.duration_animated_trans_y = 300;

	cameraEasing.begin_animated_trans_z = -1000;
	cameraEasing.end_animated_trans_z = 0;
	cameraEasing.duration_animated_trans_z = 300;

	cameraEasing.start();

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
	warp2.reshape(ofRectangle(ofGetWidth()-640*2-10,0,640,480));
}


void testApp::audioReceived( float * input, int bufferSize, int nChannels ){
	sc_renderer.audioReceived(input,bufferSize,nChannels);
}
