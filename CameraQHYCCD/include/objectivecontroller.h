#ifndef OBJECTIVECONTROLLER_H
#define OBJECTIVECONTROLLER_H
#include <iostream>
#include <chrono>
#include <thread>
#include "serialib.h"

using namespace std;

class ObjectiveController {
public:
    explicit ObjectiveController(const char* serialPort);

    ~ObjectiveController();

    bool connectToController(const char* serialPort);

    void setDiaphragmLevel(const string& value);

    void setFocusing(const double value);

    double getCurrentFocusing();

    string currentError() const;

private:

    bool testControllerActive();

    serialib mObjective;
    string mSerialPort;
    string mError;
    const string focusCommand = "M#";
    static constexpr const int16_t diaphragmCommandSize = 4;
};

#endif // OBJECTIVECONTROLLER_H
