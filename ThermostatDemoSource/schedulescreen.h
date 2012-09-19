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
    void showButtons(SchedulePoint *);
    void showPoints(int);
    void removeAllBlur();
    void updateData();
    void createScheduleScene();
    void showEvent(QShowEvent *);
    void addSchedulePoints();
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

    // create time markings
    QLabel *hour4;
    QLabel *hour8;
    QLabel *hour12;
    QLabel *hour16;
    QLabel *hour20;

    QRectF pointArea;
    qreal weekHeight;
    qreal timeBlockWidth;
    qreal timeWidth;

    GlobalSettings* m_globalSettings;

    bool m_initialized;

protected:
    void mousePressEvent(QMouseEvent */* event */);
    bool eventFilter(QObject *, QEvent *);

};

#endif // SCHEDULESCREEN_H
