#include "rf_dialog.h"
#include "ui_rf_dialog.h"
#include "QMessageBox"
RF_Dialog::RF_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RF_Dialog)
{
    ui->setupUi(this);
    refresh_ports();
    tmr.setSingleShot(false);
    connect(&tmr,SIGNAL(timeout()),this,SLOT(on_tmr()));
}

RF_Dialog::~RF_Dialog()
{
    if (pt.isOpen())pt.close();
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
    //pt.close();
}

void RF_Dialog::on_btnOpen_clicked()
{
    if (ui->cmb_ports->currentText().length()<4){
       QMessageBox::warning(this,"Warning","choose a serial port first");
        return;
    }

    //QSerialPort pt(ui->cmb_ports->currentText());
    if ((!pt.isOpen())||(pt.portName()!=ui->cmb_ports->currentText())){
        if (pt.isOpen())pt.close();
        pt.setPortName(ui->cmb_ports->currentText());

        pt.setBaudRate(pt.Baud115200);
        pt.setDataBits(pt.Data8);
        pt.setStopBits(pt.OneStop);
        pt.setParity(pt.NoParity);
        pt.setFlowControl(pt.NoFlowControl);

        if (!pt.open(QIODevice::ReadWrite)){
            QMessageBox::critical(this,"Error","Error opening serial port.");
            return;
        }
        read_mode=0;
        tmr.start(50);
    }
}

void RF_Dialog::on_tmr()//19 byte packet
{
    if (!pt.isOpen()){
        tmr.stop();
        return;
    }
    //read comm port for values
    unsigned char ch;
    static int counter=0;
    while (pt.bytesAvailable()>0){
        pt.read((char*)&ch,1);
        if ((ch=0xee) && (read_mode==0)){read_mode=1;buffer[0]=ch;counter=0;continue;}
        if (read_mode){
            counter++;
            buffer[counter]=ch;
            if (buffer[1]!=0xee){read_mode=0;continue;}
            if (counter<18){continue;}
            //process buffer
            if ((buffer[17]!=0x88) || (buffer[18]!=0x88)){read_mode=0;continue;}
            //calc checksum
            unsigned char chk=buffer[2];
            for (int z=3;z<16;z++){
                chk+=buffer[z];
            }
            if (chk!=buffer[16]){
                //checksum failure
                read_mode=0;
                continue;
            }
            //all passed, feed to GUI
            float freq=buffer[2]*1000.0+buffer[3]*100+buffer[4]*10+buffer[5]+float(buffer[6])/10.0;
            float attn1=buffer[7]+float(buffer[8])/10.0;
            float attn2=buffer[9]+float(buffer[10])/10.0;
            float attn3=buffer[11]+float(buffer[12])/10.0;
            bool pll=buffer[13]!=0;
            char mode=buffer[14];
            ui->lblReadFreq->setText(QString::number(freq));
            ui->lblAttn1->setText(QString::number(attn1));
            ui->lblAttn2->setText(QString::number(attn2));
            ui->lblAttn3->setText(QString::number(attn3));
            QString plls=(pll)?"on":"off";
            ui->lblPLL->setText(plls);
            QString md="Keypad";
            if (mode==1)md="RS232";
            if (mode==2)md="Ethernet";
            ui->lblMode->setText(md);
            read_mode=0;
        }
    }
}
