#ifndef PLOTHANDLER_H
#define PLOTHANDLER_H
#include "qcustomplot.h"

class plotHandler
{
public:
    plotHandler();
    static void setData(QCPGraph* graph, const QVector<int>& keys, const QVector<float>& values, bool alreadySorted = false);
};

#endif // PLOTHANDLER_H
