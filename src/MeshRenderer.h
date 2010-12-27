/*
 * MeshRenderer.h
 *
 *  Created on: 27/12/2010
 *      Author: arturo
 */

#ifndef MESHRENDERER_H_
#define MESHRENDERER_H_

#include "Renderer.h"
#include "VertexArray.h"

class MeshRenderer: public Renderer {
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void setup();

	void update(float * vertexes, int w,int h);
	void update(float * vertexes, unsigned char* rgb, int w,int h);
	void draw(ofTexture * tex=NULL);

	float objectDepthThreshold; // how far in cm points should be to consider them a different object

private:
	VertexArray va;
};

#endif /* MESHRENDERER_H_ */
