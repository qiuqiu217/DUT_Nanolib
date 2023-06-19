#ifndef BLEPAGE_H
#define BLEPAGE_H

#include <QWidget>
#include <QListWidget>
#include <Qmessagebox.h>
#include <QTimer>
#include "bluetooth/dataprocess.h"


namespace Ui {
class BlePage;
}




class BlePage : public QWidget
{
    Q_OBJECT

public:
    explicit BlePage(QWidget *parent = nullptr);
    ~BlePage();
    static QObject *parent;






private slots:


signals:



private:
    Ui::BlePage *ui;

    QTimer *readChrTimer;


};





#endif // BLEPAGE_H
