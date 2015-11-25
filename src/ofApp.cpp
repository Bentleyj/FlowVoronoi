#include "ofApp.h"

#define WIDTH ofGetWidth()
#define HEIGHT ofGetHeight()
#define FLOW_WIDTH 320
#define FLOW_HEIGHT 240
#define DAMPING 0.8

//--------------------------------------------------------------
void ofApp::setup(){
    voronoi = new ofShader();
    blurVert = new ofShader();
    blurHor = new ofShader();
    
    voronoi->load("Shaders/DummyVert.glsl", "Shaders/VoronoiFrag.glsl");
    blurVert->load("Shaders/DummyVert.glsl", "Shaders/GaussianFragVert.glsl");
    blurHor->load("Shaders/DummyVert.glsl", "Shaders/GaussianFragHor.glsl");
    seedLocs.resize(NUM_SEEDS);
    seedCols.resize(NUM_SEEDS);
    seedVels.resize(NUM_SEEDS);
    initVels.resize(NUM_SEEDS);
    for(int i=0; i < NUM_SEEDS; i++) {
        ofVec2f loc = ofVec2f(ofRandom(WIDTH), ofRandom(HEIGHT));
        ofVec3f col = ofVec3f(ofRandom(255), ofRandom(255), ofRandom(255));
        ofVec2f vel = ofVec2f(ofRandom(-0.5, 0.5), ofRandom(-0.5, 0.5));
        seedLocs[i] = loc;
        seedCols[i] = col;
        seedVels[i] = vel;
        initVels[i] = vel;
    }
    
    blurOnePass.allocate(WIDTH, HEIGHT);
    blurTwoPass.allocate(WIDTH, HEIGHT);

    animating  = true;
    
    //sunflower.loadImage("Images/monaLisa.jpeg");
    cam.initGrabber(FLOW_WIDTH, FLOW_HEIGHT);
    
    flow = new ofxCv::FlowFarneback();
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if(cam.isFrameNew()) {
        
        sunflower.setFromPixels(cam.getPixelsRef());
        sunflower.mirror(false, true);
        
        flow->setPyramidScale( 0.5 );
        flow->setNumLevels( 4 );
        flow->setWindowSize( 8 );
        flow->setNumIterations( 2 );
        flow->setPolyN( 7 );
        flow->setPolySigma( 1.5 );
        flow->setUseGaussian(false);
        
        flow->calcOpticalFlow(sunflower);
    }
    //sunflower.setFromPixels(cam.getPixelsR(), WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
    //sunflower.mirror(false, true);
    if(animating) {
        for(int i=0; i < seedLocs.size(); i++) {
            seedLocs[i] += seedVels[i];
            float projX = ofMap(seedLocs[i].x, 0, WIDTH, 1, FLOW_WIDTH-1, true);
            float projY = ofMap(seedLocs[i].y, 0, HEIGHT, 1, FLOW_HEIGHT-1, true);

            seedVels[i] += flow->getAverageFlowInRegion(ofRectangle(projX - 1, projY - 1, 2, 2));
            seedVels[i] *= DAMPING;
            seedVels[i] = (seedVels[i].length() < 1) ? initVels[i] : seedVels[i];
            if(seedLocs[i].x < 0 || seedLocs[i].x > WIDTH) {
                seedVels[i].x *= -1;
                initVels[i].x *= -1;
                seedLocs[i].x = (seedLocs[i].x < 0) ? 0 : WIDTH;
            }
            if(seedLocs[i].y < 0 || seedLocs[i].y > HEIGHT) {
                seedVels[i].y *= -1;
                initVels[i].y *= -1;
                seedLocs[i].y = (seedLocs[i].y < 0) ? 0 : HEIGHT;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    blurOnePass.begin();
        ofClear(0);
        blurVert->begin();
            blurVert->setUniform1f("blurAmnt", 10.0);
            sunflower.draw(0, 0, WIDTH, HEIGHT);
        blurVert->end();
    blurOnePass.end();
    
    blurTwoPass.begin();
        ofClear(0);
        blurHor->begin();
            blurHor->setUniform1f("blurAmnt", 10.0);
            blurOnePass.draw(0, 0, WIDTH, HEIGHT);
        blurHor->end();
    blurTwoPass.end();
    
    voronoi->begin();
        voronoi->setUniform2fv("locs", (float *)&seedLocs[0], seedLocs.size());
        blurTwoPass.draw(0, 0);
    voronoi->end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' ') {
        animating = !animating;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    seedLocs.push_back(ofVec2f(mouseX, mouseY));
    seedVels.push_back(ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1)));
    seedCols.push_back(ofVec3f(ofRandom(1.0), ofRandom(1.0), ofRandom(1.0)));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    seedLocs.push_back(ofVec2f(mouseX, mouseY));
    seedVels.push_back(ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1)));
    seedCols.push_back(ofVec3f(ofRandom(1.0), ofRandom(1.0), ofRandom(1.0)));
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
