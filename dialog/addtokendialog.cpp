﻿#include "addtokendialog.h"
#include "ui_addtokendialog.h"

#include <QDebug>

#include "blockchain.h"
#include "rpcthread.h"
#include "../commondialog.h"


AddTokenDialog::AddTokenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTokenDialog)
{
    ui->setupUi(this);

    setParent(CTC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");

    ui->contractAddressLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->contractAddressLineEdit->setTextMargins(8,0,0,0);
    ui->contractAddressLineEdit->setFocus();

    connect( CTC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));
}

AddTokenDialog::~AddTokenDialog()
{
    delete ui;
}

void AddTokenDialog::pop()
{
    move(0,0);
    exec();
}

void AddTokenDialog::on_okBtn_clicked()
{
    if( CTC::getInstance()->addressMap.keys().size() < 1) return;
    QString contractAddress = ui->contractAddressLineEdit->text().simplified();
    if(contractAddress.isEmpty())   return;
    CTC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_state_addtokendialog+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
                                               << CTC::getInstance()->addressMap.keys().at(0) << "state" << ""
                                               ));
}

void AddTokenDialog::on_cancelBtn_clicked()
{
    close();
}

void AddTokenDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_contract_call_offline_state_addtokendialog+") )
    {
        QString result = CTC::getInstance()->jsonDataValue(id);
        if( result.startsWith( "\"error\":"))
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Wrong contract address!"));
            commonDialog.pop();
        }
        else if( result == "\"result\":\"NOT_INITED\"")
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Contract uninitialized!"));
            commonDialog.pop();
        }
        else
        {
            QString contractAddress = id.mid(46);
            if( !CTC::getInstance()->ERC20TokenInfoMap.keys().contains(contractAddress))
            {
                CTC::getInstance()->ERC20TokenInfoMap.insert(contractAddress,ERC20TokenInfo());
            }
            CTC::getInstance()->ERC20TokenInfoMap[contractAddress].contractAddress = contractAddress;
            CTC::getInstance()->configFile->setValue("/AddedContractToken/" + contractAddress,1);
            CTC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+tokenName+" + contractAddress,
                                                       "contract_call_offline",
                                                       QStringList() << contractAddress
                                                                     << CTC::getInstance()->addressMap.keys().at(0) << "tokenName" << ""
                                                       ));

            CTC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+precision+" + contractAddress,
                                                       "contract_call_offline",
                                                       QStringList() << contractAddress
                                                                     << CTC::getInstance()->addressMap.keys().at(0) << "precision" << ""
                                                       ));

            CTC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+admin+" + contractAddress,
                                                       "contract_call_offline",
                                                       QStringList() << contractAddress
                                                                     << CTC::getInstance()->addressMap.keys().at(0) << "admin" << ""
                                                       ));

            CTC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+tokenSymbol+" + contractAddress,
                                                       "contract_call_offline",
                                                       QStringList() << contractAddress
                                                                     << CTC::getInstance()->addressMap.keys().at(0) << "tokenSymbol" << ""
                                                       ));

            CTC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_addtokendialog+totalSupply+" + contractAddress,
                                                       "contract_call_offline",
                                                       QStringList() << contractAddress
                                                                     << CTC::getInstance()->addressMap.keys().at(0) << "totalSupply" << ""
                                                       ));

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Token added!"));
            commonDialog.pop();

            close();
        }

        return;
    }

    if( id.startsWith( "id_contract_call_offline_addtokendialog+") )
    {
        QString result = CTC::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = 40;
            QString func = id.mid( pos, id.indexOf("+" , pos) - pos);
            QString contractAddress = id.mid(id.indexOf("+" , pos) + 1);

            if( func == "tokenName")
            {
                QString tokenName = result.mid(9);
                tokenName.remove('\"');
                CTC::getInstance()->ERC20TokenInfoMap[contractAddress].name = tokenName;
            }
            else if( func == "precision")
            {
                QString precision = result.mid(9);
                precision.remove('\"');
                CTC::getInstance()->ERC20TokenInfoMap[contractAddress].precision = precision.toULongLong();
            }
            else if( func == "admin")
            {
                QString admin = result.mid(9);
                admin.remove('\"');
                CTC::getInstance()->ERC20TokenInfoMap[contractAddress].admin = admin;
            }
            else if( func == "tokenSymbol")
            {
                QString tokenSymbol = result.mid(9);
                tokenSymbol.remove('\"');
                CTC::getInstance()->ERC20TokenInfoMap[contractAddress].symbol = tokenSymbol;
            }
            else if( func == "totalSupply")
            {
                QString totalSupply = result.mid(9);
                totalSupply.remove('\"');
                CTC::getInstance()->ERC20TokenInfoMap[contractAddress].totalSupply = totalSupply.toULongLong();
            }

        }

        return;
    }

}

void AddTokenDialog::on_contractAddressLineEdit_returnPressed()
{
    on_okBtn_clicked();
}
