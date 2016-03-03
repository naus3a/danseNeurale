#import "ofMain.h"

#define MAX_PARTICLES 100

class Particle{
public:
	void create(ofPoint p);
	void update();
	void draw();
	ofPoint pos;
	ofPoint tgt;
};