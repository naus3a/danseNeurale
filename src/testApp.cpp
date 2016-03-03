#include "testApp.h"

string s;
int curS;

//--------------------------------------------------------------
void testApp::setup()
{
	//the pirmary listener can't yet be set by default, so even if you only have one window, you need to call this line
	ofxFensterManager::get()->getPrimaryWindow()->addListener(this);
	ofSetFrameRate(30);
	ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetWindowTitle("Control Panel");
	
	vout.setup();
	ofxFenster* win=ofxFensterManager::get()->createFenster(500,0,1024,768,OF_WINDOW);
	win->addListener(&vout);
	win->setWindowTitle("Output");
	
	if(calibFile.open("calibration")){
		printf("Calibration file found\n");
		calibFile.close();
		loadCalibration();
	}else{
		printf("Calibration file not found: using defaults\n");
		skHratio = ofGetWidth()/640.0;
		skVratio = ofGetHeight()/480.0;
		skelHoffset = 0;
		skelVoffset = 0;
		spdWings = 0;
		hgtWings = 0;
		sclWings = 1;
		saveCalibration();
	}
	
	bKinect = true;
	if(bKinect){
		context.setupUsingXMLFile(ofToDataPath("openni/config/ofxopenni_config.xml",true));
		context.getXnContext().SetGlobalMirror(false);
		depth.setup(&context);
		user.setup(&context, &depth);
	}
	
	wing.setup();
	wLa.set(ofGetWidth()/2,ofGetHeight()/2);
	wRa.set(ofGetWidth()/2,ofGetHeight()/2);
	
	head.setup();
	
	bDebug = true;
	bCalib = false;
	bMouseMode = false;
	bWingsGrown= false;
	
	trkEase = 0.6;
	
	scene = 0;
	
	growAmtR = 0;
	growAmtL = 0;
	oldHandL = 0;
	oldHandR = 0;
	
	cellEmitter.clear();
	font.loadFont("verdana.ttf",60);
	curS = 0;
}


//--------------------------------------------------------------
void testApp::update()
{
	if(bKinect){
		context.update();
		user.update();
	}
	
	wing.update();
	wing.drawWing();
	
	if(scene==1){
		head.update();
		
	}else if(scene==2){
		//grow wing
		if(bMouseMode){
			wRa.set(mouseX,mouseY);
			wLa.set(mouseX,mouseY);
		}else if(performer.size()>0){
			if(performer[0]!=NULL){
				wRa.set(performer[0]->right_shoulder.end);
				wLa.set(performer[0]->left_shoulder.end);
				
				if(performer[0]->right_lower_arm.end.y<oldHandR){
					growAmtR=growAmtR<1?growAmtR+0.1:1;
				}else if(growAmtR>0){
					if(!bWingsGrown)
					growAmtR-=.03;
				}
				
				if(performer[0]->left_lower_arm.end.y<oldHandL){
					growAmtL=growAmtL<1?growAmtL+0.1:1;
				}else if(growAmtL>0){
					if(!bWingsGrown)
					growAmtL-=.03;
				}
				
				oldHandR=performer[0]->right_lower_arm.end.y-10;
				oldHandL=performer[0]->left_lower_arm.end.y-10;
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw()
{
	performer = user.getTrackedUsers();
	
	if(bCalib){
		vout.begin();
		ofSetHexColor(0x000000);
		ofRect(0, 0, vout.fbo.getWidth(), vout.fbo.getHeight());
		ofSetHexColor(0xffffff);
		glPushMatrix();
		glScalef(skHratio, skVratio, 1);
		glTranslatef(skelHoffset, skelVoffset, 0);
		depth.draw(0, 0, 640, 480);
		//performer = user.getTrackedUsers();
		if(performer.size()>0){
			if(performer[0]!=NULL){
				glPushAttrib(GL_ALL_ATTRIB_BITS);
				performer[0]->debugDraw();
				glPopAttrib();
			}
		}
		glPopMatrix();
		vout.end();
	}else{
		switch (scene) {
			case 0:
				drawWhite();
				break;
			case 1:
				drawSynch();
				break;
			case 2:
				drawGrow();
				break;
			case 3:
				drawWings();
				break;
			case 4:
				drawWings();
				break;
			case 8:
				drawDpt();
				break;
			case 9:
				drawBlack();
				break;

		}
	}
	wing.drawBuffers();
	glPushMatrix();
	glTranslatef(10, 250, 0);
	glScalef(.5, .5, 1);
	depth.draw(0, 0);
	if(performer.size()>0){
		if(performer[0]!=NULL){
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			performer[0]->debugDraw();
			glPopAttrib();
		}
	}
	glPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key, ofxFenster* win)
{
	switch (key) {
		case ' ':
			wing.toggleAnim();
			break;
		/*case 'd':
			bDebug=!bDebug;
			break;*/
		case 'c':
			bCalib=!bCalib;
			break;
			
		case 'q':
			spdWings+=0.1;
			break;
		case 'a':
			spdWings-=0.1;
			break;
		case 'w':
			hgtWings+=0.1;
			break;
		case 's':
			hgtWings-=0.1;
			break;
		case 'e':
			sclWings+=0.1;
			break;
		case 'd':
			sclWings-=0.1;
			break;
			
		case 'g':
			bWingsGrown=!bWingsGrown;
			break;

			
		case OF_KEY_RETURN:
			saveCalibration();
			//bCalib=!bCalib;
			break;
		case 'm':
			bMouseMode=!bMouseMode;
			break;
		case 'f':
			vout.toggleFullScreen();
			break;
		
		case '0':
			scene = 0;
			break;
		case '1':
			head.setup();
			scene = 1;
			break;
		case '2':
			scene = 2;
			break;
		case '3':
			scene = 3;
			wing.curAnim = 1;
			if(!bWingsGrown){
				wing.bPlayAnim=true;
				bWingsGrown = true;
			}
			break;
		case '4':
			scene = 3;
			wing.curAnim = 0;
			break;
		case '8':
			scene = 8;
			break;
		case '9':
			scene = 9;
			break;
	}
	
	if(bCalib){
		switch(key){
			case 'h'://OF_KEY_DOWN:
				skelVoffset+=1.0;
				//vout.p.y+=1.0;
				//warper.moveCorner(0,0,1);
				break;
			case 'y'://OF_KEY_UP:
				skelVoffset-=1.0;
				//vout.p.y-=1.0;
				//warper.moveCorner(0,0,-1);
				break;
			case 'u'://OF_KEY_LEFT:
				skelHoffset-=1.0;
				//vout.p.x-=1.0;
				//warper.moveCorner(0,-1,0);
				break;
			case 'j'://OF_KEY_RIGHT:
				//printf("cippa\n");
				skelHoffset+=1.0;
				//vout.p.x+=1.0;
				//warper.moveCorner(0,1,0);
				break;
			case '+':
				skHratio+=0.1;
				skVratio+=0.1;
				break;
			case '-':
				skHratio-=0.1;
				skVratio-=0.1;
				break;
			case 'z':
				skelHoffset+=10;
				break;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key, ofxFenster* win)
{
	
}

void testApp::mouseMoved(int x, int y, ofxFenster* win)
{
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}

void testApp::mouseMovedEvent(ofMouseEventArgs &args)
{
	//cout << "MOUSE WAS MOVED" << endl;
}

//-----//
void testApp::loadCalibration(){
	ofBuffer buf = ofBufferFromFile("calibration", false);
	
	string s = buf.getNextLine();
	skHratio = ofToFloat(s);
	
	s = buf.getNextLine();
	skVratio = ofToFloat(s);
	
	s = buf.getNextLine();
	skelHoffset = ofToFloat(s);
	
	s = buf.getNextLine();
	skelVoffset = ofToFloat(s);
	
	s = buf.getNextLine();
	spdWings = ofToFloat(s);
	
	s = buf.getNextLine();
	hgtWings = ofToFloat(s);
	
	s = buf.getNextLine();
	sclWings = ofToFloat(s);
	
	int _x = ofToInt(buf.getNextLine());
	int _y = ofToInt(buf.getNextLine());
	vout.move(_x,_y);
}

void testApp::saveCalibration(){
	string data = ofToString(skHratio,2)+"\n"+
	ofToString(skVratio,2)+"\n"+
	ofToString(skelHoffset,2)+"\n"+
	ofToString(skelVoffset,2)+"\n"+
	ofToString(spdWings,2)+"\n"+
	ofToString(hgtWings,2)+"\n"+
	ofToString(sclWings,2)+"\n"+
	ofToString(vout.getPosX())+"\n"+
	ofToString(vout.getPosY());
	ofBuffer buf;
	buf.set(data.c_str(),data.size());
	if(ofBufferToFile("calibration", buf, false)){
		printf("Calibration file saved \n");
	}else{
		printf("Could not save Calibration file\n");
	}
}

void testApp::drawWhite(){
	vout.begin();
	ofSetHexColor(0xffffff);
	ofRect(0, 0, vout.fbo.getWidth(), vout.fbo.getHeight());
	vout.end();
}

void testApp::drawBlack(){
	vout.begin();
	ofSetHexColor(0x000000);
	ofRect(0, 0, vout.fbo.getWidth(), vout.fbo.getHeight());
	ofSetHexColor(0xffffff);
	vout.end();
}

void testApp::drawDpt(){
	vout.begin();
	ofSetHexColor(0x000000);
	ofRect(0, 0, vout.fbo.getWidth(), vout.fbo.getHeight());
	ofSetHexColor(0xffffff);
	glPushMatrix();
	glScalef(skHratio, skVratio, 1);
	glTranslatef(skelHoffset, skelVoffset, 0);
	depth.drawCustom(0, 0, 640, 480);
	glPopMatrix();
	vout.end();
}

void testApp::drawSynch(){
	if(bMouseMode){
		head.pos.set(mouseX,mouseY);
	}else{
		if(performer.size()>0){
			if(performer[0]!=NULL){
				head.pos.set(performer[0]->neck.end);
				/*if(head.done>=8){
					if(cellEmitter.size()<MAX_PARTICLES){
						Particle p1;
						Particle p2;
						p1.create(performer[0]->right_lower_arm.end);
						p2.create(performer[0]->left_lower_arm.end);
						cellEmitter.push_back(p1);
						cellEmitter.push_back(p2);
					}else if(cellEmitter.size()>1){
						cellEmitter.erase(cellEmitter.begin());
						cellEmitter.erase(cellEmitter.begin());
					}
				}*/
			}
		}
	}
	vout.begin();
	ofSetHexColor(0x000000);
	ofRect(0, 0, vout.fbo.getWidth(), vout.fbo.getHeight());
	ofSetHexColor(0xffffff);
	
	glPushMatrix();
	glScalef(skHratio, skVratio, 1);
	glTranslatef(skelHoffset, skelVoffset, 0);
	
	glPushMatrix();
	glTranslatef(head.pos.x, head.pos.y, 0);
	ofSetColor(wing.medCol);
	ofNoFill();
	for(int i=0;i<8;i++){
		ofLine(0, 0, head.p[i].x, head.p[i].y);
		ofCircle(head.p[i].x, head.p[i].y, head.smallr);
		if(head.r[i]>=head.radius)
		ofDrawBitmapString(head.s[i]+ofToString(wing.curWaves[i],2), head.p[i]);
	}
	ofSetHexColor(0xffffff);
	ofFill();
	
	
	/*if(head.done>=8){
		for(int i=0;i<cellEmitter.size();i++){
			cellEmitter[i].draw();
		}
	}*/
	//ofFill();
	glPopMatrix();
	
	glPopMatrix();
	
	if(head.done<8){
		s = "synching...";
		font.drawString(s, vout.fbo.getWidth()-font.stringWidth(s), font.stringHeight(s)+10);
		
	}else{
		s = "SYNCED";
		font.drawString(s, vout.fbo.getWidth()-font.stringWidth(s), font.stringHeight(s)+10);
		for(int i=0;i<20;i++){
			int idx = (i+curS)<128?i+curS:128-i+curS;
			ofDrawBitmapString(ofToString(wing.wavesBuffer[0][i+curS]), vout.fbo.getWidth()-font.stringWidth(s), font.stringHeight(s)+50+(i*20));
		}
	}
	curS=curS<128?curS:0;
	vout.end();
}

void testApp::drawWings(){
	if(bMouseMode){
		wRa.set(mouseX,mouseY);
		wLa.set(mouseX,mouseY);
	}else if(performer.size()>0){
		if(performer[0]!=NULL){
			/*wRa.set((performer[0]->right_shoulder.end.x  + skelHoffset)*skHratio,
			 (performer[0]->right_shoulder.end.y  + skelVoffset)*skVratio);
			 wLa.set((performer[0]->left_shoulder.end.x  + skelHoffset)*skHratio,
			 (performer[0]->left_shoulder.end.y  + skelVoffset)*skVratio);*/
			//wRa.set((wRa*trkEase)+(performer[0]->right_shoulder.end*(1-trkEase)));
			//wLa.set((wLa*trkEase)+(performer[0]->left_shoulder.end*(1-trkEase)));
			wRa.set(performer[0]->right_shoulder.end);
			wLa.set(performer[0]->left_shoulder.end);
		}
	}
	//se troppo distante lo skel trema: low pass filter e controlli!
	
	vout.begin();
	ofSetHexColor(0x000000);
	ofRect(0, 0, vout.fbo.getWidth(), vout.fbo.getHeight());
	ofSetHexColor(0xffffff);
	glPushMatrix();
	glScalef(skHratio, skVratio, 1);
	glTranslatef(skelHoffset, skelVoffset, 0);
	wing.drawRx(wRa.x+spdWings, wRa.y+hgtWings, sclWings);
	wing.drawLx(wLa.x-spdWings, wLa.y+hgtWings, sclWings);
	glPopMatrix();
	vout.end();
}

void testApp::drawGrow(){
	
	
	vout.begin();
	ofSetHexColor(0x000000);
	ofRect(0, 0, vout.fbo.getWidth(), vout.fbo.getHeight());
	ofSetHexColor(0xffffff);
	glPushMatrix();
	glScalef(skHratio, skVratio, 1);
	glTranslatef(skelHoffset, skelVoffset, 0);
	
	wing.drawGrowR(wRa.x+spdWings, wRa.y+hgtWings, sclWings, growAmtR);
	wing.drawGrowL(wLa.x-spdWings, wLa.y+hgtWings, sclWings, growAmtL);
	
	//ofCircle(wRa.x+spdWings, wRa.y+hgtWings, 20+growAmtR);
	//ofCircle(wLa.x+spdWings, wLa.y+hgtWings, 20+growAmtL);
	glPopMatrix();
	vout.end();
}