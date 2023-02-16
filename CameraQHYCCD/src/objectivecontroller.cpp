#include "objectivecontroller.h"

ObjectiveController::ObjectiveController(const string& serialPort) : mSerialPort(serialPort) {
     connectToController(pattern);
}

bool ObjectiveController::connectToController(const string& serialPort) {
    error.clear();

    if(mObjective.isDeviceOpen())
        mObjective.closeDevice();

    if( mObjective.openDevice(serialPort, 38400) != 1 ) {
        error = "Can't open connection on current port";
        return false;
    }
    return true;
}

bool ObjectiveController::testControllerActive() {
    if(mObjective.isDeviceOpen())
        return true;
    return false;
}

void ObjectiveController::setDiaphragmLevel(const string& value) {
    error.clear();
    if (testControllerActive()) {
        if (value.size() == diaphragmCommandSize && value.at(0) == 'A' && value.at(value.size() - 1) == '#') {
            if( mObjective.writeString(value) != 1 ) {
                error = "Writting error";
            }
        }
        else
            error = "Wrong command format";
    }
}

void ObjectiveController::setFocusing(const string& value) {
    error.clear();
    if (testControllerActive()) {
        if (value.size() == focusCommandSize && value.at(0) == 'M' && value.at(value.size() - 1) == '#') {
            if( mObjective.writeString(value) != 1 ) {
                error = "Writting error";
            }
        }
        else
            error = "Wrong command format";
    }
}

string ObjectiveController::getCurrentFocusing() {
    error.clear();
    string result = "";
    if (testControllerActive()) {
        mObjective.flushReceiver();
        if(mObjective.writeString("P#") == 1) {
            if( mObjective.readString(result, '#', 10, 1000) != 1 ) {
                // TO DO: PARSE AND RETURN ABS VAL
                return result;
            }
            else
                error = "Can't get answer";
        }
    }
    return result;
}


