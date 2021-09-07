/* file: menu-option.cpp */

#include "Arduino.h"
#include "menu-option.h"

// is this valid here?
#include <Preferences.h>
Preferences preferences;

//class constructor
void MenuOption::MenuOption(int pos, String title, String systemName, String type, float defaultVal, float maxVal, float minVal, bool showInMenu, bool saveInFlash) {
  _pos = pos;
  _title = title;
  _systemName = systemName;
  _type = type;
  _defaultVal = defaultVal;
  _maxVal = maxVal;
  _minVal = minVal;
  _showInMenu = showInMenu;
  _saveInFlash = saveInFlash;
  _loadFromFlash();
}

void MenuOption::_loadFromFlash() {
  // load the last known setting from flash
  if (_type == "bool") {
    _boolVal = preferences.getBool(_systemName, _defaultVal == 1 ? true : false);
  } else if (_type == "int") {
    _intVal = preferences.getUInt(_systemName, Int(_defaultVal));
  } else if (_type == "float") {
    _floatVal = preferences.getFloat(_systemName, _defaultVal);
  }  
}

void MenuOption::adjustVal(bool increment) {
  if (_type == "bool") {
    _boolVal = !_boolVal;
    preferences.putBool(_systemName, _boolVal) //save to flash
  }
  else if (_type == "int") {
    if (increment) {
      _intVal++;
      if (_intVal > int(_maxVal)) {
        _intVal = int(_minVal);
      }
    } else {
      _intVal--;
      if (_intVal < int(_minVal)) {
        _intVal = int(_maxVal);
      }
    }
    preferences.putUInt(_systemName, _intVal) //save to flash
  }
  else if (_type == "float") {
    if (increment) {
      _floatVal = _floatVal + 0.5;
      if (_floatVal > _maxVal) {
        _floatVal = _minVal;
      }
    } else {
      _floatVal = _floatVal - 0.5;
      if (_floatVal < _minVal) {
        _floatVal = _maxVal;
      }
    }
    preferences.putFloat(_systemName, _floatVal) //save to flash
  }
}

int MenuOption::pos() {
  return _pos;
}
String MenuOption::title() {
  return _title;
}
String MenuOption::getType() {
  return _type;
}
bool MenuOption::defaultVal() {
  return _defaultVal;
}
void MenuOption::setBoolVal(bool value) {
  _boolVal = value;
}
void MenuOption::setIntVal(int value) {
  _intVal = value;
}
void MenuOption::setFloatVal(float value) {
  _floatVal = value;
}
bool MenuOption::boolVal() {
  return _boolVal;
}
int MenuOption::intVal() {
  return _intVal;
}
float MenuOption::floatVal() {
  return _floatVal;
}
bool MenuOption::showInMenu() {
  return _showInMenu;
}