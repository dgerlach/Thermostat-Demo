#include "schedulescreen.h"
#include "schedulepoint.h"
#include "globalsettings.h"
#include "utilities.h"

#include "QtDebug"

#include <QRect>

ScheduleScreen::ScheduleScreen(QWidget *parent) :
    QWidget(parent)
{
    setObjectName("ScheduleScreen");
    //get an instance of the global configuration
    m_globalSettings = GlobalSettings::getInstance();

    // seqNumber will track the number of points that have been created
    seqNumber = 0;

    // allow user to click off points to remove blur and see all
    connect(this, SIGNAL(clicked()), this, SLOT(removeAllBlur()));
    //allow valueChanged signal to trigger updateData() slot
    connect(this, SIGNAL(valueChanged()), this, SLOT(updateData()));
    // create title
    QLabel *title = new QLabel("Set your WEEKLY SCHEDULE");
    title->setObjectName("title");

    // create time represented by current point
    currentTime = new QLabel();
    currentTime->setObjectName("currentTime");

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
    view->show();


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

    m_initialized = false;

}

void ScheduleScreen::createScheduleScene()
{

    // create graphics view and scene
    scene = new QGraphicsScene(0,0,view->width(), view->height());
    view->setScene(scene);

    QPen pen(Qt::blue);
    QBrush brush(Qt::black);
    QFont font = this->font();
    font.setBold(true);
    QFontMetrics fm(font);

    weekHeight = (scene->height()/7.0)-(scene->height()*.02);

    pointArea.setRect(fm.boundingRect("Wed").width()+20, 15, scene->width(), weekHeight*7);
    qreal timeWidth = ((scene->width()- pointArea.left())/5.0);
    timeBlockWidth = timeWidth/16.0; //15 min increments

    QDate date = QDate::fromString("Sun", "ddd");

    for(int a = 0;a<7;a++)
    {
        pen.setColor(QColor(110+40*(a%2),110+40*(a%2),110+40*(a%2)));
        brush.setColor(QColor(120+40*(a%2),120+40*(a%2),120+40*(a%2)));

        scene->addRect(0,pointArea.top()+a*weekHeight,(qreal)scene->width(),weekHeight, pen, brush);

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

        QString timeString = formatTimeString(QTime((a+1)*4,0), m_globalSettings->timeFormat(), false);
        pen.setColor(QColor(200,200,200));
        brush.setColor(QColor(200,200,200));
        QGraphicsSimpleTextItem* textItem = scene->addSimpleText(timeString, font);

        textItem->setBrush(brush);
        textItem->setPos(timeWidth/2+pointArea.left()+timeWidth*a - textItem->boundingRect().width()/2, pointArea.bottom() +6);
        pen.setColor(QColor(188,188,188));
        pen.setStyle(Qt::DashLine);
        scene->addLine(timeWidth/2+pointArea.left()+timeWidth*a, pointArea.top()+1, timeWidth/2+pointArea.left()+timeWidth*a, pointArea.bottom()+3, pen);
    }

}


void ScheduleScreen::disableRow(bool checked)
{
    QPushButton* button = dynamic_cast<QPushButton* >(this->sender());
    int index = button->property("dayNumber").toInt()*4;

    // display only those points that are requested by weekday check boxes
    if(checked) {

        pointList.at(index+0)->show();
        pointList.at(index+1)->show();
        pointList.at(index+2)->show();
        pointList.at(index+3)->show();
        button->setStyleSheet("");
    }
    else
    {
        pointList.at(index+0)->hide();
        pointList.at(index+1)->hide();
        pointList.at(index+2)->hide();
        pointList.at(index+3)->hide();
        button->setStyleSheet("color:#CCCCCC");
    }

    return;

}

void ScheduleScreen::showEvent(QShowEvent *e)
{
    if(!m_initialized)
    {
        m_initialized = true;
        QTimer::singleShot(30, this, SLOT(createScheduleScene()));
        QTimer::singleShot(35, this, SLOT(addSchedulePoints()));
        e->accept();
    }
}

void ScheduleScreen::updateData()
{
    createScheduleScene();
}

void ScheduleScreen::addSchedulePoints()
{

    // create all 28 schedule points with proper connections
    // fill all points into pointList for easy access

    for(int a=0;a<28;a++)
    {
        SchedulePoint *schedulePoint = new SchedulePoint(a);
        connect(schedulePoint, SIGNAL(clicked(SchedulePoint *)), this, SLOT(showButtons(SchedulePoint *)));
        connect(this, SIGNAL(valueChanged()), schedulePoint, SLOT(updateUnit()));
        schedulePoint->setTimeBlockWidth(timeBlockWidth);
        schedulePoint->setWeekHeight(weekHeight);
        schedulePoint->setPointArea(pointArea);
        showPoint(schedulePoint);
        pointList.append(schedulePoint);
    }

    // create proxy widgets to hold push buttons so arrows can be added to graphics view
    proxyLeftButton = new QGraphicsProxyWidget();
    proxyRightButton = new QGraphicsProxyWidget();
    proxyUpButton = new QGraphicsProxyWidget();
    proxyDownButton = new QGraphicsProxyWidget();

}

void ScheduleScreen::showPoint(SchedulePoint *point)
{
    qreal timeWidth = ((scene->width()- pointArea.left())/5.0);

    // show point based on unique ID
    point->setPos(QPoint(pointArea.left() + timeWidth + (timeWidth * (point->getID() % 4)), pointArea.top() + weekHeight/2 + (weekHeight * ((point->getID() / 4) % 7))));
    scene->addItem(point);
 }

void ScheduleScreen::shiftLeft()
{
    // shift buttons left and shift visible points left in this column unless
    // left edge of graphics view has been reached

    if(currentPoint->pos().x()- currentPoint->boundingRect().width()/2- timeBlockWidth > pointArea.left()) {
        shiftButtonsLeft();
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
        shiftButtonsRight();
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

void ScheduleScreen::shiftButtonsLeft()
{
    // shift all 4 arrow buttons left
    shiftVerticalButtonsLeft();
    shiftHorizontalButtonsLeft();
}

void ScheduleScreen::shiftButtonsRight()
{
    // shift all 4 arrow buttons right
    shiftVerticalButtonsRight();
    shiftHorizontalButtonsRight();
}

void ScheduleScreen::shiftVerticalButtonsRight()
{
    // shift up and down buttons right
    proxyUpButton->moveBy(timeBlockWidth,0);
    proxyDownButton->moveBy(timeBlockWidth,0);
}

void ScheduleScreen::shiftVerticalButtonsLeft()
{
    // shift up and down buttons left
    proxyUpButton->moveBy(-timeBlockWidth,0);
    proxyDownButton->moveBy(-timeBlockWidth,0);
}

void ScheduleScreen::shiftHorizontalButtonsRight()
{
    // shift left and right buttons right
    proxyLeftButton->moveBy(timeBlockWidth,0);
    proxyRightButton->moveBy(timeBlockWidth,0);
}

void ScheduleScreen::shiftHorizontalButtonsLeft()
{
    // shift left and right buttons left
    proxyLeftButton->moveBy(-timeBlockWidth,0);
    proxyRightButton->moveBy(-timeBlockWidth,0);
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

void ScheduleScreen::showButtons(SchedulePoint *point)
{
    // show left, right, up, and down buttons in correct position for any input conditions
    int i;
    // track which is the point that has been clicked on
    currentPoint = point;
    // show time indicated by position of current point
    currentTime->setText(currentPoint->time());

    // first hide all 4 buttons, then we will make them reappear in the correct position
    /*hideButtons();

    // create left arrow
    QPushButton *leftArrowButton = new QPushButton();
    leftArrowButton->setFocusPolicy(Qt::NoFocus);
    leftArrowButton->setStyleSheet("background: rgba(0,0,0,0);"
                                   "border-image: url(:/Images/simple-black-square-icon-arrowleft.png) 1;");
    leftArrowButton->setFixedSize(25,25);
    connect(leftArrowButton, SIGNAL(clicked()), this, SLOT(shiftLeft()));

    // create right arrow
    QPushButton *rightArrowButton = new QPushButton();
    rightArrowButton->setFocusPolicy(Qt::NoFocus);
    rightArrowButton->setStyleSheet("background: rgba(0,0,0,0);"
                                   "border-image: url(:/Images/simple-black-square-icon-arrowright.png) 1;");
    rightArrowButton->setFixedSize(25,25);
    connect(rightArrowButton, SIGNAL(clicked()),this,SLOT(shiftRight()));

    // create up arrow
    QPushButton *upArrowButton = new QPushButton();
    upArrowButton->setFocusPolicy(Qt::NoFocus);
    upArrowButton->setStyleSheet("background: rgba(0,0,0,0);"
                                 "border-image: url(:/Images/simple-black-square-icon-arrowup.png) 1;");
    upArrowButton->setFixedSize(25,25);
    connect(upArrowButton, SIGNAL(clicked()), this, SLOT(increaseTemp()));

    // create down arrow
    QPushButton *downArrowButton = new QPushButton();
    downArrowButton->setFocusPolicy(Qt::NoFocus);
    downArrowButton->setStyleSheet("background: rgba(0,0,0,0);"
                                 "border-image: url(:/Images/simple-black-square-icon-arrowdown.png) 1;");
    downArrowButton->setFixedSize(25,25);
    connect(downArrowButton, SIGNAL(clicked()), this, SLOT(decreaseTemp()));

    // create proxy widgets so that buttons can be added to graphics view
    proxyLeftButton = new QGraphicsProxyWidget();
    proxyLeftButton->setWidget(leftArrowButton);
    proxyRightButton = new QGraphicsProxyWidget();
    proxyRightButton->setWidget(rightArrowButton);
    proxyUpButton = new QGraphicsProxyWidget();
    proxyUpButton->setWidget(upArrowButton);
    proxyDownButton = new QGraphicsProxyWidget();
    proxyDownButton->setWidget(downArrowButton);

    // set position of left and right buttons on either side of currently selected point
    proxyLeftButton->setPos(point->x()-proxyLeftButton->boundingRect().width()*1.5, point->y()-proxyLeftButton->boundingRect().height()/2);
    scene->addItem(proxyLeftButton);
    proxyRightButton->setPos(point->x()+proxyLeftButton->boundingRect().width()/2, point->y()-proxyRightButton->boundingRect().height()/2l);
    scene->addItem(proxyRightButton);

    // set up and down buttons at the top and bottom of currently visible column
    for(i=0; i<7; i++) {
        if(pointList.at((point->getID()%4 + 4*i))->isVisible()) {
            proxyUpButton->setPos(QPoint(-13 + pointList.at(point->getID()%4 + 4*i)->pos().x(),
                                         -31 + pointList.at(point->getID()%4 +4*i)->pos().y()));
            break;
        }
    }
    scene->addItem(proxyUpButton);

    for(i=0; i<7; i++) {
        if(pointList.at((24+(point->getID()%4) - 4*i))->isVisible()) { // only works if last row is visible
            proxyDownButton->setPos(QPoint(-13 + pointList.at((24+(point->getID()%4) - 4*i))->pos().x(),
                                           6 + pointList.at((24+(point->getID()%4) - 4*i))->pos().y()));
            break;
        }
    }

    scene->addItem(proxyDownButton);

    // blur all columns except the currently selected column
    blur();
    */

}

void ScheduleScreen::hideButtons()
{
    // hide up, down, left, and right buttons
    proxyLeftButton->hide();
    proxyRightButton->hide();
    proxyUpButton->hide();
    proxyDownButton->hide();

}

void ScheduleScreen::hidePoints()
{
    // hide all points
    for(int i=0; i<28; i++) {
        pointList.at(i)->hide();
    }
    return;

}

void ScheduleScreen::showPoints(int)
{

}

void ScheduleScreen::blur()
{
    // add blur to all points outside the current column
    for(int i=0; i<28; i++) {
        if(pointList.at(i)->graphicsEffect()) { // does the item have an exisiting graphics effect
            pointList.at(i)->setGraphicsEffect(NULL);
        }
        if(pointList.at(i)->getID()%4 != currentPoint->getID()%4) {
            pointList.at(i)->setGraphicsEffect(new QGraphicsBlurEffect(this));
        }
    }
}

void ScheduleScreen::mousePressEvent(QMouseEvent * /* event */)
{
    // provide event handler for whole screen, so that user can click off a
    // a column and have all points be displayed
    //emit clicked();

}

void ScheduleScreen::removeAllBlur()
{
    // display all points by removing all blur
    for(int i=0; i<28; i++) {
        if(pointList.at(i)->graphicsEffect()) { // does the item have an exisiting graphics effect
            pointList.at(i)->setGraphicsEffect(NULL);
        }
    }
}
