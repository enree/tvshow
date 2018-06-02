#include "Widget.h"

#include <QSerialPort>
#include <QApplication>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextStream standardOutput(stdout);

    QSerialPort serialPort;
    serialPort.setBaudRate(QSerialPort::Baud9600);

    if (a.arguments().size() > 1)
    {
        const auto serialPortName = a.arguments().at(1);
        serialPort.setPortName(serialPortName);

        if (!serialPort.open(QIODevice::ReadWrite)) {
            standardOutput << QObject::tr("Failed to open port %1, error: %2")
                              .arg(serialPortName)
                              .arg(serialPort.errorString())
                           << endl;
            return 1;
        }
    }



    Widget w(&serialPort);
    w.showFullScreen();
//    w.show();

    return a.exec();
}
