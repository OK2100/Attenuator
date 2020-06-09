#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTextStream>
#include "doublespinbox.h"
#include <QMessageBox>
#include <QDebug>
#include <QScrollBar>
#include <QTextStream>
#include <QDateTime>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();

    void handleTimeOut();

    void handleError(QSerialPort::SerialPortError error);

    void on_btnViewInfo_clicked();

    void on_spinAtten_dB_editingFinished();

    void on_checkEnableTextCmd_stateChanged(int arg1);

    void on_editTextCommand_editingFinished();

    void on_btnSaveLog_clicked();

    void on_btnUpdateList_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    void showStatusMessage(const QString &message);

    Ui::MainWindow *ui;

    QLabel *m_status = nullptr;
    QSerialPort *m_serial = nullptr;
    QScrollBar *bar;

    QTimer timeoutTimer;

    quint16 m_transactionCount=0;
};


//  #######################################################################################################
//  #######################################################################################################
//  #######################################################################################################
//  #######################################################################################################




#endif // MAINWINDOW_H
