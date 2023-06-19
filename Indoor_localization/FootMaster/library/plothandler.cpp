#include "plothandler.h"

plotHandler::plotHandler()
{

}

void plotHandler::setData(QCPGraph* graph, const QVector<int>& keys, const QVector<float>& values, bool alreadySorted) {
    QVector<double> keysDouble;
    for (int key : keys) {
        keysDouble.push_back(static_cast<double>(key));
    }

    QVector<double> valuesDouble;
    for (float value : values) {
        valuesDouble.push_back(static_cast<double>(value));
    }

    graph->setData(keysDouble, valuesDouble, alreadySorted);
}

