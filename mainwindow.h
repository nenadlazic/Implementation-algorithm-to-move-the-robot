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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
    void emitshowInputsCoordinateSignal(int orderNumObstacles, int option);
private Q_SLOTS:
    void showInputsForObstacles();
    void initializationFinish();
    void showInputsCoordinate(const QString&);
signals:
    void signalChangedOptions(int orderNumObstacles, int option);
};

#endif // MAINWINDOW_H
