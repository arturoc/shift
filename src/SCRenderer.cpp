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

	oneInX = 4;
	oneInY = 4;
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
	int distanceXStep = oneInX-1;
	int distanceYStep = (oneInY-1)*w;
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

	deque<float*> soundCopy;
	mutex.lock();
	for(int i=0;i<soundBuffers.size();i++){
		float* soundBuffer = new float[1024*2];
		memcpy(soundBuffer,soundBuffers[i], 1024*2*sizeof(float));
		soundCopy.push_back(soundBuffer);
	}
	mutex.unlock();

	totalSoundLength = soundCopy.size()*1024-1;

	for(int y = 0; y < h; y += oneInY) {
		for(int x = 0; x < w; x += oneInX) {
			depth = *distancePixels++;


			if(x+oneInX<w)
				distancePixels+=distanceXStep;
			else
				distancePixels+=w-x-1;

			if( depth<nearThreshold || depth>farThreshold || !blob.boundingRect.inside(x,y))continue;

			ofPoint point;
			if(useDepthFactor && !dof){ // the dof shader automatically does the depthFactor conversion
				point = calibration.getWorldCoordinateFor(x,y,depth);
			}else{
				point.set(x,y,depth);
			}
			int soundPos = ofClamp(totalDistance-ofxVec2f(x,y).distance(centroid),0,totalDistance)/totalDistance*totalSoundLength;
			int numBuffer = (float)soundPos/1024.;
			int posInBuffer = soundPos%1024;
			if(totalSoundLength){
				point.z = -point.z + soundCopy[numBuffer][posInBuffer]*soundDepthFactor;
				point.x += soundCopy[numBuffer][posInBuffer]*soundDepthFactor;
				//cur.y += soundBuffers[numBuffer][posInBuffer]*soundDepthFactor;
			}else{
				point.z = -point.z;
			}
			va.addVertex(point);
		}
		distancePixels += distanceYStep;
	}

	for(int i=0;i<soundCopy.size();i++){
		delete[] soundCopy[i];
	}
}

void SCRenderer::update(float * vertexes, ofPoint * texcoords, int w,int h){
	va.clear();
	if(contourFinder.blobs.size()==0) return;
	float depth1,depth2,depth3,depth4;
	ofPoint point,point2,point3,point4;
	ofPoint tex1,tex2,tex3,tex4;

	ofxCvBlob & blob = contourFinder.blobs[0];
	ofxVec2f centroid = blob.centroid;

	float minY=blob.boundingRect.y;
	float maxY=blob.boundingRect.y+blob.boundingRect.height;
	float totalDistance;
	if(abs(centroid.y-minY)>abs(centroid.y-maxY))
		totalDistance = abs(centroid.y-minY);
	else
		totalDistance = abs(centroid.y-maxY);

	deque<float*> soundCopy;
	mutex.lock();
	for(int i=0;i<soundBuffers.size();i++){
		float* soundBuffer = new float[1024*2];
		memcpy(soundBuffer,soundBuffers[i], 1024*2*sizeof(float));
		soundCopy.push_back(soundBuffer);
	}
	mutex.unlock();

	totalSoundLength = soundCopy.size()*1024-1;

	for(int y=0; y<h-1; y++){
		for(int x=0;x<w-1;x++){
			depth1 = *vertexes++;
			depth2 = *vertexes;
			depth3 = *(vertexes+(w-1));

			tex1 = *texcoords++;

			if(depth2==0  || depth3==0 || depth2<nearThreshold || depth2>farThreshold || depth3<nearThreshold || depth3>farThreshold || !blob.boundingRect.inside(x,y))
				continue;

			depth4 = *(vertexes+(w));

			tex2 = *texcoords;
			tex3 = *(texcoords+(w-1));
			tex4 = *(texcoords+w);

			if(useDepthFactor){
				point = calibration.getWorldCoordinateFor(x,y,depth1);
				point2 = calibration.getWorldCoordinateFor(x+1,y,depth2);
				point3 = calibration.getWorldCoordinateFor(x,y+1,depth3);
				point4 = calibration.getWorldCoordinateFor(x+1,y+1,depth4);
				if(totalSoundLength){
					int soundPos = ofClamp(totalDistance-ofxVec2f(x,y).distance(centroid),0,totalDistance)/totalDistance*totalSoundLength;
					int numBuffer = (float)soundPos/1024.;
					int posInBuffer = soundPos%1024;
					point.z = -point.z + soundCopy[numBuffer][posInBuffer]*soundDepthFactor;
					point2.z = -point2.z + soundCopy[numBuffer][posInBuffer]*soundDepthFactor;
					point3.z = -point3.z + soundCopy[numBuffer][posInBuffer]*soundDepthFactor;
					point4.z = -point4.z + soundCopy[numBuffer][posInBuffer]*soundDepthFactor;
				}else{
					point.z = -point.z;
					point2.z = -point2.z;
					point3.z = -point3.z;
					point4.z = -point4.z;
				}
			}else{
				point.set(x,y,-depth1);
				point2.set((x+1),(y),-depth2);
				point3.set((x),(y+1),-depth3);
				point4.set((x+1),(y+1),-depth4);
			}




			if(depth1>nearThreshold && depth1<farThreshold &&
					fabs(depth1-depth2)<objectDepthThreshold && fabs(depth2-depth3)<objectDepthThreshold){
				va.addVertex(point,tex1);
				va.addVertex(point2,tex2);
				va.addVertex(point3,tex3);
			}

			if(depth4>nearThreshold && depth4<farThreshold &&
				fabs(depth2-depth4)<objectDepthThreshold && fabs(depth3-depth4)<objectDepthThreshold){
				va.addVertex(point2,tex2);
				va.addVertex(point4,tex4);
				va.addVertex(point3,tex3);
			}

		}

		vertexes++;
		texcoords++;
	}


	for(int i=0;i<soundCopy.size();i++){
		delete[] soundCopy[i];
	}
}

void SCRenderer::update(float * vertexes, unsigned char* rgb, int w,int h){
	va.clear();
	if(contourFinder.blobs.size()==0) return;
	float* distancePixels = vertexes;
	int rgbXStep = (oneInX-1)*3;
	int rgbYStep = (oneInY-1)*3*w;
	int distanceXStep = oneInX-1;
	int distanceYStep = (oneInY-1)*w;
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

	deque<float*> soundCopy;
	mutex.lock();
	for(int i=0;i<soundBuffers.size();i++){
		float* soundBuffer = new float[1024*2];
		memcpy(soundBuffer,soundBuffers[i], 1024*2*sizeof(float));
		soundCopy.push_back(soundBuffer);
	}
	mutex.unlock();

	for(int y = 0; y < h; y += oneInY) {
		for(int x = 0; x < w; x += oneInX) {
			depth = *distancePixels++;
			unsigned char r = *rgb++;
			unsigned char g = *rgb++;
			unsigned char b = *rgb++;


			if(x+oneInX<w){
				distancePixels+=distanceXStep;
				rgb+=rgbXStep;
			}else{
				distancePixels+=w-x-1;
				rgb+=(w-x-1)*3;
			}

			if( depth<nearThreshold || depth>farThreshold || !blob.boundingRect.inside(x,y))continue;


			ofPoint point;
			if(useDepthFactor && !dof){ // the dof shader automatically does the depthFactor conversion
				point = calibration.getWorldCoordinateFor(x,y,depth);
			}else{
				point.set(x,y,depth);
			}
			int soundPos = ofClamp(totalDistance-ofxVec2f(x,y).distance(centroid),0,totalDistance)/totalDistance*totalSoundLength;
			int numBuffer = (float)soundPos/1024.;
			int posInBuffer = soundPos%1024;
			if(totalSoundLength){
				point.z = -point.z + soundCopy[numBuffer][posInBuffer]*soundDepthFactor;
				point.x += soundCopy[numBuffer][posInBuffer]*soundDepthFactor;
				//cur.y += soundBuffers[numBuffer][posInBuffer]*soundDepthFactor;
			}else{
				point.z = -point.z;
			}
			va.addVertex(point,r,g,b);
		}
		rgb += rgbYStep;
		distancePixels += distanceYStep;
	}


	for(int i=0;i<soundCopy.size();i++){
		delete[] soundCopy[i];
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
	}else if(mesh){
		glEnable(GL_DEPTH_TEST);
		va.draw(GL_TRIANGLES,tex);
		glDisable(GL_DEPTH_TEST);
	}else{
		glDisable(GL_DEPTH_TEST);
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
	mutex.lock();
	soundBuffers.push_back(soundBuffer);
	if(soundBuffers.size()>numSoundBuffers){
		delete[] soundBuffers.front();
		soundBuffers.pop_front();
	}
	mutex.unlock();
}
