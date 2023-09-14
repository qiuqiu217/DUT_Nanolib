#include "filedialog.h"


/**
* @brief Save_FileDialog 构造函数
* @param QTextBrowser *textBrowser 接收区指针, QWidget *parent 父窗口, QSize dialog_size 对话框大小, QString name 对话框名称
* @retval None
*/
Save_FileDialog::Save_FileDialog(QTextBrowser *textBrowser, QWidget *parent, QSize dialog_size, QString name)
    : AppWindow(parent, dialog_size, name), textBrowser(textBrowser) {

    this->setWindowFlags(Qt::WindowStaysOnTopHint);     //设置窗口始终在顶部
    this->setObjectName("fileDialog");
    this->setStyleSheet("#fileDialog { border: 3px solid #373636; background-color: white; }");

    QVBoxLayout *vDialogLayout = new QVBoxLayout(this->get_cWidgetptr());
    vDialogLayout->setContentsMargins(5, 5, 5, 5);
    vDialogLayout->setSpacing(5);
    model.setRootPath(QDir::homePath());
    treeView.setModel(&model);
    treeView.setRootIndex(model.index(QDir::homePath()));
    connect(&treeView, &QTreeView::clicked, this, &Save_FileDialog::updateLabel);
    QPushButton *saveButton = new QPushButton("保存", this);
    connect(saveButton, &QPushButton::clicked, this, &Save_FileDialog::selectDirectory);
    QPushButton *cancelButton = new QPushButton("取消", this);
    connect(cancelButton, &QPushButton::clicked, this, &Save_FileDialog::close);

    QWidget* pathWidget = new QWidget(this);
    QWidget* nameWidget = new QWidget(this);
    QWidget* saveWidget = new QWidget(this);
    QLabel* pathLabel = new QLabel(this);
    QLabel* nameLabel = new QLabel(this);
    QHBoxLayout *hPathLayout = new QHBoxLayout(pathWidget);
    hPathLayout->setContentsMargins(10, 1, 25, 1);
    hPathLayout->setSpacing(10);
    QHBoxLayout *hNameLayout = new QHBoxLayout(nameWidget);
    hNameLayout->setContentsMargins(10, 1, 25, 1);
    hNameLayout->setSpacing(10);
    QHBoxLayout *hSaveLayout = new QHBoxLayout(saveWidget);
    hSaveLayout->setContentsMargins(10, 1, 10, 1);
    hSaveLayout->setSpacing(20);
    pathLabel->setText("当前路径：");
    hPathLayout->addWidget(pathLabel);
    hPathLayout->addWidget(&path_Label);
    nameLabel->setText("当前选中文件：");
    hNameLayout->addWidget(nameLabel);
    hNameLayout->addWidget(&filenameEdit);
    hSaveLayout->addWidget(cancelButton);
    hSaveLayout->addWidget(saveButton);

    QFont font;        //设置底部栏字体
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(this->height()/20);
    cancelButton->setFont(font);
    cancelButton->setFixedSize(this->width()/3, this->height()/15);
    saveButton->setFont(font);
    saveButton->setFixedSize(this->width()/3, this->height()/15);

    pathLabel->setFont(font);
    nameLabel->setFont(font);
    path_Label.setFont(font);
    path_Label.setFixedSize(this->width()*2/3, this->height()/15);
    filenameEdit.setFont(font);
    filenameEdit.setText("test");
    filenameEdit.setFixedSize(this->width()*2/3, this->height()/15);

    vDialogLayout->addWidget(&treeView);
    vDialogLayout->setStretchFactor(&treeView, 10);
    vDialogLayout->addWidget(pathWidget);
    vDialogLayout->setStretchFactor(pathWidget, 1);
    vDialogLayout->addWidget(nameWidget);
    vDialogLayout->setStretchFactor(nameWidget, 1);
    vDialogLayout->addWidget(saveWidget);
    vDialogLayout->setStretchFactor(saveWidget, 1);
}

/**
* @brief Save_FileDialog 析构函数
* @param None
* @retval None
*/
Save_FileDialog::~Save_FileDialog() {
}

/**
* @brief Save_FileDialog::selectDirectory 路径选择槽函数
* @param None
* @retval None
*/
void Save_FileDialog::selectDirectory() {
    auto selection = treeView.selectionModel()->selectedIndexes();
    if (!selection.isEmpty()) {
        QString path = model.filePath(selection.first());
        qDebug() << "Selected directory:" << path;
        emit directorySelected(path);
        saveTextToFile(path);
    }
}

/**
* @brief Save_FileDialog::updateLabel 更新路径文本槽函数
* @param const QModelIndex &index   视图中索引
* @retval None
*/
void Save_FileDialog::updateLabel(const QModelIndex &index) {
    QString path = model.filePath(index);
    path_Label.setText(path);
}

/**
* @brief Save_FileDialog::saveTextToFile  保存文件槽函数
* @param const QString &path 文件路径
* @retval None
*/
void Save_FileDialog::saveTextToFile(const QString &path) {
    QString filename = filenameEdit.text();
    if (!filename.isEmpty()) {
        QFile file(path + "/" + filename + ".txt");
        if (file.exists()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "文件存在", "文件已经存在。你想要覆盖它吗？",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::No) {
                return;
            }
        }
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << textBrowser->toPlainText();
            file.close();
            qDebug() << "File saved:" << file.fileName();
        }
        else {
            qDebug() << "Error saving file:" << file.errorString();
        }
        this->close();  // 关闭窗口
    }
}

/**
* @brief Save_FileDialog::mousePressEvent  鼠标点击事件
* @param QMouseEvent *event
* @retval None
*/
void Save_FileDialog::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->pos().x();
    m_nMouseClick_Y_Coordinate = event->pos().y();
}

/**
* @brief Save_FileDialog::mouseMoveEvent  鼠标移动事件
* @param QMouseEvent *event
* @retval None
*/
void Save_FileDialog::mouseMoveEvent(QMouseEvent *event) {
    move(mapToParent(event->pos()) - QPoint(m_nMouseClick_X_Coordinate, m_nMouseClick_Y_Coordinate));
}

/**
* @brief Load_FileDialog 构造函数
* @param QTextEdit *textEdit  发送区指针, QWidget *parent 父窗口, QSize dialog_size 对话框大小, QString name 对话框名称
* @retval None
*/
Load_FileDialog::Load_FileDialog(QTextEdit *textEdit, QWidget *parent, QSize dialog_size, QString name)
    : AppWindow(parent, dialog_size, name), textEdit(textEdit) {

    this->setWindowFlags(Qt::WindowStaysOnTopHint);     //设置窗口始终在顶部
    this->setObjectName("fileDialog");
    this->setStyleSheet("#fileDialog { border: 3px solid #373636; background-color: white; }");

    QVBoxLayout *vDialogLayout = new QVBoxLayout(this->get_cWidgetptr());
    vDialogLayout->setContentsMargins(5, 5, 5, 5);
    vDialogLayout->setSpacing(5);
    model.setRootPath(QDir::homePath());
    treeView.setModel(&model);
    treeView.setRootIndex(model.index(QDir::homePath()));
    connect(&treeView, &QTreeView::clicked, this, &Load_FileDialog::updateLabel);
    QPushButton *loadButton = new QPushButton("加载", this);
    connect(loadButton, &QPushButton::clicked, this, &Load_FileDialog::selectFile);
    QPushButton *cancelButton = new QPushButton("取消", this);
    connect(cancelButton, &QPushButton::clicked, this, &Load_FileDialog::close);

    QWidget* pathWidget = new QWidget(this);
    QWidget* nameWidget = new QWidget(this);
    QWidget* saveWidget = new QWidget(this);
    QLabel* pathLabel = new QLabel(this);
    QLabel* nameLabel = new QLabel(this);
    QHBoxLayout *hPathLayout = new QHBoxLayout(pathWidget);
    hPathLayout->setContentsMargins(10, 1, 25, 1);
    hPathLayout->setSpacing(10);
    QHBoxLayout *hNameLayout = new QHBoxLayout(nameWidget);
    hNameLayout->setContentsMargins(10, 1, 25, 1);
    hNameLayout->setSpacing(10);
    QHBoxLayout *hSaveLayout = new QHBoxLayout(saveWidget);
    hSaveLayout->setContentsMargins(10, 1, 10, 1);
    hSaveLayout->setSpacing(20);
    pathLabel->setText("当前路径：");
    hPathLayout->addWidget(pathLabel);
    hPathLayout->addWidget(&path_Label);
    nameLabel->setText("保存文件名称：");
    hNameLayout->addWidget(nameLabel);
    hNameLayout->addWidget(&doc_Label);
    hSaveLayout->addWidget(cancelButton);
    hSaveLayout->addWidget(loadButton);

    QFont font;        //设置底部栏字体
    font.setFamily("Microsoft YaHei");
    font.setPixelSize(this->height()/20);
    cancelButton->setFont(font);
    cancelButton->setFixedSize(this->width()/3, this->height()/15);
    loadButton->setFont(font);
    loadButton->setFixedSize(this->width()/3, this->height()/15);

    pathLabel->setFont(font);
    nameLabel->setFont(font);
    path_Label.setFont(font);
    path_Label.setFixedSize(this->width()*2/3, this->height()/15);
    doc_Label.setFont(font);
    doc_Label.setFixedSize(this->width()*2/3, this->height()/15);

    vDialogLayout->addWidget(&treeView);
    vDialogLayout->setStretchFactor(&treeView, 10);
    vDialogLayout->addWidget(pathWidget);
    vDialogLayout->setStretchFactor(pathWidget, 1);
    vDialogLayout->addWidget(nameWidget);
    vDialogLayout->setStretchFactor(nameWidget, 1);
    vDialogLayout->addWidget(saveWidget);
    vDialogLayout->setStretchFactor(saveWidget, 1);
}

/**
* @brief Load_FileDialog 析构函数
* @param None
* @retval None
*/
Load_FileDialog::~Load_FileDialog() {
}

/**
* @brief Load_FileDialog::selectFile 路径选择槽函数
* @param None
* @retval None
*/
void Load_FileDialog::selectFile() {
    auto selection = treeView.selectionModel()->selectedIndexes();
    if (!selection.isEmpty()) {
        QString path = model.filePath(selection.first());
        QFileInfo check_file(path);
        if (check_file.exists() && check_file.isFile() && check_file.suffix() == "txt") {
            qDebug() << "Selected file:" << path;
            emit fileSelected(path);
            loadTextFromFile(path);
        }
        else {
            qDebug() << "Selected file is not a txt file";
             QMessageBox::warning(this, "无效文件类型","仅支持加载.txt文件");
        }
    }
}

/**
* @brief Load_FileDialog::updateLabel 更新路径文本槽函数
* @param const QModelIndex &index   视图中索引
* @retval None
*/
void Load_FileDialog::updateLabel(const QModelIndex &index) {
    QString path = model.filePath(index);
    path_Label.setText(path);

    QFileInfo fileInfo(path);
    if (fileInfo.isFile()) {
        doc_Label.setText(fileInfo.fileName());
    }
    else {
        doc_Label.setText("");
    }
}

/**
* @brief Load_FileDialog::loadTextFromFile  加载文件槽函数
* @param const QString &path 文件路径
* @retval None
*/
void Load_FileDialog::loadTextFromFile(const QString &path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        textEdit->setText(in.readAll());
        file.close();
        qDebug() << "File loaded:" << file.fileName();
    }
    else {
        qDebug() << "Error loading file:" << file.errorString();
    }
    this->close();  //关闭窗口
}

/**
* @brief Load_FileDialog::mousePressEvent  鼠标点击事件
* @param QMouseEvent *event
* @retval None
*/
void Load_FileDialog::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->pos().x();
    m_nMouseClick_Y_Coordinate = event->pos().y();
}

/**
* @brief Load_FileDialog::mouseMoveEvent  鼠标移动事件
* @param QMouseEvent *event
* @retval None
*/
void Load_FileDialog::mouseMoveEvent(QMouseEvent *event) {
    move(mapToParent(event->pos()) - QPoint(m_nMouseClick_X_Coordinate, m_nMouseClick_Y_Coordinate));
}
