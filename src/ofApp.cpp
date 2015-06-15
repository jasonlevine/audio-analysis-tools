#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    aa.setup();
    aa.playStems(0);
}

//--------------------------------------------------------------
void ofApp::update(){
    aa.updateAnalytics();
}

//--------------------------------------------------------------
void ofApp::draw(){
    aa.drawAnalytics();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    aa.keyPressed(key);
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
