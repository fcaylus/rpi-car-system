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
        FlatButton *_tbToggleLibrary;

        //
        // "Playing" widgets
        //

        ContainerWidget *_container1;

        CoverIcon *_c1_coverWidget;
        ilixi::Label *_c1_titleLabel;
        ilixi::Label *_c1_artistLabel;

        ilixi::ProgressBar *_c1_musicProgressBar;
        ilixi::Label *_c1_musicTimeLabel;

        ilixi::ProgressBar *_c1_volumeBar;
        FlatButton *_c1_volumePlus;
        FlatButton *_c1_volumeMinus;

        FlatButton *_c1_musicForward;
        FlatButton *_c1_musicRewind;
        ilixi::ToolButton *_c1_queueButton;

        int64_t _musicLength;
        std::string _musicLengthStr;

        //
        // "Library" widgets
        //

        ContainerWidget *_container2;


};

class CoverIcon : public ilixi::Icon
{
    public:
        CoverIcon(ilixi::Widget* parent = 0);
        ilixi::Size preferredSize() const;
};

#endif // MUSICACTIVITY_H
