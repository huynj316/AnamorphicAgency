#pragma once

#include "ofMain.h"
#include "ofxThinkGear.h"
#include "ofxTweenLite.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 12345

#define NUM_MSG_BYTES 3		//how many bytes we are expecting to receive


//--------------------------------------------------------

class testApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
	
	ofxOscSender sender;
	
	
	//	void sendToMotor(int val);
	
	//we'll use this to count how many msgs have been received so far
	//(for debugging)
	long numMsgRecvd;			// when did we last read?
	
//	ofTrueTypeFont font;
	ofSerial	mySerial;
	
	bool bSendSerialMessage;			// a flag for sending serial
	bool isInitialized;
	
	unsigned char bytesToSend[NUM_MSG_BYTES];
	
	int numMsgSent;
	//serial end
    
private:
    ofxThinkGear thinkGear;
    void attentionListener(float &param);
    void meditationListener(float &param);
	void blinkListener(float &param);
    
    ofTrueTypeFont font;
    
    float atChangeTime;
    float meChangeTime;
    float attention;
    float meditation;
	float blink;
    float distAw;
    float prevAw;
    float currAw;
    float distMw;
    float prevMw;
    float currMw;
	
	float scaledMeditation;
	float scaledAttention;
	float scaledAttention2;
	float scaledFocus;
	float volumeScaled;
	
	float radius;
	ofPoint pos;
    
};
