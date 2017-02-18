#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include "user_interface.h"
#include <QDebug>
#include <QComboBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFrame>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QTimer>
#include <QThread>
#include <QtCore/qmath.h>
#include <math.h>
#include <QPair>
#include <QFileDialog>
#include <QtGlobal>
#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QtGui>
#include <QSpinBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event);

private:
    Ui::MainWindow *ui;
    Ui_MainWindow *UIMW;
    //scena na kojoj crtamo i prikazujemo rad algoritma
    QGraphicsScene scene;
    //ovde cuvamo labele koje se isisuju iznad unosa podataka zasvaku prepreku
    QList<QLabel*> itemsList;
    //cuvamo comboBox-eve za svaku prepreku(odabir tipa prepreke)
    QList<QComboBox*> comboBoxList;
    //u zavisnosti od odabraneopcije u comboBox-u prikazemo odredjen broj polja za unos
    QList<QGridLayout*> pointsBoxList;
    //sadrzi unete podatke
    QList<QList<QLineEdit*>*> listPoints;
    QVBoxLayout *vbox;
    int numberObstacles;
    //lista geometrijskih figura dodatih na scenu
    QList <QGraphicsItem *> listGraphItems;
    //start point
    qreal S_x;
    qreal S_y;
    //goal point
    qreal G_x;
    qreal G_y;
    QGraphicsEllipseItem *robot;
    QGraphicsEllipseItem *goal;
    QTimer *timer;
    int m;
    double Pn;
    double Pk;

    //trenutne koordinate robota
    double vx;
    double vy;

    int minX;
    int maxX;
    int minY;
    int maxY;

    qreal h11;
    qreal h12;
    qreal h21;
    qreal h22;


    //korak po X i Y koji se menja
    double deltaX;
    double deltaY;

    // 1 ka cilju
    // 2 nazad
    // 3 promena pravca
    int prevStep;

    QPair<double, double> start;
    QPair<double, double> target;
    QPair<double, double> current;
    QPair<double, double> directionVector;
    QPair<double, double> unitDirectionVector;
    QPair<double, double> normalVector;
    QPair<double, double> notHaveColision;
    int counter = 0;

    //robot step size
    int speed = 2;
    QAction* actionReboot;
    void showCurrent();
    bool flagDrawLine = true;

    double minimum(double a, double b);
    double maximum(double a, double b);

private Q_SLOTS:
    void showInputsForObstacles();
    void initializationFinish();
    void showInputsCoordinate(const QString&);
    void readEnteredData();
    void BUG_algorithm();
    void resetAll();
    void loadFromFile();
    void helpDialog();
    void changedSpeed(int v);
};

#endif // MAINWINDOW_H
