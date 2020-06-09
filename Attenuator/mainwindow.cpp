#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_serial(new QSerialPort(this))
{
    ui->setupUi(this);
    this->statusBar()->addWidget(m_status);

    setWindowTitle("Attenuator OZ DA-100");
    showStatusMessage("Disconnected");

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->boxPorts->addItem(info.portName());

    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(0,0,0,50));
    p.setColor(QPalette::Text, Qt::black);

    ui->textArea->setPalette(p);
    ui->textArea->setReadOnly(1);

    ui->checkEnableTextCmd->setChecked(1);
    ui->checkEnableTextCmd->setChecked(0);

    ui->frame2->setEnabled(0);

    bar = ui->textArea->verticalScrollBar();

    timeoutTimer.setInterval(ui->spinWaitTime->value());

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    connect(ui->btnStart,&QPushButton::clicked,this,&MainWindow::openSerialPort);
    connect(ui->btnStop,&QPushButton::clicked,this,&MainWindow::closeSerialPort);
    connect(&timeoutTimer,&QTimer::timeout,this,&MainWindow::handleTimeOut);
}

MainWindow::~MainWindow()
{
    closeSerialPort();
    delete ui;
}

void MainWindow::openSerialPort()
{
    ui->boxPorts->setEnabled(0);
    ui->spinWaitTime->setEnabled(0);
    ui->btnStart->setEnabled(0);
    ui->btnStop->setEnabled(1);

    m_serial->setPortName(ui->boxPorts->currentText());
    m_serial->setBaudRate(QSerialPort::Baud9600);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        QString str = tr("Connected to %1 : %2, %3, %4, %5, %6")
                .arg(ui->boxPorts->currentText()).arg(QSerialPort::Baud9600).arg(QSerialPort::Data8)
                .arg(QSerialPort::NoParity).arg(QSerialPort::OneStop).arg(QSerialPort::NoFlowControl);
        showStatusMessage(str);
        ui->textArea->insertPlainText(str);
        ui->textArea->insertPlainText("\n\r");
        bar->setValue(bar->maximum());
        ui->frame2->setEnabled(1);

    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        ui->textArea->insertPlainText("Open error\n");
        ui->textArea->insertPlainText(m_serial->errorString());
        ui->textArea->insertPlainText("\n\r");
        bar->setValue(bar->maximum());

        showStatusMessage("Open error");
    }
}

void MainWindow::closeSerialPort()
{
    ui->boxPorts->setEnabled(1);
    ui->spinWaitTime->setEnabled(1);
    ui->btnStart->setEnabled(1);
    ui->btnStop->setEnabled(0);
    ui->frame2->setEnabled(0);


    if (m_serial->isOpen())
        m_serial->close();
//    m_console->setEnabled(false);
    ui->textArea->insertPlainText("Disconected\n\r");
    bar->setValue(bar->maximum());
    showStatusMessage("Disconnected");
}

void MainWindow::writeData(const QByteArray &data)
{
    ui->textArea->insertPlainText(tr("Traffic, transaction>: #%1:\n>request:\t").arg(++m_transactionCount));
    ui->textArea->insertPlainText(data);
    ui->textArea->insertPlainText("\n");

    bar->setValue(bar->maximum());

    m_serial->write(data);
    timeoutTimer.start(ui->spinWaitTime->value());
}

void MainWindow::readData()
{
    timeoutTimer.stop();
    const QByteArray data = m_serial->readAll();
    ui->textArea->insertPlainText(">response:\t");
    ui->textArea->insertPlainText(data);
    ui->textArea->insertPlainText("\n");

    bar->setValue(bar->maximum());
}

void MainWindow::handleTimeOut()
{
    timeoutTimer.stop();
    ui->textArea->insertPlainText("Timeout "+QString::number(ui->spinWaitTime->value())+" ms\n\r");
    bar->setValue(bar->maximum());
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        ui->textArea->insertPlainText("Critical Error\n");
        ui->textArea->insertPlainText(m_serial->errorString());
        ui->textArea->insertPlainText("\n\r");
        bar->setValue(bar->maximum());
        closeSerialPort();
    }
}


void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::on_btnViewInfo_clicked()
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        if(info.portName() == ui->boxPorts->currentText()){

            const QString blankString = "N/A";
            QString description;
            QString manufacturer;
            QString serialNumber;

            description = info.description();
            manufacturer = info.manufacturer();
            serialNumber = info.serialNumber();

            QString text;
            QTextStream out(&text,QIODevice::WriteOnly);

            out
                << "Port: " << info.portName() << "\n"
                << "Location: " << info.systemLocation() << "\n"
                << "Description: " << (!description.isEmpty() ? description : blankString) << "\n"
                << "Manufacturer: " << (!manufacturer.isEmpty() ? manufacturer : blankString) << "\n"
                << "Serial number: " << (!serialNumber.isEmpty() ? serialNumber : blankString) << "\n"
                << "Vendor Identifier: " << (info.hasVendorIdentifier()
                                             ? QByteArray::number(info.vendorIdentifier(), 16)
                                             : blankString) << "\n"
                << "Product Identifier: " << (info.hasProductIdentifier()
                                              ? QByteArray::number(info.productIdentifier(), 16)
                                              : blankString) << "\n"
                << "Busy: " << (info.isBusy() ? "Yes" : "No") << "\n\r";

            ui->textArea->appendPlainText(text);

            break;
        }
}


void MainWindow::on_spinAtten_dB_editingFinished()
{
    ui->spinAtten_dB->blockSignals(true);
    ui->spinAtten_dB->clearFocus();
    QByteArray data = tr("A%1\r").arg(ui->spinAtten_dB->value(),5,'f',2,QLatin1Char('0')).toLocal8Bit();
//    for(int elem : data) qDebug() << hex << elem;
    this->writeData(data);
    ui->spinAtten_dB->blockSignals(false);
}

void MainWindow::on_checkEnableTextCmd_stateChanged(int arg1)
{
    ui->lblTextCommand->setEnabled(arg1);
    ui->editTextCommand->setEnabled(arg1);
}

void MainWindow::on_editTextCommand_editingFinished()
{
    ui->editTextCommand->blockSignals(true);
    ui->editTextCommand->clearFocus();
    QString str_data = ui->editTextCommand->text();
    str_data.replace(QRegExp("\\\\r"),QString("\r"));
    str_data.replace(QRegExp("\\\\n"),QString("\n"));
    str_data.replace(QRegExp("\\\\t"),QString("\t"));
    QByteArray data = tr(qPrintable(str_data)).toLocal8Bit();
//    for(int elem : data) qDebug() << hex << elem;
    this->writeData(data);
    ui->editTextCommand->blockSignals(false);

}

void MainWindow::on_btnSaveLog_clicked()
{
    QDateTime::currentDateTime().toString();
    QLocale l(QLocale::English);
    if(!QDir("./logs").exists()){QDir().mkdir("./logs");}
    QString name(tr("./logs/Log_%1.txt").arg(l.toString(QDateTime::currentDateTime(),"dd_MM_yyyy_hh_mm")));
    QFile log_file(name);
    if(!log_file.open(QIODevice::WriteOnly)){
        ui->textArea->insertPlainText("Can't open file, try again please\n\r");
        return;
    }
    QTextStream out(&log_file);
    out << ui->textArea->toPlainText();
    showStatusMessage("Log is saved into "+log_file.fileName());
    log_file.close();
}

void MainWindow::on_btnUpdateList_clicked()
{
    ui->boxPorts->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->boxPorts->addItem(info.portName());
    this->statusBar()->showMessage("Update COM ports list",1000);
}

void MainWindow::on_pushButton_clicked()
{
    QString str_data("A10.00\r");
    QByteArray data = tr(qPrintable(str_data)).toLocal8Bit();
    writeData(data);
}

void MainWindow::on_pushButton_2_clicked()
{
    QString str_data("A20.00\r");
    QByteArray data = tr(qPrintable(str_data)).toLocal8Bit();
    writeData(data);
}
