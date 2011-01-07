/*
 * ofVertexArray.cpp
 *
 *  Created on: 14/12/2010
 *      Author: arturo
 */

#include "VertexArray.h"

VertexArray::VertexArray() {
	colorVertexes = false;
	texVertexes = false;
}

VertexArray::~VertexArray() {
	// TODO Auto-generated destructor stub
}

void VertexArray::addVertex(float x, float y, float z){
	ComplexVertex vertex = {0,0,255,255,255,255,x,y,z};
	addVertex(vertex);
}

void VertexArray::addVertex(float x, float y, float z, unsigned char r, unsigned char g, unsigned char b){
	ComplexVertex vertex = {0.f,0.f,r,g,b,255,x,y,z};
	addVertex(vertex);
	colorVertexes = true;
}

void VertexArray::addVertex(float x, float y, float z, float u, float v){
	ComplexVertex vertex = {u,v,255,255,255,255,x,y,z};
	addVertex(vertex);
	texVertexes = true;
}

void VertexArray::addVertex(float x, float y, float z, float u, float v, unsigned char r, unsigned char g, unsigned char b){
	ComplexVertex vertex = {u,v,r,g,b,255,x,y,z};
	addVertex(vertex);
	colorVertexes = true;
	texVertexes = true;
}

void VertexArray::addVertex(const ofPoint & vertex){
	addVertex(vertex.x,vertex.y,vertex.z);
}

void VertexArray::addVertex(const ofPoint & vertex, const ofColor & color){
	addVertex(vertex.x,vertex.y,vertex.z,color.r,color.g,color.b);
}

void VertexArray::addVertex(const ofPoint & vertex, unsigned char r, unsigned char g, unsigned char b){
	addVertex(vertex.x,vertex.y,vertex.z,r,g,b);
}

void VertexArray::addVertex(const ofPoint & vertex, float u, float v){
	addVertex(vertex.x,vertex.y,vertex.z,u,v);
}

void VertexArray::addVertex(const ofPoint & vertex, const ofPoint & texCoord){
	addVertex(vertex.x,vertex.y,vertex.z,texCoord.x,texCoord.y);
}

void VertexArray::addVertex(const ComplexVertex & vertex){
	vertexes.push_back(vertex);
}

void VertexArray::draw(GLenum drawType,ofTexture * tex){
	glEnableClientState(GL_VERTEX_ARRAY);
	if(tex!=NULL) tex->bind();
	if(drawType==GL_POINTS && tex && !texVertexes){

		//glEnable(texData.textureTarget);
		/*glBindTexture( tex->texData.textureTarget, (GLuint)tex->texData.textureID);

		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		*/
		//glDisable(GL_DEPTH_TEST);
		//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		//glEnable( GL_POINT_SMOOTH );
	    //glEnable( GL_BLEND );
	    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		//tex->bind();
		glTexEnvi( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE );
		glEnable( GL_POINT_SPRITE );
		//glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
	    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);



		glVertexPointer(3, GL_FLOAT, sizeof(ComplexVertex), &vertexes[0].x);
		if(colorVertexes){
			glEnableClientState (GL_COLOR_ARRAY);
			glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(ComplexVertex), &vertexes[0].r);
		}
		glDrawArrays(drawType, 0, vertexes.size());
		//glBindBuffer(GL_ARRAY_BUFFER,0);
		glDisable(GL_POINT_SPRITE);
	}else{
		glVertexPointer(3, GL_FLOAT, sizeof(ComplexVertex), &vertexes[0].x);
		if(colorVertexes){
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3,GL_UNSIGNED_BYTE,sizeof(ComplexVertex),&vertexes[0].r);
		}
		if(texVertexes && tex){
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2,GL_FLOAT,sizeof(ComplexVertex),&vertexes[0].u);
		}
		/*GLenum format=GL_T2F_C4UB_V3F;
		if(colorVertexes && texVertexes){
			format = GL_T2F_C4UB_V3F;
		}else if(colorVertexes){
			format = GL_T2F_C4UB_V3F;
		}else if(texVertexes){
			format = GL_T2F_V3F;
		}
		glInterleavedArrays(format,sizeof(ComplexVertex),&vertexes[0].u);*/
		glDrawArrays(drawType, 0, vertexes.size());
	}

	if(tex!=NULL) tex->unbind();
	if(colorVertexes){
		glDisableClientState(GL_COLOR_ARRAY);
	}
	if(texVertexes){
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

		//glEnableClientState(GL_POINT_SIZE_ARRAY_OES);
	//glPointSizePointerOES(GL_FLOAT, sizeof(PointSprite2D), (GLvoid*) (sizeof(m_vertices[0].position) + sizeof(m_vertices[0].color)));
}

void VertexArray::clear(){
	vertexes.clear();
	colorVertexes = false;
	texVertexes = false;
}

const vector<ComplexVertex> &  VertexArray::getVertexes(){
	return vertexes;
}
