#include "Widget.h"
#include "ui_Widget.h"

#include <QMovie>
#include <QDesktopWidget>
#include <QKeyEvent>

#include <QMediaPlaylist>
#include <QMediaPlayer>

#include <QDebug>
#include <QTimer>

namespace
{

constexpr auto VOLUME_STEP = 5;

}

Widget::Widget(QSerialPort *serialPort, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    _index(0),
    _isOn(false),
    _playlist(new QMediaPlaylist(this)),
    _audioPlayer(new QMediaPlayer(this)),
    _serialPort(serialPort),
    _timer(new QTimer(this))
{
    ui->setupUi(this);
    setCursor(Qt::BlankCursor);

    _animations.push_back(new QMovie(":/gif/boat.gif"));
    _animations.push_back(new QMovie(":/gif/corrupted_soul.gif"));
    _animations.push_back(new QMovie(":/gif/tree.gif"));
    _animations.push_back(new QMovie(":/gif/code.gif"));
    _animations.push_back(new QMovie(":/gif/owl.gif"));
    _off = new QMovie(":/gif/blanck.gif");

    _playlist->addMedia(QUrl::fromLocalFile("/work/sandbox/TvShow/noise0.mp3"));
    _playlist->addMedia(QUrl::fromLocalFile("/work/sandbox/TvShow/noise1.mp3"));
    _playlist->addMedia(QUrl::fromLocalFile("/work/sandbox/TvShow/noise2.mp3"));
    _playlist->addMedia(QUrl::fromLocalFile("/work/sandbox/TvShow/noise3.mp3"));
    _playlist->addMedia(QUrl::fromLocalFile("/work/sandbox/TvShow/noise4.mp3"));

    _audioPlayer->setPlaylist(_playlist);
    _playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);

    setImage(_index);
    turnOff();

    connect(_serialPort, &QSerialPort::readyRead, this, &Widget::readFromSerial);
    connect(_serialPort, &QSerialPort::errorOccurred, this, &Widget::processSerialError);
    connect(_serialPort, &QSerialPort::bytesWritten, this, &Widget::bytesWritten);

    QTimer::singleShot(1600, [this](){_serialPort->write("V", 1);});

    _timer->setSingleShot(true);
    connect(_timer, &QTimer::timeout, this, &Widget::turnOff);
}

Widget::~Widget()
{
    qDeleteAll(_animations);
    delete ui;
}

void Widget::setImage(unsigned index)
{
    if (index < _animations.size())
    {
        _movie = _animations.at(index);
        setMovie(_movie);

        _playlist->setCurrentIndex(index);
        _audioPlayer->play();
    }
}

void Widget::volumeUp()
{
    _audioPlayer->setVolume(_audioPlayer->volume() + VOLUME_STEP);
}

void Widget::volumeDown()
{
    _audioPlayer->setVolume(_audioPlayer->volume() - VOLUME_STEP);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
    {
        if (_isOn)
        {
            _index = (_index + 1) % _animations.size();
            setImage(_index);
        }
        break;
    }
    case Qt::Key_0:
    {
        turnOff();
        break;
    }
    case Qt::Key_1:
    {
        turnOn();
        break;
    }
    case Qt::Key_6:
    {
        volumeDown();
        break;
    }
    case Qt::Key_7:
    {
        volumeUp();
        break;
    }
    default:
        break;
    }
}

void Widget::turnOff()
{
    _serialPort->write("P", 1);
    setMovie(_off);
    _audioPlayer->setMuted(true);
    _isOn = false;
}

void Widget::turnOn()
{
    setMovie(_movie);
    _audioPlayer->setMuted(false);
    _isOn = true;
}

void Widget::setVolume(int volume)
{
    _audioPlayer->setVolume(volume);
}

void Widget::setMovie(QMovie *movie)
{
    ui->label->setMovie(movie);
    movie->start();
    movie->setScaledSize(QApplication::desktop()->screenGeometry().size());
}

void Widget::readFromSerial()
{
    qDebug() << "Reading from serial";

    auto message = _serialPort->readAll();
    _dataRead += message;
    while (_dataRead.size() >= 2)
    {
        processCommand();
    }
    qDebug() << message;
}

void Widget::processSerialError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        qDebug() << QObject::tr("An I/O error occurred while reading "
                                        "the data from port %1, error: %2")
                            .arg(_serialPort->portName())
                            .arg(_serialPort->errorString())
                         << endl;
    }
}

void Widget::bytesWritten(qint64 count)
{
    qDebug() << QObject::tr("%1 bytes written to port %2")
                .arg(count).arg(_serialPort->portName()) << endl;
}

void Widget::processCommand()
{
    char command = _dataRead.at(0);
    char value = _dataRead.at(1);

    qDebug() << QObject::tr("Command: %1, value %2").arg(command).arg(value);
    if (command == 'C')
    {
        setImage(value);
    }
    else if (command == 'V')
    {
        setVolume(value);
    }
    else if (command == 'P')
    {
        if (value == 0x01)
        {
            turnOn();
        }
        else
        {
            turnOff();
        }
    }

    _dataRead = _dataRead.right(_dataRead.size() - 2);
    _timer->start(1000 * 60 * 2);
}
