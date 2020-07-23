#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(Readconfig("WINDOW"));
    this->setWindowIcon(QIcon(Readconfig("ICON")));

    ConnectDB();
    InitialState();
    UpdatePageUpdateCombo();

    connect(ui->Valider, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->marque, SIGNAL(currentIndexChanged(int)), ui->modele_cam, SLOT(show()));
    connect(ui->marque, SIGNAL(currentIndexChanged(int)), ui->marque_2, SLOT(show()));
    connect(ui->marque_2, SIGNAL(currentIndexChanged(int)), ui->modele_cam_2, SLOT(show()));
    connect(ui->up_table, SIGNAL(currentIndexChanged(int)), this, SLOT(adaptiveDisplay()));
    connect(ui->up_valider, SIGNAL(clicked()), this, SLOT(update()));
    connect(ui->selection, SIGNAL(currentIndexChanged(int)), this, SLOT(manageSelector()));
    connect(ui->cam_marque, SIGNAL(currentIndexChanged(int)), ui->cam_modele, SLOT(show()));
    connect(ui->cam_marque, SIGNAL(currentIndexChanged(int)), ui->label_21, SLOT(show()));
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
    ui->cam_affaire->clear();
    ui->pb_affaire->clear();
    QSqlQueryModel *affaire = new QSqlQueryModel;
    affaire->setQuery("SELECT affaire FROM Machine");
    for(int i = 0; i<affaire->rowCount(); i++)
    {
        ui->cam_affaire->addItem(affaire->record(i).value("affaire").toString());
        ui->pb_affaire->addItem(affaire->record(i).value("affaire").toString());
    }
    ui->cam_modele->hide();
    ui->label_21->hide();

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
    UpdatePageUpdateCombo();
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

void MainWindow::UpdatePageUpdateCombo()
{
    ui->up_affaire->clear();
    ui->up_table->clear();
    ui->up_champ->clear();

    QSqlQueryModel *table = new QSqlQueryModel;
    table->setQuery("SELECT table_name as tables FROM information_schema.tables "
                    "WHERE table_schema = 'Sidel'");
    for(int i = 0; i<table->rowCount(); i++)
    {
        ui->up_table->addItem(table->record(i).value("tables").toString());
    }


}

void MainWindow::adaptiveDisplay()
{
    ui->up_champ->clear();
    ui->up_affaire->clear();

    QString cur = ui->up_table->currentText();

    QSqlQueryModel *affaire = new QSqlQueryModel;

    if(cur.toStdString()=="machine")
    {
        affaire->setQuery("SELECT affaire AS affaire FROM Machine");
    }
    else
    {
        affaire->setQuery("SELECT machine AS affaire FROM " + cur);
    }
    for(int i = 0; i<affaire->rowCount(); i++)
    {
        ui->up_affaire->addItem(affaire->record(i).value("affaire").toString());
    }


    QSqlQuery query;
    QSqlQueryModel *champ = new QSqlQueryModel;
    query.prepare("SELECT column_name AS columns "
                    "FROM information_schema.columns "
                    "WHERE table_name = :table");
    query.bindValue(":table", cur);
    query.exec();
    champ->setQuery(query);
    for(int i = 0; i<champ->rowCount(); i++)
    {
        ui->up_champ->addItem(champ->record(i).value("columns").toString());
    }
}

void MainWindow::update()
{
    QString affaire = ui->up_affaire->currentText();
    QString table = ui->up_table->currentText();
    QString champ = ui->up_champ->currentText();
    QString valeur = ui->up_valeur->text();

    QSqlQueryModel *myType = new QSqlQueryModel;
    myType->setQuery("SELECT DATA_TYPE AS type "
                         "FROM INFORMATION_SCHEMA.COLUMNS "
                         "WHERE "
                              "TABLE_NAME = " + table +" AND "
                              "COLUMN_NAME = " + champ);


    QSqlQuery *query = new QSqlQuery;
    if(table.toStdString()=="machine")
    {
        query->prepare("UPDATE "+ table +" "
                       "SET "+ champ +" = :valeur "
                       "WHERE affaire = :affaire");
        query->bindValue(":affaire", affaire);
    }
    else
    {
        query->prepare("UPDATE "+ table +" "
                       "SET "+ champ +" = :valeur "
                       "WHERE machine = :affaire");
        query->bindValue(":affaire", affaire);
    }

    if(!strcmp(myType->record(0).value("type").toString().toStdString().c_str(), "smallint") ||
            !strcmp(myType->record(0).value("type").toString().toStdString().c_str(), "tinyint") ||
            !strcmp(myType->record(0).value("type").toString().toStdString().c_str(), "int"))
    {
        query->bindValue(":valeur", valeur.toInt());
    }
    else
    {
        query->bindValue(":valeur", valeur);
    }

    query->exec();

    ui->up_valeur->clear();
}

void MainWindow::manageSelector()
{
    ui->stacked->setCurrentIndex(ui->selection->currentIndex());
}
