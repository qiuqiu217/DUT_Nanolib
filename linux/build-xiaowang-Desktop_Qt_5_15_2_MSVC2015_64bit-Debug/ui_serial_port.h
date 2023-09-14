/********************************************************************************
** Form generated from reading UI file 'serial_port.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERIAL_PORT_H
#define UI_SERIAL_PORT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *main_widget;
    QHBoxLayout *horizontalLayout_2;
    QWidget *settings_widget;
    QVBoxLayout *verticalLayout_3;
    QWidget *serial_swidget;
    QFormLayout *formLayout;
    QLabel *serial_label;
    QLabel *com_label;
    QComboBox *com_combobox;
    QLabel *baud_label;
    QComboBox *baud_combobox;
    QLabel *databit_label;
    QComboBox *databit_combobox;
    QLabel *stopbit_label;
    QComboBox *stopbit_combobox;
    QLabel *checkbit_label;
    QComboBox *checkbit_combobox;
    QLabel *motion_label;
    QPushButton *motion_button;
    QWidget *rx_swidget;
    QFormLayout *formLayout_2;
    QRadioButton *rxascii_radio;
    QRadioButton *rxhex_radio;
    QPushButton *rxclear_button;
    QPushButton *rxsave_button;
    QLabel *rx_label;
    QWidget *tx_swidget;
    QFormLayout *formLayout_3;
    QLabel *period_label;
    QWidget *period_widget;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *period_lineEdit;
    QLabel *ms_label;
    QRadioButton *txascii_radio;
    QRadioButton *txhex_radio;
    QCheckBox *newline_checkbox;
    QCheckBox *autosend_checkbox;
    QLabel *tx_label;
    QWidget *display_widget;
    QVBoxLayout *verticalLayout_2;
    QTextBrowser *rx_textbrowser;
    QWidget *tx_widget;
    QVBoxLayout *verticalLayout_4;
    QTextEdit *tx_textedit;
    QWidget *tx_sbwidget;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *txhistory_combobox;
    QPushButton *txload_button;
    QPushButton *txsend_button;
    QWidget *bottom_widget;
    QHBoxLayout *horizontalLayout;
    QLabel *welcome_label;
    QLabel *txnum_label;
    QLabel *rxnum_label;
    QPushButton *clear_button;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->setWindowModality(Qt::NonModal);
        Form->resize(1024, 717);
        verticalLayout = new QVBoxLayout(Form);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        main_widget = new QWidget(Form);
        main_widget->setObjectName(QString::fromUtf8("main_widget"));
        horizontalLayout_2 = new QHBoxLayout(main_widget);
        horizontalLayout_2->setSpacing(10);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(10, 10, 10, 10);
        settings_widget = new QWidget(main_widget);
        settings_widget->setObjectName(QString::fromUtf8("settings_widget"));
        verticalLayout_3 = new QVBoxLayout(settings_widget);
        verticalLayout_3->setSpacing(5);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout_3->setContentsMargins(2, 2, 2, 2);
        serial_swidget = new QWidget(settings_widget);
        serial_swidget->setObjectName(QString::fromUtf8("serial_swidget"));
        serial_swidget->setStyleSheet(QString::fromUtf8("#serial_swidget {\n"
"    border: 2px solid black;\n"
"}\n"
""));
        formLayout = new QFormLayout(serial_swidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setHorizontalSpacing(5);
        formLayout->setVerticalSpacing(5);
        formLayout->setContentsMargins(8, 8, 8, 8);
        serial_label = new QLabel(serial_swidget);
        serial_label->setObjectName(QString::fromUtf8("serial_label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, serial_label);

        com_label = new QLabel(serial_swidget);
        com_label->setObjectName(QString::fromUtf8("com_label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, com_label);

        com_combobox = new QComboBox(serial_swidget);
        com_combobox->setObjectName(QString::fromUtf8("com_combobox"));

        formLayout->setWidget(1, QFormLayout::FieldRole, com_combobox);

        baud_label = new QLabel(serial_swidget);
        baud_label->setObjectName(QString::fromUtf8("baud_label"));

        formLayout->setWidget(2, QFormLayout::LabelRole, baud_label);

        baud_combobox = new QComboBox(serial_swidget);
        baud_combobox->setObjectName(QString::fromUtf8("baud_combobox"));

        formLayout->setWidget(2, QFormLayout::FieldRole, baud_combobox);

        databit_label = new QLabel(serial_swidget);
        databit_label->setObjectName(QString::fromUtf8("databit_label"));

        formLayout->setWidget(3, QFormLayout::LabelRole, databit_label);

        databit_combobox = new QComboBox(serial_swidget);
        databit_combobox->setObjectName(QString::fromUtf8("databit_combobox"));

        formLayout->setWidget(3, QFormLayout::FieldRole, databit_combobox);

        stopbit_label = new QLabel(serial_swidget);
        stopbit_label->setObjectName(QString::fromUtf8("stopbit_label"));

        formLayout->setWidget(4, QFormLayout::LabelRole, stopbit_label);

        stopbit_combobox = new QComboBox(serial_swidget);
        stopbit_combobox->setObjectName(QString::fromUtf8("stopbit_combobox"));

        formLayout->setWidget(4, QFormLayout::FieldRole, stopbit_combobox);

        checkbit_label = new QLabel(serial_swidget);
        checkbit_label->setObjectName(QString::fromUtf8("checkbit_label"));

        formLayout->setWidget(5, QFormLayout::LabelRole, checkbit_label);

        checkbit_combobox = new QComboBox(serial_swidget);
        checkbit_combobox->setObjectName(QString::fromUtf8("checkbit_combobox"));

        formLayout->setWidget(5, QFormLayout::FieldRole, checkbit_combobox);

        motion_label = new QLabel(serial_swidget);
        motion_label->setObjectName(QString::fromUtf8("motion_label"));

        formLayout->setWidget(6, QFormLayout::LabelRole, motion_label);

        motion_button = new QPushButton(serial_swidget);
        motion_button->setObjectName(QString::fromUtf8("motion_button"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(motion_button->sizePolicy().hasHeightForWidth());
        motion_button->setSizePolicy(sizePolicy);
        motion_button->setFocusPolicy(Qt::ClickFocus);

        formLayout->setWidget(6, QFormLayout::FieldRole, motion_button);


        verticalLayout_3->addWidget(serial_swidget);

        rx_swidget = new QWidget(settings_widget);
        rx_swidget->setObjectName(QString::fromUtf8("rx_swidget"));
        rx_swidget->setStyleSheet(QString::fromUtf8("#rx_swidget{\n"
"    border: 2px solid black;\n"
"}\n"
""));
        formLayout_2 = new QFormLayout(rx_swidget);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setHorizontalSpacing(5);
        formLayout_2->setVerticalSpacing(5);
        formLayout_2->setContentsMargins(8, 8, 8, 8);
        rxascii_radio = new QRadioButton(rx_swidget);
        rxascii_radio->setObjectName(QString::fromUtf8("rxascii_radio"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, rxascii_radio);

        rxhex_radio = new QRadioButton(rx_swidget);
        rxhex_radio->setObjectName(QString::fromUtf8("rxhex_radio"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, rxhex_radio);

        rxclear_button = new QPushButton(rx_swidget);
        rxclear_button->setObjectName(QString::fromUtf8("rxclear_button"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, rxclear_button);

        rxsave_button = new QPushButton(rx_swidget);
        rxsave_button->setObjectName(QString::fromUtf8("rxsave_button"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, rxsave_button);

        rx_label = new QLabel(rx_swidget);
        rx_label->setObjectName(QString::fromUtf8("rx_label"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, rx_label);


        verticalLayout_3->addWidget(rx_swidget);

        tx_swidget = new QWidget(settings_widget);
        tx_swidget->setObjectName(QString::fromUtf8("tx_swidget"));
        tx_swidget->setStyleSheet(QString::fromUtf8("#tx_swidget {\n"
"    border: 2px solid black;\n"
"}\n"
""));
        formLayout_3 = new QFormLayout(tx_swidget);
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        formLayout_3->setHorizontalSpacing(5);
        formLayout_3->setVerticalSpacing(5);
        formLayout_3->setContentsMargins(8, 8, 8, 8);
        period_label = new QLabel(tx_swidget);
        period_label->setObjectName(QString::fromUtf8("period_label"));

        formLayout_3->setWidget(3, QFormLayout::LabelRole, period_label);

        period_widget = new QWidget(tx_swidget);
        period_widget->setObjectName(QString::fromUtf8("period_widget"));
        horizontalLayout_3 = new QHBoxLayout(period_widget);
        horizontalLayout_3->setSpacing(1);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        period_lineEdit = new QLineEdit(period_widget);
        period_lineEdit->setObjectName(QString::fromUtf8("period_lineEdit"));

        horizontalLayout_3->addWidget(period_lineEdit);

        ms_label = new QLabel(period_widget);
        ms_label->setObjectName(QString::fromUtf8("ms_label"));

        horizontalLayout_3->addWidget(ms_label);


        formLayout_3->setWidget(3, QFormLayout::FieldRole, period_widget);

        txascii_radio = new QRadioButton(tx_swidget);
        txascii_radio->setObjectName(QString::fromUtf8("txascii_radio"));

        formLayout_3->setWidget(4, QFormLayout::LabelRole, txascii_radio);

        txhex_radio = new QRadioButton(tx_swidget);
        txhex_radio->setObjectName(QString::fromUtf8("txhex_radio"));

        formLayout_3->setWidget(4, QFormLayout::FieldRole, txhex_radio);

        newline_checkbox = new QCheckBox(tx_swidget);
        newline_checkbox->setObjectName(QString::fromUtf8("newline_checkbox"));

        formLayout_3->setWidget(5, QFormLayout::LabelRole, newline_checkbox);

        autosend_checkbox = new QCheckBox(tx_swidget);
        autosend_checkbox->setObjectName(QString::fromUtf8("autosend_checkbox"));

        formLayout_3->setWidget(5, QFormLayout::FieldRole, autosend_checkbox);

        tx_label = new QLabel(tx_swidget);
        tx_label->setObjectName(QString::fromUtf8("tx_label"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, tx_label);


        verticalLayout_3->addWidget(tx_swidget);


        horizontalLayout_2->addWidget(settings_widget);

        display_widget = new QWidget(main_widget);
        display_widget->setObjectName(QString::fromUtf8("display_widget"));
        verticalLayout_2 = new QVBoxLayout(display_widget);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(2, 2, 2, 2);
        rx_textbrowser = new QTextBrowser(display_widget);
        rx_textbrowser->setObjectName(QString::fromUtf8("rx_textbrowser"));

        verticalLayout_2->addWidget(rx_textbrowser);

        tx_widget = new QWidget(display_widget);
        tx_widget->setObjectName(QString::fromUtf8("tx_widget"));
        verticalLayout_4 = new QVBoxLayout(tx_widget);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        tx_textedit = new QTextEdit(tx_widget);
        tx_textedit->setObjectName(QString::fromUtf8("tx_textedit"));

        verticalLayout_4->addWidget(tx_textedit);

        tx_sbwidget = new QWidget(tx_widget);
        tx_sbwidget->setObjectName(QString::fromUtf8("tx_sbwidget"));
        horizontalLayout_4 = new QHBoxLayout(tx_sbwidget);
        horizontalLayout_4->setSpacing(10);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, -1, 0, 11);
        txhistory_combobox = new QComboBox(tx_sbwidget);
        txhistory_combobox->setObjectName(QString::fromUtf8("txhistory_combobox"));
        txhistory_combobox->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_4->addWidget(txhistory_combobox);

        txload_button = new QPushButton(tx_sbwidget);
        txload_button->setObjectName(QString::fromUtf8("txload_button"));

        horizontalLayout_4->addWidget(txload_button);

        txsend_button = new QPushButton(tx_sbwidget);
        txsend_button->setObjectName(QString::fromUtf8("txsend_button"));

        horizontalLayout_4->addWidget(txsend_button);


        verticalLayout_4->addWidget(tx_sbwidget);

        verticalLayout_4->setStretch(0, 4);
        verticalLayout_4->setStretch(1, 1);

        verticalLayout_2->addWidget(tx_widget);

        verticalLayout_2->setStretch(0, 2);
        verticalLayout_2->setStretch(1, 1);

        horizontalLayout_2->addWidget(display_widget);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 5);

        verticalLayout->addWidget(main_widget);

        bottom_widget = new QWidget(Form);
        bottom_widget->setObjectName(QString::fromUtf8("bottom_widget"));
        horizontalLayout = new QHBoxLayout(bottom_widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(10, 2, 10, 2);
        welcome_label = new QLabel(bottom_widget);
        welcome_label->setObjectName(QString::fromUtf8("welcome_label"));

        horizontalLayout->addWidget(welcome_label);

        txnum_label = new QLabel(bottom_widget);
        txnum_label->setObjectName(QString::fromUtf8("txnum_label"));

        horizontalLayout->addWidget(txnum_label);

        rxnum_label = new QLabel(bottom_widget);
        rxnum_label->setObjectName(QString::fromUtf8("rxnum_label"));

        horizontalLayout->addWidget(rxnum_label);

        clear_button = new QPushButton(bottom_widget);
        clear_button->setObjectName(QString::fromUtf8("clear_button"));
        clear_button->setEnabled(true);
        clear_button->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(clear_button);

        horizontalLayout->setStretch(0, 2);
        horizontalLayout->setStretch(1, 2);
        horizontalLayout->setStretch(2, 2);
        horizontalLayout->setStretch(3, 1);

        verticalLayout->addWidget(bottom_widget);

        verticalLayout->setStretch(0, 15);
        verticalLayout->setStretch(1, 1);

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QCoreApplication::translate("Form", "Form", nullptr));
        serial_label->setText(QCoreApplication::translate("Form", "\344\270\262\345\217\243\351\205\215\347\275\256", nullptr));
        com_label->setText(QCoreApplication::translate("Form", "\347\253\257\345\217\243", nullptr));
        baud_label->setText(QCoreApplication::translate("Form", "\346\263\242\347\211\271\347\216\207", nullptr));
        databit_label->setText(QCoreApplication::translate("Form", "\346\225\260\346\215\256\344\275\215", nullptr));
        stopbit_label->setText(QCoreApplication::translate("Form", "\346\243\200\351\252\214\344\275\215", nullptr));
        checkbit_label->setText(QCoreApplication::translate("Form", "\345\201\234\346\255\242\344\275\215", nullptr));
        motion_label->setText(QCoreApplication::translate("Form", "\346\223\215\344\275\234", nullptr));
        motion_button->setText(QCoreApplication::translate("Form", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        rxascii_radio->setText(QCoreApplication::translate("Form", "ASCII", nullptr));
        rxhex_radio->setText(QCoreApplication::translate("Form", "HEX", nullptr));
        rxclear_button->setText(QCoreApplication::translate("Form", "\346\270\205\347\251\272\346\216\245\346\224\266\345\214\272", nullptr));
        rxsave_button->setText(QCoreApplication::translate("Form", "\344\277\235\345\255\230\345\210\260\346\226\207\344\273\266", nullptr));
        rx_label->setText(QCoreApplication::translate("Form", "\346\216\245\346\224\266\345\214\272\350\256\276\347\275\256", nullptr));
        period_label->setText(QCoreApplication::translate("Form", "\350\207\252\345\212\250\345\217\221\351\200\201\345\221\250\346\234\237", nullptr));
        ms_label->setText(QCoreApplication::translate("Form", "ms", nullptr));
        txascii_radio->setText(QCoreApplication::translate("Form", "ASCII", nullptr));
        txhex_radio->setText(QCoreApplication::translate("Form", "HEX", nullptr));
        newline_checkbox->setText(QCoreApplication::translate("Form", "\345\217\221\351\200\201\346\226\260\350\241\214", nullptr));
        autosend_checkbox->setText(QCoreApplication::translate("Form", "\350\207\252\345\212\250\345\217\221\351\200\201", nullptr));
        tx_label->setText(QCoreApplication::translate("Form", "\345\217\221\351\200\201\345\214\272\350\256\276\347\275\256", nullptr));
        txload_button->setText(QCoreApplication::translate("Form", "\345\212\240\350\275\275\346\226\207\344\273\266", nullptr));
        txsend_button->setText(QCoreApplication::translate("Form", "\345\217\221\351\200\201\346\225\260\346\215\256", nullptr));
        welcome_label->setText(QCoreApplication::translate("Form", "\346\254\242\350\277\216\344\275\277\347\224\250\344\270\262\345\217\243\350\260\203\350\257\225\345\212\251\346\211\213", nullptr));
        txnum_label->setText(QCoreApplication::translate("Form", "\345\217\221\351\200\201\345\255\227\350\212\202\346\225\260\357\274\2320", nullptr));
        rxnum_label->setText(QCoreApplication::translate("Form", "\346\216\245\346\224\266\345\255\227\350\212\202\346\225\260\357\274\2320", nullptr));
        clear_button->setText(QCoreApplication::translate("Form", "\346\270\205\347\251\272\350\256\241\346\225\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERIAL_PORT_H
