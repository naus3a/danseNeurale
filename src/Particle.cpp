#include "Particle.h"

#define PRADIUS 20

void Particle::create(ofPoint p){
	pos = p;
	tgt.set(0,0);
}

void Particle::update(){
	pos.x = (pos.x*0.07)+(tgt.x*0.03);
	pos.y = (pos.y*0.07)+(tgt.y*0.03);
}

void Particle::draw(){
	ofSetColor(255, 0, 0);
	ofCircle(pos.x, pos.y, PRADIUS);
	ofSetColor(255, 255, 255);
}