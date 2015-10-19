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
    // Set Central Widgets
    // ---------------------------------------

    _coverWidget = new CoverIcon(this);

    _titleLabel = new Label("", this);
    _titleLabel->setSingleLine(true);
    _titleLabel->setLayoutAlignment(TextLayout::Center);
    Font *titleFont = new Font(*_titleLabel->font());
    titleFont->setStyle(Font::Style::Bold);
    titleFont->setSize(titleFont->size() + 15);
    _titleLabel->setFont(titleFont);

    _artistLabel = new Label("", this);
    _artistLabel->setSingleLine(true);
    _artistLabel->setLayoutAlignment(TextLayout::Center);
    Font *artistFont = new Font(*_artistLabel->font());
    artistFont->setStyle(Font::Style::Italic);
    artistFont->setSize(artistFont->size() + 3);
    _artistLabel->setFont(artistFont);

    _musicProgressBar = new ProgressBar(this);
    _musicProgressBar->setRange(0, 100);

    _musicTimeLabel = new Label("0:00 / 0:00");
    _musicTimeLabel->setSingleLine(true);
    _musicTimeLabel->setLayoutAlignment(TextLayout::Left);

    _volumeBar = new ProgressBar(this);
    _volumeBar->setRange(0, 100);
    _volumeBar->setValue(SoundManager::instance().volume());
    _volumeBar->sigValueChanged.connect([this](int newValue) {
        SoundManager::instance().setVolume(newValue);
    });

    _volumeMinus = new FlatButton("minus", 40, true, this);
    _volumeMinus->setOnClickHandler([this](int) {
        _volumeBar->setValue(_volumeBar->value() - 2);
    });

    _volumePlus = new FlatButton("plus", 40, true, this);
    _volumePlus->setOnClickHandler([this](int) {
        _volumeBar->setValue(_volumeBar->value() + 2);
    });

    _musicForward = new FlatButton("fast_forward", 50, false, this);
    _musicForward->setOnClickHandler([](int){
        SoundManager::instance().forward();
    });

    _musicRewind = new FlatButton("fast_rewind", 50, false, this);
    _musicRewind->setOnClickHandler([](int){
        SoundManager::instance().rewind();
    });

    _queueButton = new ToolButton("", this);
    _queueButton->setToolButtonStyle(ToolButton::ToolButtonStyle::IconOnly);
    _queueButton->setIcon(LauncherApp::iconPathForName("playlist"));
    _queueButton->setRepeatable(false);
    _queueButton->setDrawFrame(true);

    // Layouts

    ContainerWidget *volumeContainer = new ContainerWidget(this);
    volumeContainer->setYConstraint(WidgetResizeConstraint::FixedConstraint);
    HBoxLayout *volumeLayout = new HBoxLayout(this);
    volumeLayout->setVerticalAlignment(Alignment::Vertical::Middle);
    volumeContainer->setLayout(volumeLayout);
    volumeContainer->addWidget(new Spacer(Orientation::Horizontal, this));
    volumeContainer->addWidget(_volumeMinus);
    volumeContainer->addWidget(_volumeBar);
    volumeContainer->addWidget(_volumePlus);
    volumeContainer->addWidget(new Spacer(Orientation::Horizontal, this));

    ContainerWidget *textContainer = new ContainerWidget(this);
    textContainer->setYConstraint(WidgetResizeConstraint::FixedConstraint);
    VBoxLayout *textLayout = new VBoxLayout(this);
    textLayout->setHorizontalAlignment(Alignment::Horizontal::Center);
    textContainer->setLayout(textLayout);
    textContainer->addWidget(new LineSeperator(Orientation::Horizontal, this));
    textContainer->addWidget(_titleLabel);
    textContainer->addWidget(_artistLabel);
    textContainer->addWidget(new LineSeperator(Orientation::Horizontal, this));
    textContainer->addWidget(new LineSeperator(Orientation::Horizontal, this));

    ContainerWidget *buttonsContainer = new ContainerWidget(this);
    buttonsContainer->setYConstraint(WidgetResizeConstraint::FixedConstraint);
    HBoxLayout *buttonsLayout = new HBoxLayout(this);
    buttonsLayout->setVerticalAlignment(Alignment::Vertical::Middle);
    buttonsContainer->setLayout(buttonsLayout);
    buttonsContainer->addWidget(new Spacer(Orientation::Horizontal, this));
    buttonsContainer->addWidget(_musicRewind);
    buttonsContainer->addWidget(new Label("    ", this));
    buttonsContainer->addWidget(_queueButton);
    buttonsContainer->addWidget(new Label("    ", this));
    buttonsContainer->addWidget(_musicForward);
    buttonsContainer->addWidget(new Spacer(Orientation::Horizontal, this));

    VBoxLayout *rightLayout = new VBoxLayout(this);
    textLayout->setHorizontalAlignment(Alignment::Horizontal::Center);
    rightLayout->addWidget(textContainer);
    //rightLayout->addWidget(new Spacer(Orientation::Vertical, this));
    rightLayout->addWidget(volumeContainer);
    rightLayout->addWidget(new LineSeperator(Orientation::Horizontal, this));
    rightLayout->addWidget(buttonsContainer);

    ContainerWidget *progressContainer = new ContainerWidget(this);
    progressContainer->setYConstraint(WidgetResizeConstraint::FixedConstraint);
    HBoxLayout *progressLayout = new HBoxLayout(this);
    progressLayout->setVerticalAlignment(Alignment::Vertical::Middle);
    progressContainer->setLayout(progressLayout);
    progressContainer->addWidget(_musicProgressBar);
    progressContainer->addWidget(_musicTimeLabel);

    GridLayout *centralLayout = new GridLayout(4, 4);
    centralLayout->setSpacing(20);
    centralLayout->addWidget(new Spacer(Orientation::Vertical, this), 0, 0, 1, 4);
    centralLayout->addWidget(_coverWidget, 1, 0);
    centralLayout->addWidget(rightLayout, 1, 1, 1, 3);
    centralLayout->addWidget(new Spacer(Orientation::Vertical, this), 2, 0, 1, 4);
    centralLayout->addWidget(progressContainer, 3, 0, 1, 4);
    setLayout(centralLayout);

    // ---------------------------------------
    // Register new media listener
    // ---------------------------------------

    SoundManager::instance().setOnNewMediaHandler([this](MediaInfo info, bool autoPlay){
        if(autoPlay)
            _tbPlayButton->setCurrentState(1);
        else
            _tbPlayButton->setCurrentState(0);

        _coverWidget->setImage(info.coverFile);

        _titleLabel->setText(info.title);
        _artistLabel->setText(info.artist);

        _musicProgressBar->setValue(0);
        _musicLength = info.length;
        _musicLengthStr = SoundManager::timeToString(info.length);
        _musicTimeLabel->setText(std::string("0:00 / ") + _musicLengthStr);
    });

    SoundManager::instance().setOnNewTimeHandler([this](libvlc_time_t time) {
        _musicProgressBar->setValue((time * 100) / _musicLength);
        _musicTimeLabel->setText(SoundManager::timeToString(time) + std::string(" / ") + _musicLengthStr);

        _tbPlayButton->setCurrentState(1);

        // Update volume
        _volumeBar->setValue(SoundManager::instance().volume());

    });

    SoundManager::instance().setOnEndReachedHandler([this]() {
        _musicProgressBar->setValue(100);
        _musicTimeLabel->setText(_musicLengthStr + std::string(" / ") + _musicLengthStr);
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

