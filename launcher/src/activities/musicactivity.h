#ifndef MUSICACTIVITY_H
#define MUSICACTIVITY_H

#include "baseactivity.h"

#include "../widgets/flatbutton.h"

/*
 * This class launch a music activity
 */
class MusicActivity : public BaseActivity
{
    public:
        MusicActivity(ilixi::Widget* parent = 0);

    private:

        ContainerWidget *_toolBarContainer;
        FlatButton *_tbPlayButton;
        FlatButton *_tbNextButton;
        FlatButton *_tbPreviousButton;
        FlatButton *_tbRepeatButton;
        FlatButton *_tbShuffleButton;
};

#endif // MUSICACTIVITY_H
