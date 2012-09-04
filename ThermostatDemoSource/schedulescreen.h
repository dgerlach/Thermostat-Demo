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
