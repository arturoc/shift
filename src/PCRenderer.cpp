/*
 * PCRenderer.cpp
 *
 *  Created on: 14/12/2010
 *      Author: arturo
 */

#include "PCRenderer.h"
#include "ofMain.h"
#include "VertexArray.h"

PCRenderer::PCRenderer() {
	oneInX=1;
	oneInY=1;
}

PCRenderer::~PCRenderer() {
	// TODO Auto-generated destructor stub
}


void PCRenderer::setup(){

}

void PCRenderer::update(float * vertexes, int w,int h){
	va.clear();
	/// 125
	/// 346
	float depth;
	float depthFactor;
	ofPoint point;

	for(int y=0; y<h; y+=oneInY){
		for(int x=0;x<w;x+=oneInX){
			depth = *vertexes++;

			if(useDepthFactor){
				depthFactor = getDepthFactor(depth);
				point.set((x-halfW)*depthFactor,(y-halfH)*depthFactor,-depth);
			}else{
				point.set(x,y,-depth);
			}

			if(x+oneInX<w)
				vertexes+=oneInX-1;
			else
				vertexes+=w-x-1;

			if(depth==0 || depth>depthThreshold) continue;

			va.addVertex(point);
		}
		vertexes += (oneInY-1)*w;
	}
}

void PCRenderer::update(float * vertexes, unsigned char* rgb, int w,int h){
	float depth;
	int inc=1;
	for(int y=0; y<h; y+=inc){
		for(int x=0;x<w;x+=inc){
			depth = *vertexes++;
			if(depth!=0 && depth<depthThreshold){
				float depthFactor = (-depth + minDistance) * scaleFactor;
				va.addVertex((x) * depthFactor,(y) * depthFactor,depth,rgb[0],rgb[1],rgb[2]);
			}

			rgb+=3;
		}
	}
}

void PCRenderer::draw(ofTexture * tex){
	va.draw(GL_POINTS,tex);
}
