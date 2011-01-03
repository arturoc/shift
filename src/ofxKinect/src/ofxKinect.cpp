#include "ofxKinect.h"
#include "ofMain.h"

// pointer to this class for static callback member functions
ofxKinect* thisKinect = NULL;

bool ofxKinect::lookupsCalculated = false;
float ofxKinect::distancePixelsLookup[2048];
unsigned char ofxKinect::depthPixelsLookupNearWhite[2048];
unsigned char ofxKinect::depthPixelsLookupFarWhite[2048];

double ofxKinect::fx_d = 1.0 / 5.9421434211923247e+02;
double ofxKinect::fy_d = 1.0 / 5.9104053696870778e+02;
float ofxKinect::cx_d = 3.3930780975300314e+02;
float ofxKinect::cy_d = 2.4273913761751615e+02;
double ofxKinect::fx_rgb = 5.2921508098293293e+02;
double ofxKinect::fy_rgb = 5.2556393630057437e+02;
float ofxKinect::cx_rgb = 3.2894272028759258e+02;
float ofxKinect::cy_rgb = 2.6748068171871557e+02;
/*double ofxKinect::fx_rgb = 5.11017181e+02;
float ofxKinect::cx_rgb = 3.20916016e+02;
double ofxKinect::fy_rgb = 5.10642212e+02;
float ofxKinect::cy_rgb = 2.53051437e+02;*/

ofxVec3f operator *(const ofxMatrix3x3 &m,const ofxVec3f &v)
  {
	ofxVec3f vv;

    vv.x = v.x*m.a + v.y*m.b + v.z*m.c;
    vv.y = v.x*m.d + v.y*m.e + v.z*m.f;
    vv.z = v.x*m.g + v.y*m.h + v.z*m.i;

    return vv;
  }



//--------------------------------------------------------------------
ofxKinect::ofxKinect():
		T_rgb( 1.9985242312092553e-02f, -7.4423738761617583e-04f, -1.0916736334336222e-02f ),
		//T_rgb(2.1354778990792557e-02, 2.5073334719943473e-03, -1.2922411623995907e-02),
		/*R_rgb(9.9984628826577793e-01, 1.2635359098409581e-03,
				-1.7487233004436643e-02, -1.4779096108364480e-03,
				9.9992385683542895e-01, -1.2251380107679535e-02,
				1.7470421412464927e-02, 1.2275341476520762e-02,
				9.9977202419716948e-01)*/
		R_rgb(9.9984628826577793e-01f, 1.2635359098409581e-03f, -1.7487233004436643e-02f, 0,
			 -1.4779096108364480e-03f, 9.9992385683542895e-01f, -1.2251380107679535e-02f, 0,
			  1.7470421412464927e-02f, 1.2275341476520762e-02f, 9.9977202419716948e-01f, 0,
			  0,0,0,1)
		/*R_rgb(9.9977321644139494e-01, 1.7292658422779497e-03, -2.1225581878346968e-02, 0,
			 -2.0032487074002391e-03, 9.9991486643051353e-01, -1.2893676196675344e-02, 0,
			  2.1201478274968936e-02, 1.2933272242365573e-02,  9.9969156632836553e-01, 0,
			  0.					, 0.					,  0.					 , 1)*/
{
	ofLog(OF_LOG_VERBOSE, "Creating ofxKinect.");

	//TODO: reset the right ones of these on close
	// common
	bVerbose 				= false;
	bGrabberInited 			= false;
	bUseTexture				= true;
	depthPixels				= NULL;
	depthPixelsRaw			= NULL;
	depthPixelsBack			= NULL;
	videoPixels		  		= NULL;
	videoPixelsBack			= NULL;
	calibratedRGBPixels		= NULL;
	distancePixels 			= NULL;

	bNeedsUpdate			= false;
	bUpdateTex				= false;
	
	bDepthNearValueWhite	= false;

	kinectContext			= NULL;
	kinectDevice			= NULL;
	
	targetTiltAngleDeg		= 0;
	bTiltNeedsApplying		= false;

	thisKinect = this;

	/*rgbDepthMatrix.getPtr()[0]=0.942040;
	rgbDepthMatrix.getPtr()[1]=-0.005672;
	rgbDepthMatrix.getPtr()[2]=0.000000;
	rgbDepthMatrix.getPtr()[3]=23.953022;
	rgbDepthMatrix.getPtr()[4]=0.004628;
	rgbDepthMatrix.getPtr()[5]=0.939875;
	rgbDepthMatrix.getPtr()[6]=0.000000;
	rgbDepthMatrix.getPtr()[7]=31.486654;
	rgbDepthMatrix.getPtr()[8]=0.000000;
	rgbDepthMatrix.getPtr()[9]=0.000000;
	rgbDepthMatrix.getPtr()[10]=0.000000;
	rgbDepthMatrix.getPtr()[11]=0.000000;
	rgbDepthMatrix.getPtr()[12]=0.000005;
	rgbDepthMatrix.getPtr()[13]=0.000003;
	rgbDepthMatrix.getPtr()[14]=0.000000;
	rgbDepthMatrix.getPtr()[15]=1.000000;*/
	
	calculateLookups();
	R_rgb.preMultTranslate(-T_rgb);
	R_rgb = ofxMatrix4x4::getTransposedOf(R_rgb);
}

void ofxKinect::calculateLookups() {
	if(!lookupsCalculated) {
		ofLog(OF_LOG_VERBOSE, "Setting up LUT for distance and depth values.");
		for(int i = 0; i < 2048; i++){
			if(i == 2047) {
				distancePixelsLookup[i] = 0;
				depthPixelsLookupNearWhite[i] = 0;
				depthPixelsLookupFarWhite[i] = 0;
			} else {
				// using equation from http://openkinect.org/wiki/Imaging_Information
				const float k1 = 0.1236;
				const float k2 = 2842.5;
				const float k3 = 1.1863;
				const float k4 = 0.0370;
				distancePixelsLookup[i] = k1 * tanf(i / k2 + k3) - k4; // calculate in meters
				distancePixelsLookup[i] *= 100; // convert to centimeters
				depthPixelsLookupNearWhite[i] = (float) (2048 * 256) / (i - 2048);
				depthPixelsLookupFarWhite[i] = 255 - depthPixelsLookupNearWhite[i];
			}
		}
	}
	lookupsCalculated = true;
}


//--------------------------------------------------------------------
ofxKinect::~ofxKinect(){
	close();
	clear();
}

//--------------------------------------------------------------------
void ofxKinect::setVerbose(bool bTalkToMe){
	bVerbose = bTalkToMe;
}

//---------------------------------------------------------------------------
unsigned char * ofxKinect::getPixels(){
	return videoPixels;
}

//---------------------------------------------------------------------------
unsigned char	* ofxKinect::getDepthPixels(){
	return depthPixels;
}

//---------------------------------------------------------------------------
unsigned short 	* ofxKinect::getRawDepthPixels(){
	return depthPixelsBack;
}

//---------------------------------------------------------------------------
float* ofxKinect::getDistancePixels() {
	return distancePixels;
}

//---------------------------------------------------------------------------
unsigned char * ofxKinect::getCalibratedRGBPixels(){
	/*ofxVec3f texcoord3d;
	unsigned char * calibratedPixels = calibratedRGBPixels;
	for ( int y = 0; y < 480; y++) {
		for ( int x = 0; x < 640; x++) {
			texcoord3d.set(x,y,0);
			texcoord3d = rgbDepthMatrix * texcoord3d ;
			texcoord3d.x = ofClamp(texcoord3d.x,0,640);
			texcoord3d.y = ofClamp(texcoord3d.y,0,480);
			int pos = int(texcoord3d.y)*640*3+int(texcoord3d.x)*3;
			*calibratedPixels++ = videoPixels[pos];
			*calibratedPixels++ = videoPixels[pos+1];
			*calibratedPixels++ = videoPixels[pos+2];
		}
	}
	return calibratedRGBPixels;*/

	//calibration method from:  http://nicolas.burrus.name/index.php/Research/KinectCalibration
	static ofxVec3f texcoord3d;
	static ofxVec2f texcoord2d;
	unsigned char * calibratedPixels = calibratedRGBPixels;
	float * _distancePixels = distancePixels;

	for ( int y = 0; y < 480; y++) {
		for ( int x = 0; x < 640; x++) {
			texcoord3d = getWorldCoordinateFor(x,y,(*_distancePixels++)*.01);
			if(texcoord3d.z){
				texcoord3d = R_rgb * texcoord3d;
				const float invZ = 1.0f / texcoord3d.z;
				//texcoord3d = rgbDepthMatrix * texcoord3d;
				texcoord2d.x = ofClamp(round(texcoord3d.x * fx_rgb *invZ) + cx_rgb,0,639);
				texcoord2d.y = ofClamp(round(texcoord3d.y * fy_rgb *invZ) + cy_rgb,0,479);

				int pos = int(texcoord2d.y)*640*3+int(texcoord2d.x)*3;
				*calibratedPixels++ = videoPixels[pos];
				*calibratedPixels++ = videoPixels[pos+1];
				*calibratedPixels++ = videoPixels[pos+2];
			}else{
				*calibratedPixels++ = 0;
				*calibratedPixels++ = 0;
				*calibratedPixels++ = 0;
			}
		}
	}
	return calibratedRGBPixels;

}

//------------------------------------
ofTexture & ofxKinect::getTextureReference(){
	if(!videoTex.bAllocated()){
		ofLog(OF_LOG_WARNING, "ofxKinect: getTextureReference - texture is not allocated");
	}
	return videoTex;
}

//---------------------------------------------------------------------------
ofTexture & ofxKinect::getDepthTextureReference(){
	if(!depthTex.bAllocated()){
		ofLog(OF_LOG_WARNING, "ofxKinect: getDepthTextureReference - texture is not allocated");
	}
	return depthTex;
}

//--------------------------------------------------------------------
bool ofxKinect::isFrameNew(){
	if(isThreadRunning()){
		return !bNeedsUpdate;
	}
	return false;	
}

//--------------------------------------------------------------------
bool ofxKinect::open(){
	if(!bGrabberInited){
		ofLog(OF_LOG_WARNING, "ofxKinect: Cannot open, init not called");
		return false;
	}

	if (freenect_init(&kinectContext, NULL) < 0) {
		ofLog(OF_LOG_ERROR, "ofxKinect: freenet_init failed");
		return false;
	}

	int number_devices = freenect_num_devices(kinectContext);
	ofLog(OF_LOG_VERBOSE, "ofxKinect: Number of Devices found: " + ofToString(number_devices));

	if (number_devices < 1) {
		ofLog(OF_LOG_ERROR, "ofxKinect: Did not find a device");
		return false;
	}

	if (freenect_open_device(kinectContext, &kinectDevice, 0) < 0) {
		ofLog(OF_LOG_ERROR, "ofxKinect: Could not open device");
		return false;
	}

	freenect_set_user(kinectDevice, this);

	startThread(true, false);	// blocking, not verbose

	return true;
}

//---------------------------------------------------------------------------
void ofxKinect::close(){
	if(isThreadRunning()){
		waitForThread(true);
	}

	//usleep(500000); // some time while thread is stopping ...

	//libusb_exit(NULL);
}

//We update the value here - but apply it in kinect thread.
//--------------------------------------------------------------------
bool ofxKinect::setCameraTiltAngle(float angleInDegrees){


	if(!kinectContext){
		return false;
	}

	targetTiltAngleDeg = ofClamp(angleInDegrees,-30,30);
	bTiltNeedsApplying = true;

	return true;
}

//--------------------------------------------------------------------
bool ofxKinect::init(bool infrared, bool setUseTexture){
	clear();

	bInfrared = infrared;
	bytespp = infrared?1:3;

	bUseTexture = setUseTexture;

	int length = width*height;
	depthPixels = new unsigned char[length];
	depthPixelsRaw = new unsigned short[length];
	depthPixelsBack = new unsigned short[length];
	distancePixels = new float[length];

	videoPixels = new unsigned char[length*bytespp];
	videoPixelsBack = new unsigned char[length*bytespp];
	calibratedRGBPixels = new unsigned char[length*bytespp];
	
	memset(depthPixels, 0, length*sizeof(unsigned char));
	memset(depthPixelsRaw, 0, length*sizeof(unsigned short));
	memset(depthPixelsBack, 0, length*sizeof(unsigned short));
	memset(distancePixels, 0, length*sizeof(float));

	memset(videoPixels, 0, length*bytespp*sizeof(unsigned char));
	memset(videoPixelsBack, 0, length*bytespp*sizeof(unsigned char));

	if(bUseTexture){
		depthTex.allocate(width, height, GL_LUMINANCE);
		videoTex.allocate(width, height, infrared?GL_LUMINANCE:GL_RGB);
	}

	bGrabberInited = true;

	ofLog(OF_LOG_VERBOSE, "ofxKinect: Inited");

	return bGrabberInited;
}

//---------------------------------------------------------------------------
void ofxKinect::clear(){
	if(depthPixels != NULL){
		delete[] depthPixels; depthPixels = NULL;
		delete[] depthPixelsRaw; depthPixelsRaw = NULL;
		delete[] depthPixelsBack; depthPixelsBack = NULL;
		delete[] distancePixels; distancePixels = NULL;

		delete[] videoPixels; videoPixels = NULL;
		delete[] videoPixelsBack; videoPixelsBack = NULL;
	}

	depthTex.clear();
	videoTex.clear();

	bGrabberInited = false;
}

//----------------------------------------------------------
void ofxKinect::update(){
	if(!kinectContext){
		return;
	}

	if (!bNeedsUpdate){
		return;
	} else {
		bUpdateTex = true;
	}

	if ( this->lock() ) {

		int n = width * height;
		if(bDepthNearValueWhite) {
			for(int i = 0; i < n; i++){
				distancePixels[i] = distancePixelsLookup[depthPixelsBack[i]];
				depthPixels[i] = depthPixelsLookupNearWhite[depthPixelsBack[i]];
			}
		} else {
			for(int i = 0; i < n; i++){
				distancePixels[i] = distancePixelsLookup[depthPixelsBack[i]];
				depthPixels[i] = depthPixelsLookupFarWhite[depthPixelsBack[i]];
			}
		}
		
		memcpy(videoPixels, videoPixelsBack, n * bytespp);

		//we have done the update
		bNeedsUpdate = false;

		this->unlock();
	}

	if(bUseTexture){
		depthTex.loadData(depthPixels, width, height, GL_LUMINANCE);
		videoTex.loadData(videoPixelsBack, width, height, bInfrared?GL_LUMINANCE:GL_RGB);
		bUpdateTex = false;
	}
}


//------------------------------------
float ofxKinect::getDistanceAt(int x, int y) {
	return distancePixels[y * width + x];
}

//------------------------------------
float ofxKinect::getDistanceAt(const ofPoint & p) {
	return getDistanceAt(p.x, p.y);
}

//------------------------------------
ofxPoint3f ofxKinect::getWorldCoordinateFor(int x, int y) {
	//Based on http://graphics.stanford.edu/~mdfisher/Kinect.html
	
	ofxVec3f result;
	const double depth = getDistanceAt(x,y)/100.0;
	result.x = float((x - cx_d) * depth * fx_d);
	result.y = float((y - cy_d) * depth * fy_d);
	result.z = depth;
	
	return result;	
}

//------------------------------------
ofxPoint3f ofxKinect::getWorldCoordinateFor(int x, int y, float z) {
	//Based on http://graphics.stanford.edu/~mdfisher/Kinect.html

	ofxVec3f result;
	//const double depth = getDistanceAt(x,y)/100.0;
	result.x = float((x - cx_d) * z * fx_d);
	result.y = float((y - cy_d) * z * fy_d);
	result.z = z;

	return result;
}

//------------------------------------
ofColor	ofxKinect::getColorAt(int x, int y) {
	int index = (y * width + x) * 3;
	ofColor c;
	c.r = videoPixels[index++];
	c.g = videoPixels[index++];
	c.b = videoPixels[index];
	c.a = 255;

	return c;
}

//------------------------------------
ofColor ofxKinect::getColorAt(const ofPoint & p) {
	return getColorAt(p.x, p.y);
}

//------------------------------------
ofColor ofxKinect::getCalibratedColorAt(int x, int y){
	//calibration method from:  http://nicolas.burrus.name/index.php/Research/KinectCalibration
	ofxVec3f texcoord3d;
	ofxVec2f texcoord2d;
	texcoord3d = getWorldCoordinateFor(x,y);
	texcoord3d = R_rgb * texcoord3d;
	const float invZ = 1/ texcoord3d.z;
	texcoord2d.x = ofClamp((texcoord3d.x * fx_rgb *invZ) + cx_rgb,0,640);
	texcoord2d.y = ofClamp((texcoord3d.y * fy_rgb *invZ) + cy_rgb,0,480);

	/*int pos = int(texcoord2d.y)*640*3+int(texcoord2d.x)*3;
	texcoord3d.set(x,y,0);
	texcoord3d = rgbDepthMatrix * texcoord3d;*/
	return getColorAt(texcoord2d);
}

//------------------------------------
ofColor ofxKinect::getCalibratedColorAt(const ofPoint & p){
	return getCalibratedColorAt(p.x,p.y);
}

//------------------------------------
/*ofxMatrix4x4 ofxKinect::getRGBDepthMatrix(){
	return rgbDepthMatrix;
}

//------------------------------------
void ofxKinect::setRGBDepthMatrix(const ofxMatrix4x4 & matrix){
	rgbDepthMatrix=matrix;
}*/

//------------------------------------
void ofxKinect::setUseTexture(bool bUse){
	bUseTexture = bUse;
}

//----------------------------------------------------------
void ofxKinect::draw(float _x, float _y, float _w, float _h){
	if(bUseTexture) {
		videoTex.draw(_x, _y, _w, _h);
	}
}

//----------------------------------------------------------
void ofxKinect::draw(float _x, float _y){
	draw(_x, _y, (float)width, (float)height);
}

//----------------------------------------------------------
void ofxKinect::draw(const ofPoint & point){
	draw(point.x, point.y);
}

//----------------------------------------------------------
void ofxKinect::drawDepth(const ofPoint & point){
	drawDepth(point.x, point.y);
}

//----------------------------------------------------------
void ofxKinect::draw(const ofRectangle & rect){
	draw(rect.x, rect.y, rect.width, rect.height);
}

//----------------------------------------------------------
void ofxKinect::drawDepth(const ofRectangle & rect){
	drawDepth(rect.x, rect.y, rect.width, rect.height);
}


//----------------------------------------------------------
void ofxKinect::drawDepth(float _x, float _y, float _w, float _h){
	if(bUseTexture) {
		depthTex.draw(_x, _y, _w, _h);
	}
}

//---------------------------------------------------------------------------
void ofxKinect::drawDepth(float _x, float _y){
	drawDepth(_x, _y, (float)width, (float)height);
}

//----------------------------------------------------------
float ofxKinect::getHeight(){
	return (float)height;
}

//---------------------------------------------------------------------------
float ofxKinect::getWidth(){
	return (float)width;
}

//---------------------------------------------------------------------------
ofPoint ofxKinect::getRawAccel(){
	return rawAccel;
}

//---------------------------------------------------------------------------
ofPoint ofxKinect::getMksAccel(){
	return mksAccel;
}

//---------------------------------------------------------------------------
void ofxKinect::enableDepthNearValueWhite(bool bEnabled){
	bDepthNearValueWhite = bEnabled;
}

//---------------------------------------------------------------------------
bool ofxKinect::isDepthNearValueWhite(){
	return bDepthNearValueWhite;
}

/* ***** PRIVATE ***** */

//---------------------------------------------------------------------------
void ofxKinect::grabDepthFrame(freenect_device *dev, void *depth, uint32_t timestamp) {
	if (thisKinect->lock()) {
		try {
			memcpy(thisKinect->depthPixelsBack, depth, FREENECT_DEPTH_11BIT_SIZE);
			thisKinect->bNeedsUpdate = true;
		}
		catch(...) {
			ofLog(OF_LOG_ERROR, "ofxKinect: Depth memcpy failed");
		}
		thisKinect->unlock();
	} else {
		ofLog(OF_LOG_WARNING, "ofxKinect: grabDepthFrame unable to lock mutex");
	}
}

//---------------------------------------------------------------------------
void ofxKinect::grabRgbFrame(freenect_device *dev, void *rgb, uint32_t timestamp) {
	if (thisKinect->lock()) {
		try {
			memcpy(thisKinect->videoPixelsBack, rgb, thisKinect->bInfrared?FREENECT_VIDEO_IR_8BIT_SIZE:FREENECT_VIDEO_RGB_SIZE);
			thisKinect->bNeedsUpdate = true;
		}
		catch (...) {
			ofLog(OF_LOG_ERROR, "ofxKinect: Rgb memcpy failed");
		}
		thisKinect->unlock();
	} else {
		ofLog(OF_LOG_ERROR, "ofxKinect: grabRgbFrame unable to lock mutex");
	}
}

//---------------------------------------------------------------------------
void ofxKinect::threadedFunction(){	

	
	freenect_set_led(kinectDevice, LED_GREEN);
	freenect_set_video_format(kinectDevice, bInfrared?FREENECT_VIDEO_IR_8BIT:FREENECT_VIDEO_RGB);
	freenect_set_depth_format(kinectDevice, FREENECT_DEPTH_11BIT);
	freenect_set_depth_callback(kinectDevice, &grabDepthFrame);
	freenect_set_video_callback(kinectDevice, &grabRgbFrame);
	
	ofLog(OF_LOG_VERBOSE, "ofxKinect: Connection opened");

	freenect_start_depth(kinectDevice);
	freenect_start_video(kinectDevice);
	
	while (isThreadRunning()) {
		if( bTiltNeedsApplying ){

			freenect_set_tilt_degs(kinectDevice, targetTiltAngleDeg);
			bTiltNeedsApplying = false;
		}

		freenect_update_tilt_state(kinectDevice);
		freenect_raw_tilt_state * tilt = freenect_get_tilt_state(kinectDevice);

		rawAccel.set(tilt->accelerometer_x, tilt->accelerometer_y, tilt->accelerometer_z);
		
		double dx,dy,dz;
		freenect_get_mks_accel(tilt, &dx, &dy, &dz);
		mksAccel.set(dx, dy, dz);
		
		ofSleepMillis(10);

//		printf("\r raw acceleration: %4d %4d %4d  mks acceleration: %4f %4f %4f", ax, ay, az, dx, dy, dz);
	}

//TODO: uncomment these when they are implemented in freenect	
	freenect_set_tilt_degs(kinectDevice, 0);
	freenect_update_tilt_state(kinectDevice);
	freenect_stop_depth(kinectDevice);
	freenect_stop_video(kinectDevice);
	freenect_set_led(kinectDevice, LED_YELLOW);

	freenect_close_device(kinectDevice);
	freenect_shutdown(kinectContext);
	
	ofLog(OF_LOG_VERBOSE, "ofxKinect: Connection closed");
}
