#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    UIMW = new Ui_MainWindow();
    UIMW->setupUi(this);
    vbox = NULL;
    QObject::connect(UIMW->radioButton, SIGNAL(pressed()), this, SLOT(showInputsForObstacles()));
    QObject::connect(UIMW->initOk, SIGNAL(pressed()), this, SLOT(initializationFinish()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showInputsForObstacles(){
    qDebug()<<"poziva se slot";
    UIMW->lineEdit_5->setReadOnly(false);
}

void MainWindow::initializationFinish(){
    qDebug()<<"poziva se slot 2";
    scene.clear();
    scene.addText("Hello, world!");
    UIMW->graphicsView->setScene(&scene);
    UIMW->graphicsView->update();

    numberObstacles = UIMW->lineEdit_5->text().toInt();
    qDebug()<<"numberobstacles";
    qDebug()<<numberObstacles;

    QStringList listS=(QStringList()<<"odaberi"<<"krug"<<"elipsa"<<"poligon");
    UIMW->scrollArea->setWidgetResizable(true);

    //za jednu prepreku cuvacemo podatke u vise lista
    //jedna sa imenom prepreke
    //u drugoj opcija
    //trecoj unos tacaka za listu
    itemsList.clear();
    comboBoxList.clear();
    pointsBoxList.clear();

    //sada zasvaku prepreku dodamo podatke u listu kreiramo labele, combobox-eve
    for(int i = 0; i < numberObstacles; i++){
        //kreiramo labele zasvaku prpreku i dodajemo u listu
        QString s = "Obstacles ";
        s.append(QString::number(i+1));
        s.append(":");
        QLabel *lab = new QLabel(s);
        lab->setMaximumWidth(180);
        QFont f( "Arial", 8, QFont::Bold);
        lab->setFont( f);
        itemsList.push_back(lab);
        qDebug()<<lab->text();

        //kreiramo combobox-eve zasvaku prepreku
        QComboBox *cb = new QComboBox();
        cb->setMinimumHeight(27);
        cb->setMaximumWidth(180);
        cb->setFont(f);
        //cb->setStyleSheet("QComboBox QAbstractItemView::item { min-height: 10px; min-width: 50px; margin: 2px;}QListView::item:selected { color: black; background-color: lightgray}");
        cb->addItems(listS);
        //QObject::connect(cb, SIGNAL(signalChangedOptions(4,4)), this, SLOT(showInputsCoordinate()));
        QObject::connect(cb,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(showInputsCoordinate(const QString&)));
        comboBoxList.push_back(cb);

        //grid layout zasvaku prepreku u kom ce se pojaviti polja za unos tacaka
        QGridLayout *hb = new QGridLayout();
        pointsBoxList.push_back(hb);

        //pravimo liste listi za unos podataka
        QList<QLineEdit*> *l = new QList<QLineEdit*>;
        listPoints.push_back(l);
    }


    if(vbox != NULL){
        QLayoutItem *wItem;
        while ((wItem = vbox->layout()->takeAt(0)) != 0)
            delete wItem;

        delete vbox;//nisi lepo oslobodio memoriju moras ukloniti sve ono sto si dodavao
    }

    vbox = new QVBoxLayout();


//    UIMW->scrollArea->takeWidget();
//    QWidget *widget = new QWidget();
//    QSize size = UIMW->scrollArea->size();
//    widget->setMinimumSize(size.width(),size.height());
//    widget->setLayout(vbox);
//    UIMW->scrollArea->setWidget(widget);
    UIMW->scrollArea->setWidgetResizable(true);

    QFrame *inner = new QFrame(UIMW->scrollArea);
    inner->setLayout(vbox);

    UIMW->scrollArea->setWidget(inner);
    UIMW->scrollArea->show();


    QList<QComboBox*>::iterator iter = comboBoxList.begin();
    QList<QGridLayout*>::iterator ite = pointsBoxList.begin();
    for(QList<QLabel*>::iterator it = itemsList.begin(); it != itemsList.end(); ++it) {
        QLabel *lbl = *it;
        vbox->addWidget(lbl);
        vbox->addWidget(*iter);
        vbox->addLayout(*ite);
        qDebug()<<lbl->text();

        ++iter;
        ++ite;
        // your code here
    }
}

void MainWindow::showInputsCoordinate(const QString& text){
    qDebug()<<"slot3 se poziva";
    qDebug()<<text;
    qDebug()<<QObject::sender();
    QList<QComboBox*>::iterator iter = comboBoxList.begin();
    int i = 0;
    int opt = -1; //option in combobox
    int index = -1; //index in obstacles

    while(iter != comboBoxList.end()){
        if((*iter) == QObject::sender()){
            qDebug()<<"nasao sam da je u listi pod rednim brojem: ";
            qDebug()<<i;
            qDebug()<<"odabrana opcija je: ";
            qDebug()<<(*iter)->currentIndex();
            opt = (*iter)->currentIndex();
            index = i;
        }

        iter++;
        i++;
    }

    switch(opt){
    case 1://krug
    {
        qDebug()<<"opcija1";

        QLabel *lbl1 = new QLabel("C:");
        QLabel *lbl2 = new QLabel("R:");

        QLineEdit *edLine1 = new QLineEdit();
        QLineEdit *edLine2 = new QLineEdit();
        QLineEdit *edLine3 = new QLineEdit();

        pointsBoxList.at(index)->addWidget(lbl1,0,0);
        pointsBoxList.at(index)->addWidget(edLine1,0,1);
        pointsBoxList.at(index)->addWidget(edLine2,0,2);

        pointsBoxList.at(index)->addWidget(lbl2,1,0);
        pointsBoxList.at(index)->addWidget(edLine3,1,1);

        listPoints.at(index)->push_back(edLine1);
        listPoints.at(index)->push_back(edLine2);
        listPoints.at(index)->push_back(edLine3);

        break;
    }
    case 2://elipsa
    {
        qDebug()<<"opcija2";

        QLabel *lbl1 = new QLabel("F1:");
        QLabel *lbl2 = new QLabel("F2:");
        QLabel *lbl3 = new QLabel("a:");
        QLabel *lbl4 = new QLabel("b:");

        QLineEdit *edLine1 = new QLineEdit();
        QLineEdit *edLine2 = new QLineEdit();
        QLineEdit *edLine3 = new QLineEdit();
        QLineEdit *edLine4 = new QLineEdit();
        QLineEdit *edLine5 = new QLineEdit();
        QLineEdit *edLine6 = new QLineEdit();


        pointsBoxList.at(index)->addWidget(lbl1,0,0);
        pointsBoxList.at(index)->addWidget(edLine1,0,1);
        pointsBoxList.at(index)->addWidget(edLine2,0,2);

        pointsBoxList.at(index)->addWidget(lbl2,1,0);
        pointsBoxList.at(index)->addWidget(edLine3,1,1);
        pointsBoxList.at(index)->addWidget(edLine4,1,2);

        pointsBoxList.at(index)->addWidget(lbl3,2,0);
        pointsBoxList.at(index)->addWidget(edLine5,2,1);

        pointsBoxList.at(index)->addWidget(lbl4,3,0);
        pointsBoxList.at(index)->addWidget(edLine6,3,1);

        break;
    }
    case 3://poligon
    {
        qDebug()<<"opcija3";

        break;
    }
    default:
    {
        qDebug()<<"Nekorektan unos tipa prepreke za prepreku";
        qDebug()<<index;
    }
    }




    //pointsBoxList.at(index)->addWidget(edLine4,1,2);

//    pointsBoxList.at(index)->addItem(
//                );

}

void MainWindow::emitshowInputsCoordinateSignal(int orderNumObstacles, int option)
{
    emit signalChangedOptions(orderNumObstacles, option);
}
