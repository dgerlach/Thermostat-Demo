#include "schedulepoint.h"
#include "schedulescreen.h"
#include "settingscreen.h"
#include "globalsettings.h"
#include "utilities.h"

#include <QtGui>

SchedulePoint::SchedulePoint(int seqNumber)
{
    m_globalSettings = GlobalSettings::getInstance();

    // create schedule point with necessary information
    ID = seqNumber;
    myBackgroundColor = Qt::blue;
    temp = 72;
    setFlags(ItemIsSelectable);
    location=0;
    updateUnit();
}

SchedulePoint::~SchedulePoint()
{
    // destructor
}

void SchedulePoint::setText(const QString &text)
{
    // add new text
    prepareGeometryChange();
    myText = text;
    update();
}

void SchedulePoint::setTimeBlockWidth(float timeBlockWidth)
{
    m_timeBlockWidth = timeBlockWidth;
}

void SchedulePoint::setWeekHeight(int weekHeight)
{
    m_weekHeight = weekHeight;
}

void SchedulePoint::setPointArea(QRectF pointArea)
{
    m_pointArea = pointArea;
}

QRectF SchedulePoint::outlineRect() const
{
    // define outline rectangle
    const int padding = 5;
    QFontMetricsF metrics = qApp->font();
    QRectF rect = metrics.boundingRect(myText);
    //choose either to go smallest size to fit the text or 90% of the row Height
    rect.setHeight(qMax(rect.height(), (qreal)(m_weekHeight*.90)));
    //adjust to make wider and look normal
    rect.adjust(-padding, 0, padding, 0);
    //make it so the coords represent the center
    rect.translate(-rect.center());
    return rect;
}

QRectF SchedulePoint::boundingRect() const
{
    // provide size of each point for collision detection
    const int margin = 1;
    return outlineRect().adjusted(-margin, -margin, +margin, +margin);
}

QPainterPath SchedulePoint::shape() const
{
    // provide shape of each point
    QRectF rect = outlineRect();
    QPainterPath path;
    path.addRoundRect(rect,roundness(rect.width()), roundness(rect.height()));
    return path;
}

void SchedulePoint::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option, QWidget * /*widget*/)
{
    // draw point itself
    QPen pen(Qt::white); // outline color
    if(option->state & QStyle::State_Selected) {

    }
    painter->setPen(pen);
    painter->setBrush(myBackgroundColor);

    QRectF rect = outlineRect();
    painter->drawRoundRect(rect, roundness(rect.width()), roundness(rect.height()));

    painter->drawText(rect, Qt::AlignCenter, myText);
}

void SchedulePoint::mousePressEvent(QGraphicsSceneMouseEvent * /* event */)
{
    // provide event handler for mouse click
    emit clicked(this);

}

int SchedulePoint::roundness(double size) const
{
    // rounded edges for rectangle
    const int diameter = 12;
    return 100 * diameter / int(size);
}


void SchedulePoint::shiftLeft()
{
    // provide a slot to allow point shifting to the left
    location--;
    this->moveBy(-m_timeBlockWidth, 0.0);
    update();
}

void SchedulePoint::shiftRight()
{
    // provide a slot to allow point shifting to the right
    location++;
    this->moveBy(+m_timeBlockWidth, 0.0);
    update();
}

void SchedulePoint::increaseTemp()
{
    // provide a slot to increase point temperature
    temp++;
    updateUnit();
}

void SchedulePoint::decreaseTemp()
{
    // provide a slot to decrease temperature
    temp--;
    updateUnit();
}

int SchedulePoint::getID()
{
    // provide point's unique ID number
    return ID;
}

void SchedulePoint::setID(int seqNumber)
{
    // give point a unique ID number
    ID = seqNumber;
}

void SchedulePoint::setLocation(int num)
{
    // track point's horizontal movement from starting position
    location=num;
}

int SchedulePoint::getLocation()
{
    // return how much point has moved from start
    return location;
}

QString SchedulePoint::time()
{
    // output the current time represnted by this point's location
    int timeBlockCount = qRound((pos().x() -m_pointArea.left())/m_timeBlockWidth);
    int hours = (timeBlockCount/4.0f + 2.0f);
    qDebug() << "TIME: " << hours << pos().x() << timeBlockCount<< m_pointArea.left() << m_timeBlockWidth;
    int minutes = (timeBlockCount%4) *15;
    qDebug() << "TIME MINUTES: " << minutes << pos().x() << timeBlockCount<< m_pointArea.left() << m_timeBlockWidth;
    QTime time(hours, minutes);
    return formatTimeString(time, m_globalSettings->timeFormat());
}

void SchedulePoint::updateUnit()
{
    myText = formatTemperatureString(temp, m_globalSettings->temperatureFormat());
    update();
}
