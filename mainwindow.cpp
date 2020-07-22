#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitialState();
    ConnectDB();
    connect(ui->Valider, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->marque, SIGNAL(currentIndexChanged(int)), ui->modele_cam, SLOT(show()));
    connect(ui->marque, SIGNAL(currentIndexChanged(int)), ui->marque_2, SLOT(show()));
    connect(ui->marque_2, SIGNAL(currentIndexChanged(int)), ui->modele_cam_2, SLOT(show()));
}

void MainWindow::InitialState()
{
    ui->date_sortie->setDate(QDate::currentDate());
    ui->modele_cam->hide();
    ui->modele_cam_2->hide();
    ui->marque_2->hide();
    ui->affaire->clear();
    ui->nom_courant->clear();
    ui->remarque->clear();
    ui->rapport_fat->clear();
    ui->cadence->setValue(50000);
    ui->date_sortie->setDate(QDate::currentDate());
    ui->recipe->setValue(1);
}


void MainWindow::save()
{
    QString affaire = ui->affaire->text();
    QString nom_courant = ui->nom_courant->text();
    QString remarque = ui->remarque->text();
    QString rapport_fat = ui->rapport_fat->text();
    int cadence = ui->cadence->value();
    QString bouchons = ui->bouchons->currentText();
    QDate date_sortie = ui->date_sortie->date();
    QString modele = ui->modele->currentText();
    int recipe = ui->recipe->value();
    QString marque = ui->marque->currentText();
    QString marque_2 = ui->marque_2->currentText();
    QString modele_cam = ui->modele_cam->currentText();
    QString modele_cam_2 = ui->modele_cam_2->currentText();


    QSqlQuery query(db);
    query.prepare("INSERT INTO Machine (affaire, nom_courant, remarque, rapport_fat, cadence, bouchons, date_sortie, modele, recipe) "
                   "VALUES (:affaire, :nom_courant, :remarque, :rapport_fat, :cadence, :bouchons, :date_sortie, :modele, :recipe)");
    query.bindValue(":affaire", affaire);
    query.bindValue(":nom_courant", nom_courant);
    query.bindValue(":remarque", remarque);
    query.bindValue(":rapport_fat", rapport_fat);
    query.bindValue(":cadence", cadence);
    query.bindValue(":bouchons", bouchons);
    query.bindValue(":date_sortie", date_sortie);
    query.bindValue(":modele", modele);
    query.bindValue(":recipe", recipe);

    query.exec();

    CameraReg(affaire, marque, modele_cam);
    CameraReg(affaire, marque_2, modele_cam_2);

    InitialState();
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

void MainWindow::ConnectDB()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(Readconfig("HOST"));
    db.setDatabaseName(Readconfig("NAME"));
    db.setUserName(Readconfig("USER"));
    db.setPassword(Readconfig("PASSWORD"));
    if(db.open())
    {
        ui->connect->setChecked(true);
        ui->connect->setText(db.databaseName());
    }
    else
    {
        qDebug() << db.lastError();
        ui->connect->setChecked(false);
        ui->connect->setText("Not connected");
    }
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

void MainWindow::CameraReg(QString machine, QString marque, QString modele)
{
    if(marque.isEmpty()){return;}

    QSqlQuery queryCam(db);

    if(modele.isEmpty())
    {
        queryCam.prepare("INSERT INTO Camera(machine, marque) "
                         "VALUES(:machine, :marque)");
    }
    else
    {
        queryCam.prepare("INSERT INTO Camera(machine, marque, modele) "
                         "VALUES(:machine, :marque, :modele)");
        queryCam.bindValue(":modele", modele);
    }

    queryCam.bindValue(":machine", machine);
    queryCam.bindValue(":marque", marque);


    queryCam.exec();
}
