#pragma once

#include <QWidget>
#include <QSerialPort>

#include <vector>

namespace Ui {
class Widget;
}

class QMovie;
class QMediaPlayer;
class QMediaPlaylist;
class QTimer;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QSerialPort* serialPort, QWidget *parent = 0);
    ~Widget();

    void setImage(unsigned index);
    void volumeUp();
    void volumeDown();
    void turnOff();
    void turnOn();
    void setVolume(int volume);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void setMovie(QMovie* movie);
    void readFromSerial();
    void processSerialError(QSerialPort::SerialPortError serialPortError);
    void bytesWritten(qint64 count);

    void processCommand();
private:
    Ui::Widget *ui;

    std::vector<QMovie*> _animations;
    QMovie* _off;

    QMovie* _movie;
    int _index;
    bool _isOn;

    QMediaPlaylist* _playlist;
    QMediaPlayer* _audioPlayer;


    // Arduino communication
    QSerialPort* _serialPort;
    QByteArray _dataRead;

    QTimer *_timer;
};
