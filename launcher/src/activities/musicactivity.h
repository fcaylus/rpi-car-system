#ifndef MUSICACTIVITY_H
#define MUSICACTIVITY_H

#include "baseactivity.h"

#include "../widgets/flatbutton.h"
#include "../soundmanager.h"

#include <ui/Label.h>
#include <ui/ProgressBar.h>
#include <ui/ButtonGroup.h>
#include <ui/DirectionalButton.h>
#include <ui/ListBox.h>

#include <sigc++/connection.h>

class CoverIcon;
class CustomListBox;

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

        ilixi::ButtonGroup *_c2_chooseModeButton;
        ilixi::DirectionalButton *_c2_modeArtist;
        ilixi::DirectionalButton *_c2_modeAlbum;
        ilixi::DirectionalButton *_c2_modeTrack;

        ContainerWidget *_c2_artistContainer;
        ContainerWidget *_c2_albumContainer;
        ContainerWidget *_c2_trackContainer;

        // Artists widgets

        CustomListBox *_c2_artistListBox;
        ilixi::Label *_c2_artistLabel;
        std::string _c2_artistLabelPreviousText = "";
        ilixi::ToolButton *_c2_artistBackButton;
        sigc::connection _c2_artistBackButtonConnection;

        // Albums widgets

        CustomListBox *_c2_albumListBox;
        ilixi::Label *_c2_albumLabel;
        ilixi::ToolButton *_c2_albumBackButton;
        sigc::connection _c2_albumBackButtonConnection;

        // Tracks widgets

        CustomListBox *_c2_trackListBox;
        ilixi::Label *_c2_trackLabel;
        ilixi::ToolButton *_c2_trackBackButton;

        // Private methods
        void updateArtistWidgets(SortedMediaMap artistMap);
        void updateAlbumWidgets(MediaMap albumMap);
        void updateTrackWidgets(MediaList mediaList);
};

class CoverIcon : public ilixi::Icon
{
    public:
        CoverIcon(ilixi::Widget* parent = 0);
        ilixi::Size preferredSize() const;
};

class CustomListBox : public ilixi::ListBox
{
    public:
        CustomListBox(ilixi::Widget* parent = 0);
        ilixi::Size preferredSize() const;
};

class ScrollItem : public ContainerWidget
{
    public:
        ScrollItem(const std::string& label, const std::string& iconPath, const int iconSize, ilixi::Widget* parent = 0);
        ilixi::Size preferredSize() const;

        sigc::signal<void, std::string> sigClicked;

    protected:
        virtual void compose(const ilixi::PaintEvent& event);
        virtual bool consumePointerEvent(const ilixi::PointerEvent &pointerEvent);

    private:

        ilixi::Icon *_icon;
        ilixi::Label *_label;

        bool _pressed = false;

        int _iconSize;

};

#endif // MUSICACTIVITY_H
