#include "trackpage.h"
#include "ui_trackpage.h"

TrackPage::TrackPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackPage),
    m_lastCalculateTimestamp(0),
    m_caliProgressCounter(0)
{
    ui->setupUi(this);
   //初始化算法库
//   algorithm = new Algorithm();
   collectTimer = new QTimer();




    vehiclename = "Track-Test";
    ui->trajectoryMap->addTrack(vehiclename);  //在图中添加曲线
    ui->stopCollectiontButton->setEnabled(false);


}

TrackPage::~TrackPage()
{
    delete ui;
}


/**
 * @brief 开始采集
 */
void TrackPage::on_collectDataButton_clicked()
{

    if(dataL->ImuProcessedSequence.keys().isEmpty()){
        QMessageBox::critical(this,tr("错误"),"采集数据序列为空，请检查！");
        return ;
    }
    //开始进行标定
    getOriginalDistribution();
}


/**
 * @brief 开始绘图
 */
void TrackPage::startPlot(){

    //清空绘图
    ui->trajectoryMap->clear();
    //从点击按钮开始时刻，进行位移计算
    m_lastCalculateTimestamp = dataL->ImuProcessedSequence.lastKey();
    //重新赋值卡尔曼滤波器
    algorithm->KalmanFilterInit();
    algorithm->startZeroSpeedDetection();//开始零速度检测
    //先清除连接
    disconnect(dataL,&DataProcess::imu_frame_ok,this,&TrackPage::refreshData);
    //连接信号
    connect(dataL,&DataProcess::imu_frame_ok,this,&TrackPage::refreshData);
//    //先清除连接
//    disconnect(algorithm,&Algorithm::pos_updated,this,&TrackPage::refreshData);
//    //连接信号
//    connect(algorithm,&Algorithm::pos_updated,this,&TrackPage::refreshData);
    //开始绘图
    ui->trajectoryMap->start(16000);
}


/**
 * @brief 获取初始零点信息 均值&方差
 */
void TrackPage::getOriginalDistribution(){
    progressDialogInit();//进度条初始化
    connect(dataL,&DataProcess::imu_frame_ok,this,&TrackPage::collectCaliData);
}
/**
 * @brief
 */
void TrackPage::collectCaliData(unsigned int timestamp ,DataProcess::SingleFrameIMU frameData){

    //获取加速度向量
    Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
    //获取惯性坐标系->东北天坐标系四元数
    Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
    //获取ENU坐标系下加速度向量
    Vector3 accVector3_ENU = accVector3.rotate(q);
    CaliFrame CaliFrameData = {accVector3_ENU.x,accVector3_ENU.y,accVector3_ENU.z};

    //逐个添加frameData
    ImuCaliSequence.append(CaliFrameData);
}


/**
 * @brief 设置进度条
 */
void TrackPage::progressDialogInit(){

    //设置对话窗口
    caliProgressDialog = new QProgressDialog();
    caliProgressDialog->setWindowTitle("原始标定");//对话框标题
    caliProgressDialog->setLabelText("正在校准中，请保持静止！");//对话框文本
    caliProgressDialog->setCancelButtonText("取消");//设置取消按钮文本
    caliProgressDialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);//去掉对话框的问号
    caliProgressDialog->setRange(1,caliProgressFinishFlag);//设置进度条范围
    caliProgressDialog->setModal(true);//以模态方式弹出对话框
    caliProgressDialog->showNormal();
    caliProgressDialog->setFixedSize(300,150);

    //设置进度条定时器
    calibrateTimer = new QTimer();
    connect(calibrateTimer,&QTimer::timeout,this,&TrackPage::caliTimerTick);
    calibrateTimer->setInterval(caliTimerInterval);
    calibrateTimer->start();

}



/**
 * @brief 初始数据获取计时器 tick
 */
void TrackPage::caliTimerTick(){
    m_caliProgressCounter++;
    caliProgressDialog->setValue(m_caliProgressCounter);
    if(m_caliProgressCounter == caliProgressFinishFlag){
        //进度结束
        m_caliProgressCounter = 0;
        //关闭定时器
        calibrateTimer->stop();
        caliProgressDialog->setAutoReset(true);
        caliProgressDialog->setAutoClose(true);
        //隐藏进度框
        caliProgressDialog->setHidden(true);
        caliProgressDialog->reset();
        //设置按钮组
        ui->collectDataButton->setEnabled(false);
        ui->stopCollectiontButton->setEnabled(true);


        //取消获取
        disconnect(dataL,&DataProcess::imu_frame_ok,this,&TrackPage::collectCaliData);
        //计算分布参数
        calculateDistributionParam();


        startPlot();//开始绘图
    }
}

/**
 * @brief 计算零点时分布的参数 均值&方差
 */
void TrackPage::calculateDistributionParam(){

    int CaliData_length = ImuCaliSequence.length();

    for(auto frameData:ImuCaliSequence){
        CaliData_sum.acc["X"] += frameData.accX;
        CaliData_sum.acc["Y"] += frameData.accY;
        CaliData_sum.acc["Z"] += frameData.accZ;

    }

    CaliData_average.acc["X"] = CaliData_sum.acc["X"]/CaliData_length;
    CaliData_average.acc["Y"] = CaliData_sum.acc["Y"]/CaliData_length;
    CaliData_average.acc["Z"] = CaliData_sum.acc["Z"]/CaliData_length;

    for(auto frameData:ImuCaliSequence){
        CaliData_variance.acc["X"] += qPow(frameData.accX - CaliData_average.acc["X"],2);
        CaliData_variance.acc["Y"] += qPow(frameData.accY - CaliData_average.acc["Y"],2);
        CaliData_variance.acc["Z"] += qPow(frameData.accZ - CaliData_average.acc["Z"],2);
        qDebug()<<QString("frameData.acc[\"X\"] is:%1,CaliData_average.acc[X] is:%2").arg(CaliData_variance.acc["X"]).arg(CaliData_average.acc["X"]);
    }
    qDebug()<<QString("CaliData_variance.acc[\"X\"] is:%1").arg(CaliData_variance.acc["X"]);
    CaliData_variance.acc["X"] /= CaliData_length-1;
    CaliData_variance.acc["Y"] /= CaliData_length-1;
    CaliData_variance.acc["Z"] /= CaliData_length-1;



    qDebug()<<"CaliData_length is:"<<CaliData_length;


    ui->average_value->setText(QString("[%1,%2,%3]")
                               .arg(CaliData_average.acc["X"])
                               .arg(CaliData_average.acc["Y"])
                               .arg(CaliData_average.acc["Z"]));
    ui->variance_value->setText(QString("[%1,%2,%3]")
                                .arg(CaliData_variance.acc["X"])
                                .arg(CaliData_variance.acc["Y"])
                                .arg(CaliData_variance.acc["Z"]));




    ImuCaliSequence.clear();//清除数据
}


/**
 * @brief 停止采集
 */
void TrackPage::on_stopCollectiontButton_clicked()
{
    //停止绘图
    ui->trajectoryMap->stop();
    //清除连接
    disconnect(dataL,&DataProcess::imu_frame_ok,this,&TrackPage::refreshData);

    ui->collectDataButton->setEnabled(true);
    ui->stopCollectiontButton->setEnabled(false);
}

//void TrackPage::refreshData(unsigned int timestamp_current){
//    Vector3 pos = algorithm->ResultSequence.find(timestamp_current).value().pos;
//    qDebug()<<QString("pos.x is:%1,pos.y is:%2,pos.z is:%3").arg(pos.x).arg(pos.y).arg(pos.z);
//    ui->trajectoryMap->addData(vehiclename,pos.x,pos.y);
//}





void TrackPage::refreshData(unsigned int timestamp_current,DataProcess::SingleFrameIMU frameData){
    //获取加速度向量
    Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
    //获取惯性坐标系->东北天坐标系四元数
    Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
    //获取ENU坐标系下加速度向量
    Vector3 accVector3_ENU = accVector3.rotate(q);
//    accVector3_ENU.z = accVector3_ENU.z - CaliData_average.acc["Z"];//减去重力加速度
    accVector3_ENU.z = accVector3_ENU.z - 10;//减去重力加速度
    //获取dt
    double dt_ms = (static_cast<double>(timestamp_current)-static_cast<double>(m_lastCalculateTimestamp))/1000;
    //更新计算时间戳
    m_lastCalculateTimestamp = timestamp_current;

    //使用卡尔曼滤波器更新位置
    algorithm->updatePosition(accVector3_ENU,dt_ms);
    Vector3 pos_ENU_Vector3(algorithm->positionFilter_X.x,algorithm->positionFilter_Y.x,algorithm->positionFilter_Z.x);//将当前坐标系的坐标输入，并构建对象

    qDebug("accX-Y-Z is: %8.4lf,%8.4lf,%8.4lf ,vel_X-Y-Z is: %8.3lf,%8.3lf,%8.3lf,pos_X-Y-Z is: %8.3lf,%8.3lf,%8.3lf, dt_ms is: %8.3lf, sys_time is:%8u",
               algorithm->accFilter_X.x,algorithm->accFilter_Y.x,algorithm->accFilter_Z.x,
               algorithm->velocityFilter_X.x,algorithm->velocityFilter_Y.x,algorithm->velocityFilter_Z.x,
               pos_ENU_Vector3.x,pos_ENU_Vector3.y,pos_ENU_Vector3.z,dt_ms,dataL->btDevice->m_msTime);

    ui->trajectoryMap->addData(vehiclename,pos_ENU_Vector3.x,pos_ENU_Vector3.y);
}



//void TrackPage::refreshPlot(){
//    if(dataL->ImuProcessedSequence.isEmpty()){
//        return ;
//    }

//    QMap<unsigned int, DataProcess::SingleFrameIMU> ImuSequenceLocal =  dataL->ImuProcessedSequence;//获取全部的imusequence

//    unsigned int lastCalculateTimestamp = lastPlotTimestamp; //每次绘图可能对应多次计算

//    QVector<double> positionENU_X;
//    QVector<double> positionENU_Y;

//    QMap<unsigned int, DataProcess::SingleFrameIMU>::const_iterator i = ImuSequenceLocal.lowerBound(lastPlotTimestamp);

//    lastCalculateTimestamp = i.key();
//    while (i != ImuSequenceLocal.constEnd()) {


//        Vector3 accVector3(i.value().acc["X"], i.value().acc["Y"], i.value().acc["Z"]);
//        Quaternion q(i.value().quaternion["q0"],i.value().quaternion["q1"],i.value().quaternion["q2"],i.value().quaternion["q3"]);//获取当前得到的四元数，惯性坐标系->东北天坐标系

//        Vector3 accVector3_ENU = accVector3.rotate(q);//东北天坐标系下的加速度向量
//        accVector3_ENU.z = accVector3_ENU.z-9.8;//减去重力加速度,得到相对东北天坐标系的绝对加速度
//        double t_current = i.key();
//        double t_last = lastCalculateTimestamp;
//        double dt_ms = (t_current - t_last)/1000;
////        qDebug()<<"i.key(): "<< i.key() << "  lastCalculateTimestamp : "<< lastCalculateTimestamp<<"  dt_ms : "<< dt_ms  ;
//        algorithm->updatePositionCurrent(accVector3_ENU,dt_ms);//使用卡尔曼滤波得到更新过的速度和位移向量，结果保存在algorithm对象中
//        Vector3 pos_ENU_Vector3(algorithm->positionFiltered_currentVec.x,algorithm->positionFiltered_currentVec.y,algorithm->positionFiltered_currentVec.z);//将当前坐标系的坐标输入，并构建对象

//        qDebug("accX-accY-accZ is: %lf,%lf,%lf,velocity_X-Y-Z is: %lf,%lf,%lf,pos_X-Y-Z is: %lf,%lf,%lf, dt_ms is: %lf",\
//               accVector3_ENU.x,accVector3_ENU.y,accVector3_ENU.z,
//               algorithm->velocityFiltered_currentVec.x,algorithm->velocityFiltered_currentVec.y,algorithm->velocityFiltered_currentVec.z,
//               pos_ENU_Vector3.x,pos_ENU_Vector3.y,pos_ENU_Vector3.z,dt_ms);



//        positionENU_X.append(pos_ENU_Vector3.x);
//        positionENU_Y.append(pos_ENU_Vector3.y);

//        lastCalculateTimestamp = i.key();
//         ui->trajectoryMap->addData(vehiclename,pos_ENU_Vector3.x,pos_ENU_Vector3.y);
////        ui->trajectoryMap->addData(vehiclename,accVector3_ENU.x,accVector3_ENU.y);
////        ui->trajectoryMap->addData(vehiclename,algorithm->velocityFiltered_currentVec.x,algorithm->velocityFiltered_currentVec.y);
//        ++i;
//    }



//    ui->trajectoryMap->graph(0)->addData(positionENU_X,positionENU_Y);
//    ui->trajectoryMap->xAxis->setRange(positionENU_X.last(),800,Qt::AlignCenter);
//    ui->trajectoryMap->yAxis->setRange(positionENU_Y.last(),800,Qt::AlignCenter);

//    ui->trajectoryMap->replot();




//    m_lastCalculateTimestamp = ImuSequenceLocal.lastKey();
////    qDebug()<<"轨迹重绘：X_last is:"<<positionENU_X.last()<<"Y_last is:"<<positionENU_Y.last();
//}










//void TrackPage::refreshPlot(){
//    if(dataL->ImuProcessedSequence.isEmpty()){
//        return ;
//    }

//    QMap<unsigned int, DataProcess::SingleFrameIMU> ImuSequenceLocal =  dataL->ImuProcessedSequence;//获取全部的imusequence

//    unsigned int lastCalculateTimestamp = lastPlotTimestamp;
//    QVector<Vector3> velocityFiltered_ENU;
//    QVector<Vector3> positionFiltered_ENU;


//    QVector<double> positionENU_X;
//    QVector<double> positionENU_Y;

//    QMap<unsigned int, DataProcess::SingleFrameIMU>::const_iterator i = ImuSequenceLocal.lowerBound(lastPlotTimestamp);
////    if(){
////        qDebug()<<"i is empty";
////    }

//    while (i != ImuSequenceLocal.constEnd()) {


//        Vector3 accVector3(i.value().acc["X"], i.value().acc["Y"], i.value().acc["Z"]);
//        Quaternion q(i.value().quaternion["q0"],i.value().quaternion["q1"],i.value().quaternion["q2"],i.value().quaternion["q3"]);//获取当前得到的四元数

//        Vector3 accVector3_ENU = accVector3.rotate(q);//东北天坐标系下的加速度向量
//        accVector3_ENU.z = accVector3_ENU.z-9.8;//减去重力加速度

//        algorithm->updatePositionCurrent(accVector3_ENU,(i.key()-lastCalculateTimestamp)/1000);//使用卡尔曼滤波得到更新过的速度和位移向量，结果保存在algorithm对象中
//        Vector3 pos_ENU_Vector3(algorithm->positionFiltered_current.x,algorithm->positionFiltered_current.y,algorithm->positionFiltered_current.z);//将当前坐标系的坐标输入，并构建对象
//        qDebug("q0-q1-q2-q3 is: %lf,%lf,%lf,%lf,accX-accY-accZ is: %lf,%lf,%lf,velocity_X-Y-Z is: %lf,%lf,%lf,pos_X-Y-Z is: %lf,%lf,%lf",\
//               q.w,q.x,q.y,q.z,
//               accVector3_ENU.x,accVector3_ENU.y,accVector3_ENU.z,
//               algorithm->velocityFiltered_current.x,algorithm->velocityFiltered_current.y,algorithm->velocityFiltered_current.z,
//               pos_ENU_Vector3.x,pos_ENU_Vector3.y,pos_ENU_Vector3.z);

//        positionENU_X.append(pos_ENU_Vector3.x);
//        positionENU_Y.append(pos_ENU_Vector3.y);

//        lastCalculateTimestamp = i.key();
//        ++i;
//    }



//    ui->trajectoryMap->graph(0)->addData(positionENU_X,positionENU_Y);
//    ui->trajectoryMap->xAxis->setRange(positionENU_X.last(),800,Qt::AlignCenter);
//    ui->trajectoryMap->yAxis->setRange(positionENU_Y.last(),800,Qt::AlignCenter);

//    ui->trajectoryMap->replot();
//    lastPlotTimestamp = dataL->ImuProcessedSequence.lastKey();
//    qDebug()<<"轨迹重绘：X_last is:"<<positionENU_X.last()<<"Y_last is:"<<positionENU_Y.last();
//}












