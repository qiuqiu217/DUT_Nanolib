#ifndef MYPLANTARHEATMAP_H
#define MYPLANTARHEATMAP_H

#include "library/qcustomplot.h"
#include <QTimer>

class myPlantarHeatmap : public QCustomPlot
{
public:





    explicit myPlantarHeatmap(QObject *parent = nullptr);
    void start();
    void stop();
    void clear();
    void refreshData(QList<float> frameData );

private:
    QCPColorMap *colorMap;
    QCPColorScale *colorScale;
    QTimer *tickTimer;
    void PlotInit();
    void tick();



};

#endif // MYPLANTARHEATMAP_H
