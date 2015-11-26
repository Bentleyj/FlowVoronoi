#include "ofApp.h"

#define WIDTH ofGetWidth()
#define HEIGHT ofGetHeight()
#define FLOW_WIDTH 320
#define FLOW_HEIGHT 240
#define DAMPING 0.8
#define MAXVEL 50.0

//--------------------------------------------------------------
void ofApp::setup(){
    voronoi = new ofShader();
    blurVert = new ofShader();
    blurHor = new ofShader();
    fade = new ofShader();
    
    fadeAmnt = 0.0;
    
    cheating = false;
    
    currImg = 0;
    
    imageIterator = 0;
    
    ofDirectory imagesDir("Images");
    imagesDir.allowExt("png");
    imagesDir.allowExt("jpg");
    imagesDir.allowExt("jpeg");
    imagesDir.allowExt("gif");
    imagesDir.listDir();
    for(int i=0; i < imagesDir.numFiles(); i++) {
        string path = imagesDir.getPath(i);
        ImageNames.push_back(path);
    }
        
    voronoi->load("Shaders/DummyVert.glsl", "Shaders/VoronoiFrag.glsl");
    blurVert->load("Shaders/DummyVert.glsl", "Shaders/GaussianFragVert.glsl");
    blurHor->load("Shaders/DummyVert.glsl", "Shaders/GaussianFragHor.glsl");
    fade->load("Shaders/DummyVert.glsl", "Shaders/FadeFrag.glsl");
    seedLocs.resize(NUM_SEEDS);
    seedCols.resize(NUM_SEEDS);
    seedVels.resize(NUM_SEEDS);
    initVels.resize(NUM_SEEDS);
    for(int i=0; i < NUM_SEEDS; i++) {
        ofVec2f loc = ofVec2f(ofRandom(WIDTH), ofRandom(HEIGHT));
        ofVec3f col = ofVec3f(ofRandom(255), ofRandom(255), ofRandom(255));
        ofVec2f vel = ofVec2f(ofRandom(-1.0, 1.0), ofRandom(-1.0, 1.0));
        seedLocs[i] = loc;
        seedCols[i] = col;
        seedVels[i] = vel;
        initVels[i] = vel;
    }
    
    blurOnePass.allocate(WIDTH, HEIGHT);
    blurTwoPass.allocate(WIDTH, HEIGHT);
    fadePass.allocate(WIDTH, HEIGHT);
    
    animating  = true;
    
    for(int i = 0; i < ImageNames.size(); i++) {
        ofImage img;
        img.loadImage(ImageNames[i]);
        if(img.width != WIDTH || img.height != HEIGHT) {
            img.saveImage("originalSizes/" + ofSplitString(ImageNames[i], "/")[1]);
            img.resize(WIDTH, HEIGHT);
            img.saveImage(ImageNames[i]);
        }
    }
    displayImgs[imageIterator].loadImage(ImageNames[imageIterator]);
    imageIterator++;
    imageIterator%=ImageNames.size();
    displayImgs[imageIterator].loadImage(ImageNames[imageIterator]);
    imageIterator++;
    imageIterator%=ImageNames.size();
    cam.initGrabber(FLOW_WIDTH, FLOW_HEIGHT);
    cam.listDevices();
    
    flow = new ofxCv::FlowFarneback();
    
    currentTime = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if(cam.isFrameNew()) {
        
        flowImg.setFromPixels(cam.getPixelsRef());
        flowImg.mirror(false, true);
        
        flow->setPyramidScale( 0.5 );
        flow->setNumLevels( 4 );
        flow->setWindowSize( 8 );
        flow->setNumIterations( 2 );
        flow->setPolyN( 7 );
        flow->setPolySigma( 1.5 );
        flow->setUseGaussian(false);
        
        flow->calcOpticalFlow(flowImg);
    }
    
    if(fadeAmnt > 0.0 || ofGetElapsedTimef() - currentTime > 20) {
        fadeAmnt += 0.01;
        if(fadeAmnt > 1.0) {
            fadeAmnt = 0.0;
            displayImgs[0] = displayImgs[1];
            displayImgs[1].loadImage(ImageNames[imageIterator]);
            displayImgs[(currImg+1)%2].loadImage(ImageNames[imageIterator]);
            imageIterator++;
            imageIterator %= ImageNames.size();
        }
        currentTime = ofGetElapsedTimef();
    }
    
    if(animating) {
        for(int i=0; i < seedLocs.size(); i++) {
            seedLocs[i] += seedVels[i];
            float projX = ofMap(seedLocs[i].x, 0, WIDTH, 1, FLOW_WIDTH-1, true);
            float projY = ofMap(seedLocs[i].y, 0, HEIGHT, 1, FLOW_HEIGHT-1, true);

            seedVels[i] += flow->getAverageFlowInRegion(ofRectangle(projX - 1, projY - 1, 2, 2));
            seedVels[i] *= DAMPING;
            if(seedVels[i].length() > MAXVEL) {
                seedVels[i].normalize();
                seedVels[i] *= MAXVEL;
            }
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
    
    fadePass.begin();
        ofClear(0);
        ofEnableAlphaBlending();
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
        fade->begin();
            fade->setUniformTexture("texOut", displayImgs[0].getTextureReference(), 0);
            fade->setUniformTexture("texIn", displayImgs[1].getTextureReference(), 1);
            fade->setUniform1f("fadeAmnt", fadeAmnt);
            displayImgs[0].draw(0, 0, WIDTH, HEIGHT);
//            if( displayImgs[currImg].width < displayImgs[(currImg+1)%2].width)
//                displayImgs[currImg].draw(0, 0, WIDTH, HEIGHT);
//            else
//                displayImgs[(currImg+1)%2].draw(0, 0, WIDTH, HEIGHT);

        fade->end();
    fadePass.end();
    
//    fadePass.draw(0, 0);
    
    blurOnePass.begin();
        ofClear(0);
        blurVert->begin();
            blurVert->setUniform1f("blurAmnt", 10.0);
            fadePass.draw(0, 0);
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
    
    if(cheating) {
        fadePass.draw(0, 0, WIDTH/4, HEIGHT/4);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' ') {
        cheating = !cheating;
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
