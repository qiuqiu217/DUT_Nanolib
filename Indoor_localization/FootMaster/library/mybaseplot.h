#ifndef MYBASEPLOT_H
#define MYBASEPLOT_H

#include "library/qcustomplot.h"

class MyBasePlot : public QCustomPlot
{
public:



    QMap<QString, QPointer<QCPCurve>> m_curves;//使用一个map管理各条曲线
    int m_range ;//绘制点的数量范围，超范围自动覆盖
    //颜色序列
    //These colors include red🟥, orange🟧,hot pink💗, green🟩, blue🟦, indigo🟪, violet🟣, brown🟫, pink🎀,  yellow🟨,
    //pale violet red💕, light steel blue👖, dodger blue🧢, deep sky blue🏊‍♂️, aquamarine🏊‍♀️, pale green🌱, khaki👖 and wheat🌾.
    QVector<QColor> baseColors = { QColor(255, 0, 0), QColor(255, 165, 0),QColor(219, 112, 147),
                                   QColor(0, 128, 0), QColor(0, 0, 255), QColor(75, 0, 130),
                                   QColor(238, 130, 238), QColor(139, 69, 19), QColor(255, 255, 0),
                                   QColor(255, 105, 180),QColor(255, 192, 203) , QColor(176, 196, 222),
                                   QColor(30, 144, 255), QColor(0, 191, 255), QColor(127, 255, 212),
                                   QColor(152, 251, 152), QColor(240,230 ,140 ),QColor(245 ,222 ,179 )};



    uint8_t m_colorindex;//颜色序号
    int m_width;//曲线宽度


    MyBasePlot(QObject *parent = nullptr);

    void start(int range);
    void stop();
    bool clear();

    void removeDataBefore(int size);
    bool addCurve(QString curve_name);
    bool deleteCurve(QString curve_name);
    bool addData(QString curve_name, double x, double y);
    bool setVisibilityList(QList<QString> curves);

private slots:
   void tick();


private:
    QTimer* m_tickTimer;//绘图定时器






};

#endif // MYBASEPLOT_H
