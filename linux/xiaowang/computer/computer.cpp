#include "computer.h"
#include "app_window.h"

/**
* @brief App_Computer_Create构造函数
* @param None
* @retval None
*/
App_Computer_Create::App_Computer_Create(Application_List& list, int index) {
    qDebug()<<"App_Computer_Create"<<endl;
    AppInfo computerInfo;
    list.Copy_Listnode(index, computerInfo);
    computerInfo.objectptr = this;
    list.Update_Listnode(index, computerInfo);
}

/**
* @brief App_Computer_Create构造函数
* @param None
* @retval None
*/
int App_Computer_Create::appWindow_Create(QWidget *parent, QSize screen_size, QString name) {

    /* 创建应用窗口 */
    AppWindow *window = new AppWindow(parent, screen_size, name);       //创建一个新的窗口
    window->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    window->show();     //显示窗口

    /* 绘制应用中心窗口 */
    module_detail[3] = QString("%1 x %2").arg(screen_size.width()).arg(screen_size.height());    //初始化分辨率信息
    QWidget* cwindow = window->get_cWidgetptr();        //获取cwindow指针
    QPalette pal = cwindow ->palette();     //设置背景色
    pal.setColor(QPalette::Background,  "#000c66");
    cwindow ->setAutoFillBackground(true);
    cwindow ->setPalette(pal);
    QGridLayout *cwindow_layout = new QGridLayout(cwindow);        //为cwindow设置一个布局
    cwindow_layout->setContentsMargins(cwindow->height()/30, cwindow->height()/30, cwindow->height()/30, cwindow->height()/30);
    cwindow_layout->setHorizontalSpacing(cwindow->width()/6);
    cwindow_layout->setVerticalSpacing(cwindow->height()/32);
    cwindow_layout->setAlignment(Qt::AlignCenter);      //设置应用名称居中
    for (int i = 0; i < 4; i++) {       //添加系统信息模块
        /* 设置模块间布局 */
        QLabel* module_label = new QLabel(cwindow);
        module_label->setFixedSize(cwindow->width()/4, cwindow->height()*2/5);
        /* 绘制模块图形 */
        QLabel* pixmap_label = new QLabel(cwindow);
        pixmap_label->setAlignment(Qt::AlignCenter);
        QHBoxLayout* module_layout = new QHBoxLayout(module_label);     //创建桌面布局，分为上中下三部分
        module_layout->setContentsMargins(0, 0, 0, 0);
        module_layout->setSpacing(0);
        pixmap_array[i].load(path_array[i]);
        if(pixmap_array[i].isNull()) {
            qDebug()<<"Cannot found image! Invalid path"<<path_array[i]<<endl;
        }
        else {
            pixmap_array[i] = pixmap_array[i].scaled(module_label->height()*2/3, module_label->height()*2/3, Qt::KeepAspectRatio);
            pixmap_label->setPixmap(pixmap_array[i]);
        }
        /* 绘制模块文本 */
        QVBoxLayout* module_rlayout = new QVBoxLayout;
        module_rlayout->setContentsMargins(0, 0, 0, 0);
        module_rlayout->setSpacing(0);
        QLabel* mname_label = new QLabel;
        mname_label->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        QFont font = mname_label->font();
        font.setFamily("Microsoft YaHei");
        font.setPixelSize(cwindow->height()/30);
        mname_label->setFont(font);
        QPalette palette = mname_label->palette();
        palette.setColor(QPalette::WindowText, QColor("#dadada"));
        mname_label->setPalette(palette);
        mname_label->setText(module_name[i]);
        QLabel* mdetail_label = new QLabel;
        mdetail_label->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        QFont font1 = mdetail_label->font();
        font1.setFamily("Microsoft YaHei");
        font1.setPixelSize(cwindow->height()/40);
        mdetail_label->setFont(font1);
        mdetail_label->setPalette(palette);
        mdetail_label->setText(module_detail[i]);
        /* 设置模块中布局 */
        module_layout->addWidget(pixmap_label);
        module_rlayout->addWidget(mname_label);
        module_rlayout->addWidget(mdetail_label);
        module_layout->addLayout(module_rlayout);
        module_label->setLayout(module_layout);
        //module_label->setStyleSheet("border: 2px solid red;");
        cwindow_layout->addWidget(module_label, i%2, i/2);

    }

    return RET_OK;
}
