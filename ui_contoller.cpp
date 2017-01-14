#include "ui_controller.h"
#include "user_interface.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    UIMW = new Ui_MainWindow();
    UIMW->setupUi(this);
    vbox = NULL;
    prevStep = 1;
    timer = new QTimer();
    QObject::connect(UIMW->radioButton, SIGNAL(pressed()), this, SLOT(showInputsForObstacles()));
    QObject::connect(UIMW->initOk, SIGNAL(pressed()), this, SLOT(initializationFinish()));
    QObject::connect(UIMW->Apply, SIGNAL(pressed()), this, SLOT(readEnteredData()));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(BUG_algorithm()));
    QObject::connect(UIMW->resetButton, SIGNAL(pressed()), this, SLOT(resetAll()));
    QObject::connect(UIMW->loadFileButton, SIGNAL(pressed()), this, SLOT(loadFromFile()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showInputsForObstacles(){
    UIMW->lineEdit_5->setReadOnly(false);
}

void MainWindow::initializationFinish(){

    QPainter pn( this );

    timer->stop();
    //read start point coordiante
    S_x = UIMW->lineEdit->text().toFloat();
    S_y = -(UIMW->lineEdit_2->text().toFloat());
    //read goal point coordinate
    G_x = UIMW->lineEdit_3->text().toFloat();
    G_y = -(UIMW->lineEdit_4->text().toFloat());

    start = qMakePair(S_x,S_y);
    target = qMakePair(G_x,G_y);
    current = qMakePair(S_x,S_y);
    //start->target vector
    directionVector = qMakePair(target.first-start.first, target.second-start.second);
    qreal tmp1 = qPow(directionVector.first,2) + qPow(directionVector.second,2);
    double tmp = qSqrt(tmp1);
    //the unit direction vector
    unitDirectionVector = qMakePair(directionVector.first/tmp,directionVector.second/tmp);

    scene.clear();
    UIMW->graphicsView->setScene(&scene);
    UIMW->graphicsView->update();

    numberObstacles = UIMW->lineEdit_5->text().toInt();
    qDebug()<<"numberobstacles";
    qDebug()<<numberObstacles;

    QStringList listS=(QStringList()<<"odaberi"<<"krug"<<"elipsa"<<"trougao"<<"cetvorougao"<<"petougao"<<"sestougao"<<"sedmougao");
    UIMW->scrollArea->setWidgetResizable(true);

    //data about obstacles
    itemsList.clear();
    comboBoxList.clear();
    pointsBoxList.clear();
    listGraphItems.clear();

    // For every obstacle add data in lists. Create a list of labels, comboboxes
    for(int i = 0; i < numberObstacles; i++){
        //create label for all obstacles and add in list
        QString s = "Obstacles ";
        s.append(QString::number(i+1));
        s.append(":");
        QLabel *lab = new QLabel(s);
        lab->setMaximumWidth(180);
        QFont f( "Arial", 8, QFont::Bold);
        lab->setFont( f);
        itemsList.push_back(lab);
        qDebug()<<lab->text();

        //Create combobox for all obstacles
        QComboBox *cb = new QComboBox();
        cb->setMinimumHeight(27);
        cb->setMaximumWidth(180);
        cb->setFont(f);
        cb->addItems(listS);
        QObject::connect(cb,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(showInputsCoordinate(const QString&)));
        comboBoxList.push_back(cb);

        //grid layout for every obstacle
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
    }
}

void MainWindow::showInputsCoordinate(const QString& text){
    UIMW->Apply->setEnabled(true);
    qDebug()<<text;
    qDebug()<<QObject::sender();
    QList<QComboBox*>::iterator iter = comboBoxList.begin();
    int i = 0;
    int opt = -1; //option in combobox
    int index = -1; //index in obstacles

    while(iter != comboBoxList.end()){
        if((*iter) == QObject::sender()){
            qDebug()<<"find in list. order number: ";
            qDebug()<<i;
            qDebug()<<"selected option is: ";
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
        qDebug()<<"Error input";
        qDebug()<<index;
    }
    }
}

void MainWindow::readEnteredData(){
    QPen pen;
    QBrush brush;
    pen.setColor(Qt::red);
    pen.setWidth(2);
    brush.setColor(Qt::green);
    brush.setStyle(Qt::SolidPattern);

    scene.clear();

    QList<QComboBox*>::iterator iter = comboBoxList.begin();
    QList<QList<QLineEdit*>*>::iterator it = listPoints.begin();
    for(int i = 0; i < numberObstacles; i++){
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
                int y = -(ed->text().toInt());
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
                int y = -(ed->text().toInt());
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
                qDebug()<<"iscitao sam b: ";
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
                int y1 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = -(ed->text().toInt());
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
                int y1 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                innerIterator++;
                ed = (*innerIterator);
                int x4 = ed->text().toInt();
                qDebug()<<"iscitao sam x4: ";
                qDebug()<<x4;

                innerIterator++;
                ed = (*innerIterator);
                int y4 = -(ed->text().toInt());
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
                int y1 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                innerIterator++;
                ed = (*innerIterator);
                int x4 = ed->text().toInt();
                qDebug()<<"iscitao sam x4: ";
                qDebug()<<x4;

                innerIterator++;
                ed = (*innerIterator);
                int y4 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y4: ";
                qDebug()<<y4;

                innerIterator++;
                ed = (*innerIterator);
                int x5 = ed->text().toInt();
                qDebug()<<"iscitao sam x5: ";
                qDebug()<<x5;

                innerIterator++;
                ed = (*innerIterator);
                int y5 = -(ed->text().toInt());
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
                int y1 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                innerIterator++;
                ed = (*innerIterator);
                int x4 = ed->text().toInt();
                qDebug()<<"iscitao sam x4: ";
                qDebug()<<x4;

                innerIterator++;
                ed = (*innerIterator);
                int y4 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y4: ";
                qDebug()<<y4;

                innerIterator++;
                ed = (*innerIterator);
                int x5 = ed->text().toInt();
                qDebug()<<"iscitao sam x5: ";
                qDebug()<<x5;

                innerIterator++;
                ed = (*innerIterator);
                int y5 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y5: ";
                qDebug()<<y5;

                innerIterator++;
                ed = (*innerIterator);
                int x6 = ed->text().toInt();
                qDebug()<<"iscitao sam x6: ";
                qDebug()<<x6;

                innerIterator++;
                ed = (*innerIterator);
                int y6 = -(ed->text().toInt());
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
                int y1 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y1: ";
                qDebug()<<y1;

                innerIterator++;
                ed = (*innerIterator);
                int x2 = ed->text().toInt();
                qDebug()<<"iscitao sam x2: ";
                qDebug()<<x2;

                innerIterator++;
                ed = (*innerIterator);
                int y2 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y2: ";
                qDebug()<<y2;

                innerIterator++;
                ed = (*innerIterator);
                int x3 = ed->text().toInt();
                qDebug()<<"iscitao sam x3: ";
                qDebug()<<x3;

                innerIterator++;
                ed = (*innerIterator);
                int y3 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y3: ";
                qDebug()<<y3;

                innerIterator++;
                ed = (*innerIterator);
                int x4 = ed->text().toInt();
                qDebug()<<"iscitao sam x4: ";
                qDebug()<<x4;

                innerIterator++;
                ed = (*innerIterator);
                int y4 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y4: ";
                qDebug()<<y4;

                innerIterator++;
                ed = (*innerIterator);
                int x5 = ed->text().toInt();
                qDebug()<<"iscitao sam x5: ";
                qDebug()<<x5;

                innerIterator++;
                ed = (*innerIterator);
                int y5 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y5: ";
                qDebug()<<y5;

                innerIterator++;
                ed = (*innerIterator);
                int x6 = ed->text().toInt();
                qDebug()<<"iscitao sam x6: ";
                qDebug()<<x6;

                innerIterator++;
                ed = (*innerIterator);
                int y6 = -(ed->text().toInt());
                qDebug()<<"iscitao sam y6: ";
                qDebug()<<y6;

                innerIterator++;
                ed = (*innerIterator);
                int x7 = ed->text().toInt();
                qDebug()<<"iscitao sam x7: ";
                qDebug()<<x7;

                innerIterator++;
                ed = (*innerIterator);
                int y7 = -(ed->text().toInt());
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

    qDebug()<<"broj dodatih item-a u listu: ";
    qDebug()<<listGraphItems.length();

    //proveravamo kakave orjentacije je trougao koji grade vektori directionVector i normalVector
    normalVector = qMakePair(directionVector.second/directionVector.first,-1);

    double check = directionVector.first*normalVector.second-directionVector.second*normalVector.first;

    qreal pomm = qPow(normalVector.first,2) + qPow(normalVector.second,2);
    double tmp = qSqrt(pomm);
    //izracunavamo smo jedinicni vektor normale(koji ima smer ,,levo")
    if(check >= 0){
        normalVector = qMakePair(-normalVector.first/pomm,-normalVector.second/pomm);
    } else {
        normalVector = qMakePair(normalVector.first/pomm,normalVector.second/pomm);
    }

    timer = new QTimer();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(BUG_algorithm()));
    timer->start(80);

}

//ideja je da se robot krece ka cilju za konstantan korak po X koordinati i po Y kooridinati sve dok ne detektuje prepreku
//kada detektuje prepreku vrati se za taj korak levo i proba u levo da se pomeri ako nema kolizije pa proba opet napred
//ako ne moze nazad pa levo ide opet nazad, mora se uvek cuvati fleg koji oznacava prethodni potez napred nazad levo(desno)


void MainWindow::showCurrent(){
    QPen pen;
    QBrush brush;
    pen.setColor(Qt::yellow);
    pen.setWidth(3);
    brush.setColor(Qt::blue);
    brush.setStyle(Qt::SolidPattern);

    robot = scene.addEllipse(current.first-8,current.second-8,16,16, pen, brush);
    brush.setColor(Qt::red);
    goal = scene.addEllipse(target.first-8,target.second-8,16,16, pen, brush);
}

void MainWindow::resetAll(){

    timer->stop();

    //*****************

    scene.clear();
    //ovde cuvamo labele koje se isisuju iznad unosa podataka zasvaku prepreku
    itemsList.clear();
    //cuvamo comboBox-eve za svaku prepreku(odabir tipa prepreke)
    comboBoxList.clear();
    //u zavisnosti od odabraneopcije u comboBox-u prikazemo odredjen broj polja za unos
    pointsBoxList.clear();
    //sadrzi unete podatke
    listPoints.clear();
    vbox = NULL;
    listGraphItems.clear();
    robot = NULL;
    goal = NULL;
    timer = NULL;

    prevStep = 1;
    flagDrawLine = true;
    //*****************

    ui->setupUi(this);

    UIMW = new Ui_MainWindow();
    UIMW->setupUi(this);
    vbox = NULL;
    QObject::connect(UIMW->radioButton, SIGNAL(pressed()), this, SLOT(showInputsForObstacles()));
    QObject::connect(UIMW->initOk, SIGNAL(pressed()), this, SLOT(initializationFinish()));
    QObject::connect(UIMW->Apply, SIGNAL(pressed()), this, SLOT(readEnteredData()));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(BUG_algorithm()));
    QObject::connect(UIMW->resetButton, SIGNAL(pressed()), this, SLOT(resetAll()));
    QObject::connect(UIMW->loadFileButton, SIGNAL(pressed()), this, SLOT(loadFromFile()));

    prevStep = 1;
    timer = new QTimer();
}


void MainWindow::loadFromFile(){
    qDebug()<<"loadFromFile called";

    QPen pen;
    QBrush brush;

    QString path = QFileDialog::getOpenFileName(this,
        tr("Open file"), "/home/", tr("Text Files (*.txt)"));
    qDebug()<<"PATH:"<<path;

    QFile inputFile(path);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);

        QString line = in.readLine();
        start = qMakePair(line.split(" ").at(0).toInt(),line.split(" ").at(1).toInt());
        line = in.readLine();
        target = qMakePair(line.split(" ").at(0).toInt(),-(line.split(" ").at(1).toInt()));
        qDebug()<<"start and target point";
        qDebug()<<start.first<<" "<<start.second<<" "<<target.first<<" "<<target.second;

        pen.setColor(Qt::yellow);
        pen.setWidth(3);
        brush.setColor(Qt::blue);
        brush.setStyle(Qt::SolidPattern);

//        robot = scene.addEllipse(current.first-8,current.second-8,16,16, pen, brush);
//        brush.setColor(Qt::red);
//        goal = scene.addEllipse(target.first-8,target.second-8,16,16, pen, brush);

        pen.setColor(Qt::red);
        pen.setWidth(2);
        brush.setColor(Qt::green);
        brush.setStyle(Qt::SolidPattern);

        UIMW->graphicsView->setScene(&scene);
        UIMW->graphicsView->update();

        while (!in.atEnd())
        {
            line = in.readLine();
            qDebug()<<line;

            //read first word in line krug,elipsa,trougao
            int typeObstaales = line.split(" ").at(0).toInt();

            switch (typeObstaales) {
                case 1:
                {
                qDebug()<<"kruuuuuuuug";
                    int x = line.split(" ").at(1).toInt();
                    int y = -(line.split(" ").at(2).toInt());
                    int r = line.split(" ").at(3).toInt();
                    qDebug()<<x<<" "<<y<<" "<<r;


                    QGraphicsEllipseItem *elipse = scene.addEllipse(x-r/2,y-r/2,r,r, pen, brush);
                    scene.update();

                    listGraphItems.push_back(elipse);

                    break;
                }
                case 2:
                {
                    int x = line.split(" ").at(1).toInt();
                    int y = -(line.split(" ").at(2).toInt());
                    int a = line.split(" ").at(3).toInt();
                    int b = line.split(" ").at(4).toInt();

                    qDebug()<<x<<" "<<y<<" "<<a<<" "<<b;


                    QGraphicsEllipseItem *elipse = scene.addEllipse(x-a/2,y-b/2,a,b, pen, brush);
                    scene.update();

                    listGraphItems.push_back(elipse);

                    break;
                }
                case 3:{
                    int x1 = line.split(" ").at(1).toInt();
                    int y1 = -(line.split(" ").at(2).toInt());
                    int x2 = line.split(" ").at(3).toInt();
                    int y2 = -(line.split(" ").at(4).toInt());
                    int x3 = line.split(" ").at(5).toInt();
                    int y3 = -(line.split(" ").at(6).toInt());

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
                case 4:{
                    int x1 = line.split(" ").at(1).toInt();
                    int y1 = -(line.split(" ").at(2).toInt());
                    int x2 = line.split(" ").at(3).toInt();
                    int y2 = -(line.split(" ").at(4).toInt());
                    int x3 = line.split(" ").at(5).toInt();
                    int y3 = -(line.split(" ").at(6).toInt());
                    int x4 = line.split(" ").at(7).toInt();
                    int y4 = -(line.split(" ").at(8).toInt());

                    QPolygonF Triangle;
                    Triangle.append(QPointF(x1,y1));
                    Triangle.append(QPointF(x2,y2));
                    Triangle.append(QPointF(x3,y3));
                    Triangle.append(QPointF(x4,y4));
                    Triangle.append(QPointF(x1,y1));

                    QGraphicsPolygonItem *polygon = scene.addPolygon(Triangle,pen, brush);

                    scene.update();
                    listGraphItems.push_back(polygon);
                    break;
                }
                case 5:{
                    int x1 = line.split(" ").at(1).toInt();
                    int y1 = -(line.split(" ").at(2).toInt());
                    int x2 = line.split(" ").at(3).toInt();
                    int y2 = -(line.split(" ").at(4).toInt());
                    int x3 = line.split(" ").at(5).toInt();
                    int y3 = -(line.split(" ").at(6).toInt());
                    int x4 = line.split(" ").at(7).toInt();
                    int y4 = -(line.split(" ").at(8).toInt());
                    int x5 = line.split(" ").at(9).toInt();
                    int y5 = -(line.split(" ").at(10).toInt());

                    QPolygonF Triangle;
                    Triangle.append(QPointF(x1,y1));
                    Triangle.append(QPointF(x2,y2));
                    Triangle.append(QPointF(x3,y3));
                    Triangle.append(QPointF(x4,y4));
                    Triangle.append(QPointF(x5,y5));
                    Triangle.append(QPointF(x1,y1));

                    QGraphicsPolygonItem *polygon = scene.addPolygon(Triangle,pen, brush);

                    scene.update();
                    listGraphItems.push_back(polygon);
                    break;
                }
                case 6:{
                    int x1 = line.split(" ").at(1).toInt();
                    int y1 = -(line.split(" ").at(2).toInt());
                    int x2 = line.split(" ").at(3).toInt();
                    int y2 = -(line.split(" ").at(4).toInt());
                    int x3 = line.split(" ").at(5).toInt();
                    int y3 = -(line.split(" ").at(6).toInt());
                    int x4 = line.split(" ").at(7).toInt();
                    int y4 = -(line.split(" ").at(8).toInt());
                    int x5 = line.split(" ").at(9).toInt();
                    int y5 = -(line.split(" ").at(10).toInt());
                    int x6 = line.split(" ").at(11).toInt();
                    int y6 = -(line.split(" ").at(12).toInt());

                    QPolygonF Triangle;
                    Triangle.append(QPointF(x1,y1));
                    Triangle.append(QPointF(x2,y2));
                    Triangle.append(QPointF(x3,y3));
                    Triangle.append(QPointF(x4,y4));
                    Triangle.append(QPointF(x5,y5));
                    Triangle.append(QPointF(x6,y6));
                    Triangle.append(QPointF(x1,y1));

                    QGraphicsPolygonItem *polygon = scene.addPolygon(Triangle,pen, brush);

                    scene.update();
                    listGraphItems.push_back(polygon);
                    break;
                }
                case 7:{
                    int x1 = line.split(" ").at(1).toInt();
                    int y1 = -(line.split(" ").at(2).toInt());
                    int x2 = line.split(" ").at(3).toInt();
                    int y2 = -(line.split(" ").at(4).toInt());
                    int x3 = line.split(" ").at(5).toInt();
                    int y3 = -(line.split(" ").at(6).toInt());
                    int x4 = line.split(" ").at(7).toInt();
                    int y4 = -(line.split(" ").at(8).toInt());
                    int x5 = line.split(" ").at(9).toInt();
                    int y5 = -(line.split(" ").at(10).toInt());
                    int x6 = line.split(" ").at(11).toInt();
                    int y6 = -(line.split(" ").at(12).toInt());
                    int x7 = line.split(" ").at(13).toInt();
                    int y7 = -(line.split(" ").at(14).toInt());

                    QPolygonF Triangle;
                    Triangle.append(QPointF(x1,y1));
                    Triangle.append(QPointF(x2,y2));
                    Triangle.append(QPointF(x3,y3));
                    Triangle.append(QPointF(x4,y4));
                    Triangle.append(QPointF(x5,y5));
                    Triangle.append(QPointF(x6,y6));
                    Triangle.append(QPointF(x7,y7));
                    Triangle.append(QPointF(x1,y1));

                    QGraphicsPolygonItem *polygon = scene.addPolygon(Triangle,pen, brush);

                    scene.update();
                    listGraphItems.push_back(polygon);
                    break;
                }
                default:
                {
                    qDebug()<<"Error";
                    break;
                }
            }
        }
        inputFile.close();

        prevStep = 1;
        current = qMakePair(start.first, start.second);
        //start->target vector
        directionVector = qMakePair(target.first-start.first, target.second-start.second);
        qreal tmp1 = qPow(directionVector.first,2) + qPow(directionVector.second,2);
        double tmp = qSqrt(tmp1);
        //the unit direction vector
        unitDirectionVector = qMakePair(directionVector.first/tmp,directionVector.second/tmp);

        normalVector = qMakePair(directionVector.second/directionVector.first,-1);

        double check = directionVector.first*normalVector.second-directionVector.second*normalVector.first;

        qreal pomm = qPow(normalVector.first,2) + qPow(normalVector.second,2);
        tmp = qSqrt(pomm);
        //izracunavamo smo jedinicni vektor normale(koji ima smer ,,levo")
        if(check >= 0){
            normalVector = qMakePair(-normalVector.first/pomm,-normalVector.second/pomm);
        } else {
            normalVector = qMakePair(normalVector.first/pomm,normalVector.second/pomm);
        }

        timer = new QTimer();
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(BUG_algorithm()));
        timer->start(80);
    }

}
