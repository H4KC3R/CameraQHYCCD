#ifndef OBJECTIVECONTROLLER_H
#define OBJECTIVECONTROLLER_H
#include <iostream>
#include "serialib.h"

using namespace std;

class ObjectiveController {
public:
    explicit ObjectiveController(const string& pattern);

    void setDiaphragmLevel(const string& value);

    void setFocusing(const string& value);

    string getCurrentFocusing();

    string currentError() const;

    bool connectToController(const string& serialPort);

private:

    bool testControllerActive();

    serialib mObjective;
    string mSerialPort;
    string error;
    static constexpr const int16_t focusCommandSize = 6;
    static constexpr const int16_t diaphragmCommandSize = 4;
};

#endif // OBJECTIVECONTROLLER_H
