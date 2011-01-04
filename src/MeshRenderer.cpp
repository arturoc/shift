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
	float depth1,depth2,depth3,depth4;
	ofPoint point,point2,point3,point4;

	for(int y=0; y<h-1; y++){
		for(int x=0;x<w-1;x++){
			depth1 = *vertexes++;
			depth2 = *vertexes;
			depth3 = *(vertexes+(w-1));

			if(depth2==0 || depth2>depthThreshold || depth3==0 || depth3>depthThreshold){
				continue;
			}


			depth4 = *(vertexes+(w));


			if(useDepthFactor){
				point = calibration.getWorldCoordinateFor(x,y,depth1);
				point2 = calibration.getWorldCoordinateFor(x+1,y,depth2);
				point3 = calibration.getWorldCoordinateFor(x,y+1,depth3);
				point4 = calibration.getWorldCoordinateFor(x+1,y+1,depth4);

				point.z = -point.z;
				point2.z = -point2.z;
				point3.z = -point3.z;
				point4.z = -point4.z;
			}else{
				point.set(x,y,-depth1);
				point2.set((x+1),(y),-depth2);
				point3.set((x),(y+1),-depth3);
				point4.set((x+1),(y+1),-depth4);
			}




			if(depth1>0 && depth1<depthThreshold &&
					fabs(depth1-depth2)<objectDepthThreshold && fabs(depth2-depth3)<objectDepthThreshold){
				va.addVertex(point);
				va.addVertex(point2);
				va.addVertex(point3);
			}

			if(depth4>0 && depth4<depthThreshold &&
				fabs(depth2-depth4)<objectDepthThreshold && fabs(depth3-depth4)<objectDepthThreshold){
				va.addVertex(point2);
				va.addVertex(point4);
				va.addVertex(point3);
			}

		}

		vertexes++;
	}
}

void MeshRenderer::update(float * vertexes, ofPoint * texcoords, int w,int h){
	va.clear();
	float depth1,depth2,depth3,depth4;
	ofPoint point,point2,point3,point4;
	ofPoint tex1,tex2,tex3,tex4;

	for(int y=0; y<h-1; y++){
		for(int x=0;x<w-1;x++){
			depth1 = *vertexes++;
			depth2 = *vertexes;
			depth3 = *(vertexes+(w-1));

			tex1 = *texcoords++;

			if(depth2==0 || depth2>depthThreshold || depth3==0 || depth3>depthThreshold){
				continue;
			}


			depth4 = *(vertexes+(w));

			tex2 = *texcoords;
			tex3 = *(texcoords+(w-1));
			tex4 = *(texcoords+w);

			if(useDepthFactor){
				point = calibration.getWorldCoordinateFor(x,y,depth1);
				point2 = calibration.getWorldCoordinateFor(x+1,y,depth2);
				point3 = calibration.getWorldCoordinateFor(x,y+1,depth3);
				point4 = calibration.getWorldCoordinateFor(x+1,y+1,depth4);

				point.z = -point.z;
				point2.z = -point2.z;
				point3.z = -point3.z;
				point4.z = -point4.z;
			}else{
				point.set(x,y,-depth1);
				point2.set((x+1),(y),-depth2);
				point3.set((x),(y+1),-depth3);
				point4.set((x+1),(y+1),-depth4);
			}




			if(depth1>0 && depth1<depthThreshold &&
					fabs(depth1-depth2)<objectDepthThreshold && fabs(depth2-depth3)<objectDepthThreshold){
				va.addVertex(point,tex1);
				va.addVertex(point2,tex2);
				va.addVertex(point3,tex3);
			}

			if(depth4>0 && depth4<depthThreshold &&
				fabs(depth2-depth4)<objectDepthThreshold && fabs(depth3-depth4)<objectDepthThreshold){
				va.addVertex(point2,tex2);
				va.addVertex(point4,tex4);
				va.addVertex(point3,tex3);
			}

		}

		vertexes++;
		texcoords++;
	}
}

void MeshRenderer::update(float * vertexes, unsigned char* rgb, int w,int h){
	va.clear();
	float depth1,depth2,depth3,depth4;
	ofPoint point,point2,point3,point4;
	ofColor color,color2,color3,color4;
	int w3 = w*3;
	unsigned char r,g,b;

	for(int y=0; y<h-1; y++){
		for(int x=0;x<w-1;x++){
			depth1 = *vertexes++;
			depth2 = *(vertexes+1);
			depth3 = *(vertexes+(w-2));

			r= *rgb++;
			g= *rgb++;
			b= *rgb++;

			if(depth2==0 || depth2>depthThreshold || depth3==0 || depth3>depthThreshold){
				continue;
			}


			depth4 = *(vertexes+(w));

			color.set(r,g,b);
			r= *(rgb);
			g= *(rgb+1);
			b= *(rgb+2);
			color2.set(r,g,b);

			color3.set(*(rgb+((w3-3))),*(rgb+((w3-3)+1)),*(rgb+((w3-3)+2)));
			color4.set(*(rgb+((w3))),*(rgb+((w3)+1)),*(rgb+((w3)+2)));

			if(useDepthFactor){
				point = getRealWorldCoordinates(x,y,depth1);
				point2 = getRealWorldCoordinates(x+1,y,depth2);
				point3 = getRealWorldCoordinates(x,y+1,depth3);
				point4 = getRealWorldCoordinates(x+1,y+1,depth4);
			}else{
				point.set(x,y,-depth1);
				point2.set((x+1),(y),-depth2);
				point3.set((x),(y+1),-depth3);
				point4.set((x+1),(y+1),-depth4);
			}



			if(depth1>0 && depth1<depthThreshold &&
					fabs(depth1-depth2)<objectDepthThreshold && fabs(depth2-depth3)<objectDepthThreshold){
				va.addVertex(point,color);
				va.addVertex(point2,color2);
				va.addVertex(point3,color3);
			}

			if(depth4>0 && depth4<depthThreshold &&
				fabs(depth2-depth4)<objectDepthThreshold && fabs(depth3-depth4)<objectDepthThreshold){
				va.addVertex(point2,color2);
				va.addVertex(point4,color4);
				va.addVertex(point3,color3);
			}

		}

		vertexes++;
		rgb+=3;
	}
}

void MeshRenderer::draw(ofTexture * tex){
	glEnable(GL_DEPTH_TEST);
	va.draw(GL_TRIANGLES,tex);
}
