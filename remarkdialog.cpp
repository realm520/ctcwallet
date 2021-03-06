#include "remarkdialog.h"
#include "ui_remarkdialog.h"
#include "blockchain.h"
#include <QTextStream>
#include <QDebug>
#include "debug_log.h"

RemarkDialog::RemarkDialog(QString address, QWidget *parent) :
    QDialog(parent),
    name(address),
    ui(new Ui::RemarkDialog)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

//    CTC::getInstance()->appendCurrentDialogVector(this);
    setParent(CTC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");



    ui->okBtn->setText(tr("Ok"));
    ui->cancelBtn->setText(tr("Cancel"));

    ui->remarkLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->remarkLineEdit->setTextMargins(8,0,0,0);

    ui->remarkLineEdit->setFocus();
    DLOG_QT_WALLET_FUNCTION_END;
}

RemarkDialog::~RemarkDialog()
{
    delete ui;
//    CTC::getInstance()->removeCurrentDialogVector(this);
}

void RemarkDialog::pop()
{
//    QEventLoop loop;
//    show();
//    ui->remarkLineEdit->grabKeyboard();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}

void RemarkDialog::on_okBtn_clicked()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    if( !CTC::getInstance()->contactsFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "contact.dat not exist";
        return;
    }

    QByteArray ba = QByteArray::fromBase64( CTC::getInstance()->contactsFile->readAll());
    ba += (name + '=' + ui->remarkLineEdit->text() + ";").toUtf8();
    ba = ba.toBase64();
    CTC::getInstance()->contactsFile->resize(0);
    QTextStream ts(CTC::getInstance()->contactsFile);
    ts << ba;

    CTC::getInstance()->contactsFile->close();

    close();
//    emit accepted();

    DLOG_QT_WALLET_FUNCTION_END;
}

void RemarkDialog::on_cancelBtn_clicked()
{
    close();
//    emit accepted();
}

void RemarkDialog::on_remarkLineEdit_returnPressed()
{
    on_okBtn_clicked();
}

void RemarkDialog::on_remarkLineEdit_textChanged(const QString &arg1)
{
    QString remark = arg1;
    if( remark.contains("=") || remark.contains(";") || remark.contains(" ") )
    {
        remark.remove("=");
        remark.remove(";");
        remark.remove(" ");
        ui->remarkLineEdit->setText( remark);
    }
}
