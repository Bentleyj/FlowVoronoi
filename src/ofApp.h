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
    ofShader* fade;
    vector<ofVec2f> seedLocs;
    vector<ofVec3f> seedCols;
    vector<ofVec2f> seedVels;
    vector<ofVec2f> initVels;
    vector<string> ImageNames;
    ofFbo blurOnePass, blurTwoPass, fadePass;
    ofImage displayImgs[2], flowImg;
    float fadeAmnt;
    bool animating;
    bool cheating;
    ofVideoGrabber cam;
    
    int currentTime;
    
    int currImg, imageIterator;
    
    ofxCv::FlowFarneback* flow;
};
