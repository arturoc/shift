#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxVectorMath.h"
#include "ofxKinect.h"
#include "ofxWidgets.h"
#include "ofxKinectPlayer.h"
#include "PCRenderer.h"
#include "MeshRenderer.h"
#include "ofx3DUtils.h"
#include "ofxOpenCv.h"
#include "ofxFbo.h"
#include "ofxBase3DVideo.h"
#include "ofxKinectRecorder.h"
#include "SCRenderer.h"

class ControlsWindow;

class testApp : public ofBaseApp{

	public:
		testApp();
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		void audioReceived( float * input, int bufferSize, int nChannels );

		void drawScene();

		void cameraPosChanged(ofPoint & pos);
		void cameraLookAtChanged(ofPoint & pos);
		void cameraYChanged(float & pos);
		void cameraLookAtYChanged(float & pos);

		void fakeDOFChanged(float & fakeDOF);
		void dramaticZoomChanged(float & zoom);

		void liveVideoChanged(bool & pressed);
		void tiltChanged(float & tilt);
		void recordChanged(bool & pressed);

		void toggleTwoScreens(bool & pressed);
		void showWarpChanged(bool & pressed);

		ofxKinectPlayer kPlayer[10];
		ofxKinect	 kinect;
		ofxBase3DVideo * source;

		PCRenderer pc_renderer[10];
		MeshRenderer mesh_renderer[10];
		SCRenderer sc_renderer;

		ofxCamera camera;


		ofxCvGrayscaleImage cvdepth;
		ofxCvGrayscaleImage 	grayThresh;
		ofxCvGrayscaleImage 	grayThreshFar;
		ofxCvContourFinder  contourFinder;


		ofxFbo fbo,fbo2;


		ofImage pointTex,bokehTex,gaussTex;


		ofTexture texRGBCalibrated;

		ofxKinectRecorder recorder;

		ofxWQuadWarp warp, warp2;
		ofxMatrix4x4 homography,homography2;
		ofPoint src[4],dst[4];


		ControlsWindow * controlsWindow;
};

#endif
