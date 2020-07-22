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
#include <fstream>
#include <string>

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
    void save();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    void ConnectDB();
    void InitialState();
    void CameraReg(QString machine, QString marque, QString modele);
    QString Readconfig(string paramName);
};
#endif // MAINWINDOW_H
