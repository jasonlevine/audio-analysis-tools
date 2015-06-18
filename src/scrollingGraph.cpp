//
//  scrollingGraph.cpp
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-01-06.
//
//

#include "scrollingGraph.h"

void scrollingGraph::setup(int _size, float init, float min, float max) {
    size = _size;
    valHistory.assign(size, init);
    minVal = min;
    maxVal = max;
}

void scrollingGraph::addValue(float val) {
    valHistory.push_back(val);
    if (valHistory.size() > size) valHistory.erase(valHistory.begin());
}

void scrollingGraph::draw(float height) {
    ofNoFill();
    ofBeginShape();
    for (unsigned int j = 0; j < valHistory.size(); j++){
        float valNormalized = ofMap(valHistory[j], minVal, maxVal, 0.0, 1.0);
        ofVertex(j + 100, height - valNormalized * height);
    }
    ofEndShape(false);
    ofFill();
}
