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
    QObject::connect(UIMW->Apply, SIGNAL(pressed()), this, SLOT(readEnteredData()));
    m = 1;
    prevStep = 1;
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

    //read start point coordiante
    S_x = UIMW->lineEdit->text().toFloat();
    S_y = UIMW->lineEdit_2->text().toFloat();
    //read goal point coordinate
    G_x = UIMW->lineEdit_3->text().toFloat();
    G_y = UIMW->lineEdit_4->text().toFloat();

    //izracunavamo jednacinu prave  y = kx+n
    Pk = (G_y-S_y)/(G_x-S_x);
    Pn = ((G_y-S_y)/(G_x-S_x))*S_x+S_y;
    vx = S_x;
    vy = S_y;

    qDebug()<<"DEBUG"<<Pk;
    qDebug()<<"DEBUG"<<Pn;
    qDebug()<<"start point";
    qDebug()<<S_x<<" "<<S_y;
    qDebug()<<"goal point";
    qDebug()<<G_x<<" "<<G_y;

    scene.clear();
    UIMW->graphicsView->setScene(&scene);
    UIMW->graphicsView->update();


    //UIMW->graphicsView->setAlignment(Qt::AlignTop|Qt::AlignLeft);

    numberObstacles = UIMW->lineEdit_5->text().toInt();
    qDebug()<<"numberobstacles";
    qDebug()<<numberObstacles;

    QStringList listS=(QStringList()<<"odaberi"<<"krug"<<"elipsa"<<"trougao"<<"cetvorougao"<<"petougao"<<"sestougao"<<"sedmougao");
    UIMW->scrollArea->setWidgetResizable(true);

    //za jednu prepreku cuvacemo podatke u vise lista
    //jedna sa imenom prepreke
    //u drugoj opcija
    //trecoj unos tacaka za listu
    itemsList.clear();
    comboBoxList.clear();
    pointsBoxList.clear();
    listGraphItems.clear();

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

        //graph item zasvaku prepreku
        //QGraphicsItem *gi = new QGraphicsItem();
        //listGraphItems.push_back(gi);

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

        QLabel *lbl1 = new QLabel("C:");
        QLabel *lbl3 = new QLabel("a:");
        QLabel *lbl4 = new QLabel("b:");

        QLineEdit *edLine1 = new QLineEdit();
        QLineEdit *edLine2 = new QLineEdit();
        QLineEdit *edLine5 = new QLineEdit();
        QLineEdit *edLine6 = new QLineEdit();


        pointsBoxList.at(index)->addWidget(lbl1,0,0);
        pointsBoxList.at(index)->addWidget(edLine1,0,1);
        pointsBoxList.at(index)->addWidget(edLine2,0,2);

        pointsBoxList.at(index)->addWidget(lbl3,2,0);
        pointsBoxList.at(index)->addWidget(edLine5,2,1);

        pointsBoxList.at(index)->addWidget(lbl4,3,0);
        pointsBoxList.at(index)->addWidget(edLine6,3,1);

        listPoints.at(index)->push_back(edLine1);
        listPoints.at(index)->push_back(edLine2);
        listPoints.at(index)->push_back(edLine5);
        listPoints.at(index)->push_back(edLine6);

        break;
    }
    case 3://poligon
    {
        qDebug()<<"opcija3";

        QLabel *lbl1 = new QLabel("A:");
        QLabel *lbl2 = new QLabel("B:");
        QLabel *lbl3 = new QLabel("C:");

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
        pointsBoxList.at(index)->addWidget(edLine6,2,2);

        listPoints.at(index)->push_back(edLine1);
        listPoints.at(index)->push_back(edLine2);
        listPoints.at(index)->push_back(edLine3);
        listPoints.at(index)->push_back(edLine4);
        listPoints.at(index)->push_back(edLine5);
        listPoints.at(index)->push_back(edLine6);

        break;
    }
    case 4:
    {
        QLabel *lbl1 = new QLabel("A:");
        QLabel *lbl2 = new QLabel("B:");
        QLabel *lbl3 = new QLabel("C:");
        QLabel *lbl4 = new QLabel("D:");

        QLineEdit *edLine1 = new QLineEdit();
        QLineEdit *edLine2 = new QLineEdit();
        QLineEdit *edLine3 = new QLineEdit();
        QLineEdit *edLine4 = new QLineEdit();
        QLineEdit *edLine5 = new QLineEdit();
        QLineEdit *edLine6 = new QLineEdit();
        QLineEdit *edLine7 = new QLineEdit();
        QLineEdit *edLine8 = new QLineEdit();

        pointsBoxList.at(index)->addWidget(lbl1,0,0);
        pointsBoxList.at(index)->addWidget(edLine1,0,1);
        pointsBoxList.at(index)->addWidget(edLine2,0,2);

        pointsBoxList.at(index)->addWidget(lbl2,1,0);
        pointsBoxList.at(index)->addWidget(edLine3,1,1);
        pointsBoxList.at(index)->addWidget(edLine4,1,2);

        pointsBoxList.at(index)->addWidget(lbl3,2,0);
        pointsBoxList.at(index)->addWidget(edLine5,2,1);
        pointsBoxList.at(index)->addWidget(edLine6,2,2);

        pointsBoxList.at(index)->addWidget(lbl4,3,0);
        pointsBoxList.at(index)->addWidget(edLine7,3,1);
        pointsBoxList.at(index)->addWidget(edLine8,3,2);

        listPoints.at(index)->push_back(edLine1);
        listPoints.at(index)->push_back(edLine2);
        listPoints.at(index)->push_back(edLine3);
        listPoints.at(index)->push_back(edLine4);
        listPoints.at(index)->push_back(edLine5);
        listPoints.at(index)->push_back(edLine6);
        listPoints.at(index)->push_back(edLine7);
        listPoints.at(index)->push_back(edLine8);

        break;
    }
    case 5:
    {
        QLabel *lbl1 = new QLabel("A:");
        QLabel *lbl2 = new QLabel("B:");
        QLabel *lbl3 = new QLabel("C:");
        QLabel *lbl4 = new QLabel("D:");
        QLabel *lbl5 = new QLabel("E:");

        QLineEdit *edLine1 = new QLineEdit();
        QLineEdit *edLine2 = new QLineEdit();
        QLineEdit *edLine3 = new QLineEdit();
        QLineEdit *edLine4 = new QLineEdit();
        QLineEdit *edLine5 = new QLineEdit();
        QLineEdit *edLine6 = new QLineEdit();
        QLineEdit *edLine7 = new QLineEdit();
        QLineEdit *edLine8 = new QLineEdit();
        QLineEdit *edLine9 = new QLineEdit();
        QLineEdit *edLine10 = new QLineEdit();

        pointsBoxList.at(index)->addWidget(lbl1,0,0);
        pointsBoxList.at(index)->addWidget(edLine1,0,1);
        pointsBoxList.at(index)->addWidget(edLine2,0,2);

        pointsBoxList.at(index)->addWidget(lbl2,1,0);
        pointsBoxList.at(index)->addWidget(edLine3,1,1);
        pointsBoxList.at(index)->addWidget(edLine4,1,2);

        pointsBoxList.at(index)->addWidget(lbl3,2,0);
        pointsBoxList.at(index)->addWidget(edLine5,2,1);
        pointsBoxList.at(index)->addWidget(edLine6,2,2);

        pointsBoxList.at(index)->addWidget(lbl4,3,0);
        pointsBoxList.at(index)->addWidget(edLine7,3,1);
        pointsBoxList.at(index)->addWidget(edLine8,3,2);

        pointsBoxList.at(index)->addWidget(lbl5,4,0);
        pointsBoxList.at(index)->addWidget(edLine9,4,1);
        pointsBoxList.at(index)->addWidget(edLine10,4,2);

        listPoints.at(index)->push_back(edLine1);
        listPoints.at(index)->push_back(edLine2);
        listPoints.at(index)->push_back(edLine3);
        listPoints.at(index)->push_back(edLine4);
        listPoints.at(index)->push_back(edLine5);
        listPoints.at(index)->push_back(edLine6);
        listPoints.at(index)->push_back(edLine7);
        listPoints.at(index)->push_back(edLine8);
        listPoints.at(index)->push_back(edLine9);
        listPoints.at(index)->push_back(edLine10);

        break;
    }
    case 6:
    {
        QLabel *lbl1 = new QLabel("A:");
        QLabel *lbl2 = new QLabel("B:");
        QLabel *lbl3 = new QLabel("C:");
        QLabel *lbl4 = new QLabel("D:");
        QLabel *lbl5 = new QLabel("E:");
        QLabel *lbl6 = new QLabel("F:");

        QLineEdit *edLine1 = new QLineEdit();
        QLineEdit *edLine2 = new QLineEdit();
        QLineEdit *edLine3 = new QLineEdit();
        QLineEdit *edLine4 = new QLineEdit();
        QLineEdit *edLine5 = new QLineEdit();
        QLineEdit *edLine6 = new QLineEdit();
        QLineEdit *edLine7 = new QLineEdit();
        QLineEdit *edLine8 = new QLineEdit();
        QLineEdit *edLine9 = new QLineEdit();
        QLineEdit *edLine10 = new QLineEdit();
        QLineEdit *edLine11 = new QLineEdit();
        QLineEdit *edLine12 = new QLineEdit();

        pointsBoxList.at(index)->addWidget(lbl1,0,0);
        pointsBoxList.at(index)->addWidget(edLine1,0,1);
        pointsBoxList.at(index)->addWidget(edLine2,0,2);

        pointsBoxList.at(index)->addWidget(lbl2,1,0);
        pointsBoxList.at(index)->addWidget(edLine3,1,1);
        pointsBoxList.at(index)->addWidget(edLine4,1,2);

        pointsBoxList.at(index)->addWidget(lbl3,2,0);
        pointsBoxList.at(index)->addWidget(edLine5,2,1);
        pointsBoxList.at(index)->addWidget(edLine6,2,2);

        pointsBoxList.at(index)->addWidget(lbl4,3,0);
        pointsBoxList.at(index)->addWidget(edLine7,3,1);
        pointsBoxList.at(index)->addWidget(edLine8,3,2);

        pointsBoxList.at(index)->addWidget(lbl5,4,0);
        pointsBoxList.at(index)->addWidget(edLine9,4,1);
        pointsBoxList.at(index)->addWidget(edLine10,4,2);

        pointsBoxList.at(index)->addWidget(lbl6,5,0);
        pointsBoxList.at(index)->addWidget(edLine11,5,1);
        pointsBoxList.at(index)->addWidget(edLine12,5,2);

        listPoints.at(index)->push_back(edLine1);
        listPoints.at(index)->push_back(edLine2);
        listPoints.at(index)->push_back(edLine3);
        listPoints.at(index)->push_back(edLine4);
        listPoints.at(index)->push_back(edLine5);
        listPoints.at(index)->push_back(edLine6);
        listPoints.at(index)->push_back(edLine7);
        listPoints.at(index)->push_back(edLine8);
        listPoints.at(index)->push_back(edLine9);
        listPoints.at(index)->push_back(edLine10);
        listPoints.at(index)->push_back(edLine11);
        listPoints.at(index)->push_back(edLine12);

        break;
    }
    case 7:
    {
        QLabel *lbl1 = new QLabel("A:");
        QLabel *lbl2 = new QLabel("B:");
        QLabel *lbl3 = new QLabel("C:");
        QLabel *lbl4 = new QLabel("D:");
        QLabel *lbl5 = new QLabel("E:");
        QLabel *lbl6 = new QLabel("F:");
        QLabel *lbl7 = new QLabel("G:");

        QLineEdit *edLine1 = new QLineEdit();
        QLineEdit *edLine2 = new QLineEdit();
        QLineEdit *edLine3 = new QLineEdit();
        QLineEdit *edLine4 = new QLineEdit();
        QLineEdit *edLine5 = new QLineEdit();
        QLineEdit *edLine6 = new QLineEdit();
        QLineEdit *edLine7 = new QLineEdit();
        QLineEdit *edLine8 = new QLineEdit();
        QLineEdit *edLine9 = new QLineEdit();
        QLineEdit *edLine10 = new QLineEdit();
        QLineEdit *edLine11 = new QLineEdit();
        QLineEdit *edLine12 = new QLineEdit();
        QLineEdit *edLine13 = new QLineEdit();
        QLineEdit *edLine14 = new QLineEdit();

        pointsBoxList.at(index)->addWidget(lbl1,0,0);
        pointsBoxList.at(index)->addWidget(edLine1,0,1);
        pointsBoxList.at(index)->addWidget(edLine2,0,2);

        pointsBoxList.at(index)->addWidget(lbl2,1,0);
        pointsBoxList.at(index)->addWidget(edLine3,1,1);
        pointsBoxList.at(index)->addWidget(edLine4,1,2);

        pointsBoxList.at(index)->addWidget(lbl3,2,0);
        pointsBoxList.at(index)->addWidget(edLine5,2,1);
        pointsBoxList.at(index)->addWidget(edLine6,2,2);

        pointsBoxList.at(index)->addWidget(lbl4,3,0);
        pointsBoxList.at(index)->addWidget(edLine7,3,1);
        pointsBoxList.at(index)->addWidget(edLine8,3,2);

        pointsBoxList.at(index)->addWidget(lbl5,4,0);
        pointsBoxList.at(index)->addWidget(edLine9,4,1);
        pointsBoxList.at(index)->addWidget(edLine10,4,2);

        pointsBoxList.at(index)->addWidget(lbl6,5,0);
        pointsBoxList.at(index)->addWidget(edLine11,5,1);
        pointsBoxList.at(index)->addWidget(edLine12,5,2);

        pointsBoxList.at(index)->addWidget(lbl7,6,0);
        pointsBoxList.at(index)->addWidget(edLine13,6,1);
        pointsBoxList.at(index)->addWidget(edLine14,6,2);

        listPoints.at(index)->push_back(edLine1);
        listPoints.at(index)->push_back(edLine2);
        listPoints.at(index)->push_back(edLine3);
        listPoints.at(index)->push_back(edLine4);
        listPoints.at(index)->push_back(edLine5);
        listPoints.at(index)->push_back(edLine6);
        listPoints.at(index)->push_back(edLine7);
        listPoints.at(index)->push_back(edLine8);
        listPoints.at(index)->push_back(edLine9);
        listPoints.at(index)->push_back(edLine10);
        listPoints.at(index)->push_back(edLine11);
        listPoints.at(index)->push_back(edLine12);
        listPoints.at(index)->push_back(edLine13);
        listPoints.at(index)->push_back(edLine14);

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

void MainWindow::readEnteredData(){
    qDebug()<<"called readEnteredData";
//    QVector<QPointF> *points;

//    QPointF p1;
//    QPointF p2;
//    QPointF p3;
//    QPointF p4;

//    p1.setX(3);
//    p1.setY(15);

//    p2.setX(10);
//    p2.setY(15);

//    p3.setX(10);
//    p3.setY(20);

//    p4.setX(3);
//    p4.setY(20);

//    points->push_back(p1);
//    points->push_back(p2);
//    points->push_back(p3);
//    points->push_back(p4);


//    QPolygonF *pol = new QPolygon(points);
//    QGraphicsPolygonItem *polygon = new QGraphicsPolygonItem(
//                )
    QPen pen;
    QBrush brush;
    pen.setColor(Qt::red);
    pen.setWidth(2);
    brush.setColor(Qt::green);
    brush.setStyle(Qt::SolidPattern);

    //***************************
    QList<QComboBox*>::iterator iter = comboBoxList.begin();
    QList<QList<QLineEdit*>*>::iterator it = listPoints.begin();
    for(int i = 0; i < numberObstacles; i++){
        //treba proci kroz sve prepreke, iscitati koja je prepreka u pitanju njen tip i comboBox-a
        //onda iz liste LineEdit-a iscitati odgovarajuci broj polja(switch po tipu prepreke)
        //kreirati prepreku i prikazati na sceni

        int option = (*iter)->currentIndex();
        switch(option){
            case 1: //krug
            {
                QList<QLineEdit*>::iterator innerIterator = (*(*it)).begin();
                QLineEdit *ed = (*innerIterator);
                int x = ed->text().toInt();
                qDebug()<<"iscitao sam x: ";
                qDebug()<<x;

                innerIterator++;
                ed = (*innerIterator);
                int y = ed->text().toInt();
                qDebug()<<"iscitao sam y: ";
                qDebug()<<y;

                innerIterator++;
                ed = (*innerIterator);
                int r = ed->text().toInt();
                qDebug()<<"iscitao sam poluprecnik: ";
                qDebug()<<r;

                QGraphicsEllipseItem *elipse = scene.addEllipse(x-r/2,y-r/2,r,r, pen, brush);
                scene.update();

                listGraphItems.push_back(elipse);

                break;
            }
            case 2:
            {
                QList<QLineEdit*>::iterator innerIterator = (*(*it)).begin();
                QLineEdit *ed = (*innerIterator);
                int x = ed->text().toInt();
                qDebug()<<"iscitao sam x: ";
                qDebug()<<x;

                innerIterator++;
                ed = (*innerIterator);
                int y = ed->text().toInt();
                qDebug()<<"iscitao sam y: ";
                qDebug()<<y;

                innerIterator++;
                ed = (*innerIterator);
                int a = ed->text().toInt();
                qDebug()<<"iscitao sam a: ";
                qDebug()<<a;

                innerIterator++;
                ed = (*innerIterator);
                int b = ed->text().toInt();
                qDebug()<<"iscitao sam a: ";
                qDebug()<<b;

                QGraphicsEllipseItem *elipse = scene.addEllipse(x-a/2,y-b/2,a,b, pen, brush);
                scene.update();

                listGraphItems.push_back(elipse);
                break;
            }
            case 3:
            {
                QList<QLineEdit*>::iterator innerIterator = (*(*it)).begin();
                QLineEdit *ed = (*innerIterator);
                int x1 = ed->text().toInt();
                qDebug()<<"iscitao sam x1: ";
                qDebug()<<x1;

                innerIterator++;
                ed = (*innerIterator);
                int y1 = ed->text().toInt();
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = ed->text().toInt();
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = ed->text().toInt();
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                QPolygonF Triangle;
                Triangle.append(QPointF(x1,y1));
                Triangle.append(QPointF(x2,y2));
                Triangle.append(QPointF(x3,y3));
                Triangle.append(QPointF(x1,y1));

                QGraphicsPolygonItem *polygon = scene.addPolygon(Triangle,pen, brush);

                scene.update();
                listGraphItems.push_back(polygon);
                break;
            }
            case 4:
            {
                QList<QLineEdit*>::iterator innerIterator = (*(*it)).begin();
                QLineEdit *ed = (*innerIterator);
                int x1 = ed->text().toInt();
                qDebug()<<"iscitao sam x1: ";
                qDebug()<<x1;

                innerIterator++;
                ed = (*innerIterator);
                int y1 = ed->text().toInt();
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = ed->text().toInt();
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = ed->text().toInt();
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                innerIterator++;
                ed = (*innerIterator);
                int x4 = ed->text().toInt();
                qDebug()<<"iscitao sam x4: ";
                qDebug()<<x4;

                innerIterator++;
                ed = (*innerIterator);
                int y4 = ed->text().toInt();
                qDebug()<<"iscitao sam y4: ";
                qDebug()<<y4;

                QPolygonF Pol;
                Pol.append(QPointF(x1,y1));
                Pol.append(QPointF(x2,y2));
                Pol.append(QPointF(x3,y3));
                Pol.append(QPointF(x4,y4));
                Pol.append(QPointF(x1,y1));

                QGraphicsPolygonItem *polygon = scene.addPolygon(Pol,pen, brush);

                scene.update();
                listGraphItems.push_back(polygon);
                break;
            }
            case 5:
            {
                QList<QLineEdit*>::iterator innerIterator = (*(*it)).begin();
                QLineEdit *ed = (*innerIterator);
                int x1 = ed->text().toInt();
                qDebug()<<"iscitao sam x1: ";
                qDebug()<<x1;

                innerIterator++;
                ed = (*innerIterator);
                int y1 = ed->text().toInt();
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = ed->text().toInt();
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = ed->text().toInt();
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                innerIterator++;
                ed = (*innerIterator);
                int x4 = ed->text().toInt();
                qDebug()<<"iscitao sam x4: ";
                qDebug()<<x4;

                innerIterator++;
                ed = (*innerIterator);
                int y4 = ed->text().toInt();
                qDebug()<<"iscitao sam y4: ";
                qDebug()<<y4;

                innerIterator++;
                ed = (*innerIterator);
                int x5 = ed->text().toInt();
                qDebug()<<"iscitao sam x5: ";
                qDebug()<<x5;

                innerIterator++;
                ed = (*innerIterator);
                int y5 = ed->text().toInt();
                qDebug()<<"iscitao sam y5: ";
                qDebug()<<y5;

                QPolygonF Pol;
                Pol.append(QPointF(x1,y1));
                Pol.append(QPointF(x2,y2));
                Pol.append(QPointF(x3,y3));
                Pol.append(QPointF(x4,y4));
                Pol.append(QPointF(x5,y5));
                Pol.append(QPointF(x1,y1));

                QGraphicsPolygonItem *polygon = scene.addPolygon(Pol,pen, brush);

                scene.update();
                listGraphItems.push_back(polygon);
                break;
            }
            case 6:
            {
                QList<QLineEdit*>::iterator innerIterator = (*(*it)).begin();
                QLineEdit *ed = (*innerIterator);
                int x1 = ed->text().toInt();
                qDebug()<<"iscitao sam x1: ";
                qDebug()<<x1;

                innerIterator++;
                ed = (*innerIterator);
                int y1 = ed->text().toInt();
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = ed->text().toInt();
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = ed->text().toInt();
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                innerIterator++;
                ed = (*innerIterator);
                int x4 = ed->text().toInt();
                qDebug()<<"iscitao sam x4: ";
                qDebug()<<x4;

                innerIterator++;
                ed = (*innerIterator);
                int y4 = ed->text().toInt();
                qDebug()<<"iscitao sam y4: ";
                qDebug()<<y4;

                innerIterator++;
                ed = (*innerIterator);
                int x5 = ed->text().toInt();
                qDebug()<<"iscitao sam x5: ";
                qDebug()<<x5;

                innerIterator++;
                ed = (*innerIterator);
                int y5 = ed->text().toInt();
                qDebug()<<"iscitao sam y5: ";
                qDebug()<<y5;

                innerIterator++;
                ed = (*innerIterator);
                int x6 = ed->text().toInt();
                qDebug()<<"iscitao sam x6: ";
                qDebug()<<x6;

                innerIterator++;
                ed = (*innerIterator);
                int y6 = ed->text().toInt();
                qDebug()<<"iscitao sam y6: ";
                qDebug()<<y6;

                QPolygonF Pol;
                Pol.append(QPointF(x1,y1));
                Pol.append(QPointF(x2,y2));
                Pol.append(QPointF(x3,y3));
                Pol.append(QPointF(x4,y4));
                Pol.append(QPointF(x5,y5));
                Pol.append(QPointF(x6,y6));
                Pol.append(QPointF(x1,y1));

                QGraphicsPolygonItem *polygon = scene.addPolygon(Pol,pen, brush);

                scene.update();
                listGraphItems.push_back(polygon);
                break;
            }
            case 7:
            {
                QList<QLineEdit*>::iterator innerIterator = (*(*it)).begin();
                QLineEdit *ed = (*innerIterator);
                int x1 = ed->text().toInt();
                qDebug()<<"iscitao sam x1: ";
                qDebug()<<x1;

                innerIterator++;
                ed = (*innerIterator);
                int y1 = ed->text().toInt();
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = ed->text().toInt();
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = ed->text().toInt();
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                innerIterator++;
                ed = (*innerIterator);
                int x4 = ed->text().toInt();
                qDebug()<<"iscitao sam x4: ";
                qDebug()<<x4;

                innerIterator++;
                ed = (*innerIterator);
                int y4 = ed->text().toInt();
                qDebug()<<"iscitao sam y4: ";
                qDebug()<<y4;

                innerIterator++;
                ed = (*innerIterator);
                int x5 = ed->text().toInt();
                qDebug()<<"iscitao sam x5: ";
                qDebug()<<x5;

                innerIterator++;
                ed = (*innerIterator);
                int y5 = ed->text().toInt();
                qDebug()<<"iscitao sam y5: ";
                qDebug()<<y5;

                innerIterator++;
                ed = (*innerIterator);
                int x6 = ed->text().toInt();
                qDebug()<<"iscitao sam x6: ";
                qDebug()<<x6;

                innerIterator++;
                ed = (*innerIterator);
                int y6 = ed->text().toInt();
                qDebug()<<"iscitao sam y6: ";
                qDebug()<<y6;

                innerIterator++;
                ed = (*innerIterator);
                int x7 = ed->text().toInt();
                qDebug()<<"iscitao sam x7: ";
                qDebug()<<x7;

                innerIterator++;
                ed = (*innerIterator);
                int y7 = ed->text().toInt();
                qDebug()<<"iscitao sam y7: ";
                qDebug()<<y7;

                QPolygonF Pol;
                Pol.append(QPointF(x1,y1));
                Pol.append(QPointF(x2,y2));
                Pol.append(QPointF(x3,y3));
                Pol.append(QPointF(x4,y4));
                Pol.append(QPointF(x5,y5));
                Pol.append(QPointF(x6,y6));
                Pol.append(QPointF(x7,y7));
                Pol.append(QPointF(x1,y1));

                QGraphicsPolygonItem *polygon = scene.addPolygon(Pol,pen, brush);

                scene.update();
                listGraphItems.push_back(polygon);
                break;
            }
            default:
            {
                qDebug()<<"Error option";
            }
        }


        ++iter;
        ++it;
    }
    //***************************

    qDebug()<<"broj dodatih item-a u listu: ";
    qDebug()<<listGraphItems.length();



//    if(listGraphItems.at(0)->collidesWithItem(listGraphItems.at(1))){
//        qDebug()<<"Ima kolizije";
//    } else {
//        qDebug()<<"Nema kolizije";
//    }

    deltaX = 5;
    int pom = ceil((G_x - S_x) / 5);
    deltaY = (S_y - G_y) / pom;

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(BUG_algorithm()));
    timer->start(80);

}

//ideja je da se robot krece ka cilju za konstantan korak po X koordinati i po Y kooridinati sve dok ne detektuje prepreku
//kada detektuje prepreku vrati se za taj korak levo i proba u levo da se pomeri ako nema kolizije pa proba opet napred
//ako ne moze nazad pa levo ide opet nazad, mora se uvek cuvati fleg koji oznacava prethodni potez napred nazad levo(desno)

void MainWindow::BUG_algorithm(){
    qDebug()<<"timer ispalio signal";
    //timer->stop();
    QPen pen;
    QBrush brush;
    pen.setColor(Qt::yellow);
    pen.setWidth(3);
    brush.setColor(Qt::blue);
    brush.setStyle(Qt::SolidPattern);


    qDebug()<<"Pocetna tacka";
    qDebug()<<S_x;
    qDebug()<<S_y;
    qDebug()<<"Krajnja tacka";
    qDebug()<<G_x;
    qDebug()<<G_y;


    qDebug()<<"ovo su delta x i y";
    qDebug()<<deltaX;
    qDebug()<<deltaY;
    vx += deltaX;
    if(vy < G_y){
        vy += fabs(deltaY);
    } else {
        vy -= fabs(deltaY);
    }
    //        qDebug()<<vx;
    //        qDebug()<<vy;

    robot = scene.addEllipse(vx-8,vy-8,16,16, pen, brush);
    brush.setColor(Qt::red);
    goal = scene.addEllipse(G_x-8,G_y-8,16,16, pen, brush);
    QGraphicsLineItem *l = scene.addLine(vx,vy,G_x,G_y);


    for(int i = 0; i < listGraphItems.length(); i++){
        if(robot->collidesWithItem(listGraphItems.at(i))){
           //naisaoje na neku prepreku treba da se vrati nazad i postavi prevStep
            prevStep = 2;
            vx -= deltaX;
            if(vy < G_y){
                vy -= fabs(deltaY);
            } else {
                vy += fabs(deltaY);
            }
        }
    }

    robot = scene.addEllipse(vx-8,vy-8,16,16, pen, brush);
    brush.setColor(Qt::red);
    goal = scene.addEllipse(G_x-8,G_y-8,16,16, pen, brush);
    l = scene.addLine(vx,vy,G_x,G_y);

    //ako se vracao u nazad probaj da se pomeris sa tog pravca
    while(prevStep == 2){
        if(vy < G_y){
            vy -= 5;
        } else {
            vy -= 5;
        }

        robot = scene.addEllipse(vx-8,vy-8,16,16, pen, brush);
        brush.setColor(Qt::red);
        goal = scene.addEllipse(G_x-8,G_y-8,16,16, pen, brush);
        QGraphicsLineItem *l = scene.addLine(vx,vy,G_x,G_y);

        bool flag = false;
        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                flag = true;
            }
        }

        if(flag){
            if(vy < G_y){
                vy += 5;
            } else {
                vy += 5;
            }
        } else {
            break;
        }

        vx -= deltaX;
        if(vy < G_y){
            vy -= fabs(deltaY);
        } else {
            vy += fabs(deltaY);
        }


    }
    if(robot->collidesWithItem(goal)){
            timer->stop();
            qDebug()<<"Cilj dostignut";
    }
         //azuriraj delta x i y
    deltaX = 5;
    int pom = ceil((G_x - vx) / 5);
    deltaY = (vy - G_y) / pom;

}



//    if(vx <= G_x){



//        vy = Pk*vx + Pn;

//        qDebug()<<Pk;
//        qDebug()<<Pn;
//        qDebug()<<vx;
//        qDebug()<<vy;

//        robot = scene.addEllipse(vx-8,vy-8,16,16, pen, brush);
//        brush.setColor(Qt::red);
//        goal = scene.addEllipse(G_x-8,G_y-8,16,16, pen, brush);
//        QGraphicsLineItem *l = scene.addLine(vx,vy,G_x,G_y);

        //proveriti da li ima koliziju sa nekom preprekom

        //    if(listGraphItems.at(0)->collidesWithItem(listGraphItems.at(1))){
        //        qDebug()<<"Ima kolizije";
        //    } else {
        //        qDebug()<<"Nema kolizije";
        //    }

//        for(int i = 0; i < numberObstacles; i++){
//            if(robot->collidesWithItem(listGraphItems.at(i))){
//                qDebug()<<"ima koliziju sa preprekom: "<<i;
//                qDebug()<<qAtan(Pk);
//                timer->stop();
//            }
//        }


//        //scene.update();
//        vx+=5;
//    }
//    else
//    {
//        timer->stop();
//    }
//}
//        qreal x = (m*G_x + (15-m)*S_x)/15 + 30;
//        qreal y = (m*G_y + (15-m)*S_y)/15 + 30;
//        if(x > 200.0){
//            timer->stop();
//        }
//        m++;
        //n--;

    //    S_x = x;
    //    S_y = y;

//        qDebug()<<"new points";
//        qDebug()<<x;
//        qDebug()<<y;



//        robot->setX(x);
//        robot->setY(y);
//        goal->setX(G_x);
//        goal->setY(G_y);
//        scene.update();
//        qDebug()<<"ispis cilja";
//        qDebug()<<goal->x();
//        qDebug()<<goal->y();
//        qDebug()<<"ispis robota";
//        qDebug()<<robot->x();
//        qDebug()<<robot->y();
//        qDebug()<<"ispis G";
//        qDebug()<<G_x;
//        qDebug()<<G_y;


