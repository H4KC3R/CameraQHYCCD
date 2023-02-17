#include "objectivecontroller.h"

ObjectiveController::ObjectiveController(const char* serialPort) : mSerialPort(serialPort) {
    if(!connectToController(serialPort))
        throw std::logic_error("Could not connect to objective");

}

ObjectiveController::~ObjectiveController(){
    if(testControllerActive())
        mObjective.closeDevice();
}

bool ObjectiveController::connectToController(const char* serialPort) {
    mError.clear();

    if(mObjective.isDeviceOpen())
        mObjective.closeDevice();

    if( mObjective.openDevice(serialPort, 38400) != 1 ) {
        mError = "Can't open connection on current port";
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
    mError.clear();
    if (testControllerActive()) {


        if (value.size() == diaphragmCommandSize && value.at(0) == 'A' && value.at(value.size() - 1) == '#') {
            //if( mObjective.writeString(value) != 1 ) {
            //    error = "Writting error";
            //}
        }
        else
            mError = "Wrong command format";
    }
}

void ObjectiveController::setFocusing(const double value) {
    mError.clear();
    if (testControllerActive()) {
        string val = to_string(value);
        string cmd = "M" + val + "#";
        if( mObjective.writeString(cmd.c_str()) != 1 ) {
            mError = "Writting error";
        }
    }
}

double ObjectiveController::getCurrentFocusing() {
    mError.clear();
    char* rxBuff;
    std::string resultStr = "0";
    std::string::size_type sz;

    if (testControllerActive()) {
        mObjective.flushReceiver();
        if(mObjective.writeString("P#") == 1) {
            std::this_thread::sleep_for(1000ms);
            if( mObjective.readString(rxBuff, '#', 10, 1000) != 1 ) {
                std::string s(rxBuff);
                resultStr = s.substr(0, s.length() - 1);
            }
            else
                mError = "Can't get answer";
        }
    }
    double result = 0;
    try{
        result = std::stod(resultStr, &sz);
    }
    catch(...){
        mError = "Can't get answer";
    }
    return result;
}

string ObjectiveController::currentError() const{
    return mError;
}
