#ifndef MUSICACTIVITY_H
#define MUSICACTIVITY_H

#include "baseactivity.h"

#include "../widgets/flatbutton.h"

class CoverIcon;

/*
 * This class launch a music activity
 */
class MusicActivity : public BaseActivity
{
    public:
        MusicActivity(ilixi::Widget* parent = 0);

    private:

        //
        // ToolBar widgets
        //

        ContainerWidget *_toolBarContainer;
        FlatButton *_tbPlayButton;
        FlatButton *_tbNextButton;
        FlatButton *_tbPreviousButton;
        FlatButton *_tbRepeatButton;
        FlatButton *_tbShuffleButton;

        CoverIcon *_coverWidget;
        ilixi::Label *_titleLabel;
        ilixi::Label *_artistLabel;

        ilixi::ProgressBar *_musicProgressBar;
        ilixi::Label *_musicTimeLabel;
        int64_t _musicLength;
        std::string _musicLengthStr;

        ilixi::ProgressBar *_volumeBar;
        FlatButton *_volumePlus;
        FlatButton *_volumeMinus;

        FlatButton *_musicForward;
        FlatButton *_musicRewind;
        ilixi::ToolButton *_queueButton;
};

class CoverIcon : public ilixi::Icon
{
    public:
        CoverIcon(ilixi::Widget* parent = 0);
        ilixi::Size preferredSize() const;
};

#endif // MUSICACTIVITY_H
