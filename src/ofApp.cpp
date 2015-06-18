#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    soundStream.listDevices();
    
    int bufferSize = 1024;
    vector<float> emptyBuffer;
    emptyBuffer.assign(bufferSize, 0.0);
    
    for (int i = 0; i < 8; i++){
        channel.push_back(emptyBuffer);
    }
    
    smoothedVol.assign(8, 0.0);
    
    soundStream.setup(this, 0, 8, 44100, bufferSize, 4);
    aa.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    aa.updateAnalytics(channel, smoothedVol);
 
}

//--------------------------------------------------------------
void ofApp::draw(){
    aa.drawAnalytics();
//    int track = 0;
//    
//    ofNoFill();
//    ofBeginShape();
//    for (int i = 0; i < channel[track].size(); i++){
//        int x = float(i) / channel[track].size() * ofGetWidth();
//        int y = channel[track][i] * 400 + 400 * 0.5;
//        
//        ofVertex(x, y);
//    }
//    ofEndShape();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    aa.keyPressed(key);
}


//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    vector<float> curVol;
    curVol.assign(8, 0.0);
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < bufferSize; j++){
            channel[i][j] = input[j*8+i]*0.5;
            curVol[i] += channel[i][j] * channel[i][j];
        }
        
        curVol[i] /= (float)bufferSize;
        curVol[i] = sqrt( curVol[i] );
        
        smoothedVol[i] *= 0.5;
        smoothedVol[i] += 0.5 * curVol[i];

    }
    
    //this is how we get the mean of rms :)
    
    
    // this is how we get the root of rms :)
    
    
//
//    bufferCounter++;
    
}



//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    aa.mousePressed(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
