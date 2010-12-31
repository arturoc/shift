#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxVectorMath.h"
#include "ofxKinect.h"
#include "ofxWidgets.h"
#include "KinectPlayer.h"
#include "PCRenderer.h"
#include "MeshRenderer.h"
#include "ofx3DUtils.h"
#include "ofxOpenCv.h"
#include "ofxFbo.h"
#include "of3DVideo.h"

class testApp : public ofBaseApp{

	public:
		testApp(){}
		//:cameraPosSlider("CAMERA_POSITION")
		//,cameraLookAtSlider("CAMERA_LOOKAT"){}
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

		void cameraPosChanged(ofPoint & pos);
		void cameraLookAtChanged(ofPoint & pos);
		void cameraYChanged(float & pos);
		void cameraLookAtYChanged(float & pos);

		void liveVideoChanged(bool & pressed);
		void tiltChanged(float & tilt);

		KinectPlayer kPlayer;
		ofxKinect	 kinect;
		of3DVideo * source;

		PCRenderer pc_renderer;
		MeshRenderer mesh_renderer;

		ofxCamera camera;

		ofxWFrame gui;

		ofxCvGrayscaleImage cvdepth;
		ofxCvGrayscaleImage 	grayThresh;
		ofxCvGrayscaleImage 	grayThreshFar;
		ofxCvContourFinder  contourFinder;

		int min_blob, max_blob;

		float nearClip,farClip;
		float fov;

		ofPoint rot_axis;
		float	rot;
		float   translateZ,translateX,translateY;
		float	postTranslateZ;

		bool showContour, showDepth, showClipPlanes, showRGB;

		ofxFbo fbo;

		float tilt;
		int gray,alpha,psize;

		ofImage pointTex;

		bool texPoints;
		bool mesh;
		bool color;
		bool depthToGray;

		float depthThreshold,minDistance,scaleFactor;
		bool useDepthFactor;

		ofTexture texRGBCalibrated;
};

#endif
