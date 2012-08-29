#ifndef SCHEDULEPOINT_H
#define SCHEDULEPOINT_H

#include <QtGui>
#include <QApplication>
#include <QColor>
#include <QGraphicsItem>
#include <QSet>

class GlobalSettings;

class SchedulePoint : public QObject, public QGraphicsItem {
    Q_OBJECT
public:
    explicit SchedulePoint(int);
    ~SchedulePoint();
    void setText(const QString &text);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *);
    int getID();
    void setID(int);
    int getLocation();
    void setLocation(int);
    void shiftLeft();
    void shiftRight();
    void increaseTemp();
    void decreaseTemp();
    QString time();

public slots:
    void updateUnit();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent */* event */);
    void paintEvent(QPaintEvent */* event */);

private:
    QRectF outlineRect() const;
    int roundness(double size) const;
    QString myText;
    QColor myBackgroundColor;
    int temp;
    int ID;
    int location;

    GlobalSettings* m_globalSettings;
    
signals:
    void clicked(SchedulePoint *);


    
};

#endif // SCHEDULEPOINT_H
