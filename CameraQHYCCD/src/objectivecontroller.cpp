#include "objectivecontroller.h"

ObjectiveController::ObjectiveController(const string& pattern, int32_t port) : pattern(pattern) {
     connectToController(pattern, port);
}

bool ObjectiveController::testControllerActive() {
    if (!objective.isOpen())
    {
        qDebug() << "контроллер не активирован";
        return false;
    }
    return true;
}

void ObjectiveController::setDiaphragmLevel(const QString& value) {
    error.clear();
    if (testControllerActive())
    {
        if (value.size() == diaphragmCommandSize && value.at(0) == 'A' && value.at(value.size() - 1) == '#')
        {
            if (objective.write(value.toLocal8Bit())  == -1)
            {
                error = objective.errorString();
                qDebug() << error;
            }
        }
        else
        {
            error = "Wrong command format";
            qDebug() << error;
        }
    }
}

void ObjectiveController::setFocusing(const QString& value)
{
    error.clear();
    qDebug() << "OBJECTIVE" << value;
    if (testControllerActive())
    {
        if (value.size() == focusCommandSize && value.at(0) == 'M' && value.at(value.size() - 1) == '#')
        {
            if (objective.write(value.toLocal8Bit()) == -1)
            {
                error = objective.errorString();
                qDebug() << error;
            }
        }
        else
        {
            error = "Wrong command format";
            qDebug() << error;
        }
    }
}

string ObjectiveController::getCurrentFocusing() {
    error.clear();
    if (testControllerActive())
    {
        if (objective.write(QString("P#").toLocal8Bit()))
        {
            if (objective.waitForReadyRead(1000))
            {
                QByteArray array = objective.read(objective.bytesAvailable());
                return QString::fromLatin1(array.data());
            }
            else
            {
                error = "Can't get answer";
                qDebug() << error;
            }
        }
        else
        {
            error = "Wrong command format";
            qDebug() << error;
        }
    }
    return QString();
}


void ObjectiveController::connectToController(const string& ptrn, int32_t port) {
    pattern = ptrn;
    error.clear();
    objective.close();

    if (port == -1)
    {
        auto ports = QSerialPortInfo::availablePorts();
        for (auto& i : ports)
        {
            qDebug() << i.description() << i.manufacturer();
        }
        for (qint32 i = 0; i < ports.size(); ++i)
        {
           // qDebug() << ports.size() << ports[i].description() << ports[i].manufacturer();
            if (ports[i].description().contains(pattern))
            {
                port = i;
                break;
            }
        }
    }
    if (port != -1)
    {
        objective.setPort(QSerialPortInfo::availablePorts().at(port));
        if (objective.open(QIODevice::ReadWrite))
        {
            objective.setBaudRate(38400);
        }
        else
        {
            error = "Can't open connection on current port " + objective.errorString();
            qDebug() << error;
        }
    }
    else
    {
        error = "Invalid port ";
        qDebug() << error;
    }
}
