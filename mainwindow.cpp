#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->date_sortie->setDate(QDate::currentDate());
    ConnectDB();
}

void MainWindow::save()
{

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ConnectDB()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(Readconfig("HOST"));
    db.setDatabaseName(Readconfig("NAME"));
    db.setUserName(Readconfig("USER"));
    db.setPassword(Readconfig("PASSWORD"));
    if(db.open()){ui->connect->setChecked(true);}
    else{ui->connect->setChecked(false);}
}

QString MainWindow::Readconfig(string paramName)
{
    std::ifstream cfg("config.txt");
    if(!cfg.is_open())
    {
        cout << "failed" << endl;
        return 0;
    }
    string parm, value;
    while (cfg >> parm >> value)
    {
        if(parm== paramName){return QString::fromStdString(value);}
    }
    return 0;
}
