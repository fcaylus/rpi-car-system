#include "musicactivity.h"

#include "../launcherapp.h"
#include "../soundmanager.h"

#include "easylogging++.h"

using namespace ilixi;

MusicActivity::MusicActivity(Widget *parent): BaseActivity("act_music", parent)
{
    setMargins(20, 20, 20, 20);

    // ---------------------------------------
    // Set the tool bar widget
    // ---------------------------------------

    _toolBarContainer = new ContainerWidget(this);
    HBoxLayout *tbLayout = new HBoxLayout(this);
    tbLayout->setVerticalAlignment(Alignment::Middle);
    _toolBarContainer->setLayout(tbLayout);

    _tbPlayButton = new FlatButton("play", 0, false, this);
    _tbPlayButton->setCheckable(false);
    _tbPlayButton->addAdditionnalState("pause");
    _tbPlayButton->setOnClickHandler([](int newState) {
        if(newState == 0)
            SoundManager::instance().pauseMusic();
        else
            SoundManager::instance().resumeMusic();
    });

    _tbNextButton = new FlatButton("skip_next", 0, false, this);
    _tbNextButton->setOnClickHandler([](int) {
        SoundManager::instance().nextMusic();
    });

    _tbPreviousButton = new FlatButton("skip_previous", 0, false, this);
    _tbPreviousButton->setOnClickHandler([](int) {
        SoundManager::instance().previousMusic();
    });

    _tbRepeatButton = new FlatButton("repeat", 0, false, this);
    _tbRepeatButton->setCheckable(true);
    _tbRepeatButton->addAdditionnalState("repeat_one");
    _tbRepeatButton->setOnClickHandler([](int newState) {
        SoundManager::instance().setRepeatMode(static_cast<SoundManager::RepeatMode>(newState));
    });

    _tbShuffleButton = new FlatButton("shuffle", 0, false, this);
    _tbShuffleButton->setCheckable(true);
    _tbShuffleButton->setOnClickHandler([](int newState) {
        SoundManager::instance().setRandom(newState == 1);
    });

    _tbToggleLibrary = new FlatButton("music", 0, false, this);
    _tbToggleLibrary->setCheckable(true);
    _tbToggleLibrary->setOnClickHandler([this](int newState) {
        if(newState == 1)
        {
            _container1->setVisible(false);
            _container2->setVisible(true);
        }
        else
        {
            _container1->setVisible(true);
            _container2->setVisible(false);
        }
    });

    _toolBarContainer->addWidget(new Spacer(Horizontal, this));
    _toolBarContainer->addWidget(_tbToggleLibrary);
    _toolBarContainer->addWidget(new Spacer(Horizontal, this));
    _toolBarContainer->addWidget(new Spacer(Horizontal, this));
    _toolBarContainer->addWidget(_tbRepeatButton);
    _toolBarContainer->addWidget(new Spacer(Horizontal, this));
    _toolBarContainer->addWidget(_tbPreviousButton);
    _toolBarContainer->addWidget(_tbPlayButton);
    _toolBarContainer->addWidget(_tbNextButton);
    _toolBarContainer->addWidget(new Spacer(Horizontal, this));
    _toolBarContainer->addWidget(_tbShuffleButton);
    _toolBarContainer->addWidget(new Spacer(Horizontal, this));

    LauncherApp::instance().registerToolBarWidget(name(), _toolBarContainer);

    // ---------------------------------------
    // Set "Playing" widgets
    // ---------------------------------------

    setLayout(new VBoxLayout(this));
    _container1 = new ContainerWidget(this);
    addWidget(_container1);

    _c1_coverWidget = new CoverIcon(this);

    _c1_titleLabel = new Label("", this);
    _c1_titleLabel->setSingleLine(true);
    _c1_titleLabel->setLayoutAlignment(TextLayout::Center);
    Font *c1_titleFont = new Font(*_c1_titleLabel->font());
    c1_titleFont->setStyle(Font::Style::Bold);
    c1_titleFont->setSize(c1_titleFont->size() + 15);
    _c1_titleLabel->setFont(c1_titleFont);

    _c1_artistLabel = new Label("", this);
    _c1_artistLabel->setSingleLine(true);
    _c1_artistLabel->setLayoutAlignment(TextLayout::Center);
    Font *c1_artistFont = new Font(*_c1_artistLabel->font());
    c1_artistFont->setStyle(Font::Style::Italic);
    c1_artistFont->setSize(c1_artistFont->size() + 3);
    _c1_artistLabel->setFont(c1_artistFont);

    _c1_musicProgressBar = new ProgressBar(this);
    _c1_musicProgressBar->setRange(0, 100);

    _c1_musicTimeLabel = new Label("0:00 / 0:00");
    _c1_musicTimeLabel->setSingleLine(true);
    _c1_musicTimeLabel->setLayoutAlignment(TextLayout::Left);

    _c1_volumeBar = new ProgressBar(this);
    _c1_volumeBar->setRange(0, 100);
    _c1_volumeBar->setValue(SoundManager::instance().volume());
    _c1_volumeBar->sigValueChanged.connect([this](int newValue) {
        SoundManager::instance().setVolume(newValue);
    });

    _c1_volumeMinus = new FlatButton("minus", 40, true, this);
    _c1_volumeMinus->setOnClickHandler([this](int) {
        _c1_volumeBar->setValue(_c1_volumeBar->value() - 2);
    });

    _c1_volumePlus = new FlatButton("plus", 40, true, this);
    _c1_volumePlus->setOnClickHandler([this](int) {
        _c1_volumeBar->setValue(_c1_volumeBar->value() + 2);
    });

    _c1_musicForward = new FlatButton("fast_forward", 50, false, this);
    _c1_musicForward->setOnClickHandler([](int){
        SoundManager::instance().forward();
    });

    _c1_musicRewind = new FlatButton("fast_rewind", 50, false, this);
    _c1_musicRewind->setOnClickHandler([](int){
        SoundManager::instance().rewind();
    });

    _c1_queueButton = new ToolButton("", this);
    _c1_queueButton->setToolButtonStyle(ToolButton::ToolButtonStyle::IconOnly);
    _c1_queueButton->setIcon(LauncherApp::iconPathForName("playlist"));
    _c1_queueButton->setRepeatable(false);
    _c1_queueButton->setDrawFrame(true);

    // Layouts

    ContainerWidget *c1_volumeContainer = new ContainerWidget(this);
    c1_volumeContainer->setYConstraint(WidgetResizeConstraint::FixedConstraint);
    HBoxLayout *c1_volumeLayout = new HBoxLayout(this);
    c1_volumeLayout->setVerticalAlignment(Alignment::Vertical::Middle);
    c1_volumeContainer->setLayout(c1_volumeLayout);
    c1_volumeContainer->addWidget(new Spacer(Horizontal, this));
    c1_volumeContainer->addWidget(_c1_volumeMinus);
    c1_volumeContainer->addWidget(_c1_volumeBar);
    c1_volumeContainer->addWidget(_c1_volumePlus);
    c1_volumeContainer->addWidget(new Spacer(Horizontal, this));

    ContainerWidget *c1_textContainer = new ContainerWidget(this);
    c1_textContainer->setYConstraint(WidgetResizeConstraint::FixedConstraint);
    VBoxLayout *c1_textLayout = new VBoxLayout(this);
    c1_textLayout->setHorizontalAlignment(Alignment::Horizontal::Center);
    c1_textContainer->setLayout(c1_textLayout);
    c1_textContainer->addWidget(new LineSeperator(Horizontal, this));
    c1_textContainer->addWidget(_c1_titleLabel);
    c1_textContainer->addWidget(_c1_artistLabel);
    c1_textContainer->addWidget(new LineSeperator(Horizontal, this));
    c1_textContainer->addWidget(new LineSeperator(Horizontal, this));

    ContainerWidget *c1_buttonsContainer = new ContainerWidget(this);
    c1_buttonsContainer->setYConstraint(WidgetResizeConstraint::FixedConstraint);
    HBoxLayout *c1_buttonsLayout = new HBoxLayout(this);
    c1_buttonsLayout->setVerticalAlignment(Alignment::Vertical::Middle);
    c1_buttonsContainer->setLayout(c1_buttonsLayout);
    c1_buttonsContainer->addWidget(new Spacer(Horizontal, this));
    c1_buttonsContainer->addWidget(_c1_musicRewind);
    c1_buttonsContainer->addWidget(new Label("    ", this));
    c1_buttonsContainer->addWidget(_c1_queueButton);
    c1_buttonsContainer->addWidget(new Label("    ", this));
    c1_buttonsContainer->addWidget(_c1_musicForward);
    c1_buttonsContainer->addWidget(new Spacer(Horizontal, this));

    VBoxLayout *c1_rightLayout = new VBoxLayout(this);
    c1_textLayout->setHorizontalAlignment(Alignment::Horizontal::Center);
    c1_rightLayout->addWidget(c1_textContainer);
    c1_rightLayout->addWidget(c1_volumeContainer);
    c1_rightLayout->addWidget(new LineSeperator(Horizontal, this));
    c1_rightLayout->addWidget(c1_buttonsContainer);

    ContainerWidget *c1_progressContainer = new ContainerWidget(this);
    c1_progressContainer->setYConstraint(WidgetResizeConstraint::FixedConstraint);
    HBoxLayout *c1_progressLayout = new HBoxLayout(this);
    c1_progressLayout->setVerticalAlignment(Alignment::Vertical::Middle);
    c1_progressContainer->setLayout(c1_progressLayout);
    c1_progressContainer->addWidget(_c1_musicProgressBar);
    c1_progressContainer->addWidget(_c1_musicTimeLabel);

    GridLayout *c1_centralLayout = new GridLayout(4, 4);
    c1_centralLayout->setSpacing(20);
    c1_centralLayout->addWidget(new Spacer(Vertical, this), 0, 0, 1, 4);
    c1_centralLayout->addWidget(_c1_coverWidget, 1, 0);
    c1_centralLayout->addWidget(c1_rightLayout, 1, 1, 1, 3);
    c1_centralLayout->addWidget(new Spacer(Vertical, this), 2, 0, 1, 4);
    c1_centralLayout->addWidget(c1_progressContainer, 3, 0, 1, 4);
    _container1->setLayout(c1_centralLayout);

    // ---------------------------------------
    // Set "Library" widgets
    // ---------------------------------------

    _container2 = new ContainerWidget(this);
    _container2->setVisible(false);
    addWidget(_container2);



    // ---------------------------------------
    // Register new media listener
    // ---------------------------------------

    SoundManager::instance().setOnNewMediaHandler([this](MediaInfo info, bool autoPlay){
        if(autoPlay)
            _tbPlayButton->setCurrentState(1);
        else
            _tbPlayButton->setCurrentState(0);

        _c1_coverWidget->setImage(info.coverFile);

        _c1_titleLabel->setText(info.title);
        _c1_artistLabel->setText(info.artist);

        _c1_musicProgressBar->setValue(0);
        _musicLength = info.length;
        _musicLengthStr = SoundManager::timeToString(info.length);
        _c1_musicTimeLabel->setText(std::string("0:00 / ") + _musicLengthStr);
    });

    SoundManager::instance().setOnNewTimeHandler([this](libvlc_time_t time) {
        _c1_musicProgressBar->setValue((time * 100) / _musicLength);
        _c1_musicTimeLabel->setText(SoundManager::timeToString(time) + std::string(" / ") + _musicLengthStr);

        _tbPlayButton->setCurrentState(1);

        // Update volume
        _c1_volumeBar->setValue(SoundManager::instance().volume());

    });

    SoundManager::instance().setOnEndReachedHandler([this]() {
        _c1_musicProgressBar->setValue(100);
        _c1_musicTimeLabel->setText(_musicLengthStr + std::string(" / ") + _musicLengthStr);
        _tbPlayButton->setCurrentState(0);
    });
}

/***********************************/
/***** CoverIcon Class *************/
/***********************************/

CoverIcon::CoverIcon(Widget *parent): Icon(parent)
{

}

Size CoverIcon::preferredSize() const
{
    const int areaHeight = BaseActivity::availableArea().height();
    const int width = areaHeight - (areaHeight * .3);
    return Size(width, width);
}

