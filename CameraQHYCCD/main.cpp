#include <iostream>
#include "cameraqhyccd.h"

using namespace std;

// qhyccd camera usage sample
int main()
{
    CameraQHYCCD* myCamera;
    if(!CameraQHYCCD::initSDK()) {
        cout << "Init SDK fail" << endl;
        return -1;
    }

    int num = 0;
    num = CameraQHYCCD::searchCamera();
    if(num >= 1) {
        char id[32];
        cout << "Camera finded: " << num << endl;
        if(CameraQHYCCD::getID(0,id)) {
            cout << "ID getting fail" << endl;
            return -1;
        }
        myCamera = new CameraQHYCCD(id);
        myCamera->connect(single);
    }
    else
        cout << "Camera finded: " << num << endl;
    system("pause");
    return 0;
}
