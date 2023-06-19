#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <iostream>
#include <cmath>
#include <QDebug>
#include <bluetooth/dataprocess.h>
#include <QtMath>
#include <QMutex>




// 定义一个结构体表示卡尔曼滤波器
struct KalmanFilter {
    double q; // 过程噪声协方差
    double r; // 测量噪声协方差
    double x; // 状态变量
    double p; // 状态协方差
    double k; // 卡尔曼增益

    KalmanFilter(double q, double r, double x, double p) : q(q), r(r), x(x), p(p) {}

    // 更新状态
    double update(double measurement) {
        // 预测
        p = p + q;//预测当前时刻的状态协方差

        // 更新
        k = p / (p + r);//卡尔曼增益
        x = x + k * (measurement - x);//计算状态变量
        p = (1 - k) * p;//更新状态协方差（估计误差）

//        // 自适应调整过程噪声协方差q和测量噪声协方差r的

//        qDebug("x:%8.3lf,innovation:%6.3lf, p:%8.4lf,k:%6.3lf, r:%6.3lf",x,innovation,p,k,r);
//        qDebug("x:%8.3lf,p:%8.4lf,k:%6.3lf, r:%6.3lf",x,p,k,r);
        return x;
    }

    //初始化赋值
    void init(double q, double r, double x, double p){
        this->q = q;
        this->r = r;
        this->x = x;
        this->p = p;
    }




};

//int main() {
//    // 假设加速度传感器输出为以下序列
//    std::vector<double> acceleration = {0, 1, 2, 3, 4};

//    // 定义两个卡尔曼滤波器分别用于计算速度和位移
//    KalmanFilter velocityFilter(0.1, 0.1, 0, 0);
//    KalmanFilter positionFilter(0.1, 0.1, 0, 0);

//    // 定义采样时间间隔
//    double dt = 1;

//    for (double a : acceleration) {
//        // 计算速度
//        velocityFilter.update(velocityFilter.x + a * dt);

//        // 计算位移
//        positionFilter.update(positionFilter.x + velocityFilter.x * dt);

//        std::cout << "Velocity: " << velocityFilter.x << std::endl;
//        std::cout << "Position: " << positionFilter.x << std::endl;
//    }

//    return 0;
//}



//四元数结构体
struct Quaternion {
    double w, x, y, z;

    Quaternion(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {}

    Quaternion operator*(const Quaternion &q) const {
        return Quaternion(
            w * q.w - x * q.x - y * q.y - z * q.z,
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w
        );
    }

    Quaternion conjugate() const {
        return Quaternion(w, -x, -y, -z);
    }

    void init(double w, double x, double y, double z){
        this->w = w;
        this->x = x;
        this->y = y;
        this->z = z;
    }


};


//标准三维向量
struct Vector3 {
    double x, y, z;

    //构造函数
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    //坐标变换
    Vector3 rotate(const Quaternion &q) const {
        Quaternion p(0, x, y, z);
        Quaternion result = q * p * q.conjugate();
        return Vector3(result.x, result.y, result.z);
    }

    // 向量点积
    double dot(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    // 向量叉积
    Vector3 cross(const Vector3 &v) const {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    // 向量模长
    double norm() const {
        return std::sqrt(dot(*this));
    }

    // 单位化向量
    Vector3 normalize() const {
        return (*this) / norm();
    }

    // 向量加法
    Vector3 operator+(const Vector3 &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    // 向量减法
    Vector3 operator-(const Vector3 &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    // 向量数乘
    Vector3 operator*(double s) const {
        return Vector3(x * s, y * s, z * s);
    }

    // 向量数除
    Vector3 operator/(double s) const {
        return Vector3(x / s, y / s, z / s);
    }

    //一元操作符
    Vector3 operator-() const {
           return Vector3(-x, -y, -z);
       }

    //初始化赋值
    void init(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
    }


};

//获取初始坐标变换四元数
QVector<float> getOriginalPosition(QVector<QMap<QString,float>> acceleration, QVector<QMap<QString,uint16_t>> magnetism);

//位置更新
//QVector<float> updatePositionENU(QVector<float> acceleration,QVector<float> q_current_original,QVector<float> q_original_ENU);



class Algorithm : public QObject
{
    Q_OBJECT
public:
    explicit Algorithm(QObject *parent = nullptr);

    Quaternion q_original_ENU;//原始坐标系到东北天坐标系下的转换四元数
    Quaternion q_current_original;//当前参照坐标系到原始坐标系之间的转换四元数


    //加速度计算卡尔曼滤波器
    KalmanFilter accFilter_X = KalmanFilter(0.06,9*0.027,0,1);
    KalmanFilter accFilter_Y = KalmanFilter(0.06,9*0.015,0,1);
    KalmanFilter accFilter_Z = KalmanFilter(0.06,9*0.02,0,1);
    //角速度计算卡尔曼滤波器
    KalmanFilter gyroFilter_X = KalmanFilter(0.1,0.1,0,0);
    KalmanFilter gyroFilter_Y = KalmanFilter(0.1,0.1,0,0);
    KalmanFilter gyroFilter_Z = KalmanFilter(0.1,0.1,0,0);
    //角度计算卡尔曼滤波器
    KalmanFilter angleFilter_X = KalmanFilter(0.1,0.1,0,0);
    KalmanFilter angleFilter_Y = KalmanFilter(0.1,0.1,0,0);
    KalmanFilter angleFilter_Z = KalmanFilter(0.1,0.1,0,0);
    //磁力计计算卡尔曼滤波器
    KalmanFilter magFilter_X = KalmanFilter(0.1,0.1,0,0);
    KalmanFilter magFilter_Y = KalmanFilter(0.1,0.1,0,0);
    KalmanFilter magFilter_Z = KalmanFilter(0.1,0.1,0,0);
    //四元数计算卡尔曼滤波器
    KalmanFilter quaternionFilter_0 = KalmanFilter(1,10,0,0);
    KalmanFilter quaternionFilter_1 = KalmanFilter(1,10,0,0);
    KalmanFilter quaternionFilter_2 = KalmanFilter(1,10,0,0);
    KalmanFilter quaternionFilter_3 = KalmanFilter(1,10,0,0);
    //高度计算卡尔曼滤波器
    KalmanFilter heightFilter = KalmanFilter(0.1,0.1,0,0);
    //气压计计算卡尔曼滤波器
    KalmanFilter atomsFilter = KalmanFilter(0.1,0.1,0,0);



    // 定义六个卡尔曼滤波器分别用于计算三轴的速度和位移
    KalmanFilter velocityFilter_X = KalmanFilter(100000,0,0,1);
    KalmanFilter positionFilter_X = KalmanFilter(100000,0,0,1);

    KalmanFilter velocityFilter_Y = KalmanFilter(100000,0,0,1);
    KalmanFilter positionFilter_Y = KalmanFilter(100000,0,0,1);

    KalmanFilter velocityFilter_Z = KalmanFilter(100000,0,0,1);
    KalmanFilter positionFilter_Z = KalmanFilter(100000,0,0,1);


    //在imu当前坐标系下的经卡尔曼滤波后的速度与位移向量
    Vector3 velocityFiltered_currentVec;
    Vector3 positionFiltered_currentVec;

    //基于定时器更新数据
    QTimer *dataUpdateTimer;//更新数据定时器
    const int c_update_interval = 50;//更新间隔

    //定义几个卡尔曼滤波器用来测试零速度修正
    KalmanFilter velFilter_ZUPT_X = KalmanFilter(10,0.05,0,1);
    KalmanFilter velFilter_ZUPT_Y = KalmanFilter(10,0.05,0,1);
    KalmanFilter velFilter_ZUPT_Z = KalmanFilter(10,0.05,0,1);

    struct ResultFrame{
        Vector3 acc;
        Vector3 vel;
        Vector3 pos;

        ResultFrame()
            :acc(Vector3(0,0,0)),vel(Vector3(0,0,0)),pos(Vector3(0,0,0))
        {};
    };
    QMap<unsigned int,ResultFrame> ResultSequence;//结果序列
    unsigned int m_lastCalculateTimestamp;




    void updatePosition(Vector3 acceleration,double dt);
    void KalmanFilterInit();
    void startZeroSpeedDetection();//开始零速度检测


signals:
    void imu_calculate_ok(unsigned int timestamp);//imu三个统计量计算完
    void imu_detected_ok(unsigned int timestamp);
    void imu_zeroSpeed_detected(unsigned int timestamp_start,unsigned int timestamp_end);
    void plantar_detected_ok(unsigned int timestamp);//足底压力数据检测完成
    void ZeroSpeed_detect_ok(unsigned int timestamp, bool isZeroSpeed);//零速度检测
    void pos_updated(unsigned int timestamp);//位置更新

public slots:
    void setOriginaToENUQuaternion(QVector<Vector3> accVector, QVector<Vector3> magVector);
    void imu_detected_result_process(unsigned int timestamp);
//    void frameSync();
//    void on_ZeroSpeed_detected(unsigned int timestamp, bool isZeroSpeed);
    void on_ZeroSpeed_detected(unsigned int timestamp, bool isZeroSpeed);

private:

    unsigned int m_periodCounter;
//    bool m_underThresholdFlag;


    //速度滑动方差检测参数（单位：ms）
    const int c_zeroSpeed_Interval = 200;//理论上零速度区间长度L
    const int c_accVar_WindowLength = 0.5 * c_zeroSpeed_Interval;//检测窗体长度N
    const int c_accVar_SlidingStep = 1 * (c_zeroSpeed_Interval - c_accVar_WindowLength);//加速度滑动步长K,要求K<(L-N)

    //基于IMU的检测阈值
    const double c_accM_Threshold = 1;
    const double c_gyroM_Threshold = 25;
    const double c_accVar_Threshold = 100;

    //速度值
    double m_velocity;
    //位移值
    double m_position;

    //接收到的imu序列
    QMap<unsigned int,DataProcess::SingleFrameIMU> ImuSequence;
    //定义窗体内IMU序列
    QMap<unsigned int, DataProcess::SingleFrameIMU> ImuWindowSequence;
    //合成加速度序列
    QMap<unsigned int, double> accM_sequence;




    // 接收到的plantar序列
    QMap<unsigned int,QList<float>> PlantarSequence;
    //单帧Plantar和函数
    QMap<unsigned int,double> PlantarSumSequence;
//    //时间窗口内的序列
//    QMap<unsigned int,double> PlantarSumSequence_InWindow;


    //imu的计数器和计数阈值
    int imu_above_threshold_counter;
    const int  imu_above_threshold_count = 3;

    int imu_frame_counter;


    const unsigned int c_FrameAsync_diff = 250; //帧不同步差值，单位ms
    const unsigned int c_Search_num = 40;//搜寻空间，帧个数
    const unsigned int c_timestamp_tolerance = 8;//imu和plantar两个传感器之间的时间戳容差


    const unsigned int c_imu_zeroDetection_count = c_update_interval/10;//单次数据更新时imu计算的个数


    //将plantar的检测结果使用插值补全
    unsigned int m_lastPlantarDetect_timestamp;
    //最后一次完全补全的数据
    unsigned int m_lastWholeFrameDetected_timestamp;


    //大于阈值的序列
    QMap<unsigned int,double> PlantarSumSequence_ZeroSpeed;


    //压力总和连续低于阈值标志位
    bool plantarUnderThresholdFlag;
    //连续低于阈值计数器
    int plantar_continuous_counter;
    //摆动相低于阈值数目阈值
    const int c_plantarUnderThreshold_count = 3;
    //压力值总和阈值
    const double c_plantarSum_Threshold = 300;

    //上次滑动窗口对应的末尾时间戳
    unsigned int m_LastWindowEnd_timestamp;

    struct ImuUnderThresholdFlags{
        bool accM;//加速度模长低于阈值标志位
        bool gyroM;//角度模长低于阈值标志位
        bool accVar;//加速度方差低于阈值标志位
        bool Total;//imu检测到零速度区间标志位
        bool FrameDetected;//imu整体检测完标志位

        //构造函数
        ImuUnderThresholdFlags()
            :accM(false),gyroM(false),accVar(false),Total(false){};

        //几个置位函数
        void set_accM_Flag(bool accM_flag_toSet){
            accM = accM_flag_toSet;
        }
        void set_gyroM_Flag(bool gyroM_flag_toSet){
            gyroM = gyroM_flag_toSet;
        }
        void set_accVar_Flag(bool accVar_flag_toSet){
            accVar = accVar_flag_toSet;
        }
        void set_imuFrameDetected_Flag(bool imuFrameDetected_flag_toSet){
            FrameDetected = imuFrameDetected_flag_toSet;
        }
        void set_imuTotal_Flag(bool imuTotal_flag_toSet){
            Total = imuTotal_flag_toSet;
        }

    };

    struct PlantarUnderThresholdFlags{
        bool FrameDetected;//plantar单帧检测完成
        bool Total;//plantar对零速度区间的判断

        //构造函数
        PlantarUnderThresholdFlags()
            :FrameDetected(false),Total(false){};

        //几个置位函数
        void set_FrameDetected_Flag(bool FrameDetected_flag_toSet){
            FrameDetected = FrameDetected_flag_toSet;
        }
        void set_Total_Flag(bool Total_flag_toSet){
            Total = Total_flag_toSet;
        }
    };









    QMap<unsigned int,ImuUnderThresholdFlags> m_DetectedResultSequence_imu;//与时间戳绑定的阈值检测序列
    QMap<unsigned int,PlantarUnderThresholdFlags> m_DetectedResultSequence_plantar;//与时间戳绑定的阈值检测序列

    Vector3 getFilteredAcceleration(const QVector<Vector3>& samples) {
        KalmanFilter filterX(0.001, 0.1, 0, 1);
        KalmanFilter filterY(0.001, 0.1, 0, 1);
        KalmanFilter filterZ(0.001, 0.1, 0, 1);

        for (const auto& sample : samples) {
            filterX.update(sample.x);
            filterY.update(sample.y);
            filterZ.update(sample.z);
        }

        return Vector3(filterX.x, filterY.x, filterZ.x);
    }

    Vector3 getFilteredMag(const QVector<Vector3>& samples) {
        KalmanFilter filterX(0.001, 0.1, 0, 1);
        KalmanFilter filterY(0.001, 0.1, 0, 1);
        KalmanFilter filterZ(0.001, 0.1, 0, 1);

        for (const auto& sample : samples) {
            filterX.update(sample.x);
            filterY.update(sample.y);
            filterZ.update(sample.z);
        }

        return Vector3(filterX.x, filterY.x, filterZ.x);
    }

    void ImuDataPreprocess(unsigned int timestamp,DataProcess::SingleFrameIMU frameData);
    void PlantarDataPreprocess(unsigned int timestamp, QList<float> frameData);

    Quaternion calculateOriginalQuaternion(const Vector3& acceleration, const Vector3& magneticField);
    void setCurrentToOriginalQuaternion(double roll, double pitch, double yaw);

    //直接置零法，用于去除加速度传感器的零偏误差
    void directZeroing(double &accX,double &accY,double &accZ);
    //基于阈值检测的直接置零零速度更新法
    void ZUPT_ThresholdDetection(double &accX,double &accY,double &accZ,double &velocityX,double &velocityY,double &velocityZ,unsigned int dt);

    //多个统计量的综合检测法
    void imu_ComprehensiveZeroDetection();
    //加速度模长小于阈值检测法
    void accUnderThreshold(unsigned int timestamp,Vector3 accVector);
    //角速度模长小于阈值检测法
    void gyroUnderThreshold(unsigned int timestamp,Vector3 gyroVector);
    //滑动窗口上加速度方差检测法
    void accVarUnderThreshold(unsigned int timestamp);
    //足底压力数据阈值检测法
    void plantar_ZeroSpeedDetection(unsigned int timestamp, QList<float> frameData);

    void process(unsigned int timestamp, DataProcess::SingleFrameIMU frameData);
    void calculateVelocity(unsigned int timestamp);
    void calculatePosition(unsigned int timestamp);
};

extern Algorithm *algorithm;

#endif // ALGORITHM_H
