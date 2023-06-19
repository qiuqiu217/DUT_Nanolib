#include "myplantarheatmap.h"

myPlantarHeatmap::myPlantarHeatmap(QObject *parent)
{
    PlotInit();

}


/**
 * @brief 绘图初始化
 */
void myPlantarHeatmap::PlotInit()
{
//    this->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    this->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    this->axisRect()->setupFullAxesBox(true);
    this->xAxis->setLabel("x");
    this->yAxis->setLabel("y");

    // 配置colorMap
    colorMap = new QCPColorMap(this->xAxis, this->yAxis);
    colorMap->data()->setSize(50, 50); // we want the color map to have nx * ny data points
    colorMap->data()->setRange(QCPRange(-40, 40), QCPRange(-40, 40));//Key Range& Value Range

    // 添加颜色条（色标）
    colorScale = new QCPColorScale(this);
    this->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel("Magnetic Field Strength");
    //配置颜色渐变类型
    colorMap->setGradient(QCPColorGradient::gpSpectrum);

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(this);
    this->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    this->rescaleAxes();

}

/**
 * @brief 开始绘图
 */
void  myPlantarHeatmap::start()
{
    tickTimer = new QTimer();
    tickTimer->start(50);
    connect(tickTimer,&QTimer::timeout,this,&myPlantarHeatmap::tick);


}


/**
 * @brief 定时器回调
 */
void myPlantarHeatmap::tick(){
    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();
    // rescale the key (x) and value (y) axes so the whole color map is visible:
    this->rescaleAxes();
    this->replot();
}


/**
 * @brief 添加数据
 */
void myPlantarHeatmap::refreshData(QList<float> frameData){

        double x,y,z;
        int xindex = 0 ,yindex =  0;
        for(float point:frameData){
            colorMap->data()->cellToCoord(xindex, yindex, &x, &y);
            colorMap->data()->setCell(xindex++,yindex++,point);

        }

}







