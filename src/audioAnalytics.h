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
//    void recordPitchData(int track, bool save);
    
    void setMode(int chosenMode);
    void selectTrack(int x, int y);
    void drawAnalytics();
    void selectMode(int track, float height);
    void drawWaveForms(int track, float height);
    void drawAmpHistory(int track, float height);
    void drawDBHistory(int track, float height);
    void drawFFT(int track, float height);
    void drawPitchHistory(int track, float height);
    void drawCentroidHistory(int track, float height);
    void drawSpreadHistory(int track, float height);
    void drawSkewHistory(int track, float height);
    void drawKurtosisHistory(int track, float height);
    
    float getAmpNormalized(int track) { return ofMap(amp[track], 0, maxAmp[track], 0.0, 1.0); }
    
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
    vector<float> pitch;
    vector<float> centroid;
    vector<float> spread;
    vector<float> skew;
    vector<float> kurtosis;
    
    vector<scrollingGraph> ampHistory;
    vector<scrollingGraph> dBHistory;
    vector<scrollingGraph> pitchHistory;
    vector<scrollingGraph> centroidHistory;
    vector<scrollingGraph> spreadHistory;
    vector<scrollingGraph> skewHistory;
    vector<scrollingGraph> kurtosisHistory;
    
    smoother dBSmoothed;
    smoother ampSmoothed;
    smoother pitchSmoothed;
    smoother centroidSmoothed;
    smoother spreadSmoothed;
    smoother skewSmoothed;
    smoother kurtosisSmoothed;
    
    vector<scrollingGraph> ampMeanHistory;
    vector<scrollingGraph> dBMeanHistory;
    vector<scrollingGraph> pitchMeanHistory;
    vector<scrollingGraph> centroidMeanHistory;
    vector<scrollingGraph> spreadMeanHistory;
    vector<scrollingGraph> skewMeanHistory;
    vector<scrollingGraph> kurtosisMeanHistory;
    
    vector<scrollingGraph> ampMedianHistory;
    vector<scrollingGraph> dBMedianHistory;
    vector<scrollingGraph> pitchMedianHistory;
    vector<scrollingGraph> centroidMedianHistory;
    vector<scrollingGraph> spreadMedianHistory;
    vector<scrollingGraph> skewMedianHistory;
    vector<scrollingGraph> kurtosisMedianHistory;
    
    int numTracks;

    int mode;
    int selectedTrack;
    
    bool bFindMinMax;

    //analytics
    vector <float> maxdB, maxAmp, maxfft, maxPitch, maxCentroid, maxSpread, maxSkew, maxKurtosis;
    float maxdBGlobal, maxAmpGlobal, maxfftGlobal, maxPitchGlobal;
    //xml
    ofXml xml;
    
    ofTrueTypeFont faucet;
    ofTrueTypeFont faucetBold;
};
