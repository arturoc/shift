/*
 * PCRenderer.h
 *
 *  Created on: 14/12/2010
 *      Author: arturo
 */

#ifndef PCRENDERER_H_
#define PCRENDERER_H_

#include "VertexArray.h"

class PCRenderer {
public:
	PCRenderer();
	virtual ~PCRenderer();

	void setup();

	void update(float * vertexes, int w,int h);
	void update(float * vertexes, unsigned char* rgb, int w,int h);
	void draw(ofTexture * tex=NULL);

	void setDepthThreshold(float depthThres);
	void setMinDistance(float minDistance);
	void setScaleFactor(float scaleFactor);

	int oneInX, oneInY;

	float depthThreshold;
	float minDistance;
	float scaleFactor;
	float objectDepthThreshold; // how far in cm points should be to consider them a different object

	float halfW, halfH;
	float width, height;
	bool useDepthFactor;

	bool mesh;

private:
	VertexArray va;

};

#endif /* PCRENDERER_H_ */
