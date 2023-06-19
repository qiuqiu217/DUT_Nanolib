#include "imupage.h"
#include "ui_imupage.h"
#include "bluetooth/dataprocess.h"


ImuPage::ImuPage(QWidget *parent) :
    QWidget(parent),
    lastPlotSize(0),
    currentPlotSize(0),
    m_lastCalculateTimestamp(0),
    ui(new Ui::ImuPage)
{
    ui->setupUi(this);
    //获取绘图指针
    waveformPlot = ui->waveformPlot;
    //数据选择初始化
    dataSelectionInit();
    //坐标系选择单选框初始化
    coordinateSelectionInit();
    //初始化算法库
//    algorithm = new Algorithm();
}

/**
 * @brief 坐标系选择初始化
 */
void ImuPage::coordinateSelectionInit(){
    QButtonGroup *CoordinateSelect_RbtnGroup = new QButtonGroup(this);
    CoordinateSelect_RbtnGroup->addButton(ui->toIMU_RButton,0);
    CoordinateSelect_RbtnGroup->addButton(ui->toENU_RButton,1);
    //connect(CoordinateSelect_RbtnGroup,&QButtonGroup::idToggled,this,&ImuPage::coordinate_display_changed_callback);
    //connect(CoordinateSelect_RbtnGroup,&QButtonGroup::buttonToggled,this,&ImuPage::coordinate_display_changed_callback);
    ui->toIMU_RButton->setChecked(true);

}


/**
 * @brief 加速度坐标系显示变换回调
 */
//void ImuPage::coordinate_display_changed_callback(int btn, bool checked){
//    if (!checked)
//       {
//           return;
//       }

//       switch (btn)
//       {
//       case 0:
//           qDebug("This is button self");
//           m_currentCoordinate = IMU;
//           ui->KalmanFilteredCheckBox->setChecked(false);
//           break;
//       case 1:
//           qDebug("This is button ENU");
//           m_currentCoordinate = ENU;

//           break;
//       default:
//           break;if (!checked)
//           {
//               return;
//           }

//           switch (btn)
//           {
//           case 0:
//               qDebug("This is button self");
//               m_currentCoordinate = IMU;
//               ui->KalmanFilteredCheckBox->setChecked(false);
//               break;
//           case 1:
//               qDebug("This is button ENU");
//               m_currentCoordinate = ENU;

//               break;
//           default:
//               break;
//           }


//       }


//       emit dataDisplayChanged();

//}

//void ImuPage::coordinate_display_changed_callback(QAbstractButton *button, bool checked)
//{
//    // 在这里添加你的代码
//    if (!checked)
//       {
//           return;
//       }

//       switch (btn)
//       {
//       case 0:
//           qDebug("This is button self");
//           m_currentCoordinate = IMU;
//           ui->KalmanFilteredCheckBox->setChecked(false);
//           break;
//       case 1:
//           qDebug("This is button ENU");
//           m_currentCoordinate = ENU;

//           break;
//       default:
//           break;if (!checked)
//           {
//               return;
//           }

//           switch (btn)
//           {
//           case 0:
//               qDebug("This is button self");
//               m_currentCoordinate = IMU;
//               ui->KalmanFilteredCheckBox->setChecked(false);
//               break;
//           case 1:
//               qDebug("This is button ENU");
//               m_currentCoordinate = ENU;

//               break;
//           default:
//               break;
//           }


//       }


//       emit dataDisplayChanged();
//}


/**
 * @brief dataSelection 和绘图曲线初始化
 */
void ImuPage::dataSelectionInit(){
    //添加下拉框item
    ui->dataTypeComboBox->addItem("magnetism");
    ui->dataTypeComboBox->addItem("acceleration");
    ui->dataTypeComboBox->addItem("gyroscope");
    ui->dataTypeComboBox->addItem("angle");
    ui->dataTypeComboBox->addItem("quaternion");
    ui->dataTypeComboBox->addItem("height");
    ui->dataTypeComboBox->addItem("atoms");    
    ui->dataTypeComboBox->addItem("velocity");
    ui->dataTypeComboBox->addItem("position");



    ui->dataTypeComboBox->setCurrentText("magnetism");






    //设置轴按钮组
    m_curveToShowCheckBoxGroup.append(ui->axisCheckBox_1);
    m_curveToShowCheckBoxGroup.append(ui->axisCheckBox_2);
    m_curveToShowCheckBoxGroup.append(ui->axisCheckBox_3);
    m_curveToShowCheckBoxGroup.append(ui->axisCheckBox_4);
    m_curveToShowCheckBoxGroup.append(ui->KalmanFilteredCheckBox);
    for(QCheckBox* checkBox:m_curveToShowCheckBoxGroup){
        //将几个checkbox都绑定在槽函数上
        connect(checkBox,&QCheckBox::stateChanged,this,&ImuPage::axisCheckBoxsChanged_callback);
    }
    ui->axisCheckBox_1->setChecked(true);
    //下拉框数据改变信号槽
    connect(ui->dataTypeComboBox,&QComboBox::currentTextChanged,this,&ImuPage::dataType_changed_callback);
    //设置数据显示状态改变槽函数
    connect(this,&ImuPage::dataDisplayChanged,this,&ImuPage::dataDisplayChanged_callback);

    //将初始的设置为加速度
    ui->dataTypeComboBox->setCurrentText("acceleration");

    //添加曲线
    setCurvesToShow();
}




ImuPage::~ImuPage()
{
    delete ui;
}


/**
 * @brief 初始化所有曲线
 */
void ImuPage::curvesInit(){
    //添加所有曲线
    for (auto it = curveNames.constBegin(); it != curveNames.constEnd(); ++it) {
           waveformPlot->addCurve(it.value());
           qDebug()<<"curveNames.value is:"<<it.value();
       }
}

/**
 * @brief 添加想要显示的曲线
 */
void ImuPage::setCurvesToShow(){
    waveformPlot->clear();//清空所有显示的曲线
    for(auto curve_name:m_curvesToShow){
        waveformPlot->addCurve(curveNames[curve_name]);
    }
    waveformPlot->xAxis->setLabel("Timestamp (ms)");
    waveformPlot->yAxis->setLabel(m_currentText);
}


/**
 * @brief 数据刷新
 */
void ImuPage::refreshData(unsigned int timestamp_current,DataProcess::SingleFrameIMU frameData){
//    qDebug()<<"data_received,timestamp is:"<<timestamp_current<<"frameData.accx is:"<<frameData.acc["X"];
    for(auto curve_name:m_curvesToShow){
        if(curve_name == "accX"){
            waveformPlot->addData(curveNames["accX"],timestamp_current,frameData.acc["X"]);
        }else if(curve_name == "accY"){
            waveformPlot->addData(curveNames["accY"],timestamp_current,frameData.acc["Y"]);
        }else if(curve_name == "accZ"){
            waveformPlot->addData(curveNames["accZ"],timestamp_current,frameData.acc["Z"]);
        }else if(curve_name == "magX"){
            waveformPlot->addData(curveNames["magX"],timestamp_current,frameData.mag["X"]);
        }else if(curve_name == "magY"){
            waveformPlot->addData(curveNames["magY"],timestamp_current,frameData.mag["Y"]);
        }else if(curve_name == "magZ"){
            waveformPlot->addData(curveNames["magZ"],timestamp_current,frameData.mag["Z"]);
        }else if(curve_name == "gyroX"){
            waveformPlot->addData(curveNames["gyroX"],timestamp_current,frameData.gyro["X"]);
        }else if(curve_name == "gyroY"){
            waveformPlot->addData(curveNames["gyroY"],timestamp_current,frameData.gyro["Y"]);
        }else if(curve_name == "gyroZ"){
            waveformPlot->addData(curveNames["gyroZ"],timestamp_current,frameData.gyro["Z"]);
        }else if(curve_name == "angleX"){
            waveformPlot->addData(curveNames["angleX"],timestamp_current,frameData.angle["X"]);
        }else if(curve_name == "angleY"){
            waveformPlot->addData(curveNames["angleY"],timestamp_current,frameData.angle["Y"]);
        }else if(curve_name == "angleZ"){
            waveformPlot->addData(curveNames["angleZ"],timestamp_current,frameData.angle["Z"]);
        }else if(curve_name == "q0"){
            waveformPlot->addData(curveNames["q0"],timestamp_current,frameData.quaternion["q0"]);
        }else if(curve_name == "q1"){
            waveformPlot->addData(curveNames["q1"],timestamp_current,frameData.quaternion["q1"]);
        }else if(curve_name == "q2"){
            waveformPlot->addData(curveNames["q2"],timestamp_current,frameData.quaternion["q2"]);
        }else if(curve_name == "q3"){
            waveformPlot->addData(curveNames["q3"],timestamp_current,frameData.quaternion["q3"]);
        }else if(curve_name == "height"){
            waveformPlot->addData(curveNames["height"],timestamp_current,frameData.quaternion["height"]);
        }else if(curve_name == "atoms"){
            waveformPlot->addData(curveNames["atoms"],timestamp_current,frameData.quaternion["atoms"]);
        }
        //使用卡尔曼滤波器的
        else if(curve_name == "accX_KF"){
            waveformPlot->addData(curveNames["accX_KF"],timestamp_current,algorithm->accFilter_X.update(frameData.acc["X"]));
        }else if(curve_name == "accY_KF"){
            waveformPlot->addData(curveNames["accY_KF"],timestamp_current,algorithm->accFilter_Y.update(frameData.acc["Y"]));
        }else if(curve_name == "accZ_KF"){
            waveformPlot->addData(curveNames["accZ_KF"],timestamp_current,algorithm->accFilter_Z.update(frameData.acc["Z"]));
        }else if(curve_name == "magX_KF"){
            waveformPlot->addData(curveNames["magX_KF"],timestamp_current,algorithm->magFilter_X.update(frameData.mag["X"]));
        }else if(curve_name == "magY_KF"){
            waveformPlot->addData(curveNames["magY_KF"],timestamp_current,algorithm->magFilter_Y.update(frameData.mag["Y"]));
        }else if(curve_name == "magZ_KF"){
            waveformPlot->addData(curveNames["magZ_KF"],timestamp_current,algorithm->magFilter_Z.update(frameData.mag["Z"]));
        }else if(curve_name == "gyroX_KF"){
            waveformPlot->addData(curveNames["gyroX_KF"],timestamp_current,algorithm->gyroFilter_X.update(frameData.gyro["X"]));
        }else if(curve_name == "gyroY_KF"){
            waveformPlot->addData(curveNames["gyroY_KF"],timestamp_current,algorithm->gyroFilter_Y.update(frameData.gyro["Y"]));
        }else if(curve_name == "gyroZ_KF"){
            waveformPlot->addData(curveNames["gyroZ_KF"],timestamp_current,algorithm->gyroFilter_Z.update(frameData.gyro["Z"]));
        }else if(curve_name == "angleX_KF"){
            waveformPlot->addData(curveNames["angleX_KF"],timestamp_current,algorithm->angleFilter_X.update(frameData.angle["X"]));
        }else if(curve_name == "angleY_KF"){
            waveformPlot->addData(curveNames["angleY_KF"],timestamp_current,algorithm->angleFilter_Y.update(frameData.angle["Y"]));
        }else if(curve_name == "angleZ_KF"){
            waveformPlot->addData(curveNames["angleZ_KF"],timestamp_current,algorithm->angleFilter_Z.update(frameData.angle["Z"]));
        }else if(curve_name == "q0_KF"){
            waveformPlot->addData(curveNames["q0_KF"],timestamp_current,algorithm->quaternionFilter_0.update(frameData.quaternion["q0"]));
        }else if(curve_name == "q1_KF"){
            waveformPlot->addData(curveNames["q1_KF"],timestamp_current,algorithm->quaternionFilter_1.update(frameData.quaternion["q1"]));
        }else if(curve_name == "q2_KF"){
            waveformPlot->addData(curveNames["q2_KF"],timestamp_current,algorithm->quaternionFilter_2.update(frameData.quaternion["q2"]));
        }else if(curve_name == "q3_KF"){
            waveformPlot->addData(curveNames["q3_KF"],timestamp_current,algorithm->quaternionFilter_3.update(frameData.quaternion["q3"]));
        }else if(curve_name == "height_KF"){
            waveformPlot->addData(curveNames["height_KF"],timestamp_current,algorithm->heightFilter.update(frameData.quaternion["height"]));
        }else if(curve_name == "atoms_KF"){
            waveformPlot->addData(curveNames["atoms_KF"],timestamp_current,algorithm->atomsFilter.update(frameData.quaternion["atoms"]));
        }

        //ENU坐标系下的
        else if(curve_name == "accX_ENU"){
            //获取加速度向量
            Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
            //获取惯性坐标系->东北天坐标系四元数
            Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
            //获取ENU坐标系下加速度向量
            Vector3 accVector3_ENU = accVector3.rotate(q);
            waveformPlot->addData(curveNames["accX_ENU"],timestamp_current,accVector3_ENU.x);
        }
        else if(curve_name == "accY_ENU"){
            //获取加速度向量
            Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
            //获取惯性坐标系->东北天坐标系四元数
            Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
            //获取ENU坐标系下加速度向量
            Vector3 accVector3_ENU = accVector3.rotate(q);
            waveformPlot->addData(curveNames["accY_ENU"],timestamp_current,accVector3_ENU.y);
        }
        else if(curve_name == "accZ_ENU"){
            //获取加速度向量
            Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
            //获取惯性坐标系->东北天坐标系四元数
            Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
            //获取ENU坐标系下加速度向量
            Vector3 accVector3_ENU = accVector3.rotate(q);
            waveformPlot->addData(curveNames["accZ_ENU"],timestamp_current,accVector3_ENU.z);
        }else if(curve_name == "velX_KF"){
            //获取加速度向量
            Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
            //获取惯性坐标系->东北天坐标系四元数
            Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
            //获取ENU坐标系下加速度向量
            Vector3 accVector3_ENU = accVector3.rotate(q);
            accVector3_ENU.z = accVector3_ENU.z - 10;//减去重力加速度


            //获取dt
            double dt_ms = (static_cast<double>(timestamp_current)-static_cast<double>(m_lastCalculateTimestamp))/1000;
            //更新计算时间戳
            m_lastCalculateTimestamp = timestamp_current;

            //使用卡尔曼滤波器更新位置
            algorithm->updatePosition(accVector3_ENU,dt_ms);
            waveformPlot->addData(curveNames["velX_KF"],timestamp_current,algorithm->velocityFilter_X.x);
        }else if(curve_name == "velY_KF"){
            //获取加速度向量
            Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
            //获取惯性坐标系->东北天坐标系四元数
            Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
            //获取ENU坐标系下加速度向量
            Vector3 accVector3_ENU = accVector3.rotate(q);
            accVector3_ENU.z = accVector3_ENU.z - 10;//减去重力加速度


            //获取dt
            double dt_ms = (static_cast<double>(timestamp_current)-static_cast<double>(m_lastCalculateTimestamp))/1000;
            //更新计算时间戳
            m_lastCalculateTimestamp = timestamp_current;

            //使用卡尔曼滤波器更新位置
            algorithm->updatePosition(accVector3_ENU,dt_ms);
            waveformPlot->addData(curveNames["velY_KF"],timestamp_current,algorithm->velocityFilter_Y.x);
        }
        else if(curve_name == "velZ_KF"){
                    //获取加速度向量
                    Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
                    //获取惯性坐标系->东北天坐标系四元数
                    Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
                    //获取ENU坐标系下加速度向量
                    Vector3 accVector3_ENU = accVector3.rotate(q);
                    accVector3_ENU.z = accVector3_ENU.z - 10;//减去重力加速度


                    //获取dt
                    double dt_ms = (static_cast<double>(timestamp_current)-static_cast<double>(m_lastCalculateTimestamp))/1000;
                    //更新计算时间戳
                    m_lastCalculateTimestamp = timestamp_current;

                    //使用卡尔曼滤波器更新位置
                    algorithm->updatePosition(accVector3_ENU,dt_ms);
                    waveformPlot->addData(curveNames["velZ_KF"],timestamp_current,algorithm->velocityFilter_Z.x);
        }else if(curve_name == "posX_KF"){
            //获取加速度向量
            Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
            //获取惯性坐标系->东北天坐标系四元数
            Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
            //获取ENU坐标系下加速度向量
            Vector3 accVector3_ENU = accVector3.rotate(q);
            accVector3_ENU.z = accVector3_ENU.z - 10;//减去重力加速度


            //获取dt
            double dt_ms = (static_cast<double>(timestamp_current)-static_cast<double>(m_lastCalculateTimestamp))/1000;
            //更新计算时间戳
            m_lastCalculateTimestamp = timestamp_current;

            //使用卡尔曼滤波器更新位置
            algorithm->updatePosition(accVector3_ENU,dt_ms);
            waveformPlot->addData(curveNames["posX_KF"],timestamp_current,algorithm->positionFilter_X.x);
        }else if(curve_name == "posY_KF"){
            //获取加速度向量
            Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
            //获取惯性坐标系->东北天坐标系四元数
            Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
            //获取ENU坐标系下加速度向量
            Vector3 accVector3_ENU = accVector3.rotate(q);
            accVector3_ENU.z = accVector3_ENU.z - 10;//减去重力加速度


            //获取dt
            double dt_ms = (static_cast<double>(timestamp_current)-static_cast<double>(m_lastCalculateTimestamp))/1000;
            //更新计算时间戳
            m_lastCalculateTimestamp = timestamp_current;

            //使用卡尔曼滤波器更新位置
            algorithm->updatePosition(accVector3_ENU,dt_ms);
            waveformPlot->addData(curveNames["posY_KF"],timestamp_current,algorithm->positionFilter_Y.x);
        }else if(curve_name == "posZ_KF"){
            //获取加速度向量
            Vector3 accVector3(frameData.acc["X"], frameData.acc["Y"], frameData.acc["Z"]);
            //获取惯性坐标系->东北天坐标系四元数
            Quaternion q(frameData.quaternion["q0"],frameData.quaternion["q1"],frameData.quaternion["q2"],frameData.quaternion["q3"]);
            //获取ENU坐标系下加速度向量
            Vector3 accVector3_ENU = accVector3.rotate(q);
            accVector3_ENU.z = accVector3_ENU.z - 10;//减去重力加速度


            //获取dt
            double dt_ms = (static_cast<double>(timestamp_current)-static_cast<double>(m_lastCalculateTimestamp))/1000;
            //更新计算时间戳
            m_lastCalculateTimestamp = timestamp_current;

            //使用卡尔曼滤波器更新位置
            algorithm->updatePosition(accVector3_ENU,dt_ms);
            waveformPlot->addData(curveNames["posZ_KF"],timestamp_current,algorithm->positionFilter_Z.x);
        }
        else{
            qDebug()<<"UNKOWNED CurveName!";
        }

    }

}

/**
 * @brief dataTpye下拉框改变回调
 */
void ImuPage::dataType_changed_callback(QString currentText){

    m_currentText = currentText;
    emit dataDisplayChanged();
}

/**
 * @brief 根据下拉框的值，动态变更几个复选框的显示状态的文本
 */
void ImuPage::setAxisCheckBoxDisplay(const QList<bool> &visibilitys,const QStringList &texts ){
    //设置可见性
    ui->axisCheckBox_1->setVisible(visibilitys[0]);
    ui->axisCheckBox_2->setVisible(visibilitys[1]);
    ui->axisCheckBox_3->setVisible(visibilitys[2]);
    ui->axisCheckBox_4->setVisible(visibilitys[3]);
    //设置复选框文本
    ui->axisCheckBox_1->setText(texts[0]);
    ui->axisCheckBox_2->setText(texts[1]);
    ui->axisCheckBox_3->setText(texts[2]);
    ui->axisCheckBox_4->setText(texts[3]);

}



/**
 * @brief 轴复选框改变——回调
 */
void ImuPage::axisCheckBoxsChanged_callback(int currentIndex){
   //先将所有的状态置为空
   m_curvesSelectCheckBoxList.clear();
   //再依次根据复选框状态将状态置位
   for(QCheckBox* checkBox:m_curveToShowCheckBoxGroup){
       m_curvesSelectCheckBoxList.append(checkBox->isChecked());
    }
   emit dataDisplayChanged();
}

/**
 * @brief 数据显示改变回调
 */
bool ImuPage::dataDisplayChanged_callback(){
    m_curvesToShow.clear();//清空曲线显示列表
    bool showKalmanFilter = m_curvesSelectCheckBoxList[4];
    if(m_currentText == "acceleration"){
        setAxisCheckBoxDisplay({true,true,true,false},{"X轴","Y轴","Z轴",""});
        if(m_currentCoordinate == IMU ){
            //IMU坐标系
            if(!showKalmanFilter){
                if(m_curvesSelectCheckBoxList[0]){
                    m_curvesToShow.append("accX");
                }
                if(m_curvesSelectCheckBoxList[1]){
                    m_curvesToShow.append("accY");
                }
                if(m_curvesSelectCheckBoxList[2]){
                    m_curvesToShow.append("accZ");
                }
            }
            else{
                if(m_curvesSelectCheckBoxList[0]){
                    m_curvesToShow.append("accX");
                    m_curvesToShow.append("accX_KF");

                }
                if(m_curvesSelectCheckBoxList[1]){
                    m_curvesToShow.append("accY");
                    m_curvesToShow.append("accY_KF");
                }
                if(m_curvesSelectCheckBoxList[2]){
                    m_curvesToShow.append("accZ");
                    m_curvesToShow.append("accZ_KF");
                }
            }
        }
        else if(m_currentCoordinate == ENU){
            //东北天坐标系
            if(!showKalmanFilter){
                if(m_curvesSelectCheckBoxList[0]){
                    m_curvesToShow.append("accX_ENU");
                }
                if(m_curvesSelectCheckBoxList[1]){
                    m_curvesToShow.append("accY_ENU");
                }
                if(m_curvesSelectCheckBoxList[2]){
                    m_curvesToShow.append("accZ_ENU");
                }
            }
            else{
                if(m_curvesSelectCheckBoxList[0]){
                    m_curvesToShow.append("accX_ENU");
                    m_curvesToShow.append("accX_KF_ENU");

                }
                if(m_curvesSelectCheckBoxList[1]){
                    m_curvesToShow.append("accY_ENU");
                    m_curvesToShow.append("accY_KF_ENU");
                }
                if(m_curvesSelectCheckBoxList[2]){
                    m_curvesToShow.append("accZ_ENU");
                    m_curvesToShow.append("accZ_KF_ENU");
                }
            }


        }


    }else if(m_currentText == "magnetism"){
        setAxisCheckBoxDisplay({true,true,true,false},{"X轴","Y轴","Z轴",""});
        if(!showKalmanFilter){
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("magX");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("magY");
            }

            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("magZ");
            }
        }else{
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("magX");
                m_curvesToShow.append("magX_KF");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("magY");
                m_curvesToShow.append("magY_KF");
            }

            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("magZ");
                m_curvesToShow.append("magZ_KF");
            }
        }
    }else if(m_currentText == "gyroscope"){        
        setAxisCheckBoxDisplay({true,true,true,false},{"X轴","Y轴","Z轴",""});
        if(!showKalmanFilter){
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("gyroX");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("gyroY");
            }
            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("gyroZ");
            }
        }else{
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("gyroX");
                m_curvesToShow.append("gyroX_KF");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("gyroY");
                m_curvesToShow.append("gyroY_KF");
            }
            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("gyroZ");
                m_curvesToShow.append("gyroZ_KF");
            }
        }


    }else if(m_currentText == "angle"){
        setAxisCheckBoxDisplay({true,true,true,false},{"X轴","Y轴","Z轴",""});
        if(!showKalmanFilter){
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("angleX");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("angleY");
            }
            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("angleZ");
            }
        }else{
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("angleX");
                m_curvesToShow.append("angleX_KF");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("angleY");
                m_curvesToShow.append("angleY_KF");
            }
            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("angleZ");
                m_curvesToShow.append("angleZ_KF");
            }
        }


    }else if(m_currentText == "quaternion"){
        setAxisCheckBoxDisplay({true,true,true,true},{"q0","q1","q2","q3"});
        if(!showKalmanFilter){
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("q0");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("q1");
            }
            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("q2");
            }
            if(m_curvesSelectCheckBoxList[3]){
                m_curvesToShow.append("q3");
            }
        }else{
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("q0");
                m_curvesToShow.append("q0_KF");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("q1");
                m_curvesToShow.append("q1_KF");
            }
            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("q2");
                m_curvesToShow.append("q2_KF");
            }
            if(m_curvesSelectCheckBoxList[3]){
                m_curvesToShow.append("q3");
                m_curvesToShow.append("q3_KF");
            }
        }


    }else if(m_currentText == "height"){
        setAxisCheckBoxDisplay({false,false,false,false},{"","","",""});
        if(!showKalmanFilter){
            m_curvesToShow.append("height");
        }else{
            m_curvesToShow.append("height");
            m_curvesToShow.append("height_KF");
        }

    }else if(m_currentText == "atoms"){
        setAxisCheckBoxDisplay({false,false,false,false},{"","","",""});
        if(!showKalmanFilter){
             m_curvesToShow.append("atoms");
        }else{
             m_curvesToShow.append("atoms");
             m_curvesToShow.append("atoms_KF");
        }

    }else if(m_currentText == "velocity"){
        setAxisCheckBoxDisplay({true,true,true,false},{"X轴","Y轴","Z轴",""});
        if(!showKalmanFilter){
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("velX");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("velY");
            }

            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("velZ");
            }
        }else{
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("velX");
                m_curvesToShow.append("velX_KF");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("velY");
                m_curvesToShow.append("velY_KF");
            }

            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("velZ");
                m_curvesToShow.append("velZ_KF");
            }
        }

    }else if(m_currentText == "position"){
        setAxisCheckBoxDisplay({true,true,true,false},{"X轴","Y轴","Z轴",""});
        if(!showKalmanFilter){
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("posX");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("posY");
            }

            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("posZ");
            }
        }else{
            if(m_curvesSelectCheckBoxList[0]){
                m_curvesToShow.append("posX");
                m_curvesToShow.append("posX_KF");
            }
            if(m_curvesSelectCheckBoxList[1]){
                m_curvesToShow.append("posY");
                m_curvesToShow.append("posY_KF");
            }

            if(m_curvesSelectCheckBoxList[2]){
                m_curvesToShow.append("posZ");
                m_curvesToShow.append("posZ_KF");
            }
        }

    }



    else{
        qDebug()<<"currentText is:"<<m_currentText;
        QMessageBox::critical(this,tr("错误"),"未知数据类型");
        return false;
    }
    for(auto i:m_curvesToShow){
        qDebug()<<"m_curvesToShow List"<<i;
    }
    setCurvesToShow();
    return true;
}

/**
 * @brief 开始采集
 */
void ImuPage::on_startCollectionButton_clicked()
{
//    //清空绘图
//    waveformPlot->clear();
    //先清除连接
    disconnect(dataL,&DataProcess::imu_frame_ok,this,&ImuPage::refreshData);
    //连接信号
    connect(dataL,&DataProcess::imu_frame_ok,this,&ImuPage::refreshData);
    //开始绘图
    waveformPlot->start(8000);
    for(auto i:m_curvesToShow){
        qDebug()<<"Curves To Show is:"<<i;
    }
}

/**
 * @brief 停止采集
 */
void ImuPage::on_stopCollectionButton_clicked()
{
    //停止绘图
    waveformPlot->stop();
    //清除连接
    disconnect(dataL,&DataProcess::imu_frame_ok,this,&ImuPage::refreshData);
}

/**
 * @brief 导出数据
 */
void ImuPage::on_exportDataButton_clicked()
{
    //导出数据

}

/**
 * @brief 根据列表添加曲线
 */
void ImuPage::addTargetedDataSingleFrame(DataProcess::SingleFrameIMU frameData){

}


/**
 * @brief 设置是否显示卡尔曼滤波后的曲线
 */
void ImuPage::on_KalmanFilteredCheckBox_stateChanged(int arg1)
{

}

