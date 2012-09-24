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
    void setTimeBlockWidth(float timeBlockWidth);
    void setPointArea(QRectF pointArea);
    void setWeekHeight(int weekHeight);
    QColor tempToColor(int temp);
    int temp;
public slots:
    void updateUnit();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent */* event */);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * /* event */);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * /* event */);
    void paintEvent(QPaintEvent */* event */);

private:
    QRectF outlineRect() const;
    int roundness(double size) const;
    QString myText;
    QColor myBackgroundColor;

    int ID;
    int location;
    float m_timeBlockWidth;
    int m_weekHeight;
    QRectF m_pointArea;

    GlobalSettings* m_globalSettings;
    bool m_pressed;
    int m_xClickPos;
    int m_yClickPos;
    QPen pen; // outline color
    
signals:
    void clicked(SchedulePoint *);


    
};

#endif // SCHEDULEPOINT_H
