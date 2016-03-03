#include "Vout.h"

void Vout::setup(){
	ofBackground(0, 0, 0);
	p.set(0,0);
	fbo.allocate(1024,768);
	fbo.begin();
	ofSetHexColor(0x000000);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
	ofSetHexColor(0xffffff);
	fbo.end();
}

void Vout::update(){
	
}

void Vout::draw(){
	
	fbo.draw(0,0);
}

void Vout::toggleFullScreen(){
	ofToggleFullscreen();
}

void Vout::begin(){
	fbo.begin();
}

void Vout::end(){
	fbo.end();
}

int Vout::getPosX(){
	ofGetWindowPositionX();
}

int Vout::getPosY(){
	ofGetWindowPositionY();
}

void Vout::move(int x, int y){
	ofSetWindowPosition(x, y);
}