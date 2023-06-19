#include "qtrack2d.h"

QTrack2D::QTrack2D(QObject *parent )
    :m_range(800),m_colorindex(0),m_width(2)

{
    this->legend->setVisible(true);
    this->xAxis->setRange(-10,10);   //初始设置为-2到2，后续超出范围再扩大
    this->yAxis->setRange(-10,10);




    this->axisRect()->setupFullAxesBox();
    this->rescaleAxes();
    this->xAxis->setScaleRatio(this->yAxis, 1);

    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

}





/**
 * @brief 开始绘图
 * @param range 绘图点数目
 */
void QTrack2D::start(int range)
{
    //根据绘图范围设置定时器间隔,参考：https://www.cnblogs.com/mkmkbj/p/1637902319.html
    m_range = range;
    int interval = static_cast<double>(m_range)/10000*50;
    qDebug()<<"interval is: "<<interval;

    //清除原先数据
    this->clear();

    //设置绘图定时器
    m_tickTimer = new QTimer();
    m_tickTimer->setInterval(interval);
    m_tickTimer->start();
    //连接绘图tick
    connect(m_tickTimer,&QTimer::timeout,this,&QTrack2D::tick);
}

/**
 * @brief 停止绘图
 */
void QTrack2D::stop(){

    m_tickTimer->stop();

}


/**
 * @brief 每一个tick的回调
 */
void QTrack2D::tick()
{

    this->replot();
}



/**
 * @brief 添加一条新的轨迹
 * @param trackname 新轨迹的名称
 */
bool QTrack2D::addTrack(QString trackname)
{

    if(!m_tracks.contains(trackname))
    {

        QPointer<QCPCurve> curve = new QCPCurve(this->xAxis,this->yAxis);
        curve->setName(trackname);
        m_colorindex = m_colorindex%18;
//        qDebug()<<"pen index is"<<m_colorindex;
        QPen pen = QPen(baseColors[m_colorindex]);
        m_colorindex++;

        pen.setWidth(m_width);
        curve->setPen(pen);
        m_tracks.insert(trackname,curve);
        return true;
    }
    else
        return false;
}









/**
 * @brief 删除指定的轨迹
 * @param trackname 要删除轨迹的名称
 */
bool QTrack2D::deleteTrack(QString trackname)
{

    if(m_tracks.contains(trackname))
    {

        auto curveit = m_tracks.find(trackname);
        this->removePlottable(curveit.value());
        m_tracks.remove(trackname);
        return true;
    }
    else
    {

        return false;
    }
}




/**
 * @brief 向特定轨迹中添加数据
 * @param trackname 轨迹名称
 * @param xpos x方向位置
 * @param ypos y方向位置
 */
bool QTrack2D::addData(QString trackname, double xpos, double ypos)
{

    if(m_tracks.contains(trackname))
    {
        auto graphit = m_tracks.find(trackname);
        graphit.value()->addData(xpos,ypos);
        if(graphit.value().data()->dataCount()>m_range)
            graphit.value()->removeDataBefore(m_range);

        graphit.value()->rescaleValueAxis(true,true);
        return true;
    }
    else
    {

        if(addTrack(trackname))
            return true;
        return false;
    }
}

/**
 * @brief 清空绘图
 */
bool QTrack2D::clear(){
    for(QString i:m_tracks.keys()){
        deleteTrack(i);
    }
    m_colorindex = 0;
    return true;
}









