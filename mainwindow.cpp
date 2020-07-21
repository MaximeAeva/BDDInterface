#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->date_sortie->setDate(QDate::currentDate());
    connectDB();
}

void MainWindow::save()
{

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectDB()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("Sidel");
    db.setUserName("root");
    db.setPassword("GEBO0388");
    if(db.open()){ui->connect->setChecked(true);}
    else{ui->connect->setChecked(false);}
}
