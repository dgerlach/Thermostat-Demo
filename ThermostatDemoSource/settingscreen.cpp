#include "settingscreen.h"
#include "mainwindow.h"
#include "globalsettings.h"
#include "qkeyboardlineedit.h"
#include "webdataengine/openweathermapdataengine.h"

#include <QtGui>

#define MAXIMUM_CONTENT_HEIGHT 450
#define MAXIMUM_CONTENT_WIDTH 800

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
    connect(backButton,SIGNAL(clicked()),this,SLOT(commitChanges()));
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(valueChanged()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addSpacing(40);
    topLayout->addStretch();
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(backButton);

    QTabWidget* mainTabWidget = new QTabWidget(this);
    mainTabWidget->setFocusPolicy(Qt::NoFocus);

    mainTabWidget->addTab(buildGeneralSettingsWidget(), "General");
    mainTabWidget->addTab(buildFormatSettingsWidget(), "Format");
    mainTabWidget->addTab(buildNetworkSettingsWidget(), "Network");
//    mainTabWidget->addTab(buildGeneralSettingsWidget(), QIcon(":/Images/glossy-blue-orb-icon-arrowback.png"),"General");
//    mainTabWidget->addTab(buildFormatSettingsWidget(), QIcon(":/Images/glossy-blue-orb-icon-arrowback.png"),"Format");
//    mainTabWidget->addTab(buildNetworkSettingsWidget(), QIcon(":/Images/glossy-blue-orb-icon-arrowback.png"),"Network");
//
    mainTabWidget->setIconSize(QSize(40,40));
    mainTabWidget->setTabPosition(QTabWidget::West);
    mainTabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainTabWidget->setMaximumSize(MAXIMUM_CONTENT_HEIGHT, MAXIMUM_CONTENT_WIDTH);

    QHBoxLayout* contentsBox = new QHBoxLayout;
    contentsBox->addWidget(mainTabWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(contentsBox);
    mainLayout->setStretchFactor(contentsBox, 0);
    mainLayout->addStretch(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setAlignment(contentsBox,Qt::AlignCenter);


    // show layout
    setLayout(mainLayout);

}

void SettingScreen::showEvent(QShowEvent *)
{
    fillSettingValues();
}

void SettingScreen::fillSettingValues()
{
    //populate the current settings

    int index = cityBox->findText(m_globalSettings->currentCity());
    if(index>-1)
        cityBox->setCurrentIndex(index);

    if(m_globalSettings->temperatureFormat() == GlobalSettings::TempFormatC)
        setTempFormatC();
    else
        setTempFormatF();

    if(m_globalSettings->timeFormat() == GlobalSettings::TimeFormat24h)
        setTimeFormat24h();
    else
        setTimeFormat12h();

    m_proxyHostLineEdit->setText(m_globalSettings->proxyHost());

    //to prevent a zero from showing up
    if(QString::number(m_globalSettings->proxyPort()) != 0)
        m_proxyPortLineEdit->setText(QString::number(m_globalSettings->proxyPort()));
}

QWidget* SettingScreen::buildNetworkSettingsWidget()
{
    QWidget* networkSettingsWidget = new QWidget(this);
    networkSettingsWidget->setStyle(this->style());


    QLabel *m_proxyHostLabel = new QLabel("Proxy Host: ", this);
    QLabel *m_proxyPortLabel = new QLabel("Proxy Port: ", this);
    m_proxyHostLineEdit = new QKeyboardLineEdit;
    m_proxyPortLineEdit = new QKeyboardLineEdit;

    QFormLayout* proxyLayout = new QFormLayout;
    proxyLayout->addRow(m_proxyHostLabel, m_proxyHostLineEdit);
    proxyLayout->addRow(m_proxyPortLabel, m_proxyPortLineEdit);
    m_proxyHostLineEdit->setMaximumWidth(300);
    m_proxyPortLineEdit->setMaximumWidth(80);
    proxyLayout->setFormAlignment(Qt::AlignCenter);

    QVBoxLayout* proxyVBoxLayout = new QVBoxLayout(this);
    proxyVBoxLayout->addStretch();
    proxyVBoxLayout->addLayout(proxyLayout);

    QGroupBox* proxySettingsGroupBox = new QGroupBox("Proxy Settings");
    proxySettingsGroupBox->setLayout(proxyVBoxLayout);


    QLabel *m_remoteListenPort = new QLabel("Remote Listen Port: ", this);
    m_remoteListenPortLineEdit = new QKeyboardLineEdit;

    QFormLayout* remoteListenLayout = new QFormLayout;
    remoteListenLayout->addRow(m_remoteListenPort, m_remoteListenPortLineEdit);

    QGroupBox* remoteSettingsGroupBox = new QGroupBox("Remote Settings");
    remoteSettingsGroupBox->setLayout(remoteListenLayout);

    QVBoxLayout *networkSettingsLayout = new QVBoxLayout(this);
    networkSettingsLayout->addWidget(proxySettingsGroupBox);
    //networkSettingsLayout->addWidget(remoteSettingsGroupBox);
    networkSettingsLayout->addStretch();

    networkSettingsWidget->setLayout(networkSettingsLayout);

    return networkSettingsWidget;
}

QWidget* SettingScreen::buildGeneralSettingsWidget()
{
    // create box to contain all possible cities
    cityBox = new QComboBox;
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


    QVBoxLayout *cityLayout = new QVBoxLayout;
    cityLayout->addStretch();
    cityLayout->addWidget(cityBox);
    cityLayout->addStretch();

    QWidget* generalSettingsWidget = new QWidget(this);

    QGroupBox* generalSettingsGroupBox = new QGroupBox("City");
    licenseStringLabel = new QLabel("");
    licenseStringLabel->setWordWrap(true);
    licenseStringLabel->setAlignment(Qt::AlignCenter);

    generalSettingsGroupBox->setLayout(cityLayout);

    QVBoxLayout *generalSettingsLayout = new QVBoxLayout(this);
    generalSettingsLayout->addWidget(generalSettingsGroupBox);
    generalSettingsLayout->addStretch();
    generalSettingsLayout->addWidget(licenseStringLabel, 0, Qt::AlignCenter);
    generalSettingsLayout->addStretch();

    generalSettingsWidget->setLayout(generalSettingsLayout);

    return generalSettingsWidget;
}

void SettingScreen::setLicenseString(QString licenseString)
{
    licenseStringLabel->setText(licenseString);
}

QWidget* SettingScreen::buildFormatSettingsWidget()
{
    // create button to select Fahrenheit
    FButton = new QPushButton("°F", (this));
    //FButton->setMaximumWidth(150);
    FButton->setObjectName("FButton");
    FButton->setCheckable(true);
    FButton->setFocusPolicy(Qt::NoFocus);
    connect(FButton, SIGNAL(clicked()), this, SLOT(setTempFormatF()));

    // create button to select Celsius
    CButton = new QPushButton("°C", (this));
    //CButton->setMaximumWidth(150);
    CButton->setObjectName("CButton");
    CButton->setCheckable(true);
    CButton->setFocusPolicy(Qt::NoFocus);
    connect(CButton, SIGNAL(clicked()), this, SLOT(setTempFormatC()));

    // create button to select 12h time format
    Button12h = new QPushButton("12h", (this));
    Button12h->setObjectName("Button12h");
    Button12h->setCheckable(true);
    Button12h->setFocusPolicy(Qt::NoFocus);
    connect(Button12h, SIGNAL(clicked()), this, SLOT(setTimeFormat12h()));

    // create button to select 24h time format
    Button24h = new QPushButton("24h", (this));
    Button24h->setObjectName("Button24h");
    Button24h->setCheckable(true);
    Button24h->setFocusPolicy(Qt::NoFocus);
    connect(Button24h, SIGNAL(clicked()), this, SLOT(setTimeFormat24h()));

    // create layout
    QHBoxLayout *unitLayout = new QHBoxLayout;
    unitLayout->addSpacing(100);
    unitLayout->addWidget(FButton);
    unitLayout->addWidget(CButton);
    unitLayout->addSpacing(100);
    unitLayout->setSpacing(0);
    unitLayout->setContentsMargins(0,0,0,0);

    // create layout
    QHBoxLayout *timeFormatLayout = new QHBoxLayout;
    timeFormatLayout->addSpacing(100);
    timeFormatLayout->addWidget(Button12h);
    timeFormatLayout->addWidget(Button24h);
    timeFormatLayout->addSpacing(100);
    timeFormatLayout->setSpacing(0);
    timeFormatLayout->setContentsMargins(0,0,0,0);

    QGroupBox *tempFormatGroupBox = new QGroupBox("Temperature", this);
    QGroupBox *timeFormatGroupBox = new QGroupBox("Time Format", this);

    tempFormatGroupBox->setLayout(unitLayout);
    timeFormatGroupBox->setLayout(timeFormatLayout);

    QVBoxLayout *formatSettingsLayout = new QVBoxLayout(this);
    formatSettingsLayout->addWidget(tempFormatGroupBox);
    formatSettingsLayout->addWidget(timeFormatGroupBox);

    QWidget *formatSettingsWidget = new QWidget(this);
    formatSettingsWidget->setLayout(formatSettingsLayout);

    return formatSettingsWidget;
}



void SettingScreen::setTempFormatF()
{
    CButton->setDisabled(false);
    m_globalSettings->setTemperatureFormat(GlobalSettings::TempFormatF);
    CButton->setChecked(false);
    FButton->setChecked(true);
    FButton->setDisabled(true);
}

void SettingScreen::setTempFormatC()
{
    FButton->setDisabled(false);
    m_globalSettings->setTemperatureFormat(GlobalSettings::TempFormatC);
    FButton->setChecked(false);
    CButton->setChecked(true);
    CButton->setDisabled(true);
}

void SettingScreen::setTimeFormat12h()
{
    Button24h->setDisabled(false);
    m_globalSettings->setTimeFormat(GlobalSettings::TimeFormat12h);
    Button24h->setChecked(false);
    Button12h->setChecked(true);
    Button12h->setDisabled(true);
}

void SettingScreen::setTimeFormat24h()
{
    Button12h->setDisabled(false);
    m_globalSettings->setTimeFormat(GlobalSettings::TimeFormat24h);
    Button12h->setChecked(false);
    Button24h->setChecked(true);
    Button24h->setDisabled(true);
}

void SettingScreen::commitChanges()
{
    m_globalSettings->setProxyInfo(m_proxyHostLineEdit->text(),m_proxyPortLineEdit->text().toInt());
    emit(valueChanged());

    QString city = cityBox->currentText();
    m_globalSettings->setCurrentCity(city);
    emit cityChanged();

    close();
}
