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
	shader.setup("variablePointSize.vert","variablePointSize.frag");
	dof=false;

	focusDistance = 800;
	aperture = 0;
	pointBrightness = 1;
	rgbBrightness = 1;
	maxPointSize = 30;
}

PCRenderer::~PCRenderer() {
	// TODO Auto-generated destructor stub
}


void PCRenderer::setup(){

}

void PCRenderer::update(float * vertexes, int w,int h){
	va.clear();
	pointSizes.clear();
	/// 125
	/// 346
	float depth;
	float depthFactor;
	ofPoint point;

	for(int y=0; y<h; y+=oneInY){
		for(int x=0;x<w;x+=oneInX){
			depth = *vertexes++;


			if(x+oneInX<w)
				vertexes+=oneInX-1;
			else
				vertexes+=w-x-1;

			if(depth==0 || depth>depthThreshold) continue;

			if(useDepthFactor && !dof){ // the dof shader automatically does the depthFactor conversion
				point = getRealWorldCoordinates(x,y,depth);
			}else{
				point.set(x,y,-depth);
			}

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
		shader.setUniform1f("minDistance",  minDistance);
		shader.setUniform1f("scaleFactor", scaleFactor);

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
		//glEnable(GL_POINT_SMOOTH);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);



		//glVertexAttribPointerARB(sizeLoc, 1, GL_FLOAT, 0, 0, &pointSizes[0]);


		va.draw(GL_POINTS,tex);

		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		//glDisableVertexAttribArrayARB(sizeLoc);
		//glDisableVertexAttribArrayARB(alphaLoc);
		shader.end();
		//if(tex) tex->unbind();
	}else{
		va.draw(GL_POINTS,tex);
	}
}
