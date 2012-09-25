#include "schedulepoint.h"
#include "schedulescreen.h"
#include "settingscreen.h"
#include "globalsettings.h"
#include "utilities.h"

#include <QtGui>


#define MAXTEMP 100
#define MINTEMP 50

SchedulePoint::SchedulePoint(int seqNumber)
{
    m_globalSettings = GlobalSettings::getInstance();

    // create schedule point with necessary information
    ID = seqNumber;
    temp = 72;
    pen.setColor((Qt::white));
    setFlags(ItemIsSelectable);
    location=0;
    m_pressed = false;
    m_disabled = false;
    m_selected = false;
    updateUnit();
}

SchedulePoint::~SchedulePoint()
{
    // destructor
}

//calculation to make color vary linearly from defined MINTEMP to MAXTEMP
//based on passed temp

QColor SchedulePoint::tempToColor(int temp)
{
     float pct = ((float)temp-MINTEMP)/100.0 *((float)MAXTEMP/MINTEMP);
     return QColor(200*pct, 20, 200*(1-pct));
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
    QRectF rect;
    //choose either to go smallest size to fit the text or 90% of the row Height
    rect.setHeight(qMax(rect.height(), (qreal)(m_weekHeight*.90)));
    //adjust to make wider and look normal
    rect.adjust(-m_timeBlockWidth*2.5, 0, m_timeBlockWidth*2.5, 0);
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

    if(option->state & QStyle::State_Selected) {

    }



    if(m_disabled)
    {
        painter->setBrush(QColor(150,150,150));
        painter->setPen(QColor(110,110,110));
    }

    else if(m_selected)
    {
        pen.setWidth(3);
        pen.setColor(QColor(255, 189, 43));
        painter->setPen(pen);
        myBackgroundColor = tempToColor(temp);
        painter->setBrush(myBackgroundColor);
    }
    else
    {
        myBackgroundColor = tempToColor(temp);
        pen.setColor(Qt::white);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->setBrush(myBackgroundColor);
    }

    QRectF rect = outlineRect();
    painter->drawRoundRect(rect, roundness(rect.width()), roundness(rect.height()));
    painter->setPen(Qt::white);
    painter->drawText(rect, Qt::AlignCenter, myText);
}

void SchedulePoint::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if(disabled())
        return;

    // provide event handler for mouse click
    emit clicked(this);
    m_pressed = true;
    pen.setColor(Qt::black);
    setZValue(10);
    m_xClickPos = e->pos().x();
    m_yClickPos = e->pos().y();
}

void SchedulePoint::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    if(disabled())
        return;
    int oldTemp = temp;
    qreal oldX = pos().x();
    // provide event handler for mouse move
    emit clicked(this);
    if(m_pressed == true)
    {
        if(m_yClickPos - m_weekHeight/4 > e->pos().y())
        {
            m_yClickPos = e->pos().y();
            increaseTemp();
        }
        else if(m_yClickPos + m_weekHeight/4 < e->pos().y())
        {
            m_yClickPos = e->pos().y();
            decreaseTemp();
        }

        if(m_xClickPos - m_timeBlockWidth > e->pos().x())
        {
            //m_xClickPos = e->pos().x();
            shiftLeft();
        }
        else if(m_xClickPos + m_timeBlockWidth < e->pos().x())
        {
            //m_xClickPos = e->pos().x();
            shiftRight();
        }
    }

    emit(shareAdjustment(temp-oldTemp, pos().x()-oldX));

    e->accept();
}

void SchedulePoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if(disabled())
        return;
    setZValue(0);
    pen.setColor(Qt::white);
    e->accept();
}

int SchedulePoint::roundness(double size) const
{
    // rounded edges for rectangle
    const int diameter = 12;
    return 100 * diameter / int(size);
}


void SchedulePoint::shiftLeft()
{
    if(positionToTimeBlocks()<1)
        return;
    // provide a slot to allow point shifting to the left
    location--;
    this->moveBy(-m_timeBlockWidth, 0.0);
    update();
}

void SchedulePoint::shiftRight()
{
    if(positionToTimeBlocks()>94)
        return;
    // provide a slot to allow point shifting to the right
    location++;
    this->moveBy(+m_timeBlockWidth, 0.0);
    update();
}

void SchedulePoint::increaseTemp()
{
    // provide a slot to increase point temperature
    temp++;
    if(temp>MAXTEMP)temp=MAXTEMP;
    updateUnit();
}

void SchedulePoint::decreaseTemp()
{
    // provide a slot to decrease temperature
    temp--;
    if(temp<MINTEMP)temp=MINTEMP;
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
    int timeBlockCount = positionToTimeBlocks();

    int hours = (timeBlockCount/4.0f);
    int minutes = (timeBlockCount%4) *15;

    QTime time(hours%24, minutes);
    return formatTimeString(time, m_globalSettings->timeFormat());
}

int SchedulePoint::positionToTimeBlocks()
{
    return qRound((-3*m_timeBlockWidth + pos().x() -m_pointArea.left())/m_timeBlockWidth);
}

void SchedulePoint::updateUnit()
{
    myText = formatTemperatureString(temp, m_globalSettings->temperatureFormat());
    update();
}

void SchedulePoint::setDisabled(bool disabled)
{
    m_disabled = disabled;
    update();
}

bool SchedulePoint::disabled()
{
    return m_disabled;
}

void SchedulePoint::setSelected(bool selected)
{
    m_selected = selected;
    if(selected)
        setZValue(10);
    else
        setZValue(0);
    update();
}

bool SchedulePoint::selected()
{
    return m_selected;
}

void SchedulePoint::adjust(int tempAdjust, qreal xPos)
{
    if(tempAdjust>0)
        increaseTemp();
    if(tempAdjust<0)
        decreaseTemp();
    if(xPos<0)
        shiftLeft();
    if(xPos>0)
        shiftRight();
}
