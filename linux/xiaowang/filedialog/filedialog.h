#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "mainwindow.h"
#include "app_window.h"

/*
*********************************************************************************************************
*                                               类声明
*********************************************************************************************************
*/
class Save_FileDialog : public AppWindow
{
    Q_OBJECT

public:
    Save_FileDialog(QTextBrowser *textBrowser, QWidget *parent, QSize dialog_size, QString name);
    ~Save_FileDialog();

public slots:
    void selectDirectory();
    void updateLabel(const QModelIndex &index);
    void saveTextToFile(const QString &path);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void directorySelected(const QString &path);

private:
    QFileSystemModel model;
    QTreeView treeView;
    QLabel path_Label;
    QLineEdit filenameEdit;         //QLineEdit for the user to enter the filename
    QTextBrowser *textBrowser;      //pointer to the QTextEdit that contains the text to save
    int m_nMouseClick_X_Coordinate,m_nMouseClick_Y_Coordinate;
};



class Load_FileDialog : public AppWindow
{
    Q_OBJECT

public:
    Load_FileDialog(QTextEdit *textEdit, QWidget *parent, QSize dialog_size, QString name);
    ~Load_FileDialog();

public slots:
    void selectFile();
    void updateLabel(const QModelIndex &index);
    void loadTextFromFile(const QString &path);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void fileSelected(const QString &path);

private:
    QFileSystemModel model;
    QTreeView treeView;
    QLabel path_Label;
    QLabel doc_Label;
    QTextEdit *textEdit;  // pointer to the QTextEdit that will be filled with the text
    int m_nMouseClick_X_Coordinate,m_nMouseClick_Y_Coordinate;
};









#endif
