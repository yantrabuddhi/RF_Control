#ifndef RF_DIALOG_H
#define RF_DIALOG_H

#include <QDialog>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTimer>
namespace Ui {
class RF_Dialog;
}

class RF_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit RF_Dialog(QWidget *parent = 0);
    ~RF_Dialog();
    void refresh_ports();
    QString toHex(char txt);

private slots:
    void on_btn_refresh_clicked();

    void on_btn_update_clicked();

    void on_btnOpen_clicked();

private:
    Ui::RF_Dialog *ui;
    QSerialPort pt;
    int read_mode;
    unsigned int buffer[19];
    QTimer tmr;
private slots:
    void on_tmr();
};

#endif // RF_DIALOG_H
