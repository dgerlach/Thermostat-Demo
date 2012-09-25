#ifndef SCHEDULESCREEN_H
#define SCHEDULESCREEN_H

#include <QtGui>
#include "schedulepoint.h"

class GlobalSettings;

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
    void selectDay(SchedulePoint *);
    void unselectDay();
    void showPoints(int);
    void removeAllBlur();
    void updateData();
    void createScheduleScene();
    void showEvent(QShowEvent *);
    void addSchedulePoints();
    void disableRow(bool checked);
    void initializeScene();
    void initializeGraphics();

private:
    QButtonGroup *daysButtonGroup;
    QGraphicsScene *scene;
    QGraphicsView *view;

    int seqNumber;
    QGraphicsProxyWidget *proxyLeftButton;
    QGraphicsProxyWidget *proxyRightButton;
    QGraphicsProxyWidget *proxyUpButton;
    QGraphicsProxyWidget *proxyDownButton;
    QList <SchedulePoint *> pointList;
    QList <QPair<QPointF, int> > pointSave;


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

    QRectF pointArea;
    qreal weekHeight;
    qreal timeBlockWidth;
    qreal timeWidth;

    GlobalSettings* m_globalSettings;

    bool m_initialized;

protected:
    void mousePressEvent(QMouseEvent */* event */);
};

#endif // SCHEDULESCREEN_H
