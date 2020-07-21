#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->date_sortie->setDate(QDate::currentDate());
    ConnectDB();
    cout << Readconfig("HOST") << endl;

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
    db.setHostName("localhost");
    db.setDatabaseName("Sidel");
    db.setUserName("root");
    db.setPassword("GEBO0388");
    if(db.open()){ui->connect->setChecked(true);}
    else{ui->connect->setChecked(false);}
}

string MainWindow::Readconfig(string paramName)
{
    std::ifstream cfg("C:/Users/maxime.bellot/Documents/build-SidelParcInterface-Desktop_x86_windows_msvc2019_pe_64bit-Debug/debug/config.txt");
    if(!cfg.is_open())
    {
        cout << "failed" << endl;
        return 0;
    }
    string parm, value;
    while (cfg >> parm >> value)
    {
        if(parm== paramName){return value;}
    }
    return 0;
}
