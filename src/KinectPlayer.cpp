/*
 * KinectPlayer.cpp
 *
 *  Created on: 14/12/2010
 *      Author: arturo
 */

#include "KinectPlayer.h"


bool KinectPlayer::lookupsCalculated = false;
float KinectPlayer::distancePixelsLookup[2048];
unsigned char KinectPlayer::depthPixelsLookupNearWhite[2048];
unsigned char KinectPlayer::depthPixelsLookupFarWhite[2048];
double KinectPlayer::fx_d = 1.0 / 5.9421434211923247e+02;
double KinectPlayer::fy_d = 1.0 / 5.9104053696870778e+02;
float KinectPlayer::cx_d = 3.3930780975300314e+02;
float KinectPlayer::cy_d = 2.4273913761751615e+02;
double KinectPlayer::fx_rgb = 5.2921508098293293e+02;
double KinectPlayer::fy_rgb = 5.2556393630057437e+02;
float KinectPlayer::cx_rgb = 3.2894272028759258e+02;
float KinectPlayer::cy_rgb = 2.6748068171871557e+02;
/*double ofxKinect::fx_rgb = 5.11017181e+02;
float ofxKinect::cx_rgb = 3.20916016e+02;
double ofxKinect::fy_rgb = 5.10642212e+02;
float ofxKinect::cy_rgb = 2.53051437e+02;*/



//--------------------------------------------------------------------
KinectPlayer::KinectPlayer():
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
{
	f = 0;
	buf = 0;
	depthBuffer = 0;
	distanceBuffer = 0;
	rgb = 0;
	calibratedRGBPixels = 0;
	bUseTexture = true;
	calculateLookups();
	R_rgb.preMultTranslate(-T_rgb);
	R_rgb = ofxMatrix4x4::getTransposedOf(R_rgb);
	fps = 30;
}

KinectPlayer::~KinectPlayer() {
	close();

	if(buf) delete[] buf;
	if(distanceBuffer) delete[] distanceBuffer;
	if(depthBuffer) delete[] depthBuffer;
	if(rgb) delete[] rgb;
	if(calibratedRGBPixels) delete[] calibratedRGBPixels;
}

void KinectPlayer::calculateLookups() {
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

void KinectPlayer::setUseTexture(bool _bUseTexture){
	bUseTexture = _bUseTexture;
}

void KinectPlayer::setup(const string & file, bool color){
	f = fopen(ofToDataPath(file).c_str(), "rb");
	if(!buf) buf 		= new uint16_t[640*480];
	if(!distanceBuffer) distanceBuffer = new float[640*480];
	if(!depthBuffer) depthBuffer = new unsigned char[640*480];
	if(!rgb) rgb = new unsigned char[640*480*3];
	if(!calibratedRGBPixels) calibratedRGBPixels = new unsigned char[640*480*3];
	memset(rgb,255,640*480*3);
	if(!tex.bAllocated())
		tex.allocate(640,480,GL_LUMINANCE);
	readColor = color;
	lastFrameTime = ofGetElapsedTimeMillis();
}

void KinectPlayer::update(){
	if(!f) return;
	if((ofGetElapsedTimeMillis()-lastFrameTime)<(1000./float(fps))) return;
	lastFrameTime = ofGetElapsedTimeMillis();
	if(readColor)
		fread(rgb,640*480*3,1,f);
	fread(buf,640*480*sizeof(uint16_t),1,f);
	/*uint16_t *buf_ptr = buf;
	unsigned char* depth_ptr = depthBuffer;
	float * distance_ptr 	 = distanceBuffer;
	float depth;

	for ( int y = 0; y < 480; y++) {
		for ( int x = 0; x < 640; x++) {
			if(*buf_ptr == 2047) {
				depth = 0;
				*depth_ptr++ = 0;
			} else {
				// using equation from https://github.com/OpenKinect/openkinect/wiki/Imaging-Information
				depth = 100.f / (-0.00307f * *buf_ptr + 3.33f);
				*depth_ptr++ = (float) (2048 * 256) / (2048 - *buf_ptr);
			}

			buf_ptr++;
			*distance_ptr++ = depth;
		}
	}*/
	int n = 640 * 480;
	if(false) {//bDepthNearValueWhite
		for(int i = 0; i < n; i++){
			distanceBuffer[i] = distancePixelsLookup[buf[i]];
			depthBuffer[i] = depthPixelsLookupNearWhite[buf[i]];
		}
	} else {
		for(int i = 0; i < n; i++){
			distanceBuffer[i] = distancePixelsLookup[buf[i]];
			depthBuffer[i] = depthPixelsLookupFarWhite[buf[i]];
		}
	}
	if(bUseTexture)
		tex.loadData(depthBuffer,640,480,GL_LUMINANCE);

}

void KinectPlayer::draw(float x, float y){
	tex.draw(x,y);
}

void KinectPlayer::draw(float x, float y, float w, float h){
	tex.draw(x,y,w,h);
}

//----------------------------------------------------------
void KinectPlayer::draw(const ofPoint & point){
	draw(point.x, point.y);
}

//----------------------------------------------------------
void KinectPlayer::draw(const ofRectangle & rect){
	draw(rect.x, rect.y, rect.width, rect.height);
}


unsigned char * KinectPlayer::getPixels(){
	return rgb;
}

unsigned char * KinectPlayer::getDepthPixels(){
	return depthBuffer;
}

float * KinectPlayer::getDistancePixels(){
	return distanceBuffer;
}

unsigned char * KinectPlayer::getCalibratedRGBPixels(){
	static ofxVec3f texcoord3d;
	static ofxVec2f texcoord2d;
	unsigned char * calibratedPixels = calibratedRGBPixels;
	float * _distancePixels = distanceBuffer;

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
				*calibratedPixels++ = rgb[pos];
				*calibratedPixels++ = rgb[pos+1];
				*calibratedPixels++ = rgb[pos+2];
			}else{
				*calibratedPixels++ = 0;
				*calibratedPixels++ = 0;
				*calibratedPixels++ = 0;
			}
		}
	}
	return calibratedRGBPixels;
}


ofTexture & KinectPlayer::getTextureReference(){
	return tex;
}

ofTexture & KinectPlayer::getDepthTextureReference(){
	return tex;
}

float KinectPlayer::getHeight(){
	return 480;
}

float KinectPlayer::getWidth(){
	return 640;
}

bool KinectPlayer::isFrameNew(){
	return true;
}

void KinectPlayer::close(){
	if(f)
		fclose(f);
	f = 0;
}

//------------------------------------
float KinectPlayer::getDistanceAt(int x, int y) {
	return distanceBuffer[y * 640 + x];
}


//------------------------------------
ofxPoint3f KinectPlayer::getWorldCoordinateFor(int x, int y) {
	//Based on http://graphics.stanford.edu/~mdfisher/Kinect.html

	ofxVec3f result;
	const double depth = getDistanceAt(x,y)/100.0;
	result.x = float((x - cx_d) * depth * fx_d);
	result.y = float((y - cy_d) * depth * fy_d);
	result.z = depth;

	return result;
}

//------------------------------------
ofxPoint3f KinectPlayer::getWorldCoordinateFor(int x, int y, float z) {
	//Based on http://graphics.stanford.edu/~mdfisher/Kinect.html

	ofxVec3f result;
	//const double depth = getDistanceAt(x,y)/100.0;
	result.x = float((x - cx_d) * z * fx_d);
	result.y = float((y - cy_d) * z * fy_d);
	result.z = z;

	return result;
}
