/*
 * MeshRenderer.cpp
 *
 *  Created on: 27/12/2010
 *      Author: arturo
 */

#include "MeshRenderer.h"

MeshRenderer::MeshRenderer() {

	objectDepthThreshold = 10;

}

MeshRenderer::~MeshRenderer() {
	// TODO Auto-generated destructor stub
}


void MeshRenderer::setup(){

}

void MeshRenderer::update(float * vertexes, int w,int h){
	va.clear();
	float depth1,depth2,depth3,depth4,depth5,depth6;
	float depthFactor,depthFactor2,depthFactor3,depthFactor4,depthFactor5,depthFactor6;
	ofPoint point,point2,point3,point4,point5,point6;
	int inc=2;

	for(int y=0; y<h; y++){
		for(int x=0;x<w;x+=inc){
			depth1 = *vertexes++;
			depth2 = *vertexes++;
			depth3 = *(vertexes+(w-2));
			if(depth1==0 || depth1>depthThreshold || depth2==0 || depth2>depthThreshold || depth3==0 || depth3>depthThreshold){
				continue;
			}
			depth4 = *(vertexes+(w-1));
			depth5 = *(vertexes+1);
			depth6 = *(vertexes+w);

			if(useDepthFactor){
				depthFactor = getDepthFactor(depth1);
				depthFactor2 = getDepthFactor(depth2);
				depthFactor3 = getDepthFactor(depth3);
				depthFactor4 = getDepthFactor(depth4);
				depthFactor5 = getDepthFactor(depth5);
				depthFactor6 = getDepthFactor(depth6);
				point.set((x-halfW)*depthFactor,(y-halfH)*depthFactor,-depth1);
				point2.set((x+1-halfW)*depthFactor2,(y-halfH)*depthFactor2,-depth2);
				point3.set((x-halfW)*depthFactor2,(y+1-halfH)*depthFactor3,-depth3);
				point4.set((x+1-halfW)*depthFactor4,(y+1-halfH)*depthFactor4,-depth4);
				point5.set((x+2-halfW)*depthFactor5,(y-halfH)*depthFactor5,-depth5);
				point6.set((x+2-halfW)*depthFactor6,(y+1-halfH)*depthFactor6,-depth6);
			}else{
				point.set(x,y,-depth1);
				point2.set((x+1),(y),-depth2);
				point3.set((x),(y+1),-depth3);
				point4.set((x+1),(y+1),-depth4);
				point5.set((x+2),(y),-depth5);
				point6.set((x+2),(y+1),-depth6);
			}

			if(fabs(depth1-depth2)<objectDepthThreshold && fabs(depth2-depth3)<objectDepthThreshold){
				va.addVertex(point);
				va.addVertex(point2);
				va.addVertex(point3);
			}

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
}

void MeshRenderer::update(float * vertexes, unsigned char* rgb, int w,int h){

}

void MeshRenderer::draw(ofTexture * tex){
	va.draw(GL_TRIANGLES,tex);
}
