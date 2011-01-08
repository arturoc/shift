#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "ofxFenster/ofxFenster.h"
#include "ControlsWindow.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context

	testApp * app = new testApp();
	ControlsWindow * controlsWin = new ControlsWindow(app);

	app->controlsWindow = controlsWin;

	ofxFenster fenster;
	fenster.init(controlsWin,&window);
	fenster.setBounds(20,20,600,768);


	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( app );

}
