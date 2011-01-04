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

class testApp : public ofBaseApp{

	public:
		testApp(){}
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

		void fakeDOFChanged(float & fakeDOF);
		void dramaticZoomChanged(float & zoom);

		void liveVideoChanged(bool & pressed);
		void tiltChanged(float & tilt);
		void recordChanged(bool & pressed);

		void toggleTwoScreens(bool & pressed);

		ofxKinectPlayer kPlayer;
		ofxKinect	 kinect;
		ofxBase3DVideo * source;

		PCRenderer pc_renderer;
		MeshRenderer mesh_renderer;

		ofxCamera camera;

		ofxWFrame gui;
		ofxWFrame comp_gui;

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

		bool showContour, showDepth, showClipPlanes, showRGB, showStats;

		ofxFbo fbo;

		float tilt;
		int gray,alpha,psize;

		ofImage pointTex,bokehTex,gaussTex;

		bool texPoints;
		bool mesh;
		bool color;
		bool depthToGray;

		float depthThreshold,minDistance,scaleFactor;
		bool useDepthFactor;

		ofTexture texRGBCalibrated;

		ofxKinectRecorder recorder;

		float dramaticZoomFactor;

		ofxWQuadWarp warp;
		VertexArray vaWarp;
		ofxMatrix4x4 homography;
		ofPoint src[4],dst[4];
		bool whiteScreen;

		bool twoScreens;

};

#endif
