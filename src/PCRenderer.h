/*
 * PCRenderer.h
 *
 *  Created on: 14/12/2010
 *      Author: arturo
 */

#ifndef PCRENDERER_H_
#define PCRENDERER_H_

#include "VertexArray.h"
#include "Renderer.h"

class PCRenderer: public Renderer {
public:
	PCRenderer();
	virtual ~PCRenderer();

	void setup();

	void update(float * vertexes, int w,int h);
	void update(float * vertexes, unsigned char* rgb, int w,int h);
	void draw(ofTexture * tex=NULL);

	int oneInX, oneInY;

private:
	VertexArray va;

};

#endif /* PCRENDERER_H_ */
