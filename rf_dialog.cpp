#include "rf_dialog.h"
#include "ui_rf_dialog.h"
#include "QMessageBox"
RF_Dialog::RF_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RF_Dialog)
{
    ui->setupUi(this);
    refresh_ports();
}

RF_Dialog::~RF_Dialog()
{
    delete ui;
}

void RF_Dialog::refresh_ports(){
    ui->cmb_ports->clear();
    QList<QSerialPortInfo> ports=QSerialPortInfo::availablePorts();
    QSerialPortInfo inf;
    foreach(inf , ports){
        ui->cmb_ports->addItem(inf.portName());
    }
}

QString RF_Dialog::toHex(char txt){
    QString n=QString::number(txt,16);
    if (n.length()<2)n="0"+n;
    return n;
}

void RF_Dialog::on_btn_refresh_clicked()
{
    QMessageBox::information(this,"hex",toHex(15));
    refresh_ports();
}

void RF_Dialog::on_btn_update_clicked()
{
    if (ui->cmb_ports->currentText().length()<4){
       QMessageBox::warning(this,"Warning","choose a serial port first");
        return;
    }
    QSerialPort pt(ui->cmb_ports->currentText());
    pt.setBaudRate(pt.Baud115200);
    pt.setDataBits(pt.Data8);
    pt.setStopBits(pt.OneStop);
    pt.setParity(pt.NoParity);
    pt.setFlowControl(pt.NoFlowControl);
    if (!pt.open(QIODevice::ReadWrite)){
        QMessageBox::critical(this,"Error","Error opening serial port.");
        return;
    }
    QString toSend="9999";
    toSend+=toHex(ui->spin_f1->value());
    toSend+=toHex(ui->spin_f2->value());
    toSend+=toHex(ui->spin_f3->value());
    toSend+=toHex(ui->spin_f4->value());

    toSend+=toHex(ui->spin_g1->value());
    toSend+=toHex(ui->spin_g2->value());
    toSend+=toHex(ui->spin_g3->value());

    toSend+=toHex(ui->spin_store->value());

    toSend+="1111";
    //QMessageBox::information(this,"debug",toSend);
    pt.write(toSend.toStdString().c_str(),toSend.length());
    pt.waitForBytesWritten(1000);//1 sec timeout
    pt.close();
}
