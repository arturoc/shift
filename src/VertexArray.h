/*
 * ofVertexArray.h
 *
 *  Created on: 14/12/2010
 *      Author: arturo
 */

#ifndef OFVERTEXARRAY_H_
#define OFVERTEXARRAY_H_

#include "ofMain.h"

struct ComplexVertex{
	float x,y,z;
	float u,v;
	unsigned char r,g,b;
};

class VertexArray {
public:
	VertexArray();
	virtual ~VertexArray();

	void addVertex(const ofPoint & vertex);
	void addVertex(const ofPoint & vertex, const ofColor & color);
	void addVertex(const ofPoint & vertex, unsigned char r, unsigned char g, unsigned char b);
	void addVertex(const ofPoint & vertex, float u, float v);
	void addVertex(const ofPoint & vertex, const ofPoint & texCoord);
	void addVertex(float x, float y, float z);
	void addVertex(float x, float y, float z, unsigned char r, unsigned char g, unsigned char b);
	void addVertex(float x, float y, float z, float u, float v);
	void addVertex(float x, float y, float z, float u, float v, unsigned char r, unsigned char g, unsigned char b);
	void addVertex(const ComplexVertex & vertex);

	void clear();

	void draw(GLenum drawType = GL_POINTS,ofTexture * tex=NULL);


private:
	vector<ComplexVertex> vertexes;
	bool colorVertexes;
	bool texVertexes;
};

#endif /* OFVERTEXARRAY_H_ */
