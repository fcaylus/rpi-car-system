#include "musicactivity.h"

#include "../launcherapp.h"

using namespace ilixi;

MusicActivity::MusicActivity(Widget *parent): BaseActivity("act_music", parent)
{

    // ---------------------------------------
    // Set the tool bar widget
    // ---------------------------------------

    _toolBarContainer = new ContainerWidget(this);
    HBoxLayout *tbLayout = new HBoxLayout(this);
    tbLayout->setVerticalAlignment(Alignment::Middle);
    _toolBarContainer->setLayout(tbLayout);

    _tbPlayButton = new FlatButton("play", this);
    _tbPlayButton->addAdditionnalState("pause");

    _tbNextButton = new FlatButton("skip_next", this);

    _tbPreviousButton = new FlatButton("skip_previous", this);

    _tbRepeatButton = new FlatButton("repeat", this);
    _tbRepeatButton->setCheckable(true);
    _tbRepeatButton->addAdditionnalState("repeat_one");

    _tbShuffleButton = new FlatButton("shuffle", this);
    _tbShuffleButton->setCheckable(true);

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
}

