﻿#include "transferconfirmdialog.h"
#include "ui_transferconfirmdialog.h"

#include <QDebug>

#include "blockchain.h"
#include "rpcthread.h"
#include "commondialog.h"

TransferConfirmDialog::TransferConfirmDialog(QString address, QString amount, QString fee, QString remark, QString assetSymbol, QWidget *parent) :
    QDialog(parent),
    address(address),
    amount(amount),
    fee(fee),
    remark(remark),
    yesOrNo(false),
    ui(new Ui::TransferConfirmDialog)
{
    ui->setupUi(this);

    setParent(CTC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");

    connect( CTC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->addressLabel->setText( address);
    ui->amountLabel->setText( "<body><B>" + amount + "</B> " + assetSymbol + "</body>");
    ui->feeLabel->setText( "<body><font color=#409AFF>" + fee + "</font> " + ASSET_NAME +"</body>");
    ui->remarkLabel->setText( remark);
    ui->okBtn->setText(tr("Ok"));
    ui->cancelBtn->setText(tr("Cancel"));
    ui->pwdLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->pwdLineEdit->setPlaceholderText( tr("Enter login password"));
    ui->pwdLineEdit->setTextMargins(8,0,0,0);
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setFocus();
}

TransferConfirmDialog::~TransferConfirmDialog()
{
    delete ui;
}

bool TransferConfirmDialog::pop()
{
    ui->pwdLineEdit->grabKeyboard();
    move(0,0);
    exec();
    return yesOrNo;
}

void TransferConfirmDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_wallet_check_passphrase") )
    {
        QString result = CTC::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            yesOrNo = true;
            close();
        }
        else if( result == "\"result\":false")
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("Wrong password!") + "</font></body>" );
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);
            if( errorMessage == "password too short")
            {
                ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("At least 8 letters!") + "</font></body>" );
            }
            else
            {
                ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + errorMessage + "</font></body>" );
            }
        }
        return;
    }
}

void TransferConfirmDialog::on_okBtn_clicked()
{
    if( ui->pwdLineEdit->text().isEmpty())
    {
        ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" +
                               tr("Please enter the password!") + "</font></body>" );
        return;
    }

    CTC::getInstance()->postRPC(toJsonFormat(
                                     "id_wallet_check_passphrase",
                                     "wallet_check_passphrase",
                                     QStringList() << ui->pwdLineEdit->text()));
}

void TransferConfirmDialog::on_cancelBtn_clicked()
{
    yesOrNo = false;
    close();
}

void TransferConfirmDialog::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void TransferConfirmDialog::on_pwdLineEdit_returnPressed()
{
    on_okBtn_clicked();
}
