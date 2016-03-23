/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Apr 10 12:32:55 2015
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *openButton;
    QPushButton *runButton;
    QLabel *pic_label;
    QLineEdit *resultLineEdit;
    QLabel *label_2;
    QLabel *cam_label;
    QPushButton *open_cam;
    QPushButton *take_pic;
    QPushButton *close_cam;
    QLabel *label;
    QLabel *label_3;
    QLabel *temp_label;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1057, 635);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        openButton = new QPushButton(centralWidget);
        openButton->setObjectName(QString::fromUtf8("openButton"));
        openButton->setGeometry(QRect(210, 0, 75, 23));
        runButton = new QPushButton(centralWidget);
        runButton->setObjectName(QString::fromUtf8("runButton"));
        runButton->setGeometry(QRect(300, 0, 75, 23));
        runButton->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 127);\n"
"color: rgb(0, 255, 255);"));
        pic_label = new QLabel(centralWidget);
        pic_label->setObjectName(QString::fromUtf8("pic_label"));
        pic_label->setGeometry(QRect(10, 30, 640, 480));
        resultLineEdit = new QLineEdit(centralWidget);
        resultLineEdit->setObjectName(QString::fromUtf8("resultLineEdit"));
        resultLineEdit->setGeometry(QRect(60, 0, 113, 20));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 0, 54, 12));
        cam_label = new QLabel(centralWidget);
        cam_label->setObjectName(QString::fromUtf8("cam_label"));
        cam_label->setGeometry(QRect(680, 30, 320, 240));
        open_cam = new QPushButton(centralWidget);
        open_cam->setObjectName(QString::fromUtf8("open_cam"));
        open_cam->setGeometry(QRect(460, 0, 75, 23));
        take_pic = new QPushButton(centralWidget);
        take_pic->setObjectName(QString::fromUtf8("take_pic"));
        take_pic->setGeometry(QRect(550, 0, 75, 23));
        take_pic->setStyleSheet(QString::fromUtf8("color: rgb(255, 74, 19);\n"
"background-color: rgb(0, 0, 0);"));
        close_cam = new QPushButton(centralWidget);
        close_cam->setObjectName(QString::fromUtf8("close_cam"));
        close_cam->setGeometry(QRect(640, 0, 75, 23));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(900, 460, 131, 51));
        label->setStyleSheet(QString::fromUtf8("font: 11pt \"Sans Serif\";\n"
"color: rgb(255, 0, 0);\n"
"font: 14pt \"Sans Serif\";"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(940, 500, 121, 31));
        label_3->setStyleSheet(QString::fromUtf8("font: 11pt \"Sans Serif\";\n"
"color: rgb(255, 0, 0);"));
        temp_label = new QLabel(centralWidget);
        temp_label->setObjectName(QString::fromUtf8("temp_label"));
        temp_label->setGeometry(QRect(690, 280, 331, 241));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1057, 25));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        openButton->setText(QApplication::translate("MainWindow", "open_pic", 0, QApplication::UnicodeUTF8));
        runButton->setText(QApplication::translate("MainWindow", "run", 0, QApplication::UnicodeUTF8));
        pic_label->setText(QString());
        label_2->setText(QApplication::translate("MainWindow", "result", 0, QApplication::UnicodeUTF8));
        cam_label->setText(QApplication::translate("MainWindow", "cam", 0, QApplication::UnicodeUTF8));
        open_cam->setText(QApplication::translate("MainWindow", "open_cam", 0, QApplication::UnicodeUTF8));
        take_pic->setText(QApplication::translate("MainWindow", "take_pic", 0, QApplication::UnicodeUTF8));
        close_cam->setText(QApplication::translate("MainWindow", "close_cam", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "ANPR", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "By SDU", 0, QApplication::UnicodeUTF8));
        temp_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
