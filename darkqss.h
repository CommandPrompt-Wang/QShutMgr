#ifndef DARKQSS_H
#define DARKQSS_H
#include <QString>

QString qss_Dark=
    "/*background*/"
    "* {"
    "background-color: rgb(30, 30, 30);"
    "color: rgb(0, 255, 0);"
    "}"

    "/*disabled*/"
    "*:disabled {"
    "color: rgb(200, 200, 200);"
    "}"

    "/*QLabel*/"
    "QLabel {"
    "border: none;"
    "}"

    "/*QLineEdit*/"
    "QLineEdit {"
    "border: 1px solid green;"
    "border-radius: 6px;"
    "}"

    "QLineEdit:hover {"
    "background-color: rgb(0, 150, 0);"
    "}"

    "/*QTextEdit,QTextBrowser,QListView*/"
    "QTextEdit,QTextBrowser,QListView {"
    "border: 1px solid green;"
    "border-radius: 6px;"
    "}"

    "/*QMenuBar,QMenu*/"
    "QMenuBar::item {"
    "border: none;"
    "}"

    "QMenuBar::item:selected,QMenu::item:selected {"
    "background-color: rgb(0, 150, 0);"
    "}"

    "/*QDateTimeEdit*/"
    "QDateTimeEdit {"
    "border: 1px solid green;"
    "border-radius: 6px;"
    "}"

    "QDateTimeEdit:hover {"
    "background-color: rgb(0, 150, 0);"
    "}"

    "/*QComboBox*/"
    "QComboBox {"
    "background-color: rgb(0, 150, 0);"
    "border: 1px solid green;"
    "border-radius: 6px;"
    "}"

    "/*QTabWidget*/"
    "QTabWidget:pane {"
    "border: none;"
    "}"

    "QTabBar::tab {"
    "background-color: rgb(30, 30, 30);"
    "}"

    "QTabBar::tab:selected {"
    "background-color: rgb(0, 80, 0);"
    "}"

    "QTabBar::tab:hover {"
    "background-color: rgb(0, 150, 0);"
    "}"

    "/*QPushButton*/"
    "QPushButton {"
    "background-color: rgb(0, 100, 0);"
    "}"

    "QPushButton:hover {"
    "background-color: rgb(0, 150, 0);"
    "}"

    "QPushButton:pressed {"
    "background-color: rgb(0, 100, 0);"
    "}"

    "/*QScrollBar*/"
    "QScrollBar::handle {"
    "background-color: rgb(0, 100, 0);"
    "}"

    "QScrollBar::add-page,QScrollBar::sub-page {"
    "background-color: rgb(50, 50, 50);"
    "}"

    "/*QToolTip*/"
    "QToolTip {"
    "color: black;"
    "}"

    "/*QGroupBox*/"
    "QGroupBox {"
    "padding-top: 12px;"
    "border: 1px solid green;"
    "border-radius: 6px;"
    "}"

    "/*QTableView*/"
    "QTableCornerButton::section,QHeaderView::section {"
    "background-color: rgb(30, 30, 30);"
    "border: 1px solid green;"
    "}"

    "QTableView {"
    "border: 1px solid green;"
    "border-radius: 6px;"
    "}"

    "QTableView::item {"
    "background-color: rgb(30, 30, 30);"
    "border: 1px solid green;"
    "}"

    "QTableView::item:selected {background-color: rgb(0, 80, 0);}"

               "QTableView::item:hover {background-color: rgb(0, 150, 0);}";
QString qss_Light="";

#endif // DARKQSS_H
