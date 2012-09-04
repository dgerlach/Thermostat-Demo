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

QRectF SchedulePoint::outlineRect() const
{
    // define outline rectangle
    const int padding = 3;
    QFontMetricsF metrics = qApp->font();
    QRectF rect = metrics.boundingRect(myText);
    rect.adjust(-padding, -padding, +padding, +padding);
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
    this->moveBy(-3.4, 0.0);
    update();
}

void SchedulePoint::shiftRight()
{
    // provide a slot to allow point shifting to the right
    location++;
    this->moveBy(+3.4, 0.0);
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
    int hours = ((15/3.4)*(this->pos().x()+12.7)) / 60;
    int minutes = (int)((15/3.4)*(this->pos().x()+12.7)) % 60;
    QTime *time = new QTime(hours, minutes);
    return time->toString("h:mm AP");
}

void SchedulePoint::updateUnit()
{
    myText = formatTemperatureString(temp, m_globalSettings->temperatureFormat());
    update();
}
