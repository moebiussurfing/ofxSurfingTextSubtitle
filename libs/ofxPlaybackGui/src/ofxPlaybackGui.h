//
//  ofxPlaybackGui.hpp
//  playback widget
//
//  Created by Roy Macdonald on 12-06-22.
//

#pragma once

#include "ofParameter.h"
#include "ofxGui.h"
#include "ofxMiniGui.h"


/// This class is almost the same as ofxGuiGroup.
/// Unfortunately, ofxGuiGroup's updateChildrenPositions is not virtual, so overiding it is not a good idea.
/// That is why I ended up making this new class which is a stripped down version of ofxGuiGroup that places the elements horizontally
/// but it also automatically creates the needed buttons.
class ofxPlaybackGui: public ofxBaseGui{
public:
    ofxPlaybackGui();
    ~ofxPlaybackGui(){}
    
    ofxPlaybackGui* setup(bool makeRecButton = false ,float x = 10, float y = 10);
  
    
    void add(ofxBaseGui * element);
    

    virtual void sizeChangedCB();

    virtual bool mouseMoved(ofMouseEventArgs & args);
    virtual bool mousePressed(ofMouseEventArgs & args);
    virtual bool mouseDragged(ofMouseEventArgs & args);
    virtual bool mouseReleased(ofMouseEventArgs & args);
    virtual bool mouseScrolled(ofMouseEventArgs & args);


    virtual ofAbstractParameter & getParameter();

    virtual void setPosition(const glm::vec3& p);
    virtual void setPosition(float x, float y);
    
 
    ofParameter<void> forwards = {"Forwards"};
    ofParameter<void> backwards = {"Backwards"};
    ofParameter<void> stop = {"Stop"};
    ofParameter<void> rec = {"Record"};
    ofParameter<bool> play = {"Play"};
    
    
protected:

    
      std::unique_ptr<ofxMiniFwdButton> fwdButton = nullptr;//  forwards
      std::unique_ptr<ofxMiniBackButton> backButton = nullptr;//  backwards
      std::unique_ptr<ofxMiniStopButton> stopButton = nullptr;
      std::unique_ptr<ofxMiniPlayPause> playPauseButton = nullptr;
      std::unique_ptr<ofxMiniRecButton> recButton = nullptr;
      

    
    void makeButtons(bool makeRecButton);
    void updateChildrenPositions(bool bUpdateWidth = false);
    
    

    virtual void render();
    virtual bool setValue(float mx, float my, bool bCheck);
    
    virtual void generateDraw();

    ofParameterGroup parameters;


    bool bGuiActive = false;

    ofPath border;
    
    ofEventListeners listeners;
    
    void _stopPressed();
    

private:
    std::vector<ofxBaseGui*> collection;
    bool bIsFirstRender = true;
};
