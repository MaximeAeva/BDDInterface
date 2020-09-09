#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <iostream>
#include <QDate>
#include <QSqlQuery>
#include <QtSql>
#include <QString>
#include <QSqlError>
#include <QtWebEngineWidgets>
#include <QWebEngineView>
#include <QUrl>
#include <fstream>
#include <cstring>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void saveMachine();
    void saveProbleme();
    void saveCamera();
    void update(const QModelIndex &index);
    void manageSelector();
    void adaptiveDisplay();
    void listItems();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    void ConnectDB();
    void InitialState();
    void Connections();
    void webPage();
    void CameraReg(QString machine, QString marque, QString modele);
    QString Readconfig(string paramName);
};
#endif // MAINWINDOW_H
