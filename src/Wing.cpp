/*
 *  Wing.cpp
 *  
 *
 *  Created by enrico -nausea- viola on 01/07/11.
 *  Copyright 2011 a.r.t. All rights reserved.
 *
 */

#include "Wing.h"

void Wing::setup(){
	//osc
	receiver.setup( PORT );
	
	//eeg
	curPos = 0;
	curAtt = 0;
	curMed = 0;
	for(int i=0;i<8;i++){
		curWaves[i]=0;
	}
	for(int i=0;i<NEURO_HISTORY;i++){
		attentionBuffer[i]=0;
		meditationBuffer[i]=0;
		for(int j=0;j<8;j++){
			wavesBuffer[j][i]=0;
		}
	}
	medCol.set(122,247,211);
	
	//wing
	text.loadFont("verdana.ttf",15);
	poseInc = 0.05;
	
	poses[0][0].shoulder	= 80;
	poses[0][0].elbow		= -145;
	poses[0][0].hand		= 90;
	poses[0][0].finger		= 45;
	poses[0][0].fingerSpan = 20;
	
	poses[0][1].shoulder	= 52;
	poses[0][1].elbow		= -173;
	poses[0][1].hand		= 118;
	poses[0][1].finger		= 73;
	poses[0][1].fingerSpan = -8;
	
	poses[0][2].shoulder	= 15;
	poses[0][2].elbow		= -61;
	poses[0][2].hand		= 106;
	poses[0][2].finger		= 25;
	poses[0][2].fingerSpan = -32;
	
	poses[0][3].shoulder	= 39;
	poses[0][3].elbow		= -41;
	poses[0][3].hand		= 109;
	poses[0][3].finger		= -17;
	poses[0][3].fingerSpan = -40;
	
	poses[1][0].shoulder	= 80;
	poses[1][0].elbow		= -145;
	poses[1][0].hand		= 90;
	poses[1][0].finger		= 45;
	poses[1][0].fingerSpan = 20;
	
	poses[1][1].shoulder	= 56;
	poses[1][1].elbow		= -169;
	poses[1][1].hand		= 114;
	poses[1][1].finger		= 69;
	poses[1][1].fingerSpan = -4;
	
	poses[1][2].shoulder	= 19;
	poses[1][2].elbow		= -112;
	poses[1][2].hand		= 100;
	poses[1][2].finger		= 69;
	poses[1][2].fingerSpan = -26;
	
	poses[1][3].shoulder	= -3;
	poses[1][3].elbow		= -80;
	poses[1][3].hand		= 100;
	poses[1][3].finger		= 36;
	poses[1][3].fingerSpan = -39;
	
	curPose = 0;
	nxtPose = 1;
	curAnim = 0;
	skeleton.shoulder	= poses[curAnim][curPose].shoulder;
	skeleton.elbow		= poses[curAnim][curPose].elbow;
	skeleton.hand		= poses[curAnim][curPose].hand;
	skeleton.finger		= poses[curAnim][curPose].finger;
	skeleton.fingerSpan = poses[curAnim][curPose].fingerSpan;
	
	bPlayAnim = false;
	a=0;
	
	fbo.allocate(ofGetWidth()/2,ofGetHeight());
	//gbo.setup(fbo.getWidth(),fbo.getHeight());
	//fbo.begin();
	//ofSetColor(0, 0, 0, 0);
	//ofRect(0, 0, fbo.getWidth(), fbo.getHeight());
	//fbo.end();
	
	ofMesh mesh;
	int segs = 40;
	float r1 = 40;
	float r2 = 25;
	for(int i=0;i<segs;i++){
		float a = TWO_PI/segs*i;
		float r;
		if(i%2==0){
			r=r1;
		}else{
			r=r2;
		}
		float _x = cos(a)*r;
		float _y = sin(a)*r;
		mesh.addColor(ofFloatColor(0.0,1.0,0.0,0.4));
		mesh.addVertex(ofVec3f(_x,_y,0));
	}
	pie.setMesh(mesh, GL_STATIC_DRAW);
}

void Wing::update(){
	
	//osc + eeg
	while(receiver.hasWaitingMessages()){
		curPos = curPos<NEURO_HISTORY?curPos+1:0;
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		
		if(m.getAddress()=="/esense"){
			curAtt = m.getArgAsInt32(0);
			curMed = m.getArgAsInt32(1);
			attentionBuffer[curPos] = curAtt;
			meditationBuffer[curPos]= curMed;
		}else if(m.getAddress()=="/waves"){
			for(int i=0;i<8;i++){
				curWaves[i] = m.getArgAsFloat(i);
				wavesBuffer[i][curPos] = curWaves[i];
			}
		}
	}
	
	//wing
	if(bPlayAnim){
		
		if(a<1){
			a+=poseInc;
		}else{
			a=0;
			curPose = nxtPose;
			nxtPose = nxtPose<NUM_POSES-1?nxtPose+1:0;
			switch (curPose) {
				case 0:
					poseInc = 0.05;
					break;
				case 1:
					poseInc = 0.05;
					break;
				case 2:
					poseInc = 0.01;
					break;
				case 3:
					poseInc = 0.05;
					break;
			}
		}
		lerpPose(curPose,nxtPose);
	}else{
		if(a<1){
			a+=poseInc;
			lerpPose(curPose, 0);
		}else{
			a=0;
			curPose=0;
			poseInc=0.05;
			nxtPose=1;
		}
	}
}

void Wing::lerpPose(int s, int d){
	skeleton.shoulder = ofMap(a, 0, 1, poses[curAnim][s].shoulder, poses[curAnim][d].shoulder);
	skeleton.elbow = ofMap(a, 0, 1, poses[curAnim][s].elbow, poses[curAnim][d].elbow);
	skeleton.hand = ofMap(a, 0, 1, poses[curAnim][s].hand, poses[curAnim][d].hand);
	skeleton.finger = ofMap(a, 0, 1, poses[curAnim][s].finger, poses[curAnim][d].finger);
	skeleton.fingerSpan = ofMap(a, 0, 1, poses[curAnim][s].fingerSpan, poses[curAnim][d].fingerSpan);
	
	
}

void Wing::toggleAnim(){
	bPlayAnim=!bPlayAnim;
}

void Wing::drawBuffers(){
	ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate(),0)+" curPos: "+ofToString(curPos)+" att:"+ofToString(curAtt)+" med:"+ofToString(curMed), 10,10);
	for(int i=0;i<8;i++){
		ofDrawBitmapString("wave "+ofToString(i)+": "+ofToString(curWaves[i],2), 10, 25+15*i);
	}
	
	glPushMatrix();
	glScalef(.5, .5, 1);
	
	glPushMatrix();
	glTranslatef(ofGetWidth()/2-NEURO_HISTORY/2, ofGetHeight()/3, 0);
	drawBuffer(attentionBuffer, medCol);
	glTranslatef(0, 100, 0);
	drawBuffer(meditationBuffer, medCol);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(ofGetWidth()/2+NEURO_HISTORY, 50, 0);
	for(int i=0;i<4;i++){
		glTranslatef(0, 100, 0);
		drawBuffer(wavesBuffer[i], medCol);
	}
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(ofGetWidth()/2+NEURO_HISTORY+NEURO_HISTORY, 50, 0);
	for(int i=4;i<8;i++){
		glTranslatef(0, 100, 0);
		drawBuffer(wavesBuffer[i], medCol);
	}
	glPopMatrix();
	
	glPopMatrix();
}

void Wing::drawBuffer(int * b, ofColor c){
	ofSetColor(c.r, c.g, c.b);
	for(int i=0;i<NEURO_HISTORY;i++){
		ofLine(i, 0, i, -b[i]);
	}
	ofSetColor(255, 255, 255);
	ofLine(0, 0, NEURO_HISTORY, 0);
	ofLine(curPos, -50, curPos, 50);
}

void Wing::drawBuffer(float * b, ofColor c){
	ofSetColor(c.r, c.g, c.b);
	for(int i=0;i<NEURO_HISTORY;i++){
		ofLine(i, 0, i, -b[i]);
	}
	ofSetColor(255, 255, 255);
	ofLine(0, 0, NEURO_HISTORY, 0);
	ofLine(curPos, -50, curPos, 50);
}

void Wing::drawEEG(int * b, ofColor c){
	ofSetColor(255,255, 255);
	ofLine(0, 0, NEURO_HISTORY, 0);
	
	ofSetColor(c.r, c.g, c.b);
	glBegin(GL_LINE_STRIP);
	for(int i=0;i<NEURO_HISTORY;i++){
		glVertex2f(i, -b[i]);
	}
	glEnd();
	ofSetColor(255,255, 255);
}

void Wing::drawEEG(float * b, ofColor c){
	glPushMatrix();
	glTranslatef(70, 0, 0);
	glRotatef(15, 0, 0, 1);
	
	ofSetColor(255,255, 255);
	ofLine(0, 0, NEURO_HISTORY, 0);
	ofSetColor(c.r, c.g, c.b);
	glBegin(GL_LINE_STRIP);
	for(int i=0;i<NEURO_HISTORY;i++){
		glVertex2f(i, -b[i]);
	}
	glEnd();
	glPopMatrix();
	ofSetColor(255,255, 255);
}

void Wing::drawWingInfo(){
	ofDrawBitmapString("shoulder: "+ofToString(skeleton.shoulder, 2), 20, 0);
	ofDrawBitmapString("elbow: "+ofToString(skeleton.elbow, 2), 20, 20);
	ofDrawBitmapString("hand: "+ofToString(skeleton.hand, 2), 20, 40);
	ofDrawBitmapString("finger: "+ofToString(skeleton.finger, 2), 20, 60);
	ofDrawBitmapString("fingers: "+ofToString(skeleton.fingerSpan, 2)+" -> "+ofToString(poseInc,2), 20, 80);
}

void Wing::drawWing(){
	fbo.begin();
	//gbo.draw(0,0);
	ofSetColor(0, 0, 0, 255);
	ofRect(0, 0, fbo.getWidth(), fbo.getHeight());
	glPushMatrix();
	glTranslatef(40, ofGetHeight()/2, 0);
	glRotatef(skeleton.shoulder, 0, 0, 1);
	drawPie();
	//s
	drawEEG(meditationBuffer, medCol);
	ofSetColor(255, 255, 255);
	text.drawString("low alpha: "+ofToString(curWaves[0], 2), 0, 0);
	//g
	glTranslatef(NEURO_HISTORY, 0, 0);
	glRotatef(skeleton.elbow, 0, 0, 1);
	drawPie();
	drawEEG(attentionBuffer, medCol);
	text.drawString("high alpha: "+ofToString(curWaves[1], 2), 0, 0);
	//n
	glPushMatrix();
	glTranslatef(NEURO_HISTORY, 0, 0);
	glRotatef(skeleton.hand, 0, 0, 1);
	drawPie();
	text.drawString("low beta: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(0,1);
	text.drawString("high beta: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glRotatef(skeleton.fingerSpan, 0, 0, 1);
	text.drawString("low gamma: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(2,3);
	text.drawString("high gamma: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glRotatef(skeleton.fingerSpan, 0, 0, 1);
	text.drawString("low delta: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(4, 5);
	text.drawString("high delta: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	fbo.end();
	//fbo.draw(0, 0);
}

void Wing::drawPie(){
	pie.draw(GL_TRIANGLE_STRIP,0,40);
}

void Wing::drawFinger(int w1, int w2){
	drawEEG(wavesBuffer[w1],medCol);
	glTranslatef(NEURO_HISTORY, 0, 0);
	glRotatef(skeleton.finger, 0, 0, 1);
	drawEEG(wavesBuffer[w2], medCol);
	glTranslatef(70, 0, 0);
	drawPie();
}

void Wing::drawRx(float x, float y, float s){
	glPushMatrix();
	glScalef(s, s, 1);
	glTranslatef(x, y-(fbo.getHeight()/2), 0);
	
	fbo.draw(0, 0);
	glPopMatrix();
}

void Wing::drawLx(float x, float y, float s){
	glPushMatrix();
	glScalef(s, s, 1);
	glTranslatef(x, y-(fbo.getHeight()/2), 0);
	
	fbo.draw(0, 0, -fbo.getWidth(), fbo.getHeight());
	glPopMatrix();
}

void Wing::drawGrowR(float x, float y, float s, float amt){
	glPushMatrix();
	glScalef(s, s, 1);
	glTranslatef(x, y, 0);
	glTranslatef(40, 0, 0);
	drawPie();
	
	glScalef(amt, amt, 1);
	glRotatef(skeleton.shoulder, 0, 0, 1);
	//s
	drawEEG(meditationBuffer, medCol);
	ofSetColor(255, 255, 255);
	text.drawString("low alpha: "+ofToString(curWaves[0], 2), 0, 0);
	//g
	glTranslatef(NEURO_HISTORY, 0, 0);
	glRotatef(skeleton.elbow, 0, 0, 1);
	drawPie();
	drawEEG(attentionBuffer, medCol);
	text.drawString("high alpha: "+ofToString(curWaves[1], 2), 0, 0);
	glPushMatrix();
	glTranslatef(NEURO_HISTORY, 0, 0);
	glRotatef(skeleton.hand, 0, 0, 1);
	drawPie();
	text.drawString("low beta: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(0,1);
	text.drawString("high beta: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glRotatef(skeleton.fingerSpan, 0, 0, 1);
	text.drawString("low gamma: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(2,3);
	text.drawString("high gamma: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glRotatef(skeleton.fingerSpan, 0, 0, 1);
	text.drawString("low delta: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(4, 5);
	text.drawString("high delta: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glPopMatrix();
	
	glPopMatrix();
}

void Wing::drawGrowL(float x, float y, float s, float amt){
	glPushMatrix();
	glScalef(s, s, 1);
	glTranslatef(x, y, 0);
	glScalef(-1, 1, 1);
	glTranslatef(40, 0, 0);
	drawPie();
	
	glScalef(amt, amt, 1);
	glRotatef(skeleton.shoulder, 0, 0, 1);
	//s
	drawEEG(meditationBuffer, medCol);
	ofSetColor(255, 255, 255);
	text.drawString("low alpha: "+ofToString(curWaves[0], 2), 0, 0);
	//g
	glTranslatef(NEURO_HISTORY, 0, 0);
	glRotatef(skeleton.elbow, 0, 0, 1);
	drawPie();
	drawEEG(attentionBuffer, medCol);
	text.drawString("high alpha: "+ofToString(curWaves[1], 2), 0, 0);
	glPushMatrix();
	glTranslatef(NEURO_HISTORY, 0, 0);
	glRotatef(skeleton.hand, 0, 0, 1);
	drawPie();
	text.drawString("low beta: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(0,1);
	text.drawString("high beta: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glRotatef(skeleton.fingerSpan, 0, 0, 1);
	text.drawString("low gamma: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(2,3);
	text.drawString("high gamma: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glRotatef(skeleton.fingerSpan, 0, 0, 1);
	text.drawString("low delta: "+ofToString(curWaves[2], 2), 0, 0);
	glPushMatrix();
	drawFinger(4, 5);
	text.drawString("high delta: "+ofToString(curWaves[2], 2), 0, 0);
	glPopMatrix();
	glPopMatrix();
	
	glPopMatrix();
}