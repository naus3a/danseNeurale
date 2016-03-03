#pragma once

#include "ofMain.h"
#include "ofxFensterManager.h"
#include "Vout.h"

#include "ofxOpenNI.h"
#include "Wing.h"
#include "Particle.h"

class testApp : public ofxFensterListener {

public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key, ofxFenster* win);
	void keyReleased(int key, ofxFenster* win);
	void mouseMoved(int x, int y );
	void mouseMoved(int x, int y, ofxFenster* win);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void mouseMovedEvent(ofMouseEventArgs &args);
	
	void drawWhite();
	void drawSynch();
	void drawDpt();
	void drawWings();
	void drawBlack();
	void drawGrow();
	
	void saveCalibration();
	void loadCalibration();

	Vout vout;
	
	ofxOpenNIContext context;
	ofxDepthGenerator depth;
	ofxUserGenerator user;
	std::vector<ofxTrackedUser*> performer;
	
	ofFile calibFile;
	
	struct Head {
		void setup(){
			pos.set(100,100);
			radius = 50;
			smallr = 5;
			
			s[0] = " high alfa:";
			s[1] = " low alfa:";
			s[2] = " high beta:";
			s[3] = " low beta:";
			s[4] = " high gamma:";
			s[5] = " low gamma:";
			s[6] = " high delta:";
			s[7] = " low delta:";
			
			for(int i=0;i<8;i++){
				r[i] = 0;
				p[i].set(pos.x,pos.y);
			}
			done = 0;
		};
		
		void update(){
			for(int i=done;i<8;i++){
				if(r[i]<radius){
					r[i]+=.3;
					break;
				}else{
					done++;
				}
			}
			for(int i=0;i<8;i++){
				float a = TWO_PI/14*(i+4);
				p[i].set(sin(a)*r[i],cos(a)*r[i]);
			}
		};
		
		ofPoint pos;
		ofPoint p[8];
		float r[8];
		float radius;
		float smallr;
		string s[8];
		int done;
	};
	Head head;
	
	vector<Particle> cellEmitter;
	ofTrueTypeFont font;
	
	Wing wing;
	ofVec2f wLa;
	ofVec2f wRa;
	
	float skHratio;
	float skVratio;
	float skelVoffset;
	float skelHoffset;
	float spdWings;
	float hgtWings;
	float sclWings;
	float trkEase;
	
	float growAmtR;
	float growAmtL;
	float oldHandL;
	float oldHandR;
	
	int outX;
	int outY;
	
	int scene;
	
	bool bKinect;
	bool bDebug;
	bool bCalib;
	bool bMouseMode;
	bool bWingsGrown;
};
