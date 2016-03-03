#include "ofMain.h"
#include "ofxFensterManager.h"

class Vout: public ofxFensterListener{
public:
	void setup();
	void update();
	void draw();
	void toggleFullScreen();
	void begin();
	void end();
	int getPosX();
	int getPosY();
	void move(int x, int y);
	
	ofFbo fbo;
	ofPoint p;
};