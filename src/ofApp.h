#pragma once

#include "ofMain.h"
#include "ofxCv.h"

#define NUM_SEEDS 200

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofShader* voronoi;
    ofShader* blurVert;
    ofShader* blurHor;
    vector<ofVec2f> seedLocs;
    vector<ofVec3f> seedCols;
    vector<ofVec2f> seedVels;
    vector<ofVec2f> initVels;
    ofFbo blurOnePass, blurTwoPass;
    ofImage sunflower;
    bool animating;
    ofVideoGrabber cam;
    
    int noiseX = 1000;
    int noiseY = 10000;
    
    ofxCv::FlowFarneback* flow;
};
