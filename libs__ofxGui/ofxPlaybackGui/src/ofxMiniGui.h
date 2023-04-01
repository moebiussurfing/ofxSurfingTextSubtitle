//
//  ofxMiniGui.hpp
//  playback widget
//
//  Created by Roy Macdonald on 12-06-22.
//

#pragma once

#include "ofParameter.h"
#include "ofxBaseGui.h"

class ofxMiniToggle : public ofxBaseGui{
public:
    ofxMiniToggle(){};
    ~ofxMiniToggle();
    ofxMiniToggle(ofParameter<bool> _bVal, float width = defaultHeight, float height = defaultHeight);
    ofxMiniToggle * setup(ofParameter<bool> _bVal, float width = defaultHeight, float height = defaultHeight);
    ofxMiniToggle * setup(const std::string& toggleName, bool _bVal, float width = defaultHeight, float height = defaultHeight);
    

    virtual bool mouseMoved(ofMouseEventArgs & args);
    virtual bool mousePressed(ofMouseEventArgs & args);
    virtual bool mouseDragged(ofMouseEventArgs & args);
    virtual bool mouseReleased(ofMouseEventArgs & args);
    virtual bool mouseScrolled(ofMouseEventArgs & args){return false;}
    

    template<class ListenerClass, typename ListenerMethod>
    void addListener(ListenerClass * listener, ListenerMethod method){
        value.addListener(listener,method);
    }

    template<class ListenerClass, typename ListenerMethod>
    void removeListener(ListenerClass * listener, ListenerMethod method){
        value.removeListener(listener,method);
    }



    bool operator=(bool v);
    operator const bool & ();

    virtual ofAbstractParameter & getParameter();
    void setIcon(const ofPath& icon);

protected:
    virtual void render();
    ofParameter<bool> value;
    bool bGuiActive;
    
    bool setValue(float mx, float my, bool bCheck);
    void generateDraw();
    void valueChanged(bool & value);
    ofPath bg,fg,icon;
};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
class ofxMiniButton : public ofxMiniToggle{
    friend class ofPanel;
    
public:
    ofxMiniButton();
    ofxMiniButton(ofParameter<void> _bVal, float width = defaultHeight, float height = defaultHeight);
    ofxMiniButton(const std::string& toggleName, float width = defaultHeight, float height = defaultHeight);
    ~ofxMiniButton();
    ofxMiniButton* setup(ofParameter<void> _bVal, float width = defaultHeight, float height = defaultHeight);
    ofxMiniButton* setup(const std::string& toggleName, float width = defaultHeight, float height = defaultHeight);

    virtual bool mouseReleased(ofMouseEventArgs & args);
    virtual bool mouseMoved(ofMouseEventArgs & args);
    virtual bool mousePressed(ofMouseEventArgs & args);
    virtual bool mouseDragged(ofMouseEventArgs & args);

    template<class ListenerClass, typename ListenerMethod>
    void addListener(ListenerClass * listener, ListenerMethod method){
        parameter.addListener(listener,method);
    }

    template<class ListenerClass, typename ListenerMethod>
    void removeListener(ListenerClass * listener, ListenerMethod method){
        parameter.removeListener(listener,method);
    }
    virtual ofAbstractParameter & getParameter();

private:
    ofParameter<void> parameter;
    void valueChanged(bool & v);

};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
class ofxMiniPlayPause: public ofxMiniToggle{
public:
    ofxMiniPlayPause(ofParameter<bool> _bVal, float width = defaultHeight, float height = defaultHeight);
    
protected:
    virtual void generateDraw();
    virtual void render();
    ofPath iconPause;
};

//--------------------------------------------------------------------------------
class ofxMiniStopButton: public ofxMiniButton{
public:
    ofxMiniStopButton():ofxMiniButton(){}
    ofxMiniStopButton(ofParameter<void> _bVal, float width = defaultHeight, float height = defaultHeight):ofxMiniButton(_bVal, width, height){}
    ofxMiniStopButton(const std::string& buttonName, float width = defaultHeight, float height = defaultHeight):ofxMiniButton(buttonName, width, height){}
    
protected:
    virtual void generateDraw();
};

class ofxMiniFwdButton: public ofxMiniButton{
public:
    ofxMiniFwdButton():ofxMiniButton(){}
    ofxMiniFwdButton(ofParameter<void> _bVal, float width = defaultHeight, float height = defaultHeight):ofxMiniButton(_bVal, width, height){}
    ofxMiniFwdButton(const std::string& buttonName, float width = defaultHeight, float height = defaultHeight):ofxMiniButton(buttonName, width, height){}
    
protected:
    virtual void generateDraw();
};


class ofxMiniBackButton: public ofxMiniButton{
public:
    ofxMiniBackButton():ofxMiniButton(){}
    ofxMiniBackButton(ofParameter<void> _bVal, float width = defaultHeight, float height = defaultHeight):ofxMiniButton(_bVal, width, height){}
    ofxMiniBackButton(const std::string& buttonName, float width = defaultHeight, float height = defaultHeight):ofxMiniButton(buttonName, width, height){}
    
protected:
    virtual void generateDraw();
};

class ofxMiniRecButton: public ofxMiniButton{
public:
    ofxMiniRecButton():ofxMiniButton(){}
    ofxMiniRecButton(ofParameter<void> _bVal, float width = defaultHeight, float height = defaultHeight):ofxMiniButton(_bVal, width, height){}
    ofxMiniRecButton(const std::string& buttonName, float width = defaultHeight, float height = defaultHeight):ofxMiniButton(buttonName, width, height){}
    
protected:
    virtual void generateDraw();
};




