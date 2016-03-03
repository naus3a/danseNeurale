/*
 *  Wing.h
 *  
 *
 *  Created by enrico -nausea- viola on 01/07/11.
 *  Copyright 2011 a.r.t. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxOsc.h"

//#include "Arc.h"

#define PORT 12345
#define NUM_MSG_STRINGS 20
#define NEURO_HISTORY 128
#define NUM_POSES 4

class Wing{
public:
	void setup();
	void update();
	void lerpPose(int s, int d);
	void toggleAnim();
	void drawBuffers();
	void drawWing();
	void drawWingInfo();
	void drawBuffer(int * b, ofColor c);
	void drawBuffer(float * b, ofColor c);
	void drawEEG(int * b, ofColor c);
	void drawEEG(float * b, ofColor c);
	void drawPie();
	void drawFinger(int w1, int w2);
	void drawRx(float x, float y, float s);
	void drawLx(float x, float y, float s);
	void drawGrowR(float x, float y, float s, float amt);
	void drawGrowL(float x, float y, float s, float amt);
	
	ofTrueTypeFont text;
	int attentionBuffer[NEURO_HISTORY];
	int meditationBuffer[NEURO_HISTORY];
	float wavesBuffer[8][NEURO_HISTORY];
	int curAtt;
	int curMed;
	int curPos;
	int curAnim;
	float curWaves[8];
	float poseInc;
	int curPose;
	int nxtPose;
	float a;
	
	struct SKeletonAngles {
		float shoulder;
		float elbow;
		float hand;
		float finger;
		float fingerSpan;
	};
	
	SKeletonAngles poses[2][NUM_POSES];
	SKeletonAngles grpos[NUM_POSES];
	SKeletonAngles skeleton;
	bool bPlayAnim;
	
	ofColor medCol;
	
	ofFbo fbo;
	//ofFbo gbo;
	
	ofVbo pie;
	
private:
	ofxOscReceiver receiver;
};