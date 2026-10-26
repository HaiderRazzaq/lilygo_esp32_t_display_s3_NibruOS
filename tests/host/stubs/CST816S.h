#pragma once
#include <Arduino.h>
enum GESTURE { NONE=0, SWIPE_UP=1, SWIPE_DOWN=2, SWIPE_LEFT=3, SWIPE_RIGHT=4,
               SINGLE_CLICK=5, DOUBLE_CLICK=11, LONG_PRESS=12 };
struct TouchData {byte gestureID=0; int x=0; int y=0;};
extern bool fakeTouchPending;
extern TouchData fakeTouchData;
struct CST816S {
  CST816S(int,int,int,int){}
  void begin(int){}
  bool available(){
    if (!fakeTouchPending) return false;
    fakeTouchPending=false; data=fakeTouchData; return true;
  }
  TouchData data;
};
