﻿#include "assetpage.h"
#include "ui_assetpage.h"

#include <QDebug>
#include <QPainter>

#include "blockchain.h"
#include "dialog/assetissuedialog.h"
#include "commondialog.h"
#include "dialog/addtokendialog.h"
#include "showcontentdialog.h"

#define ASSETPAGE_ALLASSETBTN_SELECTED_STYLE     "QToolButton{background-color:rgb(47,94,178);color:white;border:1px solid rgb(47,94,178);border-top-left-radius: 3px;border-bottom-left-radius: 3px;}"
#define ASSETPAGE_ALLASSETBTN_UNSELECTED_STYLE   "QToolButton{background-color:white;color:rgb(47,94,178);border:1px solid rgb(47,94,178);border-top-left-radius: 3px;border-bottom-left-radius: 3px;}"
#define ASSETPAGE_MYASSETBTN_SELECTED_STYLE        "QToolButton{background-color:rgb(47,94,178);color:white;border:1px solid rgb(47,94,178);border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
#define ASSETPAGE_MYASSETBTN_UNSELECTED_STYLE      "QToolButton{background-color:white;color:rgb(47,94,178);border:1px solid rgb(47,94,178);border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"


AssetPage::AssetPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetPage)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(248, 249, 252));
    setPalette(palette);

    connect( CTC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    updateAssetInfo();
    updateMyAsset();

    ui->myAssetWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->myAssetWidget->setFocusPolicy(Qt::NoFocus);
    ui->myAssetWidget->setColumnWidth(0,80);
    ui->myAssetWidget->setColumnWidth(1,80);
    ui->myAssetWidget->setColumnWidth(2,290);
    ui->myAssetWidget->setColumnWidth(3,120);
    ui->myAssetWidget->setColumnWidth(4,100);
    ui->myAssetWidget->setColumnWidth(5,0);
    ui->myAssetWidget->setShowGrid(true);
    ui->myAssetWidget->setFrameShape(QFrame::NoFrame);
    ui->myAssetWidget->setMouseTracking(true);

    ui->myAssetWidget->horizontalHeader()->setVisible(true);


}

AssetPage::~AssetPage()
{
    delete ui;
}

void AssetPage::updateAssetInfo()
{
//    ui->assetInfoTableWidget->setRowCount(0);
//    ui->assetInfoTableWidget->setRowCount(CTC::getInstance()->assetInfoMap.size());
//    foreach (int id, CTC::getInstance()->assetInfoMap.keys())
//    {
//        ui->assetInfoTableWidget->setRowHeight(id,45);

//        AssetInfo info = CTC::getInstance()->assetInfoMap.value(id);

//        ui->assetInfoTableWidget->setItem(id,0,new QTableWidgetItem(QString::number(id)));
//        ui->assetInfoTableWidget->setItem(id,1,new QTableWidgetItem(info.symbol));

//        if( id == 0)
//        {
//            ui->assetInfoTableWidget->setItem(id,2,new QTableWidgetItem(QString::fromLocal8Bit("系统")));
//            ui->assetInfoTableWidget->setItem(id,4,new QTableWidgetItem("-"));
//        }
//        else
//        {
//            ui->assetInfoTableWidget->setItem(id,2,new QTableWidgetItem(info.owner));
//            ui->assetInfoTableWidget->setItem(id,4,new QTableWidgetItem(QString::number(info.currentSupply * 100 / info.maxSupply,'g',16) + "%"));
//        }

//        ui->assetInfoTableWidget->setItem(id,3,new QTableWidgetItem(QString::number(info.maxSupply / info.precision,'g',16)));
//        ui->assetInfoTableWidget->setItem(id,5,new QTableWidgetItem(info.description));

//        for(int j = 0; j < 6; j++)
//        {
//            ui->assetInfoTableWidget->item(id,j)->setTextAlignment(Qt::AlignCenter);
//        }

//    }
}

void AssetPage::updateMyAsset()
{
    ui->myAssetWidget->setRowCount(0);
    ui->myAssetWidget->setRowCount(CTC::getInstance()->ERC20TokenInfoMap.size());

    if(CTC::getInstance()->ERC20TokenInfoMap.keys().size() > 0)
    {
        ui->myAssetWidget->setColumnWidth(5,80);
    }
    else
    {
        ui->myAssetWidget->setColumnWidth(5,0);
    }

    int myAssetCount = 0;
    foreach (QString contractAddress, CTC::getInstance()->ERC20TokenInfoMap.keys())
    {
        ui->myAssetWidget->setRowHeight(myAssetCount,45);

        ERC20TokenInfo info = CTC::getInstance()->ERC20TokenInfoMap.value(contractAddress);

        ui->myAssetWidget->setItem(myAssetCount,0,new QTableWidgetItem(info.name));
        ui->myAssetWidget->setItem(myAssetCount,1,new QTableWidgetItem(info.symbol));
        ui->myAssetWidget->setItem(myAssetCount,2,new QTableWidgetItem(info.contractAddress));
        ui->myAssetWidget->setItem(myAssetCount,3,new QTableWidgetItem(getBigNumberString(info.totalSupply,info.precision)));
        ui->myAssetWidget->setItem(myAssetCount,4,new QTableWidgetItem(getBigNumberString(info.precision,1)));
//        ui->myAssetWidget->setItem(myAssetCount,5,new QTableWidgetItem(QString::fromLocal8Bit("取消关注")));
        ui->myAssetWidget->setItem(myAssetCount,5,new QTableWidgetItem(tr("unfollow")));
        ui->myAssetWidget->item(myAssetCount,5)->setTextColor(QColor(65,205,82));

        for(int j = 0; j < 6; j++)
        {
            ui->myAssetWidget->item(myAssetCount,j)->setTextAlignment(Qt::AlignCenter);
        }

        myAssetCount++;
    }

    ui->myAssetWidget->setRowCount(myAssetCount);
}

void AssetPage::jsonDataUpdated(QString id)
{
}

void AssetPage::on_myAssetWidget_cellClicked(int row, int column)
{
    if( column == 5)
    {
        QString symbol = ui->myAssetWidget->item(row,1)->text();
        QString contractAddress = ui->myAssetWidget->item(row,2)->text();

        CommonDialog tipDialog(CommonDialog::YesOrNo);
//        tipDialog.setText( QString::fromLocal8Bit("是否取消关注 ") + symbol + " ?");
        tipDialog.setText( tr("Sure to unfollow ") + symbol + " ?");

        if( tipDialog.pop() )
        {
            CTC::getInstance()->ERC20TokenInfoMap.remove(contractAddress);
            CTC::getInstance()->configFile->remove("/AddedContractToken/" + contractAddress);

            foreach (QString key, CTC::getInstance()->accountContractTransactionMap.keys())
            {
                if( key.contains(contractAddress))
                {
                    CTC::getInstance()->accountContractTransactionMap.remove(key);
                }
            }

            updateMyAsset();
        }
    }
}

void AssetPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(228,228,228),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248, 249, 252),Qt::SolidPattern));
    painter.drawRect(-1,-1,858,66);
}

void AssetPage::on_addTokenBtn_clicked()
{
    AddTokenDialog addTokenDialog;
    addTokenDialog.pop();
}

void AssetPage::on_myAssetWidget_cellPressed(int row, int column)
{
    if( column == 2 )
    {
        ShowContentDialog showContentDialog( ui->myAssetWidget->item(row, column)->text(),this);

        int x = ui->myAssetWidget->columnViewportPosition(column) + ui->myAssetWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->myAssetWidget->rowViewportPosition(row) - 10 + ui->myAssetWidget->horizontalHeader()->height();

        showContentDialog.move( ui->myAssetWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}
