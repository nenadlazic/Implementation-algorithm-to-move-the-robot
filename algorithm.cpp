#include "ui_controller.h"

#define use_visualisation 1
/*
 * This function is called repeatedly(multishot timer)
 * and this output QPair current that represents the coordinates
 * of the current position of the robot
 *
 * */
void MainWindow::BUG_algorithm(){
    qDebug()<<"Entered BUG algorithm";

    if(prevStep == 1){
        current = qMakePair(current.first+speed*unitDirectionVector.first, current.second+speed*unitDirectionVector.second);
        flagDrawLine = true;
        #ifdef use_visualisation
        showCurrent();
        #endif

        qDebug()<<"prevStep = 1";

        prevStep = 1;

        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                prevStep = 2;
                current = qMakePair(current.first-speed*unitDirectionVector.first, current.second-speed*unitDirectionVector.second);
                break;
            }
        }

        if(prevStep == 1){
            notHaveColision = qMakePair(current.first,current.second);
        }

        showCurrent();
    }

    //proba levo
    if(prevStep == 2){
        flagDrawLine = false;

        qDebug()<<"prevStep = 2";

        current = qMakePair(current.first+speed*normalVector.first, current.second+speed*normalVector.second);

        showCurrent();

        prevStep = 3;

        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                prevStep = 2;
                current = qMakePair(current.first-speed*normalVector.first, current.second-speed*normalVector.second);
                break;
            }
        }

        //ako nije mogao levo treba da proba nazad jos da ide to je prevStep = 5
        if(prevStep == 2){
            qDebug()<<"udario gore";
            prevStep = 5;
            counter = 0;
            current = qMakePair(notHaveColision.first,notHaveColision.second);
            showCurrent();


        }
        if(prevStep==3){
            notHaveColision = qMakePair(current.first,current.second);
        }
    }

    //treba da proba nazad
    if(prevStep == 5){
        qDebug()<<"debug1 case 5";
        current = qMakePair(current.first-speed*unitDirectionVector.first, current.second-speed*unitDirectionVector.second);
        showCurrent();

        qDebug()<<"koordinate robota nakon pomeranja levo";
        qDebug()<<current.first<<" "<<current.second;

        //proba da vidi da li ima kolizija
        bool flagCollisonBack = false;
        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                flagCollisonBack = true;
                current = qMakePair(current.first+speed*unitDirectionVector.first, current.second+speed*unitDirectionVector.second);
                break;
            }
        }

        showCurrent();

        if(flagCollisonBack == true){
            prevStep = 6;
            counter = 0;
        } else {
            counter += 1;
            notHaveColision = qMakePair(current.first,current.second);
            if(counter>1){
                prevStep = 8;//ako je vec isao dosta u nazad mozda moze levo
            }
        }
    }

    //ako upada u oaj if to znaci da je isao nazad i da ne moze vise nazad->probaj dole
    if(prevStep == 6){

        current = qMakePair(current.first-speed*normalVector.first, current.second-speed*normalVector.second);
        showCurrent();
        qDebug()<<"koordinate robota nakon pomeranja dole";
        qDebug()<<current.first<<" "<<current.second;

        bool flagCollisonDown = false;

        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                flagCollisonDown = true;
                current = qMakePair(current.first+speed*normalVector.first, current.second+speed*normalVector.second);
                break;
            }
        }

        showCurrent();

        if(flagCollisonDown == true){
            prevStep = 7;
        } else {
            prevStep = 5;
            notHaveColision = qMakePair(current.first,current.second);
        }

    }

    //ako upada u oaj if to znaci da je isao nazad i dole da ne moze vise nazad i dole->probaj malo desno
    if(prevStep == 7){
        current = qMakePair(current.first+speed*unitDirectionVector.first, current.second+speed*unitDirectionVector.second);
        showCurrent();
        qDebug()<<"koordinate robota nakon pomeranja desno";
        qDebug()<<current.first<<" "<<current.second;

        bool flagCollisonRight = false;

        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                flagCollisonRight = true;
                current = qMakePair(current.first-speed*unitDirectionVector.first, current.second-speed*unitDirectionVector.second);
                break;
            }
        }

        showCurrent();

        if(flagCollisonRight== true){
            prevStep = 7;
        } else {
            prevStep = 6;
            notHaveColision = qMakePair(current.first,current.second);
        }

    }


    //ako upada u oaj if to znaci da je isao nazad dosta i da sad mozda moze levo->probaj malo levo u odnosu na pravac ka cilju
    if(prevStep == 8){
        current = qMakePair(current.first+speed*normalVector.first, current.second+speed*normalVector.second);
        showCurrent();
        qDebug()<<"koordinate robota nakon pomeranja leviu odnosu na pravac";
        qDebug()<<current.first<<" "<<current.second;

        bool flagCollisonLeft = false;

        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                flagCollisonLeft = true;
                current = qMakePair(current.first-speed*normalVector.first, current.second-speed*normalVector.second);
                break;
            }
        }

        showCurrent();

        if(flagCollisonLeft == true){
            prevStep = 5;
        } else {
            prevStep = 2;
            notHaveColision = qMakePair(current.first,current.second);
        }

    }


    if(prevStep == 4){
        qDebug()<<"prevStep = 4";

        current = qMakePair(current.first-speed*normalVector.first, current.second-speed*normalVector.second);
        current = qMakePair(current.first-speed*unitDirectionVector.first, current.second-speed*unitDirectionVector.second);

        bool flag = false;

        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                flag = true;
                current = qMakePair(current.first+speed*unitDirectionVector.first, current.second+speed*unitDirectionVector.second);
                break;
            }
        }

        if(flag == true){
            current = qMakePair(current.first+speed*unitDirectionVector.first, current.second+speed*unitDirectionVector.second);
        }

        showCurrent();


    }

    if(prevStep == 3){
        flagDrawLine = true;
        qDebug()<<"prevStep = 3";

        directionVector = qMakePair(target.first-current.first, target.second-current.second);
        qreal tmp1 = qPow(directionVector.first,2) + qPow(directionVector.second,2);
        double tmp = qSqrt(tmp1);
        unitDirectionVector = qMakePair(directionVector.first/tmp,directionVector.second/tmp);

        prevStep = 1;
    }

    if(robot->collidesWithItem(goal)){
        timer->stop();
        timer->destroyed();
        qDebug()<<"GOAL REACHED";
    }
}
