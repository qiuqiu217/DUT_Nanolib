#include "plantarpage.h"
#include "ui_plantarpage.h"

PlantarPage::PlantarPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlantarPage)
{
    ui->setupUi(this);
    ui->timestampLabel->setText(0);


}

/**
 * @brief 刷新数据
 */
void PlantarPage::refreshData(unsigned int timestamp, QList<float> frameData){


    ui->plantarHeatmap->refreshData(frameData);
    ui->timestampLabel->setText(QString::number(timestamp));

}

PlantarPage::~PlantarPage()
{
    delete ui;
}



/**
 * @brief 开始采集
 */
void PlantarPage::on_startCollectionButton_clicked()
{
    ui->plantarHeatmap->start();
    connect(dataL,&DataProcess::plantar_frame_ok,this,&PlantarPage::refreshData);
}




