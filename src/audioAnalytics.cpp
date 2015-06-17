//
//  audioAnalytics.cpp
//  just_you
//
//  Created by Jason Levine on 2013-11-06.
//
//

#include "audioAnalytics.h"
//#include "testApp.h"


void audioAnalytics::setup() {
    loadTracks();
    setupVectors();
    setupAUGraph();
    
    faucet.loadFont("faucet.ttf", 18);
    faucetBold.loadFont("faucet.ttf", 20);
    
    bFindMinMax = true;
}


/////////////////////////////////SETUP//////////////////////////////////

//--------------------------------------------------------------
void audioAnalytics::loadTracks(){
    //load stems dir
    ofDirectory stemsDir;
    stemsDir.open("stems");
    stemsDir.listDir();
    numTracks = stemsDir.size();


    
    for ( int i = 0; i < numTracks; i++ ) {
        stemNames.push_back(stemsDir.getName(i));
        
        ofxAudioUnitFilePlayer * stem = new ofxAudioUnitFilePlayer();
        stem->setFile(ofToDataPath(stemsDir.getPath(i)));
        stems.push_back(stem);

        ofxAudioUnitTap * tap = new ofxAudioUnitTap();
        taps.push_back(tap);
        
//        ofxAudioUnitFftNode * fft = new ofxAudioUnitFftNode();
//        ffts.push_back(fft);
        
        ofxAudioFeaturesChannel * channel = new ofxAudioFeaturesChannel();
        channel->setup(512, 64, 44100);
        channel->usingPitch = true;
        channel->usingOnsets = true;
        audioFeatures.push_back(channel);
        
        ofPolyline temp;
        waves.push_back(temp);
    }
}


//--------------------------------------------------------------
void audioAnalytics::setupVectors(){
    fftPeak.assign(numTracks, 0);
    amp.assign(numTracks, 0.0);
    dB.assign(numTracks, 0.0);
    pitch.assign(numTracks, 0.0);
    centroid.assign(numTracks, 0.0);
    spread.assign(numTracks, 0.0);
    skew.assign(numTracks, 0.0);
    kurtosis.assign(numTracks, 0.0);
    
    //stemNames.assign(numTracks, "");
    
    vector<float> fftValues;
    fftValues.assign(512, 0);
    
    for ( int i = 0; i < numTracks; i++ ) {
        samples.push_back(fftValues);
    }
    
    dBSmoothed.setNumPValues(35);
    ampSmoothed.setNumPValues(35);
    pitchSmoothed.setNumPValues(45);
    centroidSmoothed.setNumPValues(70);
    spreadSmoothed.setNumPValues(35);
    skewSmoothed.setNumPValues(45);
    kurtosisSmoothed.setNumPValues(45);
    
    //analytics
    maxdB.assign(numTracks, -120);
    maxAmp.assign(numTracks, 0);
    maxfft.assign(numTracks, 0);
    maxFftPeak.assign(numTracks, 0);
    maxPitch.assign(numTracks, 0);
    maxCentroid.assign(numTracks, 75);
    maxSpread.assign(numTracks, 7000);
    maxSkew.assign(numTracks, 3);
    maxKurtosis.assign(numTracks, 15);
    
    loadMinMax();
    
   
    
    scrollingGraph pitchGraph;
    pitchGraph.setup(ofGetWidth() - 200, 0, 0, maxPitch[0]);
    
    scrollingGraph centroidGraph;
    centroidGraph.setup(ofGetWidth() - 200, 0, 0, maxCentroid[0]);
    
    scrollingGraph spreadGraph;
    spreadGraph.setup(ofGetWidth() - 200, 0, 0, maxSpread[0]);
    
    scrollingGraph skewGraph;
    skewGraph.setup(ofGetWidth() - 200, 0, 0, maxSkew[0]);
    
    scrollingGraph kurtosisGraph;
    kurtosisGraph.setup(ofGetWidth() - 200, 0, 0, maxKurtosis[0]);
    
    for (int i = 0; i < numTracks; i++) {
        scrollingGraph dBGraph;
        dBGraph.setup(ofGetWidth() - 200, -120, -120, maxdB[i]);
        
        scrollingGraph ampGraph;
        ampGraph.setup(ofGetWidth() - 200, 0, 0, maxAmp[i]);
        
        scrollingGraph fftPeakGraph;
        fftPeakGraph.setup(ofGetWidth() - 200, 0, 0, maxFftPeak[i]);
        
        
        dBHistory.push_back(dBGraph);
        ampHistory.push_back(ampGraph);
        fftPeakHistory.push_back(fftPeakGraph);
        
        
        
        pitchHistory.push_back(pitchGraph);
        centroidHistory.push_back(centroidGraph);
        spreadHistory.push_back(spreadGraph);
        skewHistory.push_back(skewGraph);
        kurtosisHistory.push_back(kurtosisGraph);
        
        dBMeanHistory.push_back(dBGraph);
        ampMeanHistory.push_back(ampGraph);
        pitchMeanHistory.push_back(pitchGraph);
        centroidMeanHistory.push_back(centroidGraph);
        spreadMeanHistory.push_back(spreadGraph);
        skewMeanHistory.push_back(skewGraph);
        kurtosisMeanHistory.push_back(kurtosisGraph);
        
        dBMedianHistory.push_back(dBGraph);
        ampMedianHistory.push_back(ampGraph);
        pitchMedianHistory.push_back(pitchGraph);
        centroidMedianHistory.push_back(centroidGraph);
        spreadMedianHistory.push_back(spreadGraph);
        skewMedianHistory.push_back(skewGraph);
        kurtosisMedianHistory.push_back(kurtosisGraph);
    }
    
    mode = 1;
    selectedTrack = -1;
}


//--------------------------------------------------------------
void audioAnalytics::setupAUGraph(){
    mixer.setInputBusCount(numTracks);
    
    for ( int i = 0; i < numTracks; i++ ) {
        stems[i]->connectTo(*taps[i]);
        (*taps[i]).connectTo(mixer, i);
        mixer.setInputVolume(1.0, i);
        mixer.enableInputMetering(i);
    }
    
    mixer.connectTo(output);
    output.start();
}

//--------------------------------------------------------------
void audioAnalytics::playStems(float timeInSeconds){

    int sampleRate = 44100;
    int timeInSamples = timeInSeconds * sampleRate;
    
    for ( int i = 0; i < numTracks; i++ ) {
        stems[i]->prime();
    }
    
    for ( int i = 0; i < numTracks; i++ ) {
        stems[i]->playAtSampleTime(timeInSamples);
    }
}

void audioAnalytics::stopStems(){
    for ( int i = 0; i < numTracks; i++ ) {
        stems[i]->stop();
    }
}

///////////////////////////UPDATE//////////////////////////
//--------------------------------------------------------------
void audioAnalytics::updateAnalytics(){
    for ( int i = 0; i < numTracks; i++ ) {

        float waveformSize = (selectedTrack == -1) ? ofGetHeight()/numTracks : ofGetHeight();
        
        taps[i]->getLeftWaveform(waves[i], ofGetWidth(), waveformSize);///numTracks
        taps[i]->getSamples(samples[i]);
        audioFeatures[i]->inputBuffer = samples[i];
        audioFeatures[i]->process(0);
        
        if (getAmpNormalized(i) > 0.2){
            float maxFft = 0.0;
            for (unsigned int j = 0; j < audioFeatures[i]->spectrum.size(); j++){
                if (audioFeatures[i]->spectrum[j] > maxFft){
                    maxFft = audioFeatures[i]->spectrum[j];
                    fftPeak[i] = j;
                }
            }
        }
        
        dB[i] = mixer.getInputLevel(i);
        amp[i] = taps[i]->getRMS(0);//ofMap(taps[i]->getRMS(0), 0, maxAmp[i], 0.0, 1.0);
        pitch[i] = audioFeatures[i]->pitch;
        centroid[i] = audioFeatures[i]->spectralFeatures["centroid"];
        spread[i] = audioFeatures[i]->spectralFeatures["spread"];
        skew[i] = audioFeatures[i]->spectralFeatures["skewness"];
        kurtosis[i] = audioFeatures[i]->spectralFeatures["kurtosis"];
        
        dBHistory[i].addValue(dB[i]);
        ampHistory[i].addValue(amp[i]);
        fftPeakHistory[i].addValue(fftPeak[i]);
        
        
        pitchHistory[i].addValue(pitch[i]);
        centroidHistory[i].addValue(centroid[i]);
        spreadHistory[i].addValue(spread[i]);
        skewHistory[i].addValue(skew[i]);
        kurtosisHistory[i].addValue(kurtosis[i]);
        
        dBSmoothed.addValue(dB[i]);
        ampSmoothed.addValue(amp[i]);
        pitchSmoothed.addValue(pitch[i]);
        centroidSmoothed.addValue(centroid[i]);
        spreadSmoothed.addValue(spread[i]);
        skewSmoothed.addValue(skew[i]);
        kurtosisSmoothed.addValue(kurtosis[i]);

        dBMeanHistory[i].addValue(dBSmoothed.getMean());
        ampMeanHistory[i].addValue(ampSmoothed.getMean());
        pitchMeanHistory[i].addValue(pitchSmoothed.getMean());
        centroidMeanHistory[i].addValue(centroidSmoothed.getMean());
        spreadMeanHistory[i].addValue(spreadSmoothed.getMean());
        skewMeanHistory[i].addValue(skewSmoothed.getMean());
        kurtosisMeanHistory[i].addValue(kurtosisSmoothed.getMean());
        
        dBMedianHistory[i].addValue(dBSmoothed.getMedian());
        ampMedianHistory[i].addValue(ampSmoothed.getMedian());
        pitchMedianHistory[i].addValue(pitchSmoothed.getMedian());
        centroidMedianHistory[i].addValue(centroidSmoothed.getMedian());
        spreadMedianHistory[i].addValue(spreadSmoothed.getMedian());
        skewMedianHistory[i].addValue(skewSmoothed.getMedian());
        kurtosisMedianHistory[i].addValue(kurtosisSmoothed.getMedian());
        
        if (bFindMinMax) findMinMax(i);
    }
    

}


//--------------------------------------------------------------
void audioAnalytics::findMinMax(int track){
    
    if (amp[track] > maxAmp[track]) maxAmp[track] = amp[track];
    if (dB[track] > maxdB[track]) maxdB[track] = dB[track];
    if (fftPeak[track] > maxFftPeak[track]) maxFftPeak[track] = fftPeak[track];
    
    for (int j = 0; j < audioFeatures[track]->spectrum.size(); j++){
        float bin = audioFeatures[track]->spectrum[j];
        if (bin > maxfft[track]) maxfft[track] = bin;
    }
    
    if (pitch[track] > maxPitch[track]) maxPitch[track] = pitch[track];
}

//--------------------------------------------------------------
void audioAnalytics::saveMinMax(){
    xml.addChild("limits");
    
    for (int i = 0; i < numTracks; i++) {
        string tag = "track-" + ofToString(i);
        xml.addChild(tag);
        xml.setTo(tag);
        
        xml.addValue("maxAmp", maxAmp[i]);
        xml.addValue("maxdB", maxdB[i]);
        xml.addValue("maxfft", maxfft[i]);
        xml.addValue("maxFftPeak", maxFftPeak[i]);
        xml.addValue("maxPitch", maxPitch[i]);
        
        xml.setToParent();
    }
    
    xml.save("dataMinMax.xml");
}

//--------------------------------------------------------------
void audioAnalytics::loadMinMax(){
    xml.load("dataMinMax.xml");
    xml.setTo("limits");
    
    for (int i = 0; i < numTracks; i++) {
        string tag = "track-" + ofToString(i);
        xml.setTo(tag);
        
        maxAmp[i] = ofToFloat(xml.getValue("maxAmp"));
        maxdB[i] = ofToFloat(xml.getValue("maxdB"));
        maxfft[i] = ofToFloat(xml.getValue("maxfft"));
        maxFftPeak[i] = ofToFloat(xml.getValue("maxFftPeak"));
        maxPitch[i] = ofToFloat(xml.getValue("maxPitch"));
        
        xml.setToParent();
    }
}

/*
//--------------------------------------------------------------
void audioAnalytics::recordPitchData(int track, bool save){
    if (save) xml.save("voice data.xml");
    else {
        if (amp[track] != 0) {
            ofXml frame;
            string child = "frame-" + ofToString(ofGetFrameNum);
            frame.addChild(child);
            frame.setTo(child);
            
            frame.addValue("pitch", pitchHistory[track][pitchHistory.size()-1]);
            frame.addValue("amplitude", amp[track]);
            
            xml.addXml(frame);
        }
    }
}
*/



///////////////////////////DRAW/////////////////////////////
//--------------------------------------------------------------
void audioAnalytics::drawAnalytics(){
    
    ofBackgroundGradient(50, 100);
    
    ofSetColor(255);
    faucet.drawString("all tracks", 30, 20);
    
    
    for ( int i = 0; i < numTracks; i++ ) {
        ofSetColor(i * 30, 255 - i * 30, (i * 100) % 255);
        ofPushMatrix();
        ofTranslate( 0, ofGetHeight()/(numTracks) * i);
        if (selectedTrack == i) faucetBold.drawString(stemNames[i], 30, ofGetHeight()/numTracks);
        else faucet.drawString(stemNames[i], 30, ofGetHeight()/numTracks);
        if (selectedTrack == -1) selectMode(i, ofGetHeight()/numTracks);
        ofPopMatrix();
    }
    
    if (selectedTrack != -1) {
        int i = selectedTrack;
        ofSetColor(i * 30, 255 - i * 30, (i * 100) % 255);
        selectMode(i, ofGetHeight());
    }
}

//--------------------------------------------------------------
void audioAnalytics::selectMode(int track, float height){
    switch (mode) {
        case 1:
            drawWaveForms(track, height);
            break;
            
        case 2:
//            ofSetColor(100,255,100);
            ampHistory[track].draw(height);
//            ofSetColor(255,100,255);
//            ampMeanHistory[track].draw(height);
//            ofSetColor(255,255,0);
//            ampMedianHistory[track].draw(height);
            break;
            
        case 3:
//            ofSetColor(100,255,100);
            dBHistory[track].draw(height);
//            ofSetColor(255,100,255);
//            dBMeanHistory[track].draw(height);
//            ofSetColor(255,255,0);
//            dBMedianHistory[track].draw(height);
            break;
            
        case 4:
            drawFFT(track, height);
            break;
            
        case 5:
//            ofSetColor(100,255,100);
            fftPeakHistory[track].draw(height);
//            ofSetColor(255,100,255);
//            pitchMeanHistory[track].draw(height);
//
//            ofSetColor(255,255,0);
//            pitchMedianHistory[track].draw(height);
            break;
            
        case 6:
            ofSetColor(100,255,100);
            centroidHistory[track].draw(height);
            ofSetColor(255,100,255);
            centroidMeanHistory[track].draw(height);
            ofSetColor(255,255,0);
            centroidMedianHistory[track].draw(height);
            break;
            
        case 7:
            ofSetColor(100,255,100);
            spreadHistory[track].draw(height);
            ofSetColor(255,100,255);
            spreadMeanHistory[track].draw(height);
            ofSetColor(255,255,0);
            spreadMedianHistory[track].draw(height);
            break;
            
        case 8:
            ofSetColor(100,255,100);
            skewHistory[track].draw(height);
            ofSetColor(255,100,255);
            skewMeanHistory[track].draw(height);
            ofSetColor(255,255,0);
            skewMedianHistory[track].draw(height);
            break;
            
        case 9:
            ofSetColor(100,255,100);
            kurtosisHistory[track].draw(height);
            ofSetColor(255,100,255);
            kurtosisMeanHistory[track].draw(height);
            ofSetColor(255,255,0);
            kurtosisMedianHistory[track].draw(height);
            break;
            
    }
}

//--------------------------------------------------------------
void audioAnalytics::drawWaveForms(int track, float height){
    //ofSetLineWidth(10);
    waves[track].draw();
}


//--------------------------------------------------------------
void audioAnalytics::drawFFT(int track, float height){
    ofBeginShape();
    for (unsigned int j = 0; j < audioFeatures[track]->spectrum.size(); j++){
        int x = (float)j / audioFeatures[track]->spectrum.size() * (ofGetWidth()-100) + 100;
        
        if( j == 0 ) ofVertex(x, height);
        
        float binNormalized = ofMap(audioFeatures[track]->spectrum[j], 0, maxfft[track], 0.0, 1.0);
        ofVertex(x, height - binNormalized * height);
        
        if( j == audioFeatures[track]->spectrum.size() -1 ) ofVertex(x, height);
    }
    ofEndShape(false);
    
    ofSetColor(255);
    int x = (float)fftPeak[track] / audioFeatures[track]->spectrum.size() * (ofGetWidth()-100) + 100;
    float binNormalized = ofMap(audioFeatures[track]->spectrum[fftPeak[track]], 0, maxfft[track], 0.0, 1.0);
    ofLine(x, height, x, height - binNormalized * height);
    
}

////////////////////////////EVENTS///////////////////////////////

//--------------------------------------------------------------
void audioAnalytics::mousePressed(int x, int y){
    selectTrack(x, y);
}

//--------------------------------------------------------------
void audioAnalytics::keyPressed(int key){
    if (key > 48 && key < 58) {
        setMode(key - 48);
    }
    
    if (key == 's') {
        saveMinMax();
        bFindMinMax = false;
    }
}


//--------------------------------------------------------------
void audioAnalytics::setMode(int chosenMode){
    mode = chosenMode;
}

//--------------------------------------------------------------
void audioAnalytics::selectTrack(int x, int y){
    if (x < 100) {
        float height = ofGetHeight() / numTracks;
        if ( y < 20 ) selectedTrack = -1;
        if ( y > 20 && y < height) selectedTrack = 0;
        
        if (y > height) {
            for (int i = 1; i < numTracks; i++) {
                if ( y > height * i && y < height * (i+1)) {
                    selectedTrack = i;
                    break;
                }
            }
        }
    }
}


