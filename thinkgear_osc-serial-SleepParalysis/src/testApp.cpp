#include "testApp.h"

void testApp::setup()
{
	ofSetCircleResolution(50);
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    
    thinkGear.setup();
    ofAddListener(thinkGear.attentionChangeEvent, this, &testApp::attentionListener);
    ofAddListener(thinkGear.meditationChangeEvent, this, &testApp::meditationListener);
	ofAddListener(thinkGear.blinkChangeEvent, this, &testApp::blinkListener);
    
    font.loadFont("font/DroidSansMono.ttf", 20);
    
    distAw = 0.0;
    prevAw = 0.0;
    currAw = 0.0;
    distMw = 0.0;
    prevMw = 0.0;
    currMw = 0.0;
    
    atChangeTime = 0.0;
    attention = 0.0;
    meChangeTime = 0.0;
    meditation = 0.0;
	
	//osc sender begin
	ofBackground(40, 100, 40);
	
	// open an outgoing connection to HOST:PORT
	sender.setup(HOST, PORT);
	//osc sender end
	
	//serial start
	ofSetVerticalSync(true);
	
	mySerial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = mySerial.getDeviceList();
	
	// this should be set to whatever com port your serial device is connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....
	int baud = 9600;
	mySerial.setup(0, baud); //open the first device
	
	
	bytesToSend[0] = ofMap(attention, 0, 100, 0, 254);
	bytesToSend[1] = 9;
//	bytesToSend[2] = ofMap(blink, 0, 100, 0, 254);
	
	mySerial.flush();
    isInitialized = false;
	
	//serial end
	
}

//--------------------------------------------------------------

void testApp::update()
{
    thinkGear.update();
    
    float cur = ofGetElapsedTimef();
    float progress = cur - atChangeTime;
    progress = ofClamp(progress, 0.0, 1.0);
    if (progress == 0.0) prevAw = currAw;
    currAw = ofxTweenLite::tween(prevAw, distAw, progress, OF_EASE_SINE_INOUT);
    
    progress = cur - meChangeTime;
    progress = ofClamp(progress, 0.0, 1.0);
    if (progress == 0.0) prevMw = currMw;
    currMw = ofxTweenLite::tween(prevMw, distMw, progress, OF_EASE_SINE_INOUT);
	
	//serial start
	if(mySerial.available() >0){ //something came in over serial port
        
        if(!isInitialized) isInitialized = true; //we heard from Arduino, we're initialized.
        
        char incoming = mySerial.readByte();
        cout << "message received: "<< incoming << endl;
        
        mySerial.flush(); //flush whatever messages were received, clean slate next frame
        
		mySerial.writeBytes(bytesToSend, NUM_MSG_BYTES); //send out current bytes to send
        numMsgSent++; //for our own count
	}
	
}



//--------------------------------------------------------------

void testApp::draw()
{
    ofBackgroundGradient(ofColor::white, ofColor::gray);
    
    ofPushStyle();
    ofSetColor(ofColor::black);
    string qStr = "";
    if (thinkGear.getSignalQuality() == 0.0)
    {
        qStr = "good";
    }
    else
    {
        qStr = "poor (" + ofToString(thinkGear.getSignalQuality()) + ")";
    }
    font.drawString("signal quality = " + qStr, 10, 40);
    ofPopStyle();
	
	//blink
	font.drawString("Blink", 10, (blink));
    
    //attention bar
    ofPushMatrix();
    ofTranslate(0, 30);
    ofPushStyle();
    ofSetColor(ofColor::black);
    font.drawString("Attention", 10, ofGetHeight()/11 - 10);
    ofNoFill();
    ofCircle(ofGetWidth()/2, ofGetHeight()/2, currAw);
    //	ofRect(1, ofGetHeight()/11, ofGetWidth() - 2, 60);
    ofSetColor(ofColor::yellow, ofMap(currAw, 0.0, ofGetWidth(), 50, 255));
    ofFill();
	ofCircle(ofGetWidth()/2, ofGetHeight()/2, currAw);
    //  ofRect(0, ofGetHeight()/11, currAw, 59);
    if (attention > 0.0)
    {
        ofSetColor(ofColor::black, ofMap(currAw, 0.0, ofGetWidth(), 50, 255));
        font.drawString(ofToString(attention), 10, ofGetHeight()/11 + 40);
    }
    ofPopStyle();
    ofPopMatrix();
    
	//key circle
	ofSetColor(ofColor::black);
    //	ofTranslate(0, 30);
	ofCircle(ofGetWidth()/2, (ofGetHeight()/2)+30, 2);
	
	//meditation bar
    ofPushMatrix();
    ofTranslate(0, 30);
    ofPushStyle();
    ofSetColor(ofColor::black);
    font.drawString("Meditation", 10, (ofGetHeight()/3.5) - 10);
    ofNoFill();
	ofCircle(ofGetWidth()/2, ofGetHeight()/2, currMw);
    //  ofRect(1, (ofGetHeight()/3.5), ofGetWidth() - 2, 60);
    ofSetColor(ofColor::cyan, ofMap(currMw, 0.0, ofGetWidth(), 50, 255));
    ofFill();
	ofCircle(ofGetWidth()/2, ofGetHeight()/2, currMw);
    //	ofRect(0, (ofGetHeight()/3.5), currMw, 59);
    if (meditation > 0.0)
    {
        ofSetColor(ofColor::black, ofMap(currMw, 0.0, ofGetWidth(), 50, 255));
        font.drawString(ofToString(meditation), 10, (ofGetHeight()/3.5) + 40);
    }
    ofPopStyle();
    ofPopMatrix();
    
    ofSetWindowTitle("fps = " + ofToString(ofGetFrameRate()));
	
//osc sender begin
	// display instructions
	string buf;
	buf = "sending osc messages to" + string(HOST) + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);
	ofDrawBitmapString("move the mouse to send osc message [/mouse/position <x> <y>]", 10, 50);
	ofDrawBitmapString("click to send osc message [/mouse/button <button> <\"up\"|\"down\">]", 10, 65);
	ofDrawBitmapString("press A to send osc message [/test 1 3.5 hello <time>]", 10, 80);

//	scaledMeditation = ofMap(meditation, 0, 100, 0, 12); //for light intensity, uncomment in .h as well to use
	scaledMeditation = ofMap(meditation, 0, 100, 0, 179); //for spotlight, uncomment in .h as well to use
    scaledAttention = ofMap(attention, 0, 100, 0, 179); // for spotlight
	scaledFocus = ofMap(attention, 0, 100, 7, 0);
	scaledAttention2 = ofMap(attention, 0, 100, 8, 0); // for demonHeart
	volumeScaled = ofMap(attention, 0, 100, 0, 10); // for volume
	
    ofxOscMessage m;
    
    m.setAddress("/neurosky");
    m.addIntArg(meditation);
    m.addIntArg(attention);
    m.addIntArg(blink);
	m.addIntArg(scaledFocus);
	m.addIntArg(scaledAttention2);
	m.addIntArg(volumeScaled);
    

    cout << "scaledMeditation: " << m.getArgAsInt32(0) <<endl;
    cout << "scaledAttention: " << m.getArgAsInt32(1) <<endl;
	
//    cout << "blink: " << m.getArgAsInt32(2) <<endl;
	cout << "scaledFocus: " << m.getArgAsInt32(3) <<endl;
	cout << "scaledAttention2: " << m.getArgAsInt32(4) <<endl;
	cout << "volumeScaled: " << m.getArgAsInt32(5) <<endl;
	

    sender.sendMessage(m);    
    
	//osc sender end
	
	//serial start
	mySerial.writeBytes(bytesToSend, NUM_MSG_BYTES);
	if (attention > 0 && attention < 100) {	
		bytesToSend[0] = ofMap(attention, 0, 60, 0, 254, true); // reverses the power
	}
	cout << "attention send:" + ofToString((int)bytesToSend[0]);
	
	bytesToSend[1] = 9;
	//serial end
	
	//start serial
	if(!isInitialized)
        ofDrawBitmapString("CONCENTRATE TO SEND KICKOFF BYTE", 50, (ofGetHeight()/2)+200);
    else {
        //draw out current bytesToSend values:
        for(int i=0; i<NUM_MSG_BYTES; i++){
            string whichByte = "bytesToSend["+ofToString(i)+"] = ";
            ofDrawBitmapString(whichByte + ofToString((int)bytesToSend[i]), 50, (ofGetHeight()/2)+30+30*i);
            if     (i == 0) ofDrawBitmapString("(attention)", 225, (ofGetHeight()/2)+30+30*i);
            else if(i == 1) ofDrawBitmapString("(Motor pin)", 225, (ofGetHeight()/2)+30+30*i);
            else if(i == 2) ofDrawBitmapString("(blink speed)", 225, (ofGetHeight()/2)+30+30*i);
			//else if(i == 3) ofDrawBitmapString("(Servo pin)", 225, 30+30*i);
        }
        
        //just for debug
        //ofDrawBitmapString("num messages sent total: "+ ofToString(numMsgSent), 50, ofGetHeight()/2);
		// ofDrawBitmapString("press 'a','b','c','A','B','C', LEFT, or RIGHT arrows to update values", 50, (ofGetHeight()/2)+50);
    }
	
}

//--------------------------------------------------------------

void testApp::attentionListener(float &param)
{
    attention = param;
    distAw = ofMap(attention, 0.0, 100.0, 0, ofGetWidth());
    atChangeTime = ofGetElapsedTimef();
}

//--------------------------------------------------------------

void testApp::meditationListener(float &param)
{
    meditation = param;
    distMw = ofMap(meditation, 0.0, 100.0, 0, ofGetWidth());
    meChangeTime = ofGetElapsedTimef();
}

//--------------------------------------------------------------

void testApp::blinkListener(float &param)
{
    //    meditation = param;
    //    distMw = ofMap(meditation, 0.0, 100.0, 0, ofGetWidth());
    //    meChangeTime = ofGetElapsedTimef();
//	cout << "blink: "<<param<<endl;
	blink = param;
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
//	if(key == 'a' || key == 'A'){
//		ofxOscMessage m;
//		m.setAddress("/test");
//		m.addIntArg(1);
//		m.addFloatArg(3.5f);
//		m.addStringArg("hello");
//		m.addFloatArg(ofGetElapsedTimef());
//		sender.sendMessage(m);
//	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
//	ofxOscMessage m;
//	m.setAddress("/mouse/position");
//	m.addIntArg(x);
//	m.addIntArg(y);
//	sender.sendMessage(m);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
//	ofxOscMessage m;
//	m.setAddress("/mouse/button");
//	m.addStringArg("down");
//	sender.sendMessage(m);
	//serial start
	bSendSerialMessage = true;
	//serial end
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
//	ofxOscMessage m;
//	m.setAddress("/mouse/button");
//	m.addStringArg("up");
//	sender.sendMessage(m);
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
	
}