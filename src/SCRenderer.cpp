/*
 * SCRenderer.cpp
 *
 *  Created on: Jan 6, 2011
 *      Author: arturo
 */

#include "SCRenderer.h"

SCRenderer::SCRenderer() {
	// TODO Auto-generated constructor stub

}

SCRenderer::~SCRenderer() {
	// TODO Auto-generated destructor stub
}

void SCRenderer::setup(){
	grayImage.allocate(640, 480);
	grayThresh.allocate(640, 480);
	grayThreshFar.allocate(640, 480);

	nearThreshold = 50;
	farThreshold  = 500;

	soundDepthFactor = 0.1;
	numSoundBuffers = 64;

	focusDistance = 800;
	aperture = 0;
	pointBrightness = 1;
	rgbBrightness = 1;
	maxPointSize = 30;
	pointSizeFactor = 1;

	step = 4;
	dof = false;
	useDepthFactor = false;
	shader.setup("variablePointSize.vert","variablePointSize.frag");
}

void SCRenderer::update(unsigned char * depth, int w, int h){
	grayImage.setFromPixels(depth, w,h);

	grayThreshFar = grayImage;
	grayThresh = grayImage;
	grayThreshFar.threshold(calibration.convertDistanceToDepth(farThreshold), true);
	grayThresh.threshold(calibration.convertDistanceToDepth(nearThreshold));
	cvAnd(grayThresh.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);

	//update the cv image
	grayImage.flagImageChanged();

    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    // also, find holes is set to true so we will get interior contours as well....
    contourFinder.findContours(grayImage, 10, (w*h)/2, 20, false);
}

void SCRenderer::update(float * vertexes, int w,int h){
	va.clear();
	if(contourFinder.blobs.size()==0) return;
	float* distancePixels = vertexes;
	int distanceXStep = step-1;
	int distanceYStep = (step-1)*w;
	float depth;
	ofxCvBlob & blob = contourFinder.blobs[0];
	ofxVec2f centroid = blob.centroid;

	float minY=blob.boundingRect.y;
	float maxY=blob.boundingRect.y+blob.boundingRect.height;
	float totalDistance;
	if(abs(centroid.y-minY)>abs(centroid.y-maxY))
		totalDistance = abs(centroid.y-minY);
	else
		totalDistance = abs(centroid.y-maxY);

	totalSoundLength = soundBuffers.size()*1024-1;

	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			depth = *distancePixels++;

			distancePixels += distanceXStep;

			if( depth<nearThreshold || depth>farThreshold || !blob.boundingRect.inside(x,y))continue;

			ofPoint point;
			if(useDepthFactor && !dof){ // the dof shader automatically does the depthFactor conversion
				point = getRealWorldCoordinates(x,y,depth);
			}else{
				point.set(x,y,depth);
			}
			int soundPos = ofClamp(totalDistance-ofxVec2f(x,y).distance(centroid),0,totalDistance)/totalDistance*totalSoundLength;
			int numBuffer = (float)soundPos/1024.;
			int posInBuffer = soundPos%1024;
			if(totalSoundLength){
				point.z = -point.z + soundBuffers[numBuffer][posInBuffer]*soundDepthFactor;
				point.x += soundBuffers[numBuffer][posInBuffer]*soundDepthFactor;
				//cur.y += soundBuffers[numBuffer][posInBuffer]*soundDepthFactor;
			}else{
				point.z = -point.z;
			}
			va.addVertex(point);
		}
		distancePixels += distanceYStep;
	}
}

void SCRenderer::update(float * vertexes, unsigned char* rgb, int w,int h){
	va.clear();
	if(contourFinder.blobs.size()==0) return;
	float* distancePixels = vertexes;
	int rgbXStep = (step-1)*3;
	int rgbYStep = (step-1)*3*w;
	int distanceXStep = step-1;
	int distanceYStep = (step-1)*w;
	float depth;
	ofxCvBlob & blob = contourFinder.blobs[0];
	ofxVec2f centroid = blob.centroid;

	float minY=blob.boundingRect.y;
	float maxY=blob.boundingRect.y+blob.boundingRect.height;
	float totalDistance;
	if(abs(centroid.y-minY)>abs(centroid.y-maxY))
		totalDistance = abs(centroid.y-minY);
	else
		totalDistance = abs(centroid.y-maxY);

	totalSoundLength = soundBuffers.size()*1024-1;

	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			depth = *distancePixels++;
			unsigned char r = *rgb++;
			unsigned char g = *rgb++;
			unsigned char b = *rgb++;


			rgb += rgbXStep;
			distancePixels += distanceXStep;

			if( depth<nearThreshold || depth>farThreshold || !blob.boundingRect.inside(x,y))continue;


			ofPoint point;
			if(useDepthFactor && !dof){ // the dof shader automatically does the depthFactor conversion
				point = getRealWorldCoordinates(x,y,depth);
			}else{
				point.set(x,y,depth);
			}
			int soundPos = ofClamp(totalDistance-ofxVec2f(x,y).distance(centroid),0,totalDistance)/totalDistance*totalSoundLength;
			int numBuffer = (float)soundPos/1024.;
			int posInBuffer = soundPos%1024;
			if(totalSoundLength){
				point.z = -point.z + soundBuffers[numBuffer][posInBuffer]*soundDepthFactor;
				point.x += soundBuffers[numBuffer][posInBuffer]*soundDepthFactor;
				//cur.y += soundBuffers[numBuffer][posInBuffer]*soundDepthFactor;
			}else{
				point.z = -point.z;
			}
			va.addVertex(point,r,g,b);
		}
		rgb += rgbYStep;
		distancePixels += distanceYStep;
	}
}

void SCRenderer::draw(ofTexture * tex){
	if(dof){
		//if(tex) tex->bind();
		shader.begin(); // Turn on the Shader
		// Get the attribute and bind it
		shader.setUniform1i("tex", 0);

		shader.setUniform1f("focusDistance", focusDistance);
		shader.setUniform1f("aperture", aperture);
		shader.setUniform1f("pointBrightness", pointBrightness);
		shader.setUniform1f("rgbBrightness",  rgbBrightness);
		shader.setUniform1f("maxPointSize", maxPointSize);
		shader.setUniform1i("depthToGray", depthToGray);
		shader.setUniform1f("invDepthThres", 1./depthThreshold);
		shader.setUniform1f("minimumGray",float(minimumGray)/255.f);
		shader.setUniform1f("pointSizeFactor",pointSizeFactor);
		//sizeLoc = shader.getAttributeLocation("particleSize");
		//glEnableVertexAttribArrayARB(sizeLoc);
		//glBindAttribLocationARB(shader.getProgram(), sizeLoc, "particleSize");
		//cout << sizeLoc << "," << shader.getProgram()<<","<<pointSizes.size() << endl;
		//alphaLoc = glGetAttribLocationARB (shader.getShader(GL_VERTEX_SHADER),"alpha");

		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0);

		// super helpful: http://pmviewer.sourceforge.net/method.php
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		//glVertexAttribPointerARB(sizeLoc, 1, GL_FLOAT, 0, 0, &pointSizes[0]);


		va.draw(GL_POINTS,tex);

		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		//glDisableVertexAttribArrayARB(sizeLoc);
		//glDisableVertexAttribArrayARB(alphaLoc);
		shader.end();
		//if(tex) tex->unbind();
	}else{
		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0);
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
		va.draw(GL_POINTS,tex);
	}
}

void SCRenderer::audioReceived( float * input, int bufferSize, int nChannels ){
	float* soundBuffer = new float[bufferSize*nChannels];
	memcpy(soundBuffer,input, bufferSize*nChannels*sizeof(float));
	soundBuffers.push_back(soundBuffer);
	if(soundBuffers.size()>numSoundBuffers){
		delete[] soundBuffers.front();
		soundBuffers.pop_front();
	}
}
