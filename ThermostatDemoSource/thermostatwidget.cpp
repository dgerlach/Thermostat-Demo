#include "thermostatwidget.h"
#include "globalsettings.h"
#include "utilities.h"


ThermostatWidget::ThermostatWidget(QWidget *parent) :
    QWidget(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    setObjectName("thermostatWidget");
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setAttribute(Qt::WA_StyledBackground,true);

    // create new thermostat widget with default values in case internet options are disabled
    setpointReached = false;
    currentTemp = new QLabel("72°");
    setpointTempInt = 72;
    setpointTemp = new QLabel("");
    setpointTemp->setMinimumWidth(77);
    increaseTempButton = new QPushButton("+");
    increaseTempButton->setFocusPolicy(Qt::NoFocus);
    decreaseTempButton = new QPushButton("-");
    decreaseTempButton->setFocusPolicy(Qt::NoFocus);

    currentTemp->setObjectName("currentTemp");

    currentTemp->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    setpointTemp->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    increaseTempButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    decreaseTempButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    // provide push button functionality
    connect(increaseTempButton,SIGNAL(clicked()),this,SLOT(increaseTemp()));
    connect(decreaseTempButton,SIGNAL(clicked()),this,SLOT(decreaseTemp()));

    // create layout
    QHBoxLayout *setpointLayout = new QHBoxLayout;
    setpointLayout->addWidget(decreaseTempButton);
    setpointLayout->addWidget(setpointTemp);
    setpointLayout->addWidget(increaseTempButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(currentTemp);
    mainLayout->addLayout(setpointLayout);

    // set layout
    setLayout(mainLayout);

    // provide timer to let current temp follow set point
    updateTimer = new QTimer(this);
    connect(updateTimer,SIGNAL(timeout()),this,SLOT(update()));
    connect(updateTimer, SIGNAL(timeout()), this, SIGNAL(timeout()));
    updateTimer->start(10000);
}

void ThermostatWidget::resizeEvent(QResizeEvent *e)
{
    //scale the temperature size
    currentTemp->setStyleSheet("font-size:"+QString::number(this->size().height()/2)+"px");
    e->accept();
}

void ThermostatWidget::increaseTemp()
{
    // provide function to increase current setpoint
    setpointTempInt++;
    if (setpointTempInt == *currentTempInt) {
        // setpoint should disappear when reached
        setpointTemp->setText("");
        setpointReached = true;
        emit setpointIsReached(setpointReached);
    } else {
        setpointTemp->setText(formatTemperatureString(setpointTempInt, m_globalSettings->temperatureFormat()));
        setpointReached = false;
        emit setpointIsReached(setpointReached);
    }
}

void ThermostatWidget::decreaseTemp()
{
    // provid functino to decrease current setpoint
    setpointTempInt--;
    if (setpointTempInt == *currentTempInt) {
        // setpoint should disappear when reached
        setpointTemp->setText("");
        setpointReached = true;
        emit setpointIsReached(setpointReached);
    } else {
        setpointTemp->setText(formatTemperatureString(setpointTempInt, m_globalSettings->temperatureFormat()));
        setpointReached = false;
        emit setpointIsReached(setpointReached);
    }
}

void ThermostatWidget::update()
{
    // when timer fires (every 10 seconds), have current temperature approach setpoint temperature by one degree
    if (*currentTempInt < setpointTempInt) {
        (*currentTempInt)++;
    }
    if (*currentTempInt > setpointTempInt) {
        (*currentTempInt)--;
    }
    currentTemp->setText(formatTemperatureString(*currentTempInt, m_globalSettings->temperatureFormat()));
    // if setpoint is reached, have setpoint disappear
    if (setpointTempInt == *currentTempInt) {
        setpointTemp->setText("");
        setpointReached = true;
        emit setpointIsReached(setpointReached);
    }
}

void ThermostatWidget::updateUnit()
{
    currentTemp->setText(formatTemperatureString(*currentTempInt, m_globalSettings->temperatureFormat()));
    setpointTemp->setText(formatTemperatureString(setpointTempInt, m_globalSettings->temperatureFormat()));
    update();
}

void ThermostatWidget::setCurrentTempPtr(int *currentTemp)
{
    currentTempInt = currentTemp;
    updateUnit();
}

QHash<QString, QVariant> ThermostatWidget::getCurrentData()
{
    QHash<QString, QVariant> hash;
    hash.insert("currentTemp", formatTemperatureString(*currentTempInt, m_globalSettings->temperatureFormat()));
    hash.insert("setpointTemp", formatTemperatureString(setpointTempInt, m_globalSettings->temperatureFormat()));
    return hash;
}

