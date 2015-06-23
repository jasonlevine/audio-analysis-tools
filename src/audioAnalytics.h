//
//  audioAnalytics.h
//  just_you
//
//  Created by Jason Levine on 2013-11-06.
//
//
#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"
#include "ofxAudioFeaturesChannel.h"
#include "scrollingGraph.h"
#include "smoother.h"

#define FINDMINMAX

class audioAnalytics {
public:
    void setup();
    
    void setupVectors();
    void loadTracks();
    void setupAUGraph();
    void playStems(float timeInSeconds);
    void stopStems();
    
    void updateAnalytics();
    void findMinMax(int track);
    void saveMinMax();
    void loadMinMax();
    
    void setMode(int chosenMode);
    void selectTrack(int x, int y);
    void drawAnalytics();
    void selectMode(int track, float height);
    void drawWaveForms(int track, float height);
    void drawAmpHistory(int track, float height);
    void drawDBHistory(int track, float height);
    void drawFFT(int track, float height);
    void drawFFTPeakHistory(int track, float height);
    
    float getAmpNormalized(int track) { return ofMap(amp[track], 0, maxAmp[selectedTrack], 0.0, 1.0); }
    float getAmpNormalized() { return ofMap(amp[selectedTrack], 0, maxAmp[selectedTrack], 0.0, 1.0); }
    float getFFTPeakNormalized() { return ofMap(fftPeak[selectedTrack], 0, maxFftPeak[selectedTrack], 0.0, 1.0); }
    
    vector<float>& getSamples() { return samples[selectedTrack]; }
    
    void mousePressed(int x, int y);
    void keyPressed(int key);
    
    vector<ofxAudioUnitFilePlayer*> stems;
    vector<string> stemNames;
    vector<ofxAudioUnitTap*> taps;
    vector<int> fftPeak;
    vector<ofPolyline> waves;
    vector<vector<float> > samples;
    ofxAudioUnitMixer mixer;
    ofxAudioUnitOutput output;

    vector<ofxAudioFeaturesChannel*> audioFeatures;

    vector<float> dB;
    vector<float> amp;

    vector<scrollingGraph> ampHistory;
    vector<scrollingGraph> dBHistory;
    vector<scrollingGraph> fftPeakHistory;
    
//    smoother dBSmoothed;
//    smoother ampSmoothed;

    
//    vector<scrollingGraph> ampMeanHistory;
//    vector<scrollingGraph> dBMeanHistory;
//
//    
//    vector<scrollingGraph> ampMedianHistory;
//    vector<scrollingGraph> dBMedianHistory;

    
    int numTracks;

    int mode;
    int selectedTrack;
    
    bool bFindMinMax;

    //analytics
    vector <float> maxdB, maxAmp, maxfft, maxFftPeak;
    //xml
    ofXml xml;
    
    ofTrueTypeFont faucet;
    ofTrueTypeFont faucetBold;
};
