/*****************************************************************************
 * main_interface.hpp : Main Interface
 ****************************************************************************
 * Copyright (C) 2006-2010 VideoLAN and AUTHORS
 * $Id$
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef QVLC_MAIN_INTERFACE_H_
#define QVLC_MAIN_INTERFACE_H_

#include "qt4.hpp"

#include "util/qvlcframe.hpp"

#ifdef _WIN32
 #include <vlc_windows_interfaces.h>
#endif

#include <QSystemTrayIcon>
#include <QStackedWidget>

class QSettings;
class QCloseEvent;
class QKeyEvent;
class QLabel;
class QEvent;
class VideoWidget;
class BackgroundWidget;
class PlaylistWidget;
class VisualSelector;
class ControlsWidget;
class InputControlsWidget;
class FullscreenControllerWidget;
class QVBoxLayout;
class QMenu;
class QSize;
class QTimer;
class StandardPLPanel;
struct vout_window_t;

class MainInterface : public QVLCMW
{
    Q_OBJECT

    friend class PlaylistWidget;

public:
    /* tors */
    MainInterface( intf_thread_t *);
    virtual ~MainInterface();

    static const QEvent::Type ToolbarsNeedRebuild;

    /* Video requests from core */
    WId  getVideo( struct vout_window_t *,
                   unsigned int *pi_width, unsigned int *pi_height );
    void releaseVideo( void );
    int  controlVideo( int i_query, va_list args );

    /* Getters */
    QSystemTrayIcon *getSysTray() { return sysTray; }
    QMenu *getSysTrayMenu() { return systrayMenu; }
    FullscreenControllerWidget* getFullscreenControllerWidget() { return fullscreenControls; }
    enum
    {
        CONTROLS_VISIBLE  = 0x1,
        CONTROLS_HIDDEN   = 0x2,
        CONTROLS_ADVANCED = 0x4,
    };
    int getControlsVisibilityStatus();
    bool isPlDocked() { return ( b_plDocked != false ); }
    bool isInterfaceFullScreen() { return b_interfaceFullScreen; }
    StandardPLPanel* getPlaylistView();

protected:
    void dropEventPlay( QDropEvent* event, bool b_play ) { dropEventPlay(event, b_play, true); }
    void dropEventPlay( QDropEvent *, bool, bool );
#ifdef _WIN32
    virtual bool winEvent( MSG *, long * );
#endif
    void changeEvent( QEvent * ) Q_DECL_OVERRIDE;
    void dropEvent( QDropEvent *) Q_DECL_OVERRIDE;
    void dragEnterEvent( QDragEnterEvent * ) Q_DECL_OVERRIDE;
    void dragMoveEvent( QDragMoveEvent * ) Q_DECL_OVERRIDE;
    void dragLeaveEvent( QDragLeaveEvent * ) Q_DECL_OVERRIDE;
    void closeEvent( QCloseEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent( QKeyEvent *) Q_DECL_OVERRIDE;
    void wheelEvent( QWheelEvent * ) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;

private:
    /* Main Widgets Creation */
    void createMainWidget( QSettings* );
    void createStatusBar();
    void createPlaylist();
    void createResumePanel( QWidget *w );

    /* Systray */
    void createSystray();
    void initSystray();
    void handleSystray();

    /* Central StackWidget Management */
    void showTab( QWidget *);
    void showVideo();
    void restoreStackOldWidget();

    /* */
    void setMinimalView( bool );
    void setInterfaceFullScreen( bool );
    void computeMinimumSize();

    /* */
    QSettings           *settings;
    QSystemTrayIcon     *sysTray;
    QMenu               *systrayMenu;

    QString              input_name;
    QVBoxLayout         *mainLayout;
    ControlsWidget      *controls;
    InputControlsWidget *inputC;
    FullscreenControllerWidget *fullscreenControls;

    /* Widgets */
    QStackedWidget      *stackCentralW;

    VideoWidget         *videoWidget;
    BackgroundWidget    *bgWidget;
    PlaylistWidget      *playlistWidget;
    //VisualSelector      *visualSelector;

    /* resume panel */
    QWidget             *resumePanel;
    QTimer              *resumeTimer;
    int64_t             i_resumeTime;

    /* Status Bar */
    QLabel              *nameLabel;
    QLabel              *cryptedLabel;

    /* Status and flags */
    QWidget             *stackCentralOldWidget;

    QMap<QWidget *, QSize> stackWidgetsSizes;

    /* Flags */
    unsigned             i_notificationSetting; /// Systray Notifications
    bool                 b_autoresize;          ///< persistent resizable window
    bool                 b_videoEmbedded;       ///< Want an external Video Window
    bool                 b_videoFullScreen;     ///< --fullscreen
    bool                 b_hideAfterCreation;
    bool                 b_minimalView;         ///< Minimal video
    bool                 b_interfaceFullScreen;
    bool                 b_pauseOnMinimize;

    /* States */
    bool                 playlistVisible;       ///< Is the playlist visible ?
//    bool                 videoIsActive;       ///< Having a video now / THEMIM->hasV
//    bool                 b_visualSelectorEnabled;
    bool                 b_plDocked;            ///< Is the playlist docked ?

    bool                 b_hasPausedWhenMinimized;
    bool                 b_statusbarVisible;
    Qt::WindowStates     stateBeforeFullScreen;  ///< Windows state before fullscreen

#ifdef _WIN32
    HWND WinId( QWidget *);
    HIMAGELIST himl;
    ITaskbarList3 *p_taskbl;
    UINT taskbar_wmsg;
    void createTaskBarButtons();
#endif

    static const Qt::Key kc[10]; /* easter eggs */
    int i_kc_offset;

public slots:
    void dockPlaylist( bool b_docked = true );
    void toggleMinimalView( bool );
    void togglePlaylist();
    void toggleUpdateSystrayMenu();
    void showUpdateSystrayMenu();
    void hideUpdateSystrayMenu();
    void toggleAdvancedButtons();
    void toggleInterfaceFullScreen();
    void toggleFSC();

    void setStatusBarVisibility(bool b_visible);
    void setPlaylistVisibility(bool b_visible);

#ifdef _WIN32
    void changeThumbbarButtons( int );
#endif

    /* Manage the Video Functions from the vout threads */
    void getVideoSlot( WId *p_id, struct vout_window_t *,
                       unsigned *pi_width, unsigned *pi_height );
    void releaseVideoSlot( void );

    void emitBoss();
    void emitRaise();

    void reloadPrefs();
    void toolBarConfUpdated();

private slots:
    void debug();
    void recreateToolbars();
    void setName( const QString& );
    void setVLCWindowsTitle( const QString& title = "" );
#if 0
    void visual();
#endif
    void handleSystrayClick( QSystemTrayIcon::ActivationReason );
    void updateSystrayTooltipName( const QString& );
    void updateSystrayTooltipStatus( int );
    void showCryptedLabel( bool );

    void handleKeyPress( QKeyEvent * );

    void showBuffering( float );

    void resizeStack( int w, int h )
    {
        if( !isFullScreen() && !isMaximized() )
        {
            if( b_minimalView )
                resize( w, h ); /* Oh yes, it shouldn't
                                   be possible that size() - stackCentralW->size() < 0
                                   since stackCentralW is contained in the QMW... */
            else
                resize( size() - stackCentralW->size() + QSize( w, h ) );
        }
        debug();
    }

    void setVideoSize( unsigned int, unsigned int );
    void videoSizeChanged( int, int );
    void setVideoFullScreen( bool );
    void setVideoOnTop( bool );
    void setBoss();
    void setRaise();

    void showResumePanel( int64_t);
    void hideResumePanel();
    void resumePlayback();

signals:
    void askGetVideo( WId *, struct vout_window_t *, unsigned *, unsigned * );
    void askReleaseVideo( );
    void askVideoToResize( unsigned int, unsigned int );
    void askVideoSetFullScreen( bool );
    void askVideoOnTop( bool );
    void minimalViewToggled( bool );
    void fullscreenInterfaceToggled( bool );
    void askToQuit();
    void askBoss();
    void askRaise();
    void kc_pressed(); /* easter eggs */
};

#endif
