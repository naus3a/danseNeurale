//------------------------
//1. Insert usb radio key
//2. Start TGConnector
//3. Switch headset on
//4. Run OSC sketch
//5. Run performance sw
//------------------------

import processing.net.*;

import oscP5.*;
import netP5.*;

OscP5 osc;
NetAddress ofLocation;

Client tgc;
int meditation, attention;
float[] waves;
int port = 13854;
String localHost = "127.0.0.1";
String localIp;
String localBc;
int updates;
boolean bUseRandom;

void setup() {
  size(200,200);
  tgc = new Client(this, localHost, port);
  waves = new float[8];
  meditation = attention = updates = 0;
  bUseRandom = false;
  
  localIp = NetInfo.lan();
  String[] brkIp = split(localIp,'.');
  localBc = brkIp[0]+"."+brkIp[1]+"."+brkIp[2]+".255";
  
  ofLocation = new NetAddress(localBc,12345);
  
}

void draw() {
  background(0);
  // Look for available bytes
  if(bUseRandom){
    updateRandom();
    updates++;
    sendOsc();
    
    noStroke();
    fill(0,255,0);
    rect(0,height-20,20,20);
    fill(255,255,255);
    
    fill(100);
    rect(width-20,height-20,20,20);
    stroke(255);
    text("R",width-10,height);
  }else{
    if(tgc.available() > 0) {
    
      // Grab bytes and save to waves array and att-med
      updateMindSet();
      //println("Attention is "+attention+" Meditation is "+meditation);
      updates++;
    
    
      sendOsc();
      noStroke();
      fill(0,255,0);
      rect(0,height-20,20,20);
      fill(255,255,255);
    }else{
      //println("not connected");
      noStroke();
      fill(255,0,0);
      rect(0,height-20,20,20);
      fill(255,255,255);
    }
    
    stroke(100);
    noFill();
    rect(width-20,height-20,20,20);
    stroke(255);
    text("R",width-10,height);
  }
  
  text("Sending brain activity over network:",0,height-30);
  text(localIp+"->"+localBc, 20,height-10);
  
  text("Attention: "+attention+" Meditation: "+meditation, 20,20);
  for(int i=0;i<8;i++){
    text("Wave: "+waves[i],20,40+15*i);
  }
  
  
}

void updateRandom(){
  meditation = int(random(50));
  attention = int(random(50));
  for(int i=0;i<8;i++){  
    waves[i] = random(50);
  }
}

void updateMindSet(){
  // Typical data string from NeuroSky Device:
  // aaaa0200 04390500 812036ec 237b3687 87c73734 edc73870 c7f136f2 af4f3690 bf783513 2d0535bd ab96
  // aaaa sync
  // 0200 signal (0-200, 0 is good, 200 is off-head)
  // 0439 attention  (0 is non
  // 0500 meditation
  // 8120 wave code + length (20 is Hexidecimal for 32, 8 numbers x 4 bytes each (for a float)
  // 36ec 237b3687 87c73734 edc73870 c7f136f2 af4f3690 bf783513 2d0535bd ab96			

  byte[] buffer = new byte[42];
  int byteCount = tgc.readBytes(buffer);
  if(byteCount == 42) {
    meditation = buffer[7];
    attention = buffer[5];
    for (int i = 0; i<8; i++) {
      byte[] floatBuffer = new byte[4];
      for(int j=0; j<4; j++) {
        floatBuffer[j] = buffer[10 + i*4 + j];
      }
      waves[i] = byteArrayToFloat(floatBuffer);
    }
  }
  tgc.clear();
}

float byteArrayToFloat(byte test[]) {
  int bits = 0;
  int i = 0;
  for (int shifter = 3; shifter >= 0; shifter--) {
    bits |= ((int) test[i] & 0xff) << (shifter * 8);
    i++;
  }
  return Float.intBitsToFloat(bits);
}

void keyPressed(){
  switch(key){
    case 'r':
      bUseRandom=!bUseRandom;
      break;
  }
}

void mousePressed(){
  if(mouseX>width-20 && mouseY>height-20){
    bUseRandom=!bUseRandom;
  }
}
