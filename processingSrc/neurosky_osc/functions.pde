void sendOsc(){
  OscMessage esenseMsg = new OscMessage("/esense");
  esenseMsg.add(attention);//+random(-50,50));
  esenseMsg.add(meditation);//+random(-50,50));
  OscP5.flush(esenseMsg,ofLocation);
  
  OscMessage wavesMsg = new OscMessage("/waves");
  for(int i=0;i<8;i++){
    waves[i]*=.001;//random(-50,50);
  }
  
  wavesMsg.add(waves);
  OscP5.flush(wavesMsg,ofLocation);
  
  OscMessage noteMsg = new OscMessage("/note");
  noteMsg.add(waves[int(random(7))]*.001);
  OscP5.flush(noteMsg,ofLocation);
}
