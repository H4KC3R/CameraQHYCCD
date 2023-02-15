#ifndef OBJECTIVECONTROLLER_H
#define OBJECTIVECONTROLLER_H
#include <iostream>

using namespace std;

class ObjectiveController {
public:
    explicit ObjectiveController(const string& pattern, int32_t port = -1);

    void setDiaphragmLevel(const string& value);

    void setFocusing(const string& value);

    string getCurrentFocusing();

    string currentError() const;

    void connectToController(const string& ptrn, int32_t port = -1);

private:

    bool testControllerActive();
    string pattern;
    string objective;
    string error;
    static constexpr const int16_t focusCommandSize = 6;
    static constexpr const int16_t diaphragmCommandSize = 4;
};

#endif // OBJECTIVECONTROLLER_H
