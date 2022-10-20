//
//  ofxPlaybackGui.cpp
//  playback widget
//
//  Created by Roy Macdonald on 12-06-22.
//

#include "ofxPlaybackGui.h"

#include "ofGraphics.h"
#include "ofxGuiGroup.h"

using namespace std;

ofxPlaybackGui::ofxPlaybackGui(){
    bGuiActive = false;
}


ofxPlaybackGui* ofxPlaybackGui::setup(bool makeRecButton, float x, float y){
    b.x = x;
    b.y = y;
    b.height = defaultHeight;
    if(parent != nullptr){
        b.width = parent->getWidth();
    }else{
        b.width = defaultWidth;
    }
    
    bGuiActive = false;
    
    makeButtons(makeRecButton);
    
    updateChildrenPositions(true);
    registerMouseEvents();
    
    setNeedsRedraw();
    
    return this;
}


void ofxPlaybackGui::makeButtons(bool makeRecButton)
{
    
    fwdButton = make_unique<ofxMiniFwdButton> (forwards);
    backButton = make_unique<ofxMiniBackButton> (backwards);
    stopButton = make_unique<ofxMiniStopButton> (stop);
    playPauseButton = make_unique<ofxMiniPlayPause>(play);
    
    if(makeRecButton){
        recButton =  make_unique<ofxMiniRecButton>(rec);
    }
    
    listeners.push(stop.newListener(this, &ofxPlaybackGui::_stopPressed));
    
    
    parameters.setName("Playback gui");
    
    this->add(backButton.get());
    this->add(playPauseButton.get());
    this->add(fwdButton.get());
    this->add(stopButton.get());
    if(makeRecButton){
        this->add(recButton.get());
    }
    
}




void ofxPlaybackGui::add(ofxBaseGui * element){
    collection.push_back(element);
    
    element->unregisterMouseEvents();
    
    element->setParent(this);
    // updateChild(...) could be called instead of updateChildrenPositions(...), here but the latter ensures that all the elements are placed properly.
    updateChildrenPositions(true);
    parameters.add(element->getParameter());
    setNeedsRedraw();
}


bool ofxPlaybackGui::mouseMoved(ofMouseEventArgs & args){
    if(!isGuiDrawing())return false;
    ofMouseEventArgs a = args;
    for(std::size_t i = 0; i < collection.size(); i++){
        if(collection[i]->mouseMoved(a)){
            
            return true;
        }
    }
    if(b.inside(args)){
        return true;
    }else{
        return false;
    }
}

bool ofxPlaybackGui::mousePressed(ofMouseEventArgs & args){
    if(!isGuiDrawing())return false;
    
    if(setValue(args.x, args.y, true)){
        return true;
    }
    auto attended = false;
    ofMouseEventArgs a = args;
    for(std::size_t i = 0; i < collection.size(); i++){
        if(collection[i]->mousePressed(a)){
            attended = true;
        }
    }
    return attended || b.inside(args);
}

bool ofxPlaybackGui::mouseDragged(ofMouseEventArgs & args){
    if(!isGuiDrawing())return false;
    if(bGuiActive){
        if(setValue(args.x, args.y, false)){
            return true;
        }
        ofMouseEventArgs a = args;
        for(std::size_t i = 0; i < collection.size(); i++){
            if(collection[i]->mouseDragged(a)){
                return true;
            }
        }
    }
    return false;
}

bool ofxPlaybackGui::mouseReleased(ofMouseEventArgs & args){
    if(!isGuiDrawing()){
        bGuiActive = false;
        return false;
    }
    if(bGuiActive){
        bGuiActive = false;
        for(std::size_t k = 0; k < collection.size(); k++){
            ofMouseEventArgs a = args;
            if(collection[k]->mouseReleased(a)){
                return true;
            }
        }
        if(b.inside(ofPoint(args.x, args.y))){
            return true;
        }else{
            return false;
        }
    }
    return false;
}

bool ofxPlaybackGui::mouseScrolled(ofMouseEventArgs & args){
    if(!isGuiDrawing())return false;
    ofMouseEventArgs a = args;
    for(std::size_t i = 0; i < collection.size(); i++){
        if(collection[i]->mouseScrolled(a)){
            return true;
        }
    }
    if(b.inside(args)){
        return true;
    }else{
        return false;
    }
}

void ofxPlaybackGui::generateDraw(){
    border.clear();
    border.setFillColor(thisBorderColor);
    //    border.setFillColor(ofColor::red);
    border.setFilled(true);
    border.rectangle(b);
    
}

void ofxPlaybackGui::render(){
    
    if(bIsFirstRender){
        bIsFirstRender = false;
        sizeChangedCB();
    }
    
    border.draw();
    
    ofBlendMode blendMode = ofGetStyle().blendingMode;
    if(blendMode != OF_BLENDMODE_ALPHA){
        ofEnableAlphaBlending();
    }
    for(std::size_t i = 0; i < collection.size(); i++){
        collection[i]->draw();
    }
    
    if(blendMode != OF_BLENDMODE_ALPHA){
        ofEnableBlendMode(blendMode);
    }
}


bool ofxPlaybackGui::setValue(float mx, float my, bool bCheck){
    
    if(!isGuiDrawing()){
        bGuiActive = false;
        return false;
    }
    
    
    if(bCheck){
        if(b.inside(mx, my)){
            bGuiActive = true;
        }
    }
    
    return false;
}


void ofxPlaybackGui::updateChildrenPositions(bool bUpdateWidth){
    
    
    float x = b.x;
    float w = b.width;
    
    if(parent){//if the group has no parent dont indent the children. it just looks better
        x += ofxGuiGroup::childrenLeftIndent;
        w -= ofxGuiGroup::childrenLeftIndent + ofxGuiGroup::childrenRightIndent;
    }
    float elementSpacing = 0;
    if(collection.size() > 1){
        float elems_w = 0;
        for(auto e: collection){
            if(e){
                elems_w += e->getShape().width;
            }
        }
        
        float remaining_space = w - elems_w;
        
        elementSpacing = remaining_space /float(collection.size() -1);
        
        elementSpacing = std::min(elementSpacing, 5.0f);
        
        
        x += (remaining_space - ((collection.size() -1)*elementSpacing) )/2;
        
    }
    
    for(auto e: collection){
        if(e){
            //            if(bUpdateWidth){
            //                e->setShapeNoNotification(x, b.y, b.width, );
            //            }else{
            e->setPosition(x, b.y);
            //            }
            x += e->getShape().width + elementSpacing;
        }
    }
    //    backButton->setPosition(x, b.y);
    //    x += stopButton->getShape().width + ofxPlaybackGui::elementSpacing;
    //
    //    playPauseButton->setPosition(x, b.y);
    //    x += playPauseButton->getShape().width + ofxPlaybackGui::elementSpacing;
    //
    //    fwdButton->setPosition(x, b.y);
    //    x += fwdButton->getShape().width + ofxPlaybackGui::elementSpacing;
    //    stopButton->setPosition(x, b.y);
    
    
    
}

void ofxPlaybackGui::sizeChangedCB(){
    updateChildrenPositions(true);
    if(parent){
        parent->sizeChangedCB();
    }
    setNeedsRedraw();
}


ofAbstractParameter & ofxPlaybackGui::getParameter(){
    return parameters;
}

void ofxPlaybackGui::setPosition(const glm::vec3& p){
    b.setPosition(p);
    updateChildrenPositions(false);
    setNeedsRedraw();
}

void ofxPlaybackGui::setPosition(float x, float y){
    setPosition({x, y, 0});
}


void ofxPlaybackGui::_stopPressed(){
    play = false;
}




