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

	source = &kPlayer[0];

	kPlayer[0].setup("depth.bin",true);
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
	showLive=false;
	//showPlayer[0]=true;
	//showPlayer[1]=true;
	for(int i=0;i<10;i++)
		showPlayer[i]=false;

	fps=30;


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
	ofAddListener(gui.addToggle("video/live",false).boolEvent,this,&testApp::liveVideoChanged);
	ofAddListener(gui.addSpinSlider("tilt",0,-30,30,1).floatEvent,this,&testApp::tiltChanged);
	gui.addSpinSlider("player fps",&fps,0,60,1);
	gui.addToggle("no render",&noRender);
	ofAddListener(gui.addToggle("record",false).boolEvent,this,&testApp::recordChanged);

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

	if(!noRender){
		if(showLive){
			//cout << "updating live "  << endl;
			if(mesh){
				mesh_renderer[0].objectDepthThreshold=objectDepthThreshold;
				mesh_renderer[0].depthThreshold = depthThreshold;
				mesh_renderer[0].useDepthFactor = useDepthFactor;
				//mesh_renderer.update(source->getDistancePixels(),source->getCalibratedRGBPixels(),640,480);
				mesh_renderer[0].update(source->getDistancePixels(),source->getCalibratedTexCoords(), 640,480);
			}else{
				pc_renderer[0].oneInX=oneInX;
				pc_renderer[0].oneInY=oneInY;
				pc_renderer[0].dof=dof;
				pc_renderer[0].depthToGray=depthToGray;
				pc_renderer[0].minimumGray=minimumGray;
				pc_renderer[0].focusDistance=focusDistance,
				pc_renderer[0].aperture=aperture,
				pc_renderer[0].pointBrightness=pointBrightness,
				pc_renderer[0].rgbBrightness=rgbBrightness,
				pc_renderer[0].maxPointSize=maxPointSize,
				pc_renderer[0].pointSizeFactor=pointSizeFactor;
				pc_renderer[0].depthThreshold = depthThreshold;
				pc_renderer[0].useDepthFactor = useDepthFactor;
				if(color){
					pc_renderer[0].update(source->getDistancePixels(),source->getCalibratedRGBPixels(),640,480);
				}else{
					pc_renderer[0].update(source->getDistancePixels(),640,480);
				}
			}
		}

		for(int i=1;i<numPlayers+1;i++){
			kPlayer[i-1].update();
			kPlayer[i-1].fps=fps;
			if(!(showPlayer[i-1])) continue;
			//cout << "updating player " << i-1 << endl;
			if(mesh){
				mesh_renderer[i].objectDepthThreshold=objectDepthThreshold;
				mesh_renderer[i].depthThreshold = depthThreshold;
				mesh_renderer[i].useDepthFactor = useDepthFactor;
				//mesh_renderer.update(kPlayer[i-1].getDistancePixels(),kPlayer[i-1].getCalibratedRGBPixels(),640,480);
				mesh_renderer[i].update(kPlayer[i-1].getDistancePixels(),kPlayer[i-1].getCalibratedTexCoords(), 640,480);
			}else{
				pc_renderer[i].oneInX=oneInX;
				pc_renderer[i].oneInY=oneInY;
				pc_renderer[i].dof=dof;
				pc_renderer[i].depthToGray=depthToGray;
				pc_renderer[i].minimumGray=minimumGray;

				pc_renderer[i].focusDistance=focusDistance,
				pc_renderer[i].aperture=aperture,
				pc_renderer[i].pointBrightness=pointBrightness,
				pc_renderer[i].rgbBrightness=rgbBrightness,
				pc_renderer[i].maxPointSize=maxPointSize,
				pc_renderer[i].pointSizeFactor=pointSizeFactor;
				pc_renderer[i].depthThreshold = depthThreshold;
				pc_renderer[i].useDepthFactor = useDepthFactor;
				if(color)
					pc_renderer[i].update(kPlayer[i-1].getDistancePixels(),kPlayer[i-1].getCalibratedRGBPixels(),640,480);
				else
					pc_renderer[i].update(kPlayer[i-1].getDistancePixels(),640,480);
			}
		}

		if(showContour){
			cvdepth.setFromPixels(source->getDepthPixels(),640,480);
			grayThreshFar = cvdepth;
			grayThresh = cvdepth;
			grayThreshFar.threshold(farClip, true);
			grayThresh.threshold(nearClip);
			cvAnd(grayThresh.getCvImage(), grayThreshFar.getCvImage(), cvdepth.getCvImage(), NULL);
			contourFinder.findContours(cvdepth,min_blob,max_blob,10,false,true);
		}

		for(int i=0;i<4;i++){
			src[i]=warp.src[i]-ofPoint(warp.origin.x,warp.origin.y);
			dst[i]=warp.dst[i]-ofPoint(warp.origin.x,warp.origin.y);
		}

		findOpenCvHomography(src,dst,homography.getPtr());
	}
	if(recorder.isOpened() && source==(ofxBase3DVideo*)&kinect){
		recorder.newFrame(kinect.getPixels(),kinect.getRawDepthPixels());
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if(!noRender){
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

				if(useDepthFactor || dof){
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

				if(useDepthFactor || dof){
					ofTranslate(translateX,translateY);
				}

				ofSetColor(gray,gray,gray,alpha);
				for(int i=0;i<numPlayers+1;i++){
					if(i==0 && !showLive) continue;
					if(i>0 && !(showPlayer[i-1])) continue;
					//cout << "drawing player" << i-1 << endl;
					if((texPoints || dof) && !mesh){
						if(gui.getValueB("DOF_TEX_CIRCLE")){
							pc_renderer[i].draw(&pointTex.getTextureReference());
						}else if(gui.getValueB("DOF_TEX_BOKEH")){
							pc_renderer[i].draw(&bokehTex.getTextureReference());
						}else if(gui.getValueB("DOF_TEX_GAUSS")){
							pc_renderer[i].draw(&gaussTex.getTextureReference());
						}

					}else if(!mesh){// && !color){
						pc_renderer[i].draw();
					/*else if(!mesh)
						pc_renderer.draw(&source->getTextureReference());*/
					}else if(color){
						mesh_renderer[i].draw(&source->getTextureReference());
					}else if(pc_renderer[i].depthToGray){
						mesh_renderer[i].draw(&source->getDepthTextureReference());
					}else{
						mesh_renderer[i].draw();
					}
				}

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
		kPlayer[0].setup("5-16-52-46/depth.bin",true);
		kPlayer[1].setup("5-17-8-59/depth.bin",true);
		kPlayer[2].setup("5-18-59-54/depth.bin",true);
		kPlayer[3].setup("5-19-13-23/depth.bin",true);
		kPlayer[4].setup("5-19-33-10/depth.bin",true);
		source = &kinect;

		numPlayers = 5;
	}else{
		kPlayer[0].setup("5-16-52-46/depth.bin",true);
		source = &kPlayer[0];

		numPlayers = 1;
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
	psize = fakeDOF;
	for(int i=0;i<numPlayers+1;i++){
		pc_renderer[i].oneInX = 2+fakeDOF*(1/(20-fakeDOF));
		pc_renderer[i].oneInY = 2+fakeDOF*(1/(20-fakeDOF));
	}
	alpha = ofClamp(255./(fakeDOF*0.5),100,255);
}


void testApp::dramaticZoomChanged(float & zoom){
	fov = 60+zoom;
	postTranslateZ = zoom*dramaticZoomFactor;
	for(int i=0;i<numPlayers+1;i++){
		pc_renderer[i].focusDistance = 800.-zoom*dramaticZoomFactor;
	}
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

