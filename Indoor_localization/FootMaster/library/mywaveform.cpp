#include "library/mywaveform.h"

MyWaveform::MyWaveform(QObject *parent)
    :m_range(800),m_colorindex(0),m_width(2)
{
    //绘制出基本轮廓
    this->xAxis->setRange(-10,10);   //初始设置为-2到2，后续超出范围再扩大
    this->yAxis->setRange(-10,10);
    this->axisRect()->setupFullAxesBox();//占据全部空间显示
    this->rescaleAxes();//重新缩放
    //设置交互方式为可伸缩
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);




}



/**
 * @brief 开始绘图
 * @param interval 绘图间隔(ms)
 */
void MyWaveform::start(int range)
{
    //根据绘图范围设置定时器间隔,参考：https://www.cnblogs.com/mkmkbj/p/16379319.html
    m_range = range;
    int interval = static_cast<double>(m_range)/1000*13.6;
    qDebug()<<"interval is: "<<interval;
    //设置绘图定时器
    m_tickTimer = new QTimer();
    m_tickTimer->setInterval(interval);
    m_tickTimer->start();
    this->legend->setVisible(true);
    //连接绘图tick
    connect(m_tickTimer,&QTimer::timeout,this,&MyWaveform::tick);
}

/**
 * @brief 停止绘图
 */
void MyWaveform::stop(){

    m_tickTimer->stop();


}

/**
 * @brief 清空绘图
 */
bool MyWaveform::clear(){
    for(QString i:m_curves.keys()){
        deleteCurve(i);
    }
    m_colorindex = 0;
    return true;
}

/**
 * @brief 每一个tick的回调
 */
void MyWaveform::tick()
{

    this->replot();
}



/**
 * @brief 添加一条新的轨迹
 * @param Curvename 新轨迹的名称
 */
bool MyWaveform::addCurve(QString curve_name)
{

    if(!m_curves.contains(curve_name))
    {

        QCPGraph *curve = this->addGraph(this->xAxis,this->yAxis);
        curve->setName(curve_name);
        m_colorindex = m_colorindex%18;
        QPen pen = QPen(baseColors[m_colorindex]);
        m_colorindex++;
        pen.setWidth(m_width);
        curve->setPen(pen);
        m_curves.insert(curve_name,curve);
        return true;
    }
    else
        return false;
}









/**
 * @brief 删除指定的曲线
 * @param curve_name 要删除曲线的名称
 */
bool MyWaveform::deleteCurve(QString curve_name)
{

    if(m_curves.contains(curve_name))
    {

        auto curveit = m_curves.find(curve_name);
        this->removePlottable(curveit.value());
        m_curves.remove(curve_name);
        return true;
    }
    else
    {

        return false;
    }
}




/**
 * @brief 向特定轨迹中添加数据
 * @param curve_name 轨迹名称
 * @param xpos x方向位置
 * @param ypos y方向位置
 */
bool MyWaveform::addData(QString curve_name, double x, double y)
{

    if(m_curves.contains(curve_name))
    {

        auto graphit = m_curves.find(curve_name).value();
        graphit->addData(x,y);
        if(graphit->dataCount()>m_range){
            // 计算要保留的数据点的最小键值
            double minKey = graphit->dataMainKey(graphit->dataCount() - m_range);
            qDebug()<<"data over range "<<graphit->dataCount();
            graphit->data()->removeBefore(minKey);
        }


//        graphit.value()->rescaleValueAxis(true,true);

        this->xAxis->setRange(x, 6000, Qt::AlignRight);
//        this->yAxis->setRange(y, qAbs(y) * 3, Qt::AlignRight);
        return true;
    }
    else
    {

        if(addCurve(curve_name)){
            return true;
        }

        return false;
    }
}

/**
 * @brief 设置曲线可见性
 * @param curves 希望设置可见的曲线名称列表
 */
bool MyWaveform::setVisibilityList(QList<QString> curesToSetList){
    //设置所有的曲线不可见
    for(auto curve:m_curves){

        curve->setVisible(false);
        qDebug()<<"curveName is:"<<curve->name()<<",is visible? "<<curve->visible();
    }
    //设置当前列表中的曲线为可见
    for(auto curve_name:curesToSetList){
        auto curve = m_curves.find(curve_name);
        curve.value()->setVisible(true);
        qDebug()<<"Curve_name is: "<<curve_name;

    }

    return true;
}
