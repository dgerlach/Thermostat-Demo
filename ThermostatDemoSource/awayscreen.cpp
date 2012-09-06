#include "awayscreen.h"
#include "thermostatwidget.h"
#include "globalsettings.h"
#include "utilities.h"
#include "mainwindow.h"

AwayScreen::AwayScreen(QWidget *parent) :
    QWidget(parent)
{
    // the away screen provides the user an AWAY MODE with the tap of a button
    // it is meant to be used for long periods of away time (i.e. vacation)
    // it is called from the mainwindow

    m_currentTemp = NULL;

    m_globalSettings = GlobalSettings::getInstance();

    m_upperBound = 84;
    m_lowerBound = 60;

    // title
    awayLabel = new QLabel("System is using energy saving AWAY MODE");
    awayLabel->setAlignment(Qt::AlignHCenter);
    awayLabel->setObjectName("awayLabel");

    // button to return to main screen
    QPushButton *backButton = new QPushButton();
    backButton->setIcon(QIcon(":/Images/glossy-blue-orb-icon-arrowback.png"));
    backButton->setIconSize(QSize(40,40));
    backButton->setFocusPolicy(Qt::NoFocus);
    backButton->setObjectName("backButton");
    connect(backButton,SIGNAL(clicked()),this,SLOT(close()));

    // green leaf
    ecoLabel = new QLabel();
    ecoLabel->setAlignment(Qt::AlignHCenter);
    QPixmap *ecoLeaf = new QPixmap(":/Images/eco-leaf.png");
    *ecoLeaf = ecoLeaf->scaled(60,60,Qt::KeepAspectRatio);
    ecoLabel->setPixmap(*ecoLeaf);
    ecoLabel->setObjectName("ecoLabel");
    ecoLabel->setAlignment(Qt::AlignTop);

    // provides current temp - same value as listed in MainWindow adjust dynamically
    currentTempLabel = new QLabel;
    currentTempLabel->setText("");
    currentTempLabel->setAlignment(Qt::AlignHCenter);
    currentTempLabel->setObjectName("currentTemp");
    currentTempLabel->setMargin(0);

    // inside label
    QLabel *insideLabel = new QLabel("Inside");
    insideLabel->setAlignment(Qt::AlignHCenter);
    insideLabel->setMargin(0);

    // layout for current temp
    QVBoxLayout *currentLayout = new QVBoxLayout;
    currentLayout->addStretch();
    currentLayout->addWidget(insideLabel);
    currentLayout->addWidget(currentTempLabel);
    currentLayout->addStretch();

    // upper bound control widget
    upperBound = new QSpinBox;
    upperBound->setValue(m_upperBound);
    upperBound->setSuffix("°");
    upperBound->findChild<QLineEdit*>()->setReadOnly(true);

    // lower bound control widget
    lowerBound = new QSpinBox;
    lowerBound->setValue(m_lowerBound);
    lowerBound->setSuffix("°");
    lowerBound->findChild<QLineEdit*>()->setReadOnly(true);

    // upper and lower bound labels
    QLabel *heatTo = new QLabel("Heat to");
    heatTo->setAlignment(Qt::AlignHCenter);
    QLabel *coolTo = new QLabel("Cool to");
    coolTo->setAlignment(Qt::AlignHCenter);

    // combines upper and lower bounds into one layout
    QVBoxLayout *boundsLayout = new QVBoxLayout;
    boundsLayout->addStretch();
    boundsLayout->addWidget(coolTo);
    boundsLayout->addWidget(upperBound);
    boundsLayout->addWidget(heatTo);
    boundsLayout->addWidget(lowerBound);
    boundsLayout->addStretch();

    // combines current temp layout and bounds layout
    QHBoxLayout *centerLayout = new QHBoxLayout;
    centerLayout->addStretch();
    centerLayout->addLayout(currentLayout);
    centerLayout->addSpacing(20);
    centerLayout->addLayout(boundsLayout);
    centerLayout->addStretch();

    // combines center layout with title
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(awayLabel);
    rightLayout->insertSpacing(1,10);


    // combines existing layout with green leaf
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(ecoLabel);
    topLayout->addStretch();
    topLayout->addLayout(rightLayout);
    topLayout->addStretch();
    topLayout->addWidget(backButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(centerLayout);
    mainLayout->setAlignment(centerLayout,Qt::AlignHCenter);

    // show final layout
    setLayout(mainLayout);
}

void AwayScreen::updateCurrentTemp()
{
    // updates current temp in away screen based on current temp in main window
    // connects to 10 second update timer within thermostat widget
    currentTempLabel->setText(formatTemperatureString(*m_currentTemp, m_globalSettings->temperatureFormat()));
}

void AwayScreen::updateUnit()
{
    upperBound->setValue(formatTemperature(m_upperBound, m_globalSettings->temperatureFormat()));
    lowerBound->setValue(formatTemperature(m_lowerBound, m_globalSettings->temperatureFormat()));

    updateCurrentTemp();
}

void AwayScreen::setCurrentTempPtr(int *currentTempPointer)
{
    m_currentTemp = currentTempPointer;
}
