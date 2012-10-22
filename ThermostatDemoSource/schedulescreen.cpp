#include "schedulescreen.h"
#include "schedulepoint.h"
#include "globalsettings.h"
#include "utilities.h"

#include "QtDebug"

#include <QRect>

//array that holds the times the columns should be initially positioned
//0-23.75 in .25 increments (15 min)
float columnTimes [] = {5,9.5,17,22};

ScheduleScreen::ScheduleScreen(QWidget *parent) :
    QWidget(parent)
{
    setObjectName("ScheduleScreen");
    //get an instance of the global configuration
    m_globalSettings = GlobalSettings::getInstance();

    // seqNumber will track the number of points that have been created
    seqNumber = 0;

    // allow user to click off points to remove blur and see all
    connect(this, SIGNAL(clicked()), this, SLOT(unselectDay()));
    //allow valueChanged signal to trigger updateData() slot
    connect(this, SIGNAL(valueChanged()), this, SLOT(updateData()));
    // create title
    QLabel *title = new QLabel("Set your WEEKLY SCHEDULE");
    title->setObjectName("title");

    // create time represented by current point
    currentTime = new QLabel();
    currentTime->setObjectName("currentTime");
    currentTime->setMinimumWidth(80);

    // create back button
    QPushButton *backButton = new QPushButton();
    backButton->setIcon(QIcon(":/Images/glossy-blue-orb-icon-arrowback.png"));
    backButton->setIconSize(QSize(40,40));
    backButton->setFocusPolicy(Qt::NoFocus);
    backButton->setObjectName("backButton");
    connect(backButton,SIGNAL(clicked()),this,SLOT(close()));

    // create layout for top of screen
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(currentTime);
    topLayout->addStretch();
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(backButton);

    view = new QGraphicsView();
    view->setBackgroundBrush(QBrush(Qt::black));
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // create final layout

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    QHBoxLayout* viewLayout = new QHBoxLayout;
    viewLayout->addWidget(view);
    viewLayout->setContentsMargins(0,0,0,0);
    viewLayout->setSpacing(0);
    mainLayout->addLayout(viewLayout);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    QWidget *sizeLimiterWidget = new QWidget;
    sizeLimiterWidget->setMaximumSize(800, 450);

    sizeLimiterWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizeLimiterWidget->setLayout(mainLayout);
    sizeLimiterWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout* contentsBox = new QHBoxLayout;
    contentsBox->setContentsMargins(0,0,0,0);
    contentsBox->addWidget(sizeLimiterWidget);

    // show final layout
    setLayout(contentsBox);

    resize(qApp->desktop()->size());
    updateGeometry();

    scene = NULL;
    m_initialized = false;
    currentPoint  = NULL;
}

void ScheduleScreen::createScheduleScene()
{
    QPen pen(Qt::blue);
    QBrush brush(Qt::black);
    QFont font = this->font();
    font.setBold(true);
    QFontMetrics fm(font);

    weekHeight = (scene->height()/7.0)-(scene->height()*.025);

    pointArea.setRect(fm.boundingRect("Wed").width()+20, 15, scene->width(), weekHeight*7);
    timeWidth = ((pointArea.width() - pointArea.left())/4.0)-((pointArea.width() - pointArea.left())/4.0)/16;
    timeBlockWidth = timeWidth/24.0; //15 min increments
    pointArea.adjust(0,0,-timeBlockWidth, 0);
    QDate date = QDate::fromString("Sun", "ddd");

    for(int a = 0;a<7;a++)
    {
        pen.setColor(QColor(110+40*(a%2),110+40*(a%2),110+40*(a%2)));
        brush.setColor(QColor(120+40*(a%2),120+40*(a%2),120+40*(a%2)));

        scene->addRect(0,pointArea.top()+a*weekHeight,pointArea.width(),weekHeight, pen, brush);

        pen.setColor(QColor(150,150,150));
        brush.setColor(QColor(180,180,180));
        scene->addRect(0,pointArea.top()+a*weekHeight,pointArea.left(),weekHeight-1, pen, brush);

        date = date.addDays(1);
        QString dateString = date.toString("ddd");
        pen.setColor(QColor(50,50,50));
        QPushButton* b = new QPushButton;
        connect(b, SIGNAL(clicked(bool)), this, SLOT(disableRow(bool)));
        b->setFlat(true);
        b->setFont(font);
        b->setFocusPolicy(Qt::NoFocus);
        b->setMaximumWidth(pointArea.left()-4);
        b->setMaximumHeight(weekHeight-4);
        QPalette buttonPalette(QColor(180,180,180));
        //buttonPalette.setColor(QPalette::Active, QPalette::Button, QColor(180,180,180));
        b->setPalette(buttonPalette);
        b->setCheckable(true);
        b->setChecked(true);
        b->setText(dateString);
        b->setProperty("dayNumber", date.dayOfWeek()-1);
        QGraphicsProxyWidget* textItem = new QGraphicsProxyWidget;
        textItem->setWidget(b);
        scene->addItem(textItem);

        textItem->setPos(pointArea.left()/2 - textItem->boundingRect().width()/2, pointArea.top()+ a*weekHeight + weekHeight/2 - textItem->boundingRect().height()/2);
    }

    scene->addLine(pointArea.left(), pointArea.top()+1, pointArea.left(), scene->height()-1);

    for(int a = 0;a<5;a++)
    {
        pen.setColor(QColor(110+40*(a%2),110+40*(a%2),110+40*(a%2)));
        brush.setColor(QColor(120+40*(a%2),120+40*(a%2),120+40*(a%2)));

        QString timeString = formatHourString(QTime(((a)*6)%24,0), m_globalSettings->timeFormat());

        pen.setColor(QColor(200,200,200));
        brush.setColor(QColor(200,200,200));
        QGraphicsSimpleTextItem* textItem = scene->addSimpleText(timeString, font);

        textItem->setBrush(brush);
        textItem->setPos(timeBlockWidth*3+pointArea.left()+timeWidth*a - textItem->boundingRect().width()/2, pointArea.bottom() +6);
        pen.setColor(QColor(188,188,188));
        pen.setStyle(Qt::DashLine);
        scene->addLine(timeBlockWidth*3+pointArea.left()+timeWidth*a, pointArea.top()+1, timeBlockWidth*3+pointArea.left()+timeWidth*a, pointArea.bottom()+3, pen);
    }

}


void ScheduleScreen::disableRow(bool checked)
{
    QPushButton* button = dynamic_cast<QPushButton* >(this->sender());
    int index = button->property("dayNumber").toInt()*4;

    // display only those points that are requested by weekday check boxes
    if(checked) {
        //enable all points in the row
        for(int a = 0;a<4;a++)
        {
            pointList.at(index+a)->setDisabled(false);

            //check each point in the column, if one is selected then select this one and break
            for(int b=0;b<7;b++)
                if(pointList.at(a +4*b)->selected())
                {
                    pointList.at(index+a)->setSelected(true);
                    break;
                }
        }
        button->setStyleSheet("");
    }
    else
    {
        for(int a = 0;a<4;a++)
            pointList.at(index+a)->setDisabled(true);
        button->setStyleSheet("color:#CCCCCC");
    }

    return;

}

void ScheduleScreen::showEvent(QShowEvent *e)
{
    if(!m_initialized)
    {
        e->accept();
        currentPoint = NULL;
        m_initialized = true;
        view->show();
        QTimer::singleShot(0, this, SLOT(initializeGraphics()));
    }
}

void ScheduleScreen::initializeGraphics()
{
    initializeScene();
    createScheduleScene();
    addSchedulePoints();
}

void ScheduleScreen::initializeScene()
{
    // create graphics view and scene
    scene = new QGraphicsScene(0,0,view->width(), view->height());
    view->setScene(scene);
}

void ScheduleScreen::updateData()
{
    if(scene)
    {
        scene->clear();
        delete scene;
        scene = 0;
        m_initialized = false;
        pointList.empty();
        QApplication::sendEvent(this, new QShowEvent);
    }
}

void ScheduleScreen::addSchedulePoints()
{
    // create all 28 schedule points with proper connections
    // fill all points into pointList for easy access

    for(int a=0;a<28;a++)
    {
        SchedulePoint *schedulePoint = new SchedulePoint(a);
        connect(schedulePoint, SIGNAL(clicked(SchedulePoint *)), this, SLOT(selectDay(SchedulePoint*)));
        connect(this, SIGNAL(valueChanged()), schedulePoint, SLOT(updateUnit()));
        schedulePoint->setTimeBlockWidth(timeBlockWidth);
        schedulePoint->setWeekHeight(weekHeight);
        schedulePoint->setPointArea(pointArea);
        showPoint(schedulePoint);
        pointList.insert(a,schedulePoint);
    }

}

void ScheduleScreen::showPoint(SchedulePoint *point)
{
    // show point based on unique ID
    point->setPos(QPoint(timeBlockWidth*3 + pointArea.left() + (timeBlockWidth * 4 * columnTimes[point->getID()%4]), pointArea.top() + weekHeight/2 + (weekHeight * ((point->getID() / 4) % 7))));
    scene->addItem(point);
 }

void ScheduleScreen::shiftLeft()
{
    // shift buttons left and shift visible points left in this column unless
    // left edge of graphics view has been reached

    if(currentPoint->pos().x()- currentPoint->boundingRect().width()/2- timeBlockWidth > pointArea.left()) {
        for(int i=0; i<7; i++) {
            if(pointList.at((currentPoint->getID() + 4*i) % 28)->isVisible()) {
                pointList.at((currentPoint->getID() + 4*i) % 28)->shiftLeft();
            }
        }
        // change current time represented by position of current point
        currentTime->setText(currentPoint->time());
        //qDebug() << currentPoint->pos().x();
    }

}

void ScheduleScreen::shiftRight()
{
    // shift buttons right and shift visible points right in this column unless
    // right edge of graphics view has been reached
    if(currentPoint->pos().x()+ currentPoint->boundingRect().width()/2+ timeBlockWidth < pointArea.right()) {
        for(int i=0; i<7; i++) {
            if(pointList.at((currentPoint->getID() + 4*i) % 28)->isVisible()) {
                pointList.at((currentPoint->getID() + 4*i) % 28)->shiftRight();
            }
        }
        // change time to reflect new position
        currentTime->setText(currentPoint->time());
        //qDebug() << currentPoint->pos().x();
    }

}

void ScheduleScreen::increaseTemp()
{
    // increase temp of all visible buttons in this colummn
    for(int i=0; i<7; i++) {
        if(pointList.at((currentPoint->getID() + 4*i) % 28)->isVisible()) {
            pointList.at((currentPoint->getID() + 4*i) % 28)->increaseTemp();
        }
    }
}

void ScheduleScreen::decreaseTemp()
{
    // decrease temp of all visible buttons in this column
    for(int i=0; i<7; i++) {
        if(pointList.at((currentPoint->getID() + 4*i) % 28)->isVisible()) {
            pointList.at((currentPoint->getID() + 4*i) % 28)->decreaseTemp();
        }
    }
}

void ScheduleScreen::selectDay(SchedulePoint *point)
{
    //ifa point has already been clicked on we need to unselect it.
    if(currentPoint)
        unselectDay();

    // track which is the point that has been clicked on
    currentPoint = point;
    // show time indicated by position of current point
    currentTime->setText(currentPoint->time());

    for(int a = 0; a<7; a++)
    {
        SchedulePoint* columnPoint = pointList.at(point->getID()%4 + a*4);
        if(!columnPoint->disabled())
        {
            pointList.at(point->getID()%4 + a*4)->setSelected(true);
            if(point!=columnPoint)
                connect(point, SIGNAL(shareAdjustment(int, qreal)), columnPoint, SLOT(adjust(int, qreal)));
        }
    }


}

void ScheduleScreen::unselectDay()
{
    if(!currentPoint)
        return;
    for(int a = 0; a<28; a++)
    {
        SchedulePoint* point = pointList.at(a);

        point->setSelected(false);
        if(currentPoint!=point)
            disconnect(currentPoint, SIGNAL(shareAdjustment(int, qreal)), point, SLOT(adjust(int, qreal)));

    }
}

void ScheduleScreen::mousePressEvent(QMouseEvent * /* event */)
{
    // provide event handler for whole screen, so that user can click off a
    // a column and have all points be displayed
    emit clicked();
}
