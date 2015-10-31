#include "musicactivity.h"

#include "../launcherapp.h"
#include "../soundmanager.h"
#include "easylogging++.h"

#include <ui/Spacer.h>
#include <ui/LineSeperator.h>
#include <ui/VBoxLayout.h>
#include <ui/HBoxLayout.h>
#include <ui/ScrollArea.h>

#include <map>
#include <set>
#include <vector>

using namespace ilixi;

MusicActivity::MusicActivity(Widget *parent): BaseActivity("act_music", parent)
{
    setMargins(20, 20, 20, 20);
    setConstraints(NoConstraint, FixedConstraint);

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

    _tbToggleLibrary->setCurrentState(1);
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

    setLayout(new HBoxLayout(this));
    _container1 = new ContainerWidget(this);
    _container1->setVisible(false);
    addWidget(_container1);

    _c1_coverWidget = new CoverIcon(this);

    _c1_titleLabel = new Label("", this);
    _c1_titleLabel->setSingleLine(true);
    _c1_titleLabel->setLayoutAlignment(TextLayout::Center);
    setWidgetFont(_c1_titleLabel, 15, Font::Style::Bold);

    _c1_artistLabel = new Label("", this);
    _c1_artistLabel->setSingleLine(true);
    _c1_artistLabel->setLayoutAlignment(TextLayout::Center);
    setWidgetFont(_c1_artistLabel, 3, Font::Style::Italic);

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
    _c1_queueButton->setDisabled();

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
    _container2->setConstraints(NoConstraint, FixedConstraint);
    addWidget(_container2);

    _c2_modeAlbum = new DirectionalButton("Albums", this);
    _c2_modeAlbum->setIcon(LauncherApp::iconPathForName("album"), Size(32,32));
    setWidgetFont(_c2_modeAlbum, 4);
    _c2_modeAlbum->sigClicked.connect([this]() {
        _c2_artistContainer->setVisible(false);
        _c2_trackContainer->setVisible(false);
        _c2_albumContainer->setVisible(true);

        _c2_artistBackButton->setVisible(false);
        _c2_trackBackButton->setVisible(false);
        _c2_albumBackButton->setVisible(true);
    });

    _c2_modeArtist = new DirectionalButton("Artistes", this);
    _c2_modeArtist->setIcon(LauncherApp::iconPathForName("person"), Size(32,32));
    setWidgetFont(_c2_modeArtist, 4);
    _c2_modeArtist->sigClicked.connect([this]() {
        _c2_albumContainer->setVisible(false);
        _c2_trackContainer->setVisible(false);
        _c2_artistContainer->setVisible(true);

        _c2_albumBackButton->setVisible(false);
        _c2_trackBackButton->setVisible(false);
        _c2_artistBackButton->setVisible(true);
    });

    _c2_modeTrack = new DirectionalButton("Titres", this);
    _c2_modeTrack->setIcon(LauncherApp::iconPathForName("track"), Size(32,32));
    setWidgetFont(_c2_modeTrack, 4);
    _c2_modeTrack->sigClicked.connect([this]() {
        _c2_artistContainer->setVisible(false);
        _c2_albumContainer->setVisible(false);
        _c2_trackContainer->setVisible(true);

        _c2_albumBackButton->setVisible(false);
        _c2_artistBackButton->setVisible(false);
        _c2_trackBackButton->setVisible(true);
    });

    _c2_chooseModeButton = new ButtonGroup(Vertical, this);
    _c2_chooseModeButton->addButton(_c2_modeArtist);
    _c2_chooseModeButton->addButton(_c2_modeAlbum);
    _c2_chooseModeButton->addButton(_c2_modeTrack);

    _c2_artistContainer = new ContainerWidget(this);
    _c2_artistContainer->setConstraints(NoConstraint, FixedConstraint);
    _c2_artistContainer->setVisible(false);
    _c2_albumContainer = new ContainerWidget(this);
    _c2_albumContainer->setConstraints(NoConstraint, FixedConstraint);
    _c2_trackContainer = new ContainerWidget(this);
    _c2_trackContainer->setConstraints(NoConstraint, FixedConstraint);
    _c2_trackContainer->setVisible(false);

    _c2_artistListBox = new CustomListBox(this);
    _c2_artistListBox->setDrawFrame(true);

    _c2_artistLabel = new Label("Liste des artistes", this);
    _c2_artistLabel->setConstraints(NoConstraint, FixedConstraint);
    setWidgetFont(_c2_artistLabel, 3);

    _c2_artistBackButton = new ToolButton("", this);
    _c2_artistBackButton->setToolButtonStyle(ToolButton::IconOnly);
    _c2_artistBackButton->setIcon(LauncherApp::iconPathForName("back"));
    _c2_artistBackButton->setDisabled();
    _c2_artistBackButton->setVisible(false);

    _c2_albumListBox = new CustomListBox(this);
    _c2_albumListBox->setDrawFrame(true);

    _c2_albumLabel = new Label("Liste des albums", this);
    _c2_albumLabel->setConstraints(NoConstraint, FixedConstraint);
    setWidgetFont(_c2_albumLabel, 3);

    _c2_albumBackButton = new ToolButton("", this);
    _c2_albumBackButton->setToolButtonStyle(ToolButton::IconOnly);
    _c2_albumBackButton->setIcon(LauncherApp::iconPathForName("back"));
    _c2_albumBackButton->setDisabled();

    _c2_trackListBox = new CustomListBox(this);
    _c2_trackListBox->setDrawFrame(true);

    _c2_trackLabel = new Label("Liste des morceaux", this);
    _c2_trackLabel->setConstraints(NoConstraint, FixedConstraint);
    setWidgetFont(_c2_trackLabel, 3);

    _c2_trackBackButton = new ToolButton("", this);
    _c2_trackBackButton->setToolButtonStyle(ToolButton::IconOnly);
    _c2_trackBackButton->setIcon(LauncherApp::iconPathForName("back"));
    _c2_trackBackButton->setDisabled();
    _c2_trackBackButton->setVisible(false);

    // Layouts


    HBoxLayout *c2_centralLayout = new HBoxLayout(this);
    c2_centralLayout->setVerticalAlignment(Alignment::Vertical::Middle);
    c2_centralLayout->setYConstraint(FixedConstraint);
    _container2->setLayout(c2_centralLayout);

    ContainerWidget *c2_buttonsContainer = new ContainerWidget(this);
    c2_buttonsContainer->setConstraints(FixedConstraint, FixedConstraint);
    VBoxLayout *c2_buttonsLayout = new VBoxLayout(this);
    c2_buttonsLayout->setHorizontalAlignment(Alignment::Center);
    c2_buttonsContainer->setLayout(c2_buttonsLayout);
    c2_buttonsContainer->addWidget(_c2_chooseModeButton);
    c2_buttonsContainer->addWidget(new Spacer(Vertical, this));
    c2_buttonsContainer->addWidget(new LineSeperator(Horizontal, this));
    c2_buttonsContainer->addWidget(new Spacer(Vertical, this));
    c2_buttonsContainer->addWidget(_c2_artistBackButton);
    c2_buttonsContainer->addWidget(_c2_albumBackButton);
    c2_buttonsContainer->addWidget(_c2_trackBackButton);

    c2_centralLayout->addWidget(c2_buttonsContainer);
    c2_centralLayout->addWidget(new LineSeperator(Vertical, this));
    c2_centralLayout->addWidget(_c2_albumContainer);
    c2_centralLayout->addWidget(_c2_artistContainer);
    c2_centralLayout->addWidget(_c2_trackContainer);

    VBoxLayout *c2_artistContainerLayout = new VBoxLayout(this);
    c2_artistContainerLayout->setConstraints(FixedConstraint, FixedConstraint);
    _c2_artistContainer->setLayout(c2_artistContainerLayout);
    _c2_artistContainer->addWidget(_c2_artistLabel);
    _c2_artistContainer->addWidget(_c2_artistListBox);

    VBoxLayout *c2_albumContainerLayout = new VBoxLayout(this);
    c2_albumContainerLayout->setConstraints(FixedConstraint, FixedConstraint);
    _c2_albumContainer->setLayout(c2_albumContainerLayout);
    _c2_albumContainer->addWidget(_c2_albumLabel);
    _c2_albumContainer->addWidget(_c2_albumListBox);

    VBoxLayout *c2_trackContainerLayout = new VBoxLayout(this);
    c2_trackContainerLayout->setConstraints(FixedConstraint, FixedConstraint);
    _c2_trackContainer->setLayout(c2_trackContainerLayout);
    _c2_trackContainer->addWidget(_c2_trackLabel);
    _c2_trackContainer->addWidget(_c2_trackListBox);

    // ---------------------------------------
    // Register all media listeners
    // ---------------------------------------

    SoundManager::instance().setOnNewMediaHandler([this](MediaInfo info, libvlc_time_t duration, bool autoPlay){
        if(autoPlay)
            _tbPlayButton->setCurrentState(1);
        else
            _tbPlayButton->setCurrentState(0);

        _c1_coverWidget->setImage(info.coverFile);

        _c1_titleLabel->setText(info.title);
        _c1_artistLabel->setText(info.artist);

        _c1_musicProgressBar->setValue(0);
        _musicLength = duration;
        _musicLengthStr = SoundManager::timeToString(duration);
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

    SoundManager::instance().setOnMediaListUpdatedHandler([this](MediaList mediaList, MediaMap albumMap, SortedMediaMap artistMap) {

        updateArtistWidgets(artistMap);
        updateAlbumWidgets(albumMap);
        updateTrackWidgets(mediaList);

    });

    // Check for music files
    SoundManager::instance().checkForNewMusicFiles();
}

//
// Private methods
//
void MusicActivity::updateArtistWidgets(SortedMediaMap artistMap)
{
    _c2_artistListBox->clear();

    for(SortedMediaMap::iterator it = artistMap.begin(); it != artistMap.end(); ++it)
    {
        CustomListBox *albumListBox = new CustomListBox(this);
        albumListBox->setDrawFrame(true);
        albumListBox->setVisible(false);
        _c2_artistContainer->addWidget(albumListBox);

        for(MediaMap::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            CustomListBox *trackListBox = new CustomListBox(this);
            trackListBox->setDrawFrame(true);
            trackListBox->setVisible(false);
            _c2_artistContainer->addWidget(trackListBox);

            // The cover of the first song is used as album cover
            std::string albumCover = "";

            for(MediaInfo trackInfo : it2->second)
            {
                std::string iconPath = trackInfo.coverFile;
                if(iconPath == DirUtility::executableDir() + "/theme/default_artwork.png")
                    iconPath = LauncherApp::iconPathForName("track");

                if(albumCover.empty())
                {
                    albumCover = iconPath;
                    if(albumCover == LauncherApp::iconPathForName("track"))
                        albumCover = LauncherApp::iconPathForName("album");
                }

                ScrollItem *trackItem = new ScrollItem(trackInfo.title, iconPath, 48, this);
                trackListBox->addItem(trackItem);
                trackItem->sigClicked.connect([this, trackInfo](std::string) {
                    _tbToggleLibrary->click();
                    SoundManager::instance().playFromMedia(trackInfo);
                });
            }

            ScrollItem *albumItem = new ScrollItem(it2->first, albumCover, 64, this);
            albumListBox->addItem(albumItem);
            albumItem->sigClicked.connect([this, trackListBox, albumListBox](std::string label) {
                albumListBox->setVisible(false);
                trackListBox->setVisible(true);

                _c2_artistLabelPreviousText = _c2_artistLabel->text();
                _c2_artistLabel->setText(label + std::string(" - ") + _c2_artistLabel->text());

                _c2_artistBackButtonConnection.disconnect();
                _c2_artistBackButtonConnection = sigc::connection(_c2_artistBackButton->sigClicked.connect([this, trackListBox, albumListBox]() {
                    trackListBox->setVisible(false);
                    albumListBox->setVisible(true);

                    _c2_artistLabel->setText(_c2_artistLabelPreviousText);

                    _c2_artistBackButtonConnection.disconnect();
                    _c2_artistBackButtonConnection = sigc::connection(_c2_artistBackButton->sigClicked.connect([this, albumListBox](){
                        _c2_artistBackButton->setDisabled();
                        albumListBox->setVisible(false);
                        _c2_artistListBox->setVisible(true);

                        _c2_artistLabel->setText("Liste des artistes");
                    }));
                }));
            });
        }

        ScrollItem *item = new ScrollItem(it->first, LauncherApp::iconPathForName("person"), 48, this);
        item->sigClicked.connect([this, albumListBox](std::string label) {
            _c2_artistListBox->setVisible(false);
            albumListBox->setVisible(true);
            _c2_artistBackButton->setEnabled();

            _c2_artistLabelPreviousText = _c2_artistLabel->text();
            _c2_artistLabel->setText(label);

            _c2_artistBackButtonConnection.disconnect();
            _c2_artistBackButtonConnection = sigc::connection(_c2_artistBackButton->sigClicked.connect([this, albumListBox](){
                _c2_artistBackButton->setDisabled();
                albumListBox->setVisible(false);
                _c2_artistListBox->setVisible(true);

                _c2_artistLabel->setText(_c2_artistLabelPreviousText);
            }));

        });

        _c2_artistListBox->addItem(item);

    }
}

void MusicActivity::updateAlbumWidgets(MediaMap albumMap)
{
    _c2_albumListBox->clear();

    for(MediaMap::iterator it = albumMap.begin(); it != albumMap.end(); ++it)
    {
        CustomListBox *trackListBox = new CustomListBox(this);
        trackListBox->setDrawFrame(true);
        trackListBox->setVisible(false);
        _c2_albumContainer->addWidget(trackListBox);

        // The cover of the first song is used as album cover
        std::string albumCover = "";

        for(MediaInfo trackInfo : it->second)
        {
            std::string iconPath = trackInfo.coverFile;
            if(iconPath == DirUtility::executableDir() + "/theme/default_artwork.png")
                iconPath = LauncherApp::iconPathForName("track");

            if(albumCover.empty())
            {
                albumCover = iconPath;
                if(albumCover == LauncherApp::iconPathForName("track"))
                    albumCover = LauncherApp::iconPathForName("album");
            }

            ScrollItem *trackItem = new ScrollItem(trackInfo.title, iconPath, 48, this);
            trackListBox->addItem(trackItem);
            trackItem->sigClicked.connect([this, trackInfo](std::string) {
                _tbToggleLibrary->click();
                SoundManager::instance().playFromMedia(trackInfo);
            });
        }

        ScrollItem *albumItem = new ScrollItem(it->first, albumCover, 64, this);
        _c2_albumListBox->addItem(albumItem);
        albumItem->sigClicked.connect([this, trackListBox](std::string label) {
            _c2_albumListBox->setVisible(false);
            trackListBox->setVisible(true);
            _c2_albumBackButton->setEnabled();

            _c2_albumLabel->setText(label);

            _c2_albumBackButton->sigClicked.connect([this, trackListBox](){
                _c2_albumBackButton->setDisabled();
                trackListBox->setVisible(false);
                _c2_albumListBox->setVisible(true);

                _c2_artistLabel->setText("Liste des albums");
            });
        });
    }
}

void MusicActivity::updateTrackWidgets(MediaList mediaList)
{
    _c2_trackListBox->clear();

    for(MediaInfo &trackInfo : mediaList)
    {
        std::string iconPath = trackInfo.coverFile;
        if(iconPath == DirUtility::executableDir() + "/theme/default_artwork.png")
            iconPath = LauncherApp::iconPathForName("track");

        ScrollItem *trackItem = new ScrollItem(trackInfo.title, iconPath, 48, this);
        _c2_trackListBox->addItem(trackItem);
        trackItem->sigClicked.connect([this, trackInfo](std::string) {
            _tbToggleLibrary->click();
            SoundManager::instance().playFromMedia(trackInfo);
        });
    }
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

/***********************************/
/***** CustomListBox Class *********/
/***********************************/

CustomListBox::CustomListBox(Widget *parent): ListBox(parent)
{
}

Size CustomListBox::preferredSize() const
{
    return Size(BaseActivity::availableArea().width() * 0.6, BaseActivity::availableArea().height()-30*2);
}

/***********************************/
/***** ScrollItem Class ************/
/***********************************/

ScrollItem::ScrollItem(const std::string& label, const std::string &iconPath, const int iconSize, Widget *parent): ContainerWidget(parent)
{
    setConstraints(NoConstraint, FixedConstraint);
    setInputMethod(PointerInput);

    _icon = new Icon(iconPath, this);
    _icon->setMaximumSize(iconSize, iconSize);
    _icon->setMinimumSize(iconSize, iconSize);
    _iconSize = iconSize;
    _label = new Label(" " + label, this);
    BaseActivity::setWidgetFont(_label, _iconSize / 16);

    HBoxLayout *layout = new HBoxLayout(this);
    setLayout(layout);
    layout->setVerticalAlignment(Alignment::Vertical::Middle);

    addWidget(_icon);
    addWidget(_label);
}

Size ScrollItem::preferredSize() const
{
    return Size(200, _iconSize + 4);
}

void ScrollItem::compose(const PaintEvent &event)
{
    (void)event;
}

bool ScrollItem::consumePointerEvent(const PointerEvent &pointerEvent)
{
    if(frameGeometry().contains(pointerEvent.x, pointerEvent.y, true)
       && pointerEvent.button == PointerButton::ButtonLeft)
    {
        if(pointerEvent.eventType == PointerButtonDown)
            _pressed = true;
        else if(pointerEvent.eventType == PointerButtonUp && _pressed)
        {
            _pressed = false;
            sigClicked(_label->text());
        }
        return true;
    }
    _pressed = false;

    return false;
}

