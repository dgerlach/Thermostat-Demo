#include "settingscreen.h"
#include "mainwindow.h"
#include "globalsettings.h"
#include "qkeyboardlineedit.h"

#include <QtGui>

SettingScreen::SettingScreen(QWidget *parent) :
    QWidget(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    // create title
    QLabel *title = new QLabel("Select your CURRENT SETTINGS");

    // create return button
    QPushButton *backButton = new QPushButton((this));
    backButton->setIcon(QIcon(":/Images/glossy-blue-orb-icon-arrowback.png"));
    backButton->setIconSize(QSize(40,40));
    backButton->setFocusPolicy(Qt::NoFocus);
    backButton->setObjectName("backButton");
    connect(backButton,SIGNAL(clicked()),this,SLOT(close()));

    // create button to select Fahrenheit
    FButton = new QPushButton("°F", (this));
    FButton->setObjectName("FButton");
    FButton->setCheckable(true);
    FButton->setChecked(true);
    FButton->setFocusPolicy(Qt::NoFocus);
    connect(FButton, SIGNAL(clicked()), this, SLOT(unitIsF()));

    // create button to select Celsius
    CButton = new QPushButton("°C", (this));
    CButton->setObjectName("CButton");
    CButton->setCheckable(true);
    CButton->setFocusPolicy(Qt::NoFocus);
    connect(CButton, SIGNAL(clicked()), this, SLOT(unitIsC()));

    // when either button is clicked, tell the rest of the program to switch between C/F
    connect(FButton, SIGNAL(clicked()), this, SIGNAL(valueChanged()));
    connect(CButton, SIGNAL(clicked()), this, SIGNAL(valueChanged()));

    // create box to contain all possible cities
    QComboBox *cityBox = new QComboBox;
    cityBox->setFocusPolicy(Qt::NoFocus);
    cityBox->addItem("Dallas,TX");
    cityBox->addItem("Tokyo,Japan");
    cityBox->addItem("Chicago,IL");
    cityBox->addItem("Hong Kong,China");
    cityBox->addItem("Munich,Germany");
    cityBox->addItem("Taipei,Taiwan");
    cityBox->addItem("San Jose,CA");
    cityBox->addItem("Shanghai,China");
    cityBox->addItem("New York,NY");
    cityBox->addItem("Bangalore,India");
    cityBox->addItem("Sao Paulo,Brazil");
    cityBox->addItem("Seoul,South Korea");
    cityBox->addItem("Moscow,Russia");
    cityBox->addItem("Mexico City,Mexico");
    cityBox->addItem("Johannesburg,South Africa");

    int index = cityBox->findText(m_globalSettings->currentCity());

    if(index>-1)
        cityBox->setCurrentIndex(index);

    // when city is changed, create chain of events to send to web request and update main screen
    connect(cityBox, SIGNAL(activated(QString)), this, SLOT(changeCity(QString)));

    // create city label
    QLabel *cityLabel = new QLabel("City: ", (this));

    // create layout
    QHBoxLayout *unitLayout = new QHBoxLayout;
    unitLayout->addSpacing(100);
    unitLayout->addWidget(FButton);
    unitLayout->addWidget(CButton);
    unitLayout->addSpacing(100);
    unitLayout->setSpacing(0);
    unitLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *cityLayout = new QHBoxLayout;
    cityLayout->addStretch();
    cityLayout->addWidget(cityLabel);
    cityLayout->addWidget(cityBox);
    cityLayout->addStretch();

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addSpacing(40);
    topLayout->addStretch();
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(backButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(cityLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(unitLayout);
    mainLayout->addStretch();

    // show layout
    setLayout(mainLayout);
}

void SettingScreen::unitIsF()
{
    // mark F button as down and enable C button to be clicked
    CButton->setDisabled(false);
    // unit: true means F & false means C
    m_globalSettings->setTemperatureFormat(GlobalSettings::TempFormatF);
    CButton->setChecked(false);
    FButton->setDisabled(true);
}

void SettingScreen::unitIsC()
{
    // mark C button as down and enable F button to be clicked
    FButton->setDisabled(false);
    // unit: true means F & false means C
    m_globalSettings->setTemperatureFormat(GlobalSettings::TempFormatC);
    FButton->setChecked(false);
    CButton->setDisabled(true);


}

void SettingScreen::changeCity(QString city)
{
    m_globalSettings->setCurrentCity(city);
    emit cityChanged(city);
}
