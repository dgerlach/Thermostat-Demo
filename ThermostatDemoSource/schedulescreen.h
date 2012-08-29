#ifndef SCHEDULESCREEN_H
#define SCHEDULESCREEN_H

#include <QtGui>
#include "schedulepoint.h"

class ScheduleScreen : public QWidget
{
    Q_OBJECT
public:
    explicit ScheduleScreen(QWidget *parent = 0);
    
signals:
    void valueChanged();
    void clicked();
    
public slots:
    void shiftLeft();
    void shiftRight();
    void increaseTemp();
    void decreaseTemp();
    void showButtons(SchedulePoint *);
    void showPoints(int);
    void removeAllBlur();

private:
    QCheckBox *MondayButton;
    QCheckBox *TuesdayButton;
    QCheckBox *WednesdayButton;
    QCheckBox *ThursdayButton;
    QCheckBox *FridayButton;
    QCheckBox *SaturdayButton;
    QCheckBox *SundayButton;
    QButtonGroup *daysButtonGroup;
    QGraphicsScene *scene;
    QGraphicsView *view;
    void addSchedulePoints();
    int seqNumber;
    QGraphicsProxyWidget *proxyLeftButton;
    QGraphicsProxyWidget *proxyRightButton;
    QGraphicsProxyWidget *proxyUpButton;
    QGraphicsProxyWidget *proxyDownButton;
    QList <SchedulePoint *> pointList;
    SchedulePoint *point0;
    SchedulePoint *point1;
    SchedulePoint *point2;
    SchedulePoint *point3;
    SchedulePoint *point4;
    SchedulePoint *point5;
    SchedulePoint *point6;
    SchedulePoint *point7;
    SchedulePoint *point8;
    SchedulePoint *point9;
    SchedulePoint *point10;
    SchedulePoint *point11;
    SchedulePoint *point12;
    SchedulePoint *point13;
    SchedulePoint *point14;
    SchedulePoint *point15;
    SchedulePoint *point16;
    SchedulePoint *point17;
    SchedulePoint *point18;
    SchedulePoint *point19;
    SchedulePoint *point20;
    SchedulePoint *point21;
    SchedulePoint *point22;
    SchedulePoint *point23;
    SchedulePoint *point24;
    SchedulePoint *point25;
    SchedulePoint *point26;
    SchedulePoint *point27;
    SchedulePoint *currentPoint;
    void showPoint(SchedulePoint *);
    void hidePoints();
    void hideButtons();
    void shiftVerticalButtonsLeft();
    void shiftVerticalButtonsRight();
    void shiftHorizontalButtonsLeft();
    void shiftHorizontalButtonsRight();
    void shiftButtonsLeft();
    void shiftButtonsRight();
    QLabel *currentTime;
    void blur();

protected:
    void mousePressEvent(QMouseEvent */* event */);

};

#endif // SCHEDULESCREEN_H
