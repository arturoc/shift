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
	depthThreshold 	= 1000;
	minDistance 	= -10;
	scaleFactor 	= .0021;
	oneInX=1;
	oneInY=1;
	objectDepthThreshold = 10;
	mesh = false;
	width = 640;
	height = 480;
	halfW = (width*0.5);
	halfH = (height*0.5);
	useDepthFactor=true;
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
	float depth1,depth2,depth3;
	float depthFactor,depthFactor2,depthFactor3,depthFactor4,depthFactor5,depthFactor6;
	ofPoint point,point2,point3,point4,point5,point6;
	int inc=0;
	if(mesh) inc=2;

	for(int y=0; y<h; y+=mesh?1:oneInY){
		for(int x=0;x<w;x+=mesh?inc:oneInX){
			depth1 = *vertexes++;
			if(mesh){
				depth2 = *vertexes++;
				depth3 = *(vertexes+(w-2));
				if(fabs(depth1-depth2)>objectDepthThreshold || fabs(depth1-depth3)>objectDepthThreshold || fabs(depth2-depth3)>objectDepthThreshold || depth1==0 || depth1>depthThreshold || depth2==0 || depth2>depthThreshold || depth3==0 || depth3>depthThreshold){
					continue;
				}
			}

			if(useDepthFactor){
				depthFactor = (depth1 + minDistance) * scaleFactor;
				point.set((x-halfW)*depthFactor,(y-halfH)*depthFactor,-depth1);
			}else{
				point.set(x,y,-depth1);
			}

			if(!mesh){
				if(x+oneInX<w)
					vertexes+=oneInX-1;
				else
					vertexes+=w-x-1;

				if(depth1==0 || depth1>depthThreshold) continue;

				va.addVertex(point);
			}else{
				float depth4 = *(vertexes+(w-1));
				float depth5 = *(vertexes+1);
				float depth6 = *(vertexes+w);
				if(useDepthFactor){
					depthFactor2 = (depth2 + minDistance) * scaleFactor;
					depthFactor3 = (depth3 + minDistance) * scaleFactor;
					depthFactor4 = (depth4 + minDistance) * scaleFactor;
					depthFactor5 = (depth5 + minDistance) * scaleFactor;
					depthFactor6 = (depth6 + minDistance) * scaleFactor;
					point2.set((x+1-halfW)*depthFactor2,(y-halfH)*depthFactor2,-depth2);
					point3.set((x-halfW)*depthFactor2,(y+1-halfH)*depthFactor3,-depth3);
					point4.set((x+1-halfW)*depthFactor4,(y+1-halfH)*depthFactor4,-depth4);
					point5.set((x+2-halfW)*depthFactor5,(y-halfH)*depthFactor5,-depth5);
					point6.set((x+2-halfW)*depthFactor6,(y+1-halfH)*depthFactor6,-depth6);
				}else{
					point2.set((x+1),(y),-depth2);
					point3.set((x),(y+1),-depth3);
					point4.set((x+1),(y+1),-depth4);
					point5.set((x+2),(y),-depth5);
					point6.set((x+2),(y+1),-depth6);

				}


				va.addVertex(point);
				va.addVertex(point2);
				va.addVertex(point3);

				if(fabs(depth2-depth4)<objectDepthThreshold && fabs(depth3-depth4)<objectDepthThreshold){
					va.addVertex(point2);
					va.addVertex(point4);
					va.addVertex(point3);
				}

				if(fabs(depth2-depth5)<objectDepthThreshold && fabs(depth5-depth4)<objectDepthThreshold){
					va.addVertex(point2);
					va.addVertex(point5);
					va.addVertex(point4);
				}

				if(fabs(depth5-depth6)<objectDepthThreshold && fabs(depth6-depth4)<objectDepthThreshold){
					va.addVertex(point5);
					va.addVertex(point6);
					va.addVertex(point4);
				}
			}
		}
		if(!mesh)
			vertexes += (oneInY-1)*w;
	}
}

void PCRenderer::update(float * vertexes, unsigned char* rgb, int w,int h){
	float depth;
	int inc;
	if(mesh) inc=2;
	else inc=1;
	for(int y=0; y<h; y+=inc){
		for(int x=0;x<w;x+=inc){
			depth = *vertexes++;
			if(depth!=0 && depth<depthThreshold){
				float depthFactor = (-depth + minDistance) * scaleFactor;
				va.addVertex((x) * depthFactor,(y) * depthFactor,depth,rgb[0],rgb[1],rgb[2]);
			}
			if(mesh){
				depth = *vertexes++;
				if(depth!=0 && depth<depthThreshold){
					float depthFactor = (-depth + minDistance) * scaleFactor;
					va.addVertex((x+1) * depthFactor,(y) * depthFactor,depth,rgb[0],rgb[1],rgb[2]);
				}
				depth = *(vertexes+(w-1));
				if(depth!=0 && depth<depthThreshold){
					float depthFactor = (-depth + minDistance) * scaleFactor;
					va.addVertex((x) * depthFactor,(y+1) * depthFactor,depth,rgb[0],rgb[1],rgb[2]);
				}
			}
			rgb+=3;
		}
	}
}

void PCRenderer::draw(ofTexture * tex){
	GLenum drawType;
	if(mesh) drawType = GL_TRIANGLES;
	else drawType = GL_POINTS;
	va.draw(drawType,tex);
}

void PCRenderer::setDepthThreshold(float depthThres){
	depthThreshold = depthThres;
}

void PCRenderer::setMinDistance(float _minDistance){
	minDistance = _minDistance;
}

void PCRenderer::setScaleFactor(float _scaleFactor){
	scaleFactor = _scaleFactor;
}


