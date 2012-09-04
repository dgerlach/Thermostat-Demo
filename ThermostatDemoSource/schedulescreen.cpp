#include "schedulescreen.h"
#include "schedulepoint.h"

#include "QtDebug"

ScheduleScreen::ScheduleScreen(QWidget *parent) :
    QWidget(parent)
{
    // seqNumber will track the number of points that have been created
    seqNumber = 0;

    // allow user to click off points to remove blur and see all
    connect(this, SIGNAL(clicked()), this, SLOT(removeAllBlur()));

    // create title
    QLabel *title = new QLabel("Set your WEEKLY SCHEDULE");
    title->setObjectName("title");

    // create time represented by current point
    currentTime = new QLabel();
    currentTime->setObjectName("currentTime");
    currentTime->setMinimumWidth(80);
    currentTime->setMaximumWidth(80);
    currentTime->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

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

    // create checkboxes for days of week
    MondayButton = new QCheckBox(QString("Mon"));
    MondayButton->setFocusPolicy(Qt::NoFocus);
    TuesdayButton = new QCheckBox(QString("Tue"));
    TuesdayButton->setFocusPolicy(Qt::NoFocus);
    WednesdayButton = new QCheckBox(QString("Wed"));
    WednesdayButton->setFocusPolicy(Qt::NoFocus);
    ThursdayButton = new QCheckBox(QString("Thu"));
    ThursdayButton->setFocusPolicy(Qt::NoFocus);
    FridayButton = new QCheckBox(QString("Fri"));
    FridayButton->setFocusPolicy(Qt::NoFocus);
    SaturdayButton = new QCheckBox(QString("Sat"));
    SaturdayButton->setFocusPolicy(Qt::NoFocus);
    SundayButton = new QCheckBox(QString("Sun"));
    SundayButton->setFocusPolicy(Qt::NoFocus);

    // create button group to hold weekdays check boxes
    daysButtonGroup = new QButtonGroup(this);
    daysButtonGroup->addButton(MondayButton);
    daysButtonGroup->addButton(TuesdayButton);
    daysButtonGroup->addButton(WednesdayButton);
    daysButtonGroup->addButton(ThursdayButton);
    daysButtonGroup->addButton(FridayButton);
    daysButtonGroup->addButton(SaturdayButton);
    daysButtonGroup->addButton(SundayButton);  
    connect(daysButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(showPoints(int)));

    // set all boxes to initially checked
    daysButtonGroup->setExclusive(false);
    MondayButton->setChecked(true);
    TuesdayButton->setChecked(true);
    WednesdayButton->setChecked(true);
    ThursdayButton->setChecked(true);
    FridayButton->setChecked(true);
    SaturdayButton->setChecked(true);
    SundayButton->setChecked(true);

    // create time markings
    QLabel *hour4 = new QLabel("4AM");
    QLabel *hour8 = new QLabel("8AM");
    QLabel *hour12 = new QLabel("Noon");
    QLabel *hour16 = new QLabel("4PM");
    QLabel *hour20 = new QLabel("8PM");

    // create days layout
    QVBoxLayout *daysButtonLayout = new QVBoxLayout;
    daysButtonLayout->addStretch();
    daysButtonLayout->addSpacing(15);
    daysButtonLayout->addWidget(MondayButton);
    daysButtonLayout->addWidget(TuesdayButton);
    daysButtonLayout->addWidget(WednesdayButton);
    daysButtonLayout->addWidget(ThursdayButton);
    daysButtonLayout->addWidget(FridayButton);
    daysButtonLayout->addWidget(SaturdayButton);
    daysButtonLayout->addWidget(SundayButton);
    daysButtonLayout->addSpacing(15);
    daysButtonLayout->addStretch();

    // create time layout
    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addSpacing(110);
    timeLayout->addWidget(hour4);
    timeLayout->addSpacing(24);
    timeLayout->addWidget(hour8);
    timeLayout->addSpacing(20);
    timeLayout->addWidget(hour12);
    timeLayout->addSpacing(24);
    timeLayout->addWidget(hour16);
    timeLayout->addSpacing(24);
    timeLayout->addWidget(hour20);
    timeLayout->addSpacing(50);
    timeLayout->setSizeConstraint(QLayout::SetFixedSize);

    // force time layout to retain size independent of screen size
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addLayout(timeLayout);
    bottomLayout->setAlignment(timeLayout,Qt::AlignCenter);

    // create graphics view and scene
    scene = new QGraphicsScene(0,0,300,200);
    view = new QGraphicsView;
    view->setScene(scene);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // create all 28 schedule points
    addSchedulePoints();

    // show graphics view
    view->show();

    // create layout for graphics view and weekday checkboxes
    QHBoxLayout *weekdaysLayout = new QHBoxLayout;
    weekdaysLayout->addLayout(daysButtonLayout);
    weekdaysLayout->addWidget(view);

    // create final layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(weekdaysLayout);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addStretch();

    // show final layout
    setLayout(mainLayout);
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
    // show point based on unique ID
    point->setPos(QPoint(35.025 + (85 * (point->getID() % 4)),
                        37 + (22 * ((point->getID() / 4) % 7))));
    scene->addItem(point);
}

void ScheduleScreen::shiftLeft()
{
    // shift buttons left and shift visible points left in this column unless
    // left edge of graphics view has been reached
    if(currentPoint->pos().x() > -10) {
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
    if(currentPoint->pos().x() < 310) {
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
    proxyUpButton->moveBy(3.4,0);
    proxyDownButton->moveBy(3.4,0);
}

void ScheduleScreen::shiftVerticalButtonsLeft()
{
    // shift up and down buttons left
    proxyUpButton->moveBy(-3.4,0);
    proxyDownButton->moveBy(-3.4,0);
}

void ScheduleScreen::shiftHorizontalButtonsRight()
{
    // shift left and right buttons right
    proxyLeftButton->moveBy(3.4,0);
    proxyRightButton->moveBy(3.4,0);
}

void ScheduleScreen::shiftHorizontalButtonsLeft()
{
    // shift left and right buttons left
    proxyLeftButton->moveBy(-3.4,0);
    proxyRightButton->moveBy(-3.4,0);
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
    hideButtons();

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
    proxyLeftButton->setPos(QPoint(0.075 + (85 * (point->getID() % 4)),
                                        25 + (22 * ((point->getID() / 4) % 7))));
    scene->addItem(proxyLeftButton);
    proxyRightButton->setPos(QPoint(45.075 + (85 * (point->getID() % 4)),
                                    25 + (22 * ((point->getID() / 4) % 7))));
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

    // if the point has already been moved, make sure buttons are created at the right spot
    if(point->getLocation()<0) {
        for(i=0; i>point->getLocation(); i--) {
            shiftHorizontalButtonsLeft();
        }
    } else {
        for(i=0; i<point->getLocation(); i++) {
            shiftHorizontalButtonsRight();
        }
    }

    // blur all columns except the currently selected column
    blur();

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
    // first hide everything
    hidePoints();
    hideButtons();

    // display only those points that are requested by weekday check boxes
    if(MondayButton->isChecked()) {
        pointList.at(0)->show();
        pointList.at(1)->show();
        pointList.at(2)->show();
        pointList.at(3)->show();
    }
    if(TuesdayButton->isChecked()) {
        pointList.at(4)->show();
        pointList.at(5)->show();
        pointList.at(6)->show();
        pointList.at(7)->show();
    }
    if(WednesdayButton->isChecked()) {
        pointList.at(8)->show();
        pointList.at(9)->show();
        pointList.at(10)->show();
        pointList.at(11)->show();
    }
    if(ThursdayButton->isChecked()) {
        pointList.at(12)->show();
        pointList.at(13)->show();
        pointList.at(14)->show();
        pointList.at(15)->show();
    }
    if(FridayButton->isChecked()) {
        pointList.at(16)->show();
        pointList.at(17)->show();
        pointList.at(18)->show();
        pointList.at(19)->show();
    }
    if(SaturdayButton->isChecked()) {
        pointList.at(20)->show();
        pointList.at(21)->show();
        pointList.at(22)->show();
        pointList.at(23)->show();
    }
    if(SundayButton->isChecked()) {
        pointList.at(24)->show();
        pointList.at(25)->show();
        pointList.at(26)->show();
        pointList.at(27)->show();
    }
    return;

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
    emit clicked();

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
