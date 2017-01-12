#include "ui_controller.h"

/*
 * This function is called repeatedly(multishot timer)
 * and this output QPair current that represents the coordinates
 * of the current position of the robot
 *
 * */
void MainWindow::BUG_algorithm(){

    if(prevStep == 1){
        current = qMakePair(current.first+speed*unitDirectionVector.first, current.second+speed*unitDirectionVector.second);

        showCurrent();

        prevStep = 1;

        for(int i = 0; i < listGraphItems.length(); i++){
            if(robot->collidesWithItem(listGraphItems.at(i))){
                prevStep = 2;
                current = qMakePair(current.first-speed*unitDirectionVector.first, current.second-speed*unitDirectionVector.second);
                break;
            }
        }

        showCurrent();
    }

    if(prevStep == 2){

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

        if(prevStep == 2){
            current = qMakePair(current.first-speed*unitDirectionVector.first, current.second-speed*unitDirectionVector.second);

            showCurrent();
        }
    }

    if(prevStep == 3){
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
