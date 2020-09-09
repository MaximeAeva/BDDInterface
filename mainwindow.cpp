#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(Readconfig("WINDOW"));
    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() +"/icone.ico"));

    ConnectDB();//Database Connection
    InitialState();//Initialize Window's fields
    Connections();//Initialize connections
}

void MainWindow::Connections()//Group connected Items
{
    connect(ui->Valider, SIGNAL(clicked()), this, SLOT(saveMachine()));
    connect(ui->marque, SIGNAL(currentIndexChanged(int)), ui->modele_cam, SLOT(show()));
    connect(ui->marque, SIGNAL(currentIndexChanged(int)), ui->marque_2, SLOT(show()));
    connect(ui->marque_2, SIGNAL(currentIndexChanged(int)), ui->modele_cam_2, SLOT(show()));
    connect(ui->up_table, SIGNAL(currentIndexChanged(int)), this, SLOT(adaptiveDisplay()));
    connect(ui->selection, SIGNAL(currentIndexChanged(int)), this, SLOT(manageSelector()));
    connect(ui->cam_marque, SIGNAL(currentIndexChanged(int)), ui->cam_modele, SLOT(show()));
    connect(ui->cam_marque, SIGNAL(currentIndexChanged(int)), ui->label_21, SLOT(show()));
    connect(ui->up_affaire, SIGNAL(currentIndexChanged(int)), this, SLOT(listItems()));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(update(const QModelIndex &)));
    connect(ui->pb_valider, SIGNAL(clicked()), this, SLOT(saveProbleme()));
    connect(ui->cam_valider, SIGNAL(clicked()), this, SLOT(saveCamera()));
}

void MainWindow::InitialState()//Group ui initialisation
{
    ui->useless->hide();
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
    ui->cam_modele->hide();
    ui->label_21->hide();
    ui->pb_resolu->setChecked(false);
    ui->pb_description->clear();
    ui->pb_date->setDate(QDate::currentDate());
    ui->pb_solution->clear();
    ui->up_table->clear();
    ui->up_affaire->clear();
    ui->up_valeur->clear();

    QSqlQueryModel *affaire = new QSqlQueryModel;
    affaire->setQuery("SELECT affaire FROM Machine");
    for(int i = 0; i<affaire->rowCount(); i++)
    {
        ui->cam_affaire->addItem(affaire->record(i).value("affaire").toString());
        ui->pb_affaire->addItem(affaire->record(i).value("affaire").toString());
    }
    QSqlQueryModel *table = new QSqlQueryModel;
    table->setQuery("SELECT table_name as tables FROM information_schema.tables "
                    "WHERE table_schema = 'Sidel'");
    for(int i = 0; i<table->rowCount(); i++)
    {
        ui->up_table->addItem(table->record(i).value("tables").toString());
    }
    webPage();
}

void MainWindow::saveMachine()//Create a new machine
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
    adaptiveDisplay();
}

void MainWindow::saveProbleme()//Add a new Probleme
{
    QString machine = ui->pb_affaire->currentText();
    int resolu = ui->pb_resolu->isChecked();
    QString description = ui->pb_description->text();
    QString solution = ui->pb_solution->text();
    QDate date = ui->pb_date->date();
    QString origine = ui->pb_origine->currentText();

    QSqlQuery *query = new QSqlQuery;
    query->prepare("INSERT INTO Probleme(resolu, machine, description, solution, date_apparition, origine) "
                   "VALUES(:resolu, :machine, :description, :solution, :date_apparition, :origine)");
    query->bindValue(":resolu", resolu);
    query->bindValue(":machine", machine);
    query->bindValue(":description", description);
    query->bindValue(":solution", solution);
    query->bindValue(":date_apparition", date);
    query->bindValue(":origine", origine);

    query->exec();

    InitialState();
    adaptiveDisplay();
}

void MainWindow::saveCamera()//Add a new Camera
{
    QString machine = ui->cam_affaire->currentText();
    QString marque = ui->cam_marque->currentText();
    QSqlQuery *query = new QSqlQuery;

    if(marque.toStdString()=="COGNEX")
    {
        QString modele = ui->cam_modele->currentText();
        query->prepare("INSERT INTO Camera(machine, marque, modele) "
                       "VALUES(:machine, :marque, :modele)");
        query->bindValue(":modele", modele);
    }
    else
    {
        query->prepare("INSERT INTO Camera(machine, marque) "
                       "VALUES(:machine, :marque)");
    }
    query->bindValue(":machine", machine);
    query->bindValue(":marque", marque);

    query->exec();

    InitialState();
    adaptiveDisplay();
}

MainWindow::~MainWindow()//kill
{
    db.close();

    delete ui;
}

void MainWindow::ConnectDB()//Connect to DB
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

QString MainWindow::Readconfig(string paramName)//Read config file
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

void MainWindow::CameraReg(QString machine, QString marque, QString modele)//Camera system management
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

void MainWindow::adaptiveDisplay()//Adapt ComboBoxes contents based on other ComboBoxes
{
    ui->up_affaire->clear();

    QString cur = ui->up_table->currentText();

    QSqlQueryModel *affaire = new QSqlQueryModel;

    if(cur.toStdString()=="machine")
    {
        affaire->setQuery("SELECT DISTINCT affaire FROM Machine");
    }
    else
    {
        affaire->setQuery("SELECT DISTINCT machine AS affaire FROM " + cur);
    }
    for(int i = 0; i<affaire->rowCount(); i++)
    {
        ui->up_affaire->addItem(affaire->record(i).value("affaire").toString());
    }
}

void MainWindow::update(const QModelIndex &index)//Update DB
{
    QString affaire = ui->up_affaire->currentText();
    QString table = ui->up_table->currentText();
    QString valeur = ui->up_valeur->text();

    QItemSelectionModel *select = ui->tableView->selectionModel();
    QString champ = ui->tableView->model()->headerData(select->currentIndex().column(), Qt::Horizontal).toString();
    int id = ui->tableView->model()->data(ui->tableView->model()->index(index.row(),0)).toInt();

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
                       "WHERE machine = :affaire AND numero = :id");
        query->bindValue(":affaire", affaire);
        query->bindValue(":id", id);
    }

    if(!qstrcmp(myType->record(0).value("type").toString().toStdString().c_str(), "smallint") ||
            !qstrcmp(myType->record(0).value("type").toString().toStdString().c_str(), "tinyint") ||
            !qstrcmp(myType->record(0).value("type").toString().toStdString().c_str(), "int"))
    {
        query->bindValue(":valeur", valeur.toInt());
    }
    else
    {
        query->bindValue(":valeur", valeur);
    }

    query->exec();

    ui->up_valeur->clear();

    listItems();
    webPage();
}

void MainWindow::manageSelector()//Stacked Widget management
{
    ui->stacked->setCurrentIndex(ui->selection->currentIndex());
}

void MainWindow::listItems()//Interactive Item list
{
    QString cur = ui->up_table->currentText();
    QString affaire = ui->up_affaire->currentText();

    QSqlQueryModel *model = new QSqlQueryModel;
    if(cur.toStdString()=="machine")
    {
        model->setQuery("SELECT * FROM " + cur + " WHERE affaire = '" + affaire + "'");
    }
    else
    {
        model->setQuery("SELECT * FROM " + cur + " WHERE machine = '" + affaire + "'");
    }
    ui->tableView->setModel(model);
}

void MainWindow::webPage()
{
        ui->webPage->load(QUrl("http://localhost:3000/d/7slNrbWGz/capfeeder?from=1513641600000&to=1595376000000&orgId=1"));
}
