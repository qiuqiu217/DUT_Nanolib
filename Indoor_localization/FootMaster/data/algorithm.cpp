#include "algorithm.h"

Algorithm::Algorithm(QObject *parent)
    : QObject{parent},
      q_original_ENU(1,0,0,0),
     //坐标转换四元数初始化
      q_current_original(1,0,0,0),
      velocityFiltered_currentVec(0,0,0),

     //得到的当前坐标系下的向量
      positionFiltered_currentVec(0,0,0),
      m_periodCounter(0),
      m_LastWindowEnd_timestamp(0),
      plantarUnderThresholdFlag(false),
      plantar_continuous_counter(0),
      imu_above_threshold_counter(0),
      imu_frame_counter(0),
      m_lastWholeFrameDetected_timestamp(0),
      m_lastPlantarDetect_timestamp(0),
      m_velocity(0),
      m_position(0),
      m_lastCalculateTimestamp(0)
{
    connect(dataL,&DataProcess::imu_frame_ok,this,&Algorithm::ImuDataPreprocess);
    connect(dataL,&DataProcess::plantar_frame_ok,this,&Algorithm::PlantarDataPreprocess);

    connect(this,&Algorithm::imu_calculate_ok,this,&Algorithm::imu_detected_result_process);

    //开启数据更新定时器
    dataUpdateTimer = new QTimer();
    dataUpdateTimer->setInterval(c_update_interval);
    //将plantar检测信号连接到帧同步函数
//    connect(dataUpdateTimer,&QTimer::timeout,this,&Algorithm::frameSync);
    //连接零速度检测槽函数
    connect(this,&Algorithm::ZeroSpeed_detect_ok,this,&Algorithm::on_ZeroSpeed_detected);



}


/**
 * @brief 获取原始时刻坐标转换到东北天坐标系下的四元数向量
 * @param accVector 标定时期获取到的加速度向量
 * @param magVector 标定时期获取到的磁场向量
 */
void Algorithm::setOriginaToENUQuaternion(QVector<Vector3> accVector, QVector<Vector3> magVector){
    Vector3 accFiltered = getFilteredAcceleration(accVector);
    Vector3 magFiltered = getFilteredMag(magVector);
    q_original_ENU = calculateOriginalQuaternion(accFiltered,magFiltered);
}

/**
 * @brief 由欧拉角，获取当前时刻到原始时刻坐标变换的四元数向量
 * @param roll 翻滚角
 * @param pitch 俯仰角
 * @param yaw 偏航角
 */
void Algorithm::setCurrentToOriginalQuaternion(double roll, double pitch, double yaw){
    Quaternion q(1,0,0,0);
       double cy = cos(yaw * 0.5);
       double sy = sin(yaw * 0.5);
       double cp = cos(pitch * 0.5);
       double sp = sin(pitch * 0.5);
       double cr = cos(roll * 0.5);
       double sr = sin(roll * 0.5);

       q.w = cr * cp * cy + sr * sp * sy;
       q.x = sr * cp * cy - cr * sp * sy;
       q.y = cr * sp * cy + sr * cp * sy;
       q.z = cr * cp * sy - sr * sp * cy;

       q_current_original = q;
}



/**
 * @brief 卡尔曼滤波器初始化
 */
void Algorithm::KalmanFilterInit(){
    //初始化操作
    accFilter_X .x = 0;
    accFilter_Y .x = 0;
    accFilter_Z .x = 0;
    gyroFilter_X.x = 0;
    gyroFilter_Y.x = 0;
    gyroFilter_Z.x = 0;
    angleFilter_X.x  = 0;
    angleFilter_Y.x  = 0;
    angleFilter_Z.x  = 0;
    magFilter_X.x = 0;
    magFilter_Y.x = 0;
    magFilter_Z.x = 0;
    quaternionFilter_0.x = 0;
    quaternionFilter_1.x = 0;
    quaternionFilter_2.x = 0;
    quaternionFilter_3.x = 0;
    heightFilter.x = 0;
    atomsFilter .x = 0;


    velocityFilter_X.x = 0;
    positionFilter_X.x = 0;
    velocityFilter_Y.x = 0;
    positionFilter_Y.x = 0;
    velocityFilter_Z.x = 0;
    positionFilter_Z.x = 0;

}





/**
 * @brief 计算当前坐标系下的位移向量
 * @param acceleration 惯性传感器坐标系下的加速度向量
 */
void Algorithm::updatePosition(Vector3 acceleration,double dt){

    double accX,accY,accZ;


    //更新加速度-X轴
    accX = accFilter_X.update(acceleration.x);
    //更新加速度-Y轴
    accY = accFilter_Y.update(acceleration.y);
    //更新加速度-z轴
    accZ = accFilter_Z.update(acceleration.z);

//    directZeroing(accX,accY,accZ);

    positionFilter_X.update(positionFilter_X.x + velocityFilter_X.x * dt + 0.5 * accX * dt * dt);
    // 计算位移-Y轴
    positionFilter_Y.update(positionFilter_Y.x + velocityFilter_Y.x * dt+ 0.5 * accY * dt * dt);
    // 计算位移-Z轴
    positionFilter_Z.update(positionFilter_Z.x + velocityFilter_Z.x * dt + 0.5 * accZ * dt * dt);

    // 计算速度-X轴
    velocityFilter_X.update(velocityFilter_X.x + accX * dt);
    // 计算速度-Y轴
    velocityFilter_Y.update(velocityFilter_Y.x + accY * dt);
    // 计算速度-Z轴
    velocityFilter_Z.update(velocityFilter_Z.x + accZ * dt);

    ZUPT_ThresholdDetection(accFilter_X.x,accFilter_Y.x,accFilter_Z.x,
                            velocityFilter_X.x,velocityFilter_Y.x,velocityFilter_Z.x,
                            static_cast<unsigned int>(1000*dt));


//    //将卡尔曼滤波器状态变量存入Vector3，以支持四元数坐标变换运算
//    velocityFiltered_currentVec.x = velocityFilter_X.x;
//    positionFiltered_currentVec.x = positionFilter_X.x;

//    velocityFiltered_currentVec.y = velocityFilter_Y.x;
//    positionFiltered_currentVec.y = positionFilter_Y.x;

//    velocityFiltered_currentVec.z = velocityFilter_Z.x;
//    positionFiltered_currentVec.z = positionFilter_Z.x;
}

/**
 * @brief Imu数据预处理
 */
void Algorithm::ImuDataPreprocess(unsigned int timestamp, DataProcess::SingleFrameIMU frameData){

//    qDebug()<<"inter the imuDataPreprocess,timestamp is:"<<timestamp;
    ImuSequence[timestamp] = frameData;
    //获取加速度向量
    Vector3 accVector3 = Vector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
    //获取惯性坐标系->东北天坐标系四元数
    Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
    //获取ENU坐标系下加速度向量
    Vector3 accVector3_ENU = accVector3.rotate(q);
    accVector3_ENU.z = accVector3_ENU.z - 10;//减去重力加速度

    //将acc的值插入结果序列
    ResultFrame result = ResultFrame();
    result.acc = accVector3_ENU;
    ResultSequence.insert(timestamp,result);

    imu_frame_counter++;
    if(m_lastWholeFrameDetected_timestamp == 0){
        m_lastWholeFrameDetected_timestamp = timestamp;
    }

    gyroUnderThreshold(timestamp,Vector3(frameData.gyro["X"],frameData.gyro["Y"],frameData.gyro["Z"]));
//    accVarUnderThreshold(timestamp);
    accUnderThreshold(timestamp,accVector3_ENU);


}

/**
 * @brief Imu数据预处理
 */
void Algorithm::process(unsigned int timestamp, DataProcess::SingleFrameIMU frameData){


}






/**
 * @brief Plantar数据预处理
 */
void Algorithm::PlantarDataPreprocess(unsigned int timestamp, QList<float> frameData){
    PlantarSequence[timestamp] = frameData;
    if(m_lastPlantarDetect_timestamp == 0){
        m_lastPlantarDetect_timestamp = timestamp;
    }
    plantar_ZeroSpeedDetection(timestamp,frameData);
}


/**
 * @brief 获取东北天坐标系下的初始坐标
 * @param acceleration 静止下测量到的加速度向量
 * @param magneticField 初始状态的地磁向量
 * @return  东北天坐标系下的四元数向量
 */
Quaternion Algorithm::calculateOriginalQuaternion(const Vector3& acceleration, const Vector3& magneticField) {
    Vector3 up = - acceleration.normalize();
    Vector3 north = up.cross(magneticField).normalize();
    Vector3 east = north.cross(up).normalize();

    double m00 = east.x;
    double m01 = north.x;
    double m02 = up.x;
    double m10 = east.y;
    double m11 = north.y;
    double m12 = up.y;
    double m20 = east.z;
    double m21 = north.z;
    double m22 = up.z;

    double qw = std::sqrt(1 + m00 + m11 + m22) / 2;
    double qx = (m21 - m12) / (4 * qw);
    double qy = (m02 - m20) / (4 * qw);
    double qz = (m10 - m01) / (4 * qw);

    return Quaternion(qw, qx, qy, qz);
}

//使用案例
//int main() {
//    // Example usage
//    Vector3 acceleration(0, 0, -9.81);
//    Vector3 magneticField(1.0e-5, 0.2e-5, 0.5e-5);

//    Quaternion q = getRotationQuaternion(acceleration, magneticField);

//    std::cout << "Quaternion: (" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")" << std::endl;

//    return 0;
//}

/**
 * @brief 直接置零法，基于阈值检测，小于阈值直接置零
 * @param accX
 * @param accY
 * @param accZ
 */
void Algorithm::directZeroing(double &accX,double &accY,double &accZ){
    if(qAbs(accX) <= 0.02){
        accX = 0;
    }
    if(qAbs(accY) <= 0.02){
        accY = 0;
    }
    if(qAbs(accZ) <= 0.02){
        accZ = 0;
    }
}

/**
 * @brief 基于单周期阈值检测的零速度更新法
 * @param acc
 * @param velocity
 */
void Algorithm::ZUPT_ThresholdDetection(double &accX,double &accY,double &accZ,double &velocityX,double &velocityY,double &velocityZ,unsigned int dt){

    double threshold = 0.2;

    if( qAbs(accX)<threshold && qAbs(accY)<threshold){
        //如果X、Y轴加速度均小于阈值，添加进时间计数器
        m_periodCounter += dt;
        qDebug("under threshold! accX is:%6.3lf,accY is:%6.3lf,accZ is:%6.3lf,m_periodCounter is: %u,dt is: %u",accX,accY,accZ,m_periodCounter,dt);
    }
    else{
        //如果检测到大于阈值，则将时间计数器置零
        m_periodCounter = 0;
        qDebug("over threshold! accX is:%6.3lf,accY is:%6.3lf,accZ is:%6.3lf",accX,accY,accZ);
    }

    if(m_periodCounter>100){
        m_periodCounter = 0;
        //accX = accY = accZ = 0;
        accX = accZ = 0;
        accY = 0.005;
        velocityX = velocityY  = velocityZ = 0;
        qDebug()<<"检测到零速度区间，置零";
    }else{
        qDebug()<<"m_periodCounter is :"<<m_periodCounter;
    }
}

/**
 * @brief 基于IMU的综合检测法
 *        多条件复合作为检测条件：
 *        1）加速度模长、
 *        2）滑动窗口内加速度方差、
 *        3）角速度模长检测、
 */
void Algorithm::imu_ComprehensiveZeroDetection(){

}

/**
 * @brief 加速度模长检测
 */
void Algorithm::accUnderThreshold(unsigned int timestamp,Vector3 accVector){
    double accM = accVector.norm();
//    qDebug()<<"accM value is:"<<accM<<"timestamp is:"<<timestamp;
    accM_sequence.insert(timestamp,accM);//插入序列
    if(accM <= c_accM_Threshold){
        //加速度模长小于阈值
        if(m_DetectedResultSequence_imu.contains(timestamp)){
            //当前时间戳对应的标志位组存在
            m_DetectedResultSequence_imu.find(timestamp).value().set_accM_Flag(true);
        }else{
            //当前时间戳对应的标志位组不存在
            ImuUnderThresholdFlags result = ImuUnderThresholdFlags();
            result.set_accM_Flag(true);
            //插入
            m_DetectedResultSequence_imu.insert(timestamp,result);
        }

    }else{
        //加速度模长大于阈值
        if(m_DetectedResultSequence_imu.contains(timestamp)){
            //当前时间戳对应的标志位组存在
            m_DetectedResultSequence_imu.find(timestamp).value().set_accM_Flag(false);
        }else{
            //当前时间戳对应的标志位组不存在
            ImuUnderThresholdFlags result = ImuUnderThresholdFlags();
            result.set_accM_Flag(false);
            //插入
            m_DetectedResultSequence_imu.insert(timestamp,result);
        }
    }
    emit imu_calculate_ok(timestamp);
}
/**
 * @brief 加速度滑动窗口方差检测法
 */
void Algorithm::accVarUnderThreshold(unsigned int timestamp){
    if(imu_frame_counter<=c_accVar_WindowLength||accM_sequence.count()<=c_accVar_WindowLength){
        return;
    }

    if((timestamp - m_LastWindowEnd_timestamp) > c_accVar_SlidingStep){
        //差距大于滑动窗口步长
        //计算当前窗口内的方差值
        QMap<unsigned int,double>::const_iterator i = accM_sequence.find(timestamp);//获取最后一个元素
        double accM_SUM = 0,accM_AVERAGE = 0,accM_VAR = 0;

        for(int counter = 0;counter<c_accVar_WindowLength;counter++,i--){
            accM_SUM += i.value();
        }
        accM_AVERAGE = accM_SUM/c_accVar_WindowLength;
        i = accM_sequence.find(timestamp);//获取最后一个元素
        for(int counter = 0;counter<c_accVar_WindowLength;counter++,i--){
            accM_VAR += qPow((i.value() - accM_AVERAGE),2);
            m_DetectedResultSequence_imu.find(i.key()).value().set_imuFrameDetected_Flag(true);

        }
        accM_VAR /= c_accVar_WindowLength;

        if(accM_VAR < c_accVar_Threshold){
            //方差小于阈值
            //将时间窗口内的所有标志位都置位
            i = accM_sequence.find(timestamp);//获取最后一个元素
            for(int counter = 0;counter<c_accVar_WindowLength;counter++,i--){
                if(m_DetectedResultSequence_imu.contains(i.key())){
                    //当前时间戳对应的标志位组存在
                    m_DetectedResultSequence_imu.find(i.key()).value().set_accVar_Flag(true);
                }else{
                    //当前时间戳对应的标志位组不存在
                    ImuUnderThresholdFlags result = ImuUnderThresholdFlags();
                    result.set_accVar_Flag(true);
                    //插入
                    m_DetectedResultSequence_imu.insert(i.key(),result);
                }
            }
        }
        m_LastWindowEnd_timestamp = timestamp;
//        qDebug()<<QString("timestamp is:%1 ,m_LastWindowEnd_timestamp is: %2,accVar is:%3")
//                        .arg(timestamp).arg(m_LastWindowEnd_timestamp).arg(accM_VAR);
//        emit imu_calculate_ok(timestamp);//发送信号，代表这个timestamp之前的数据的统计量全部计算完毕
    }
}



/**
 * @brief 角速度模长检测
 */
void Algorithm::gyroUnderThreshold(unsigned int timestamp,Vector3 gyroVector){

    if(gyroVector.norm() <= c_gyroM_Threshold){
        //角速度模长小于阈值
        if(m_DetectedResultSequence_imu.contains(timestamp)){
            //当前时间戳对应的标志位组存在
            m_DetectedResultSequence_imu.find(timestamp).value().set_gyroM_Flag(true);
        }else{
            //当前时间戳对应的标志位组不存在
            ImuUnderThresholdFlags result = ImuUnderThresholdFlags();
            result.set_gyroM_Flag(true);
            //插入
            m_DetectedResultSequence_imu.insert(timestamp,result);
        }
//        qDebug()<<"under the threshold " <<"gyroVector.norm is:"<<gyroVector.norm()
//                <<"gyroM is:"<<m_DetectedResultSequence_imu.find(timestamp).value().gyroM
//                <<"timestamp is:"<<timestamp;
    }else{
        //角速度模长大于阈值
//        qDebug()<<"above the threshold "<<"gyroVector.norm is:"<<gyroVector.norm();
        if(m_DetectedResultSequence_imu.contains(timestamp)){
            //当前时间戳对应的标志位组存在
            m_DetectedResultSequence_imu.find(timestamp).value().set_gyroM_Flag(false);
        }else{
            //当前时间戳对应的标志位组不存在
            ImuUnderThresholdFlags result = ImuUnderThresholdFlags();
            result.set_gyroM_Flag(false);
            //插入
            m_DetectedResultSequence_imu.insert(timestamp,result);
        }
//        qDebug()<<"under the threshold " <<"gyroVector.norm is:"<<gyroVector.norm()<<"timestamp is:"<<timestamp;
    }
}

/**
 * @brief imu detected 数据处理
 */
void Algorithm::imu_detected_result_process(unsigned int timestamp)
{
    ImuUnderThresholdFlags result = m_DetectedResultSequence_imu.find(timestamp).value();//获取迭代器，是时间窗口的最后一个时间戳
    if(result.accM && result.gyroM){
        //全部标志位都为真，imu判断为真
        result.set_imuTotal_Flag(true);
        //检测为零速度区间
        emit ZeroSpeed_detect_ok(timestamp,true);
        qDebug()<<QString("all detector is under threshold,timestamp is:%1").arg(timestamp);

    }else{
        //至少一个不为真，imu判断为假
        result.set_imuTotal_Flag(false);
        //检测为零速度区间
        emit ZeroSpeed_detect_ok(timestamp,false);
        qDebug()<<QString("above the threshold, result.accM is: %1,result.gyroM is: %2,timestamp is:%3")
                                    .arg(result.accM).arg(result.gyroM).arg(timestamp);
    }

//    QMap<unsigned int,ImuUnderThresholdFlags>::const_iterator i = m_DetectedResultSequence_imu.find(timestamp);//获取迭代器，是时间窗口的最后一个时间戳
    //    for(int counter = 0;counter<c_accVar_WindowLength;counter++,i--){
//        ImuUnderThresholdFlags result = i.value();//检测结果
//        if(result.accM && result.gyroM){
//            //全部标志位都为真，imu判断为真
//            result.set_imuTotal_Flag(true);
//        }else{
//            //至少一个不为真，imu判断为假
//            result.set_imuTotal_Flag(false);
//            qDebug()<<QString("result.accM is: %1, result.accVar is: %2,result.gyroM is: %3,timestamp is:%4")
//                                        .arg(result.accM).arg(result.accVar).arg(result.gyroM).arg(timestamp);
//        }
//    }
    emit imu_detected_ok(timestamp);//发射imu全部检测完信号
}




/**
 * @brief 帧同步处理函数，确认该时间戳对应的帧是否被plantar和imu都计算过，主要功能是imu与plantar时间戳同步函数
 */
//void Algorithm::frameSync(){
//    //获取从最后一次plantar计算过的数据开始的
//    QMap<unsigned int,ImuUnderThresholdFlags>::const_iterator imu_it = m_DetectedResultSequence_imu.find(m_lastWholeFrameDetected_timestamp);
//    qDebug()<<"m_lastPlantarDetect_timestamp is:"<<m_lastPlantarDetect_timestamp
//           <<"m_lastWholeFrameDetected_timestamp is:"<<m_lastWholeFrameDetected_timestamp;
//    //从最后一个plantar帧一直到上次检测的最后一帧
//    for(int counter = 0;counter<=c_imu_zeroDetection_count;counter++,imu_it++){
//        unsigned int timestamp_to_find = imu_it.key();
//        //从当前的timestamp开始，逐个递减，直到上次计算的imu时间戳
//        if(m_DetectedResultSequence_plantar.contains(timestamp_to_find)){
//            //检测正常
//            qDebug()<<"正确！融合检测过程检测到plantar数据"<<"imu_timestamp is"<<imu_it.key();
//            PlantarUnderThresholdFlags plantarResult = m_DetectedResultSequence_plantar.find(imu_it.key()).value();
//            //如果imu和plantar的时间戳差值小于容差值，则认为是同一个时刻的时间，将检测结果融合。
//            if(imu_it.value().Total || plantarResult.Total){
//                //检测为零速度区间
////                emit ZeroSpeed_detect_ok(imu_it.key(),true);
//            }else{
//                //检测为非零速区间
////                emit ZeroSpeed_detect_ok(imu_it.key(),false);
//            }
//        }else{
//            //未检测到相应的plantar数据
//            qDebug()<<"错误！融合检测过程未检测到plantar数据"
//                    <<"imu_timestamp is"<<imu_it.key()
//                    <<"plantar contain this:"<<m_DetectedResultSequence_plantar.contains(timestamp_to_find)
//                    <<"value_accM is:"<<m_DetectedResultSequence_imu.find(imu_it.key()).value().accM  ;

//            break;
//        }
//    }
//    m_lastWholeFrameDetected_timestamp = imu_it.key();
//    qDebug()<<"m_lastWholeFrameDetected_timestamp is:"<<imu_it.key();
//}

/**
 * @brief 基于足底压力传感器的零速点检测
 */
void Algorithm::plantar_ZeroSpeedDetection(unsigned int timestamp, QList<float> frameData){
    //将数据插入序列
    PlantarSequence.insert(timestamp,frameData);
    double frameSum = 0;
    //获取统计量
    for (float value : frameData) {
        frameSum += value;
    }

    //基于阈值检测获取零速度区间,使用插值法补全序列
    if(frameSum>c_plantarSum_Threshold){
        //大于阈值
        for(unsigned int i = m_lastPlantarDetect_timestamp;i<=timestamp;i++){
            PlantarUnderThresholdFlags result = PlantarUnderThresholdFlags();
            result.set_Total_Flag(true);
            result.set_FrameDetected_Flag(true);
            //插入
            m_DetectedResultSequence_plantar.insert(i,result);

        }
    }else{
        //小于阈值
        for(unsigned int i = m_lastPlantarDetect_timestamp;i<=timestamp;i++){
            PlantarUnderThresholdFlags result = PlantarUnderThresholdFlags();
            result.set_Total_Flag(false);//认为不是零速区间
            result.set_FrameDetected_Flag(true);
            //插入
            m_DetectedResultSequence_plantar.insert(i,result);
//            qDebug()<<"i is:"<<i;
        }

    }
    m_lastPlantarDetect_timestamp = timestamp;

    //发射检测完成信号
    emit plantar_detected_ok(timestamp);
}


/**
 * @brief 开始零速度检测
 */
void Algorithm::startZeroSpeedDetection(){
    dataUpdateTimer->start();
    m_lastPlantarDetect_timestamp = m_DetectedResultSequence_plantar.lastKey();
    m_lastWholeFrameDetected_timestamp = m_DetectedResultSequence_plantar.lastKey();
    qDebug()<<"开始采集"
            <<"m_lastPlantarDetect_timestamp is:"<<m_lastPlantarDetect_timestamp
            <<"m_lastWholeFrameDetected_timestamp is:"<<m_lastWholeFrameDetected_timestamp;

}

/**
 * @brief 辛普森数值求积分公式
 */
double simpsonIntegration(std::function<double(double)> f, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += i % 2 == 0 ? 2 * f(x) : 4 * f(x);
    }

    return (h / 3) * sum;
}

/**
 * @brief 由加速度求速度
 */
void Algorithm::calculateVelocity(unsigned int timestamp){
    unsigned int  delta_t = timestamp - m_lastCalculateTimestamp;
    if(delta_t> 100) return ;
    Vector3 acc_current = ResultSequence.find(timestamp).value().acc;
    Vector3 acc_last  = ResultSequence.find(m_lastCalculateTimestamp).value().acc;
    Vector3 acc_average  = Vector3((acc_current.x+acc_last.x)/2.0,(acc_current.y+acc_last.y)/2.0,(acc_current.z+acc_last.z)/2.0);

    Vector3 vel_last = ResultSequence.find(timestamp).value().vel;
    ResultSequence.find(timestamp).value().vel = Vector3((vel_last.x+acc_average.x*delta_t),
                                                         (vel_last.y+acc_average.y*delta_t),
                                                         (vel_last.z+acc_average.z*delta_t));

    Vector3 vel_current  = ResultSequence.find(timestamp).value().vel;
//    qDebug()<<QString("vel.x is:%1,vel.y is:%2,vel.z is:%3,timestamp is:%4,delta_t is %5")
//              .arg(vel_current.x).arg(vel_current.y).arg(vel_current.z).arg(timestamp).arg(delta_t);

}

/**
 * @brief 由速度求位移
 */
void Algorithm::calculatePosition(unsigned int timestamp){
    unsigned int  delta_t = timestamp - m_lastCalculateTimestamp;
    if(delta_t > 100) return ;
    Vector3 vel_current = ResultSequence.find(timestamp).value().vel;
    Vector3 vel_last  = ResultSequence.find(m_lastCalculateTimestamp).value().vel;
    Vector3 vel_average  = Vector3((vel_current.x+vel_last.x)/2.0,(vel_current.y+vel_last.y)/2.0,(vel_current.z+vel_last.z)/2.0);

    Vector3 pos_last = ResultSequence.find(m_lastCalculateTimestamp).value().pos;
    ResultSequence.find(timestamp).value().pos = Vector3((pos_last.x+vel_average.x*delta_t),
                                                         (pos_last.y+vel_average.y*delta_t),
                                                         (pos_last.z+vel_average.z*delta_t));
    Vector3 pos_current = ResultSequence.find(timestamp).value().pos;
//    qDebug()<<QString("pos.x is:%1,pos.y is:%2,pos.z is:%3,timestamp is:%4,delta_t is %5")
//              .arg(pos_current.x).arg(pos_current.y).arg(pos_current.z).arg(timestamp).arg(delta_t);
}

/**
 * @brief 零速度检测成功回调
 */
void Algorithm::on_ZeroSpeed_detected(unsigned int timestamp, bool isZeroSpeed){
    qDebug()<<QString("零速度检测完成，timestamp is: %1,m_lastCalculateTimestamp is:%2,result is: %3")
              .arg(timestamp).arg(m_lastCalculateTimestamp).arg(isZeroSpeed);
    if(isZeroSpeed){
        //零速区间
        velocityFilter_X.x = -0.01;
        velocityFilter_Y.x = 0.05;
        velocityFilter_Z.x = 0;
        qDebug()<<"检测到零速度区间，置零";
    }else{
        //非零速区间，使用辛普森数值积分，分别求取速度和加速度

    }

}



//void Algorithm::on_ZeroSpeed_detected(unsigned int timestamp, bool isZeroSpeed){
//    qDebug()<<QString("零速度检测完成，timestamp is: %1,m_lastCalculateTimestamp is:%2,result is: %3")
//              .arg(timestamp).arg(m_lastCalculateTimestamp).arg(isZeroSpeed);
//    if(m_lastCalculateTimestamp == 0){ m_lastCalculateTimestamp = timestamp; return ; }

//    if(isZeroSpeed){
//        //零速区间，使用卡尔曼滤波
//        calculateVelocity(timestamp);
//        //init(double q, double r, double x, double p
//        velFilter_ZUPT_X.init(10,0.1,ResultSequence.find(timestamp).value().vel.x,1);
//        velFilter_ZUPT_Y.init(10,0.1,ResultSequence.find(timestamp).value().vel.y,1);
//        velFilter_ZUPT_Z.init(10,0.1,ResultSequence.find(timestamp).value().vel.z,1);
//        double delta_t = timestamp - m_lastCalculateTimestamp;
//        Vector3 acc_current = ResultSequence.find(timestamp).value().acc;
//        velFilter_ZUPT_X.update( -acc_current.x * delta_t );
//        velFilter_ZUPT_Y.update( -acc_current.y * delta_t );
//        velFilter_ZUPT_Z.update( -acc_current.z * delta_t );
//        ResultSequence.find(timestamp).value().vel = Vector3((velFilter_ZUPT_X.x),
//                                                             (velFilter_ZUPT_Y.x),
//                                                             (velFilter_ZUPT_Z.x));
//        Vector3 vel_current  = ResultSequence.find(timestamp).value().vel;
//        qDebug()<<QString("vel.x is:%1,vel.y is:%2,vel.z is:%3,timestamp is:%4,delta_t is %5")
//                  .arg(vel_current.x).arg(vel_current.y).arg(vel_current.z).arg(timestamp).arg(delta_t);
//        calculatePosition(timestamp);

//    }else{
//        //非零速区间，使用辛普森数值积分，分别求取速度和加速度
//        calculateVelocity(timestamp);
//        calculatePosition(timestamp);
//    }
//    m_lastCalculateTimestamp = timestamp;
//    emit pos_updated(timestamp);
//}

















