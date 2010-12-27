/*
 * ofxWUtils.h
 *
 *  Created on: 27/12/2010
 *      Author: arturo
 */

#ifndef OFXWUTILS_H_
#define OFXWUTILS_H_

#include <string>

string ofxWReplace(const string & source, char replace, const string & with){
	string tmp = source;
	for(int i=0; i<tmp.length(); i++){
		if(tmp[i]==replace){
			tmp.replace(i,1,with);
			i+=with.length()-1;
		}
	}
	return tmp;
}

#endif /* OFXWUTILS_H_ */
