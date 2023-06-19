#ifndef PLANTARPAGE_H
#define PLANTARPAGE_H

#include <QWidget>
#include "library/qcustomplot.h"
#include "bluetooth/dataprocess.h"

namespace Ui {
class PlantarPage;
}

class PlantarPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlantarPage(QWidget *parent = nullptr);
    ~PlantarPage();

private slots:
    void on_startCollectionButton_clicked();
    void refreshData(unsigned int timestamp, QList<float> frameData);

private:
    Ui::PlantarPage *ui;
};

#endif // PLANTARPAGE_H
