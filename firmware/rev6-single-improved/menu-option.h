/* file: menu-option.h */

#include "Arduino.h"

#ifndef MenuOption_h
#define MenuOption_h

class MenuOption {
  public:
    void Init(int pos, String title, String systemName, String type, float defaultVal, float maxVal, float minVal, bool showInMenu, bool saveInFlash); //bool, int, or float
    void adjustVal(bool increment);
    int pos();
    String title();
    String getType();
    float defaultVal();

    void setBoolVal(bool value);
    void setIntVal(int value);
    void setFloatVal(float value);
    bool boolVal();
    int intVal();
    float floatVal();
    
    bool showInMenu();
    
  private:
    int _pos;
    String _title;
    String _systemName;
    String _type;
    float _defaultVal;
    bool _boolVal;
    int _intVal;
    float _floatVal;
    float _maxVal;
    float _minVal;
    bool _showInMenu;
    bool _saveInFlash;

    void _loadFromFlash();
};

#endif