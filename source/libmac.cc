/*
 *  File:       libmac.cc
 *  Summary:    Mac specific routines used by Crawl.
 *  Written by: Jesse Jones (jesjones@halcyon.com)
 *
 *  Change History (most recent first):
 *
 *      <4>     9/25/99         CDL             linuxlib -> liblinux
 *
 *      <3>     5/30/99         JDJ             Quit only pops up save changes dialog if game_has_started is true.
 *      <2>     4/24/99         JDJ             HandleMenu calls save_game instead of returning a 'S'
 *                                                                      character ('S' now prompts). kTermHeight is now initialized
 *                                                                      to NUMBER_OF_LINES.
 *      <2>     4/24/99         JDJ             getstr only adds printable characters to the buffer.
 *              <1>     3/23/99         JDJ             Created
 */

#include "AppHdr.h"
#include "libmac.h"

#include <cstdarg>
#include <ctype.h>
#include <path2fss.h>
#include <string.h>
#include <vector.h>

#include <Appearance.h>
#include <Sioux.h>
#include <Sound.h>

#include "debug.h"
#include "defines.h"
#include "files.h"


#if macintosh

//-----------------------------------
//      Forward References
//
class CWindow;


//-----------------------------------
//      Constants
//
const int kAppleMenu = 256;
const int kFileMenu = 257;
const int kFontMenu = 258;
const int kSizeMenu = 259;

const int kTermWidth = 80;
const int kTermHeight = NUMBER_OF_LINES;

const int kLeftArrowKey = 0x7B;
const int kUpArrowKey = 0x7E;
const int kRightArrowKey = 0x7C;
const int kDownArrowKey = 0x7D;

const int kNumPad0Key = 0x52;
const int kNumPad1Key = 0x53;
const int kNumPad2Key = 0x54;
const int kNumPad3Key = 0x55;
const int kNumPad4Key = 0x56;
const int kNumPad5Key = 0x57;
const int kNumPad6Key = 0x58;
const int kNumPad7Key = 0x59;
const int kNumPad8Key = 0x5B;
const int kNumPad9Key = 0x5C;

const int kNumPadMultKey = 0x43;
const int kNumPadAddKey = 0x45;
const int kNumPadSubKey = 0x4E;
const int kNumPadDecimalKey = 0x41;
const int kNumPadDivideKey = 0x4B;

const char kEnterChar = 0x03;
const short kEscapeKey = 0x35;
const char kCheckMarkChar = 0x12;

const short kSaveBtn = 1;
const short kCancelBtn = 2;
const short kDontSaveBtn = 3;

enum EAskSaveResult
{
    kSaveChanges = 1,
    kCancelSave = 2,
    kDontSave = 3
};


//-----------------------------------
//      Variables
//
static bool sHasAM = false;

static MenuHandle sAppleMenu = NULL;
static MenuHandle sFileMenu = NULL;
static MenuHandle sFontMenu = NULL;
static MenuHandle sSizeMenu = NULL;

static CWindow *sWindow = NULL;
static CTabHandle sColors = NULL;

extern bool game_has_started;


// ========================================================================
//      class CWindow
// ========================================================================
class CWindow
{

//-----------------------------------
    //      Initialization/Destruction
    //
    public:
    ~CWindow();

    CWindow();

    private:
    CWindow(const CWindow & rhs);

      CWindow & operator = (const CWindow & rhs);

//-----------------------------------
    //      API
    //
      public:
    void Show(bool show = true);

    void HandleUpdate();
    void HandleActivate(const EventRecord & event);
    void Invalidate();

    void Clear();
    void SetCursor(int x, int y);
    Point GetCursor() const
    {
        return mCursor;
    }
    void SetChar(unsigned char ch);
    void Print(const char *buffer);
    void ShowCursor(bool show)
    {
        mShowCursor = show;
    }

    void SetForeColor(const RGBColor & color);
    void SetBackColor(const RGBColor & color);

    void SetFont(const unsigned char *name)
    {
        this->DoSetFont(name, mPointSize);
    }
    void SetFontSize(int size)
    {
        this->DoSetFont(mFontName, size);
    }

//-----------------------------------
    //      Internal Types
    //
    protected:
    struct SCell
    {
        unsigned char ch;
        RGBColor color;

          SCell()
        {
            ch = ' ';
            color.red = color.green = color.blue = 65535;
        }
    };

    typedef vector < SCell > Line;

//-----------------------------------
    //      Internal API
    //
  protected:
    void DoScroll();

    void DoClearToEOL();

    void DoSetChar(unsigned char ch);

    void DoSetFont(const unsigned char *name, int size);

    void DoDrawCell(const Rect & area, const SCell & cell);

    void DoWritePrefs();

    bool DoReadPrefs();

//-----------------------------------
    //      Member Data
    //
  protected:
    vector < Line > mLines;
    Point mCursor;
    bool mShowCursor;

    int mCellWidth;
    int mCellHeight;

    Str255 mFontName;
    short mFontNum;
    int mPointSize;
    int mAscent;

    RGBColor mForeColor;
    RGBColor mBackColor;

    WindowRef mWindow;
    GWorldPtr mPixMap;
};


// ========================================================================
//      Internal Functions
// ========================================================================

inline void CopyColorBits(CGrafPtr srcBits, CGrafPtr dstBits, const Rect * srcRect,
                          const Rect * dstRect, int mode, RgnHandle maskRgn)
{
    CopyBits((BitMap *) & (srcBits->portPixMap), (BitMap *) & (dstBits->portPixMap), srcRect, dstRect, mode, maskRgn);
}

//---------------------------------------------------------------
//
// TestGestaltMask
//
//---------------------------------------------------------------
static bool TestGestaltMask(unsigned long selector, long bitmap)
{
    long result;
    OSErr err = Gestalt(selector, &result);

    bool has = err == noErr && (result & bitmap) == bitmap;

    return has;
}


//---------------------------------------------------------------
//
// GetControl
//
//---------------------------------------------------------------
static ControlHandle GetControl(DialogPtr dialog, short item)
{
    ASSERT(dialog != nil);
    ASSERT(item >= 1 && item <= CountDITL(dialog));

    short itemType;
    Handle itemHandle;
    Rect itemRect;

    GetDialogItem(dialog, item, &itemType, &itemHandle, &itemRect);
    if (itemType > itemDisable)
        itemType -= itemDisable;
    ASSERT(itemType >= ctrlItem + btnCtrl && itemType <= ctrlItem + resCtrl);

    return reinterpret_cast < ControlHandle > (itemHandle);
}


//---------------------------------------------------------------
//
// FlashButton
//
//---------------------------------------------------------------
static void FlashButton(DialogPtr dialog, short item)
{
    ASSERT(dialog != nil);
    ASSERT(item >= 1 && item <= CountDITL(dialog));

    unsigned long ticks;

    ControlHandle control = GetControl(dialog, item);

    HiliteControl(control, kControlEntireControl);
    Delay(8, &ticks);
    HiliteControl(control, 0);
}


//---------------------------------------------------------------
//
// ExtractChar
//
// Returns the ASCII character corresponding to the keyDown or
// keyUp event. This function can be used to find the character
// that was pressed with the option key or to find out if the
// shift key was held down with the command key. Note that this
// code is adapted from MacApp's TDispatcher::KeyEventToComponents.
//
//---------------------------------------------------------------
static char ExtractChar(const EventRecord & event)
{
    ASSERT(event.what == keyDown || event.what == keyUp || event.what == autoKey);

    const int kMaskModifier = 0xF600;   // strip command and options keys from Modifiers

    const int kMaskASCII1 = 0x000000FF;         // get key from KeyTranslate return

    const int kMaskASCII2 = 0x00FF0000;         // get key from KeyTranslate return

    const short kUpKeyMask = 0x0080;

    // Now see if the command key is down. If it is, get the correct ASCII translation by
    // masking the command key out and re-translating because the command key will
    // mask the shift modifier.

    char theChar = (char) (event.message & charCodeMask);
    unsigned short theKey = (unsigned short) ((event.message & keyCodeMask) >> 8);

    if ((event.modifiers & cmdKey) != 0 || (event.modifiers & optionKey) != 0)
    {
        // Set the upkey bit so KeyTranslate doesn't do special deadkey processing.
        // See IM-V pp. 195
        unsigned short keyCodeParameter = (unsigned short) ((event.modifiers & kMaskModifier) | theKey | kUpKeyMask);

        Ptr keyTransTable = (Ptr) (GetScriptManagerVariable(smKCHRCache));

        unsigned long state = 0;
        unsigned long keyInfo = KeyTranslate(keyTransTable, keyCodeParameter, &state);

        theChar = (char) (keyInfo & kMaskASCII1);
        if (theChar == 0)
            theChar = (char) ((keyInfo & kMaskASCII2) & 16);
    }

    return theChar;
}


//---------------------------------------------------------------
//
// IsCommandPeriod
//
// Non-US keyboards sometimes use option key to generate period
// so we'll strip off all the modifier keys before checking.
//
//---------------------------------------------------------------
static bool IsCommandPeriod(const EventRecord & event)
{
    bool cmdPeriod = false;

    if (event.what == keyDown)
        if ((event.modifiers & cmdKey) != 0 && ExtractChar(event) == '.')
            cmdPeriod = true;

    return cmdPeriod;
}


//---------------------------------------------------------------
//
// WarnUserFilter
//
//---------------------------------------------------------------
static int HandleEvent(const EventRecord & event);
static pascal Boolean WarnUserFilter(DialogPtr dptr, EventRecord * event, short *item)
{
    bool handled = false;

    if (event->what == keyDown)
    {
        char ch = (char) toupper((char) (event->message & charCodeMask));
        short key = (short) ((event->message & keyCodeMask) >> 8);

        if (ch == '\r' || ch == kEnterChar)
        {
            *item = 1;
            handled = true;
        }

        if (handled)
            FlashButton(dptr, *item);

    }
    else if (event->what == updateEvt || event->what == activateEvt)
    {
        GrafPtr oldPort;

        GetPort(&oldPort);

        (void) HandleEvent(*event);

        SetPort(oldPort);
    }

    return handled;
}


//---------------------------------------------------------------
//
// WarnUser
//
// Pops up an error dialog.
//
//---------------------------------------------------------------
static void WarnUser(const char *message)
{
    int len = strlen(message);

    if (len > 250)
        len = 250;

    Str255 text;

    for (int i = 0; i < len; i++)
        text[i + 1] = message[i];
    text[0] = len;

    if (sHasAM)
    {
#if GENERATINGCFM
        RoutineDescriptor desc = BUILD_ROUTINE_DESCRIPTOR(uppModalFilterProcInfo, WarnUserFilter);
        ModalFilterUPP theFilter = (ModalFilterUPP) & desc;

#else
        ModalFilterUPP theFilter = WarnUserFilter;

#endif

        OSErr err = noErr;
        short item;
        AlertStdAlertParamRec params;

        params.movable = true;
        params.helpButton = false;
        params.filterProc = theFilter;
        params.defaultText = (StringPtr) - 1L;  // use default (ie "OK")

        params.cancelText = nil;
        params.otherText = nil;
        params.defaultButton = 1;
        params.cancelButton = 0;
        params.position = kWindowAlertPositionParentWindowScreen;

        err = StandardAlert(kAlertCautionAlert, text, "\p", &params, &item);
        ASSERT(err == noErr);   // seems kind of pointless to throw

    }
    else
    {
        ParamText(text, "\p", "\p", "\p");

        Alert(129, 0L);
    }
}


//---------------------------------------------------------------
//
// SaveChangesFilter
//
//---------------------------------------------------------------
static pascal Boolean SaveChangesFilter(DialogPtr dptr, EventRecord * event, short *item)
{
    bool handled = false;

    if (event->what == keyDown)
    {
        char ch = (char) toupper((char) (event->message & charCodeMask));
        short key = (short) ((event->message & keyCodeMask) >> 8);

        if (ch == 'S')
        {
            *item = 1;
            handled = true;

        }
        else if (ch == 'D')
        {
            *item = 3;
            handled = true;

        }
        else if (ch == '\r' || ch == kEnterChar)
        {
            *item = 1;
            handled = true;

        }
        else if (key == kEscapeKey)
        {
            *item = 2;
            handled = true;

        }
        else if (IsCommandPeriod(*event))
        {
            *item = 2;
            handled = true;
        }

        if (handled)
            FlashButton(dptr, *item);

    }
    else if (event->what == updateEvt || event->what == activateEvt)
    {
        GrafPtr oldPort;

        GetPort(&oldPort);

        (void) HandleEvent(*event);

        SetPort(oldPort);
    }

    return handled;
}


//---------------------------------------------------------------
//
// AskSaveChanges
//
//---------------------------------------------------------------
static EAskSaveResult AskSaveChanges()
{
    InitCursor();

    OSErr err = noErr;
    EAskSaveResult result = kCancelSave;

    short item = kSaveBtn;

    if (sHasAM)
    {
#if GENERATINGCFM
        RoutineDescriptor desc = BUILD_ROUTINE_DESCRIPTOR(uppModalFilterProcInfo, SaveChangesFilter);
        ModalFilterUPP theFilter = (ModalFilterUPP) & desc;

#else
        ModalFilterUPP theFilter = SaveChangesFilter;

#endif

        AlertStdAlertParamRec params;

        params.movable = true;
        params.helpButton = false;
        params.filterProc = theFilter;
        params.defaultText = "\pSave";
        params.cancelText = "\pCancel";
        params.otherText = "\pDon't Save";
        params.defaultButton = kSaveBtn;
        params.cancelButton = kCancelBtn;
        params.position = kWindowAlertPositionParentWindowScreen;

        err = StandardAlert(kAlertStopAlert, "\pDo you want to save your game before quitting?", "\p", &params, &item);
        ASSERT(err == noErr);   // seems kind of pointless to throw

    }
    else
    {
        ParamText("\pDo you want to save your game before quitting?", "\p", "\p", "\p");

        item = CautionAlert(131, NULL);
    }

    if (item == kSaveBtn)
        result = kSaveChanges;
    else if (item == kDontSaveBtn)
        result = kDontSave;

    return result;
}


//---------------------------------------------------------------
//
// HandleUpdateMenus
//
//---------------------------------------------------------------
static void HandleUpdateMenus(const unsigned char *fontName, short fontNum, int pointSize)
{
    ASSERT(fontName != NULL);

    Str255 name;

    short numItems = CountMItems(sFontMenu);

    for (short index = 1; index <= numItems; ++index)
    {
        GetMenuItemText(sFontMenu, index, name);

        if (EqualString(fontName, name, true, true))
            SetItemMark(sFontMenu, index, kCheckMarkChar);
        else
            SetItemMark(sFontMenu, index, noMark);
    }

    numItems = CountMItems(sSizeMenu);
    for (short index = 1; index <= numItems; ++index)
    {
        GetMenuItemText(sSizeMenu, index, name);

        long size;

        StringToNum(name, &size);

        if (size == pointSize)
            SetItemMark(sSizeMenu, index, kCheckMarkChar);
        else
            SetItemMark(sSizeMenu, index, noMark);

        if (RealFont(fontNum, size))
            SetItemStyle(sSizeMenu, index, outline);
        else
            SetItemStyle(sSizeMenu, index, normal);
    }
}


//---------------------------------------------------------------
//
// HandleMenu
//
//---------------------------------------------------------------
static int HandleMenu(int menuID, int item)
{
    int ch = '\0';

    switch (menuID)
    {
    case kAppleMenu:
        if (item == 1)
            (void) Alert(256, NULL);

        else
        {
            Str255 daName;

            GetMenuItemText(sAppleMenu, item, daName);
            OpenDeskAcc(daName);
        }
        break;

    case kFileMenu:
        if (item == 1)
            save_game(false);

        else if (item == 3)
        {
            if (game_has_started)
            {
                EAskSaveResult answer = AskSaveChanges();

                if (answer == kSaveChanges)
                {
                    save_game(true);
                }
                else if (answer == kDontSave)
                {
                    deinit_mac();
                    ExitToShell();
                }
            }
            else
            {
                deinit_mac();
                ExitToShell();
            }
        }
        break;

    case kFontMenu:
        {
            Str255 fontName;

            GetMenuItemText(sFontMenu, item, fontName);
            sWindow->SetFont(fontName);
        }
        break;

    case kSizeMenu:
        {
            Str255 sizeName;

            GetMenuItemText(sSizeMenu, item, sizeName);

            long size;

            StringToNum(sizeName, &size);

            sWindow->SetFontSize(size);
        }
        break;

    default:
        ASSERT(false);
    }

    return ch;
}


//---------------------------------------------------------------
//
// HandleMouseDown
//
//---------------------------------------------------------------
static int HandleMouseDown(const EventRecord & event)
{
    int ch = '\0';

    WindowRef wptr;
    int partCode = ::FindWindow(event.where, &wptr);

    switch (partCode)
    {
    case inMenuBar:
        {
            int result = MenuSelect(event.where);
            int menuID = HiWord(result);
            int item = LoWord(result);

            if (menuID != 0)
                ch = HandleMenu(menuID, item);
            HiliteMenu(0);
        }
        break;

    case inSysWindow:
        ::SystemClick(&event, wptr);
        break;

    case inDesk:
        ::SysBeep(1);
        break;

    case inContent:
        // fall thru

    case inDrag:
#if DEBUG
        if (!SIOUXHandleOneEvent(const_cast < EventRecord * >(&event)))
#endif
        {
            Rect dragRect = (**(GetGrayRgn())).rgnBBox;

            InsetRect(&dragRect, 4, 4);
            ::DragWindow(wptr, event.where, &dragRect);
        }
        break;

    case inGoAway:
    case inGrow:
    case inZoomIn:
    case inZoomOut:
#if DEBUG
        (void) SIOUXHandleOneEvent(const_cast < EventRecord * >(&event));
#endif
        break;

    default:
        ASSERT(false);
    }

    return ch;
}


//---------------------------------------------------------------
//
// HandleEvent
//
//---------------------------------------------------------------
static int HandleEvent(const EventRecord & event)
{
    ASSERT(sWindow != NULL);

    int ch = '\0';

    switch (event.what)
    {
    case mouseDown:
        ch = HandleMouseDown(event);
        break;

    case keyDown:
        if (event.modifiers & cmdKey)
        {
            int c = event.message & charCodeMask;

            int result = MenuKey(c);
            int menuID = HiWord(result);
            int item = LoWord(result);

            if (menuID != 0)
                ch = HandleMenu(menuID, item);
            HiliteMenu(0);
        }
        // fall thru

    case autoKey:
        if ((event.modifiers & cmdKey) == 0)
        {
            int key = (event.message & keyCodeMask) >> 8;

            switch (key)
            {
            case kNumPad1Key:
                if (event.modifiers & shiftKey)
                    ch = 'B';
                else if (event.modifiers & controlKey)
                    ch = 2;
                else
                    ch = 'b';
                break;

            case kNumPad2Key:
            case kDownArrowKey:
                if (event.modifiers & shiftKey)
                    ch = 'J';
                else if (event.modifiers & controlKey)
                    ch = 10;
                else
                    ch = 'j';
                break;

            case kNumPad3Key:
                if (event.modifiers & shiftKey)
                    ch = 'N';
                else if (event.modifiers & controlKey)
                    ch = 14;
                else
                    ch = 'n';
                break;

            case kNumPad4Key:
            case kLeftArrowKey:
                if (event.modifiers & shiftKey)
                    ch = 'H';
                else if (event.modifiers & controlKey)
                    ch = 8;
                else
                    ch = 'h';
                break;

            case kNumPad5Key:
                if (event.modifiers & shiftKey)
                    ch = '5';
                else
                    ch = '.';
                break;

            case kNumPad6Key:
            case kRightArrowKey:
                if (event.modifiers & shiftKey)
                    ch = 'L';
                else if (event.modifiers & controlKey)
                    ch = 12;
                else
                    ch = 'l';
                break;

            case kNumPad7Key:
                if (event.modifiers & shiftKey)
                    ch = 'Y';
                else if (event.modifiers & controlKey)
                    ch = 25;
                else
                    ch = 'y';
                break;

            case kNumPad8Key:
            case kUpArrowKey:
                if (event.modifiers & shiftKey)
                    ch = 'K';
                else if (event.modifiers & controlKey)
                    ch = 11;
                else
                    ch = 'k';
                break;

            case kNumPad9Key:
                if (event.modifiers & shiftKey)
                    ch = 'U';
                else if (event.modifiers & controlKey)
                    ch = 21;
                else
                    ch = 'u';
                break;

            default:
                ch = event.message & charCodeMask;
#if 0
                if (event.modifiers & controlKey)
                {
                    if (ch == 'p')
                        ch = 16;
                    else
                        ch = 0;
                }
#endif
            }
        }
        break;

    case updateEvt:
        sWindow->HandleUpdate();
#if DEBUG
        (void) SIOUXHandleOneEvent(const_cast < EventRecord * >(&event));
#endif
        break;

    case diskEvt:
        if (HiWord(event.message) != noErr)
        {
            Point pt =
            {120, 120};         // System 7 will auto-center dialog

            ::DILoad();
            ::DIBadMount(pt, event.message);
            ::DIUnload();
        }
        break;

    case activateEvt:
        sWindow->HandleActivate(event);
#if DEBUG
        (void) SIOUXHandleOneEvent(const_cast < EventRecord * >(&event));
#endif
        break;

    case mouseUp:
    case keyUp:
    case osEvt:
    case kHighLevelEvent:
        break;

    case nullEvent:
        break;

    default:
        ASSERT(false);
    }

    return ch;
}


//---------------------------------------------------------------
//
// ConvertColor
//
//---------------------------------------------------------------
static RGBColor ConvertColor(int c)
{
    ASSERT(c >= 0);
    ASSERT(c < 16);

    RGBColor color;

    if (c == BLACK)
        color = (**sColors).ctTable[15].rgb;    // QD likes black and white swapped from DOS indices

    else if (c == WHITE)
        color = (**sColors).ctTable[0].rgb;
    else
        color = (**sColors).ctTable[c].rgb;

    return color;
}

#pragma mark -

// ========================================================================
//      class CWindow
// ========================================================================

//---------------------------------------------------------------
//
// CWindow::~CWindow
//
//---------------------------------------------------------------
CWindow::~CWindow()
{
    this->DoWritePrefs();

    DisposeWindow(mWindow);
    DisposeGWorld(mPixMap);
}


//---------------------------------------------------------------
//
// CWindow::CWindow
//
//---------------------------------------------------------------
CWindow::CWindow()
{
    mCursor.h = 0;
    mCursor.v = 0;

    mForeColor.red = 65535;
    mForeColor.green = 65535;
    mForeColor.blue = 65535;

    mBackColor.red = 0;
    mBackColor.green = 0;
    mBackColor.blue = 0;

    mFontName[0] = '\0';
    mFontNum = -2;
    mPointSize = 0;
    mShowCursor = false;

    Rect bounds =
    {48, 16, 49, 17};

    mWindow = NewCWindow(NULL, &bounds, "\pCrawl", false, noGrowDocProc, NULL, false, 0);
    if (mWindow == NULL)
    {
        WarnUser("Couldn't create the main window!");
        ExitToShell();
    }

    mPixMap = NULL;
    OSErr err = NewGWorld(&mPixMap, 4, &bounds, sColors, NULL, 0);

    if (err != noErr)
    {
        WarnUser("Couldn't create the offscreen!");
        ExitToShell();
    }

    if (!this->DoReadPrefs())
        this->DoSetFont("\pMonaco", 12);
    this->Clear();

    this->Show();
}


//---------------------------------------------------------------
//
// CWindow::Show
//
//---------------------------------------------------------------
void CWindow::Show(bool show)
{
    if (show)
        ShowWindow(mWindow);
    else
        HideWindow(mWindow);
}


//---------------------------------------------------------------
//
// CWindow::HandleUpdate
//
//---------------------------------------------------------------
void CWindow::HandleUpdate()
{
    ASSERT(mLines.size() == kTermHeight);

    SetPort(mWindow);
    BeginUpdate(mWindow);

    RgnHandle dirtyRgn = mWindow->visRgn;

    ASSERT(dirtyRgn != NULL);

    PixMapHandle pixels = GetGWorldPixMap(mPixMap);

    (void) LockPixels(pixels);
    SetGWorld(mPixMap, GetMainDevice());

    TextFont(mFontNum);
    TextSize(mPointSize);

    RGBBackColor(&mBackColor);
    EraseRgn(dirtyRgn);

    Rect area;

    RGBColor black =
    {0, 0, 0};
    RGBColor white =
    {65535, 65535, 65535};

    for (int y = 0; y < mLines.size(); ++y)
    {
        area.top = y * mCellHeight;
        area.bottom = area.top + mCellHeight;

        area.left = 0;
        area.right = area.left + mCellWidth;

        const Line & line = mLines[y];

        ASSERT(line.size() == kTermWidth);

        for (int x = 0; x < line.size(); ++x)
        {
            if (RectInRgn(&area, dirtyRgn))
            {
                const SCell & cell = line[x];

                this->DoDrawCell(area, cell);

                if (x == mCursor.h && y == mCursor.v && mShowCursor)
                {
                    ::RGBForeColor(&white);
                    ::MoveTo(area.left + 1, area.top + mAscent);
                    ::Line(area.right - area.left - 2, 0);
                }
            }

            area.left += mCellWidth;
            area.right += mCellWidth;
        }
    }

    SetGWorld((CGrafPtr) mWindow, GetMainDevice());

    RGBForeColor(&black);
    RGBBackColor(&white);

    Rect bounds;

    bounds.left = 0;
    bounds.top = 0;
    bounds.right = mPixMap->portRect.right - mPixMap->portRect.left;
    bounds.bottom = mPixMap->portRect.bottom - mPixMap->portRect.top;
    CopyColorBits(mPixMap, (CGrafPtr) mWindow, &bounds, &bounds, srcCopy, dirtyRgn);

    UnlockPixels(pixels);

    EndUpdate(mWindow);
}


//---------------------------------------------------------------
//
// CWindow::HandleActivate
//
//---------------------------------------------------------------
void CWindow::HandleActivate(const EventRecord & event)
{
    HiliteWindow(mWindow, event.modifiers & activeFlag);
}


//---------------------------------------------------------------
//
// CWindow::Invalidate
//
//---------------------------------------------------------------
void CWindow::Invalidate()
{
    SetPort(mWindow);

    Rect area =
    {0, 0, 16000, 16000};

    InvalRect(&area);
}


//---------------------------------------------------------------
//
// CWindow::Clear
//
//---------------------------------------------------------------
void CWindow::Clear()
{
    RGBColor color;

    color.red = 65535;
    color.green = 65535;
    color.blue = 65535;

    mLines.resize(kTermHeight);

    for (int y = 0; y < mLines.size(); ++y)
    {
        Line & line = mLines[y];
        line.resize(kTermWidth);

        for (int x = 0; x < line.size(); ++x)
        {
            SCell & cell = line[x];

            cell.ch = ' ';
            cell.color = color;
        }
    }

    mCursor.h = 0;
    mCursor.v = 0;

    this->Invalidate();
}


//---------------------------------------------------------------
//
// CWindow::SetCursor
//
//---------------------------------------------------------------
void CWindow::SetCursor(int x, int y)
{
    ASSERT(x >= 0);
    ASSERT(x < kTermWidth);
    ASSERT(y >= 0);
    ASSERT(y < kTermHeight);

    if (x != mCursor.h || y != mCursor.v)
    {
        if (mShowCursor)
        {
            Rect area;

            area.top = mCursor.v * mCellHeight;
            area.bottom = area.top + mCellHeight;
            area.left = mCursor.h * mCellWidth;
            area.right = area.left + mCellWidth;

            SetPort(mWindow);
            InvalRect(&area);
        }

        mCursor.h = x;
        mCursor.v = y;

        if (mShowCursor)
        {
            Rect area;

            area.top = mCursor.v * mCellHeight;
            area.bottom = area.top + mCellHeight;
            area.left = mCursor.h * mCellWidth;
            area.right = area.left + mCellWidth;

            SetPort(mWindow);
            InvalRect(&area);
        }
    }
}


//---------------------------------------------------------------
//
// CWindow::SetChar
//
//---------------------------------------------------------------
void CWindow::SetChar(unsigned char ch)
{
    ASSERT(ch != '\t');
    ASSERT(mLines.size() == kTermHeight);

    const int TABSIZE = 8;

    int x = mCursor.h;          // this is from the ncurses source

    int y = mCursor.v;

    switch (ch)
    {
    case '\t':
        x += (TABSIZE - (x % TABSIZE));

        // Space-fill the tab on the bottom line so that we'll get the
        // "correct" cursor position.
        if (x < kTermWidth)
        {
            char blank = ' ';

            while (mCursor.h < x)
            {
                this->DoSetChar(blank);
            }
            break;

        }
        else
        {
            this->DoClearToEOL();
            if (++y >= kTermHeight)
            {
                x = kTermWidth - 1;
                y--;
                this->DoScroll();
                x = 0;
            }
            else
            {
                x = 0;
            }
        }
        break;

#if 1
    case '\n':
    case '\r':
        this->DoClearToEOL();
        if (++y >= kTermHeight)
        {
            y--;
            this->DoScroll();
        }
        x = 0;
        break;

#else
    case '\n':
        this->DoClearToEOL();
        if (++y >= kTermHeight)
        {
            y--;
            this->DoScroll();
        }
        /* FALLTHRU */

    case '\r':
        x = 0;
        break;
#endif

    case '\b':
        if (x == 0)
            return;
        mCursor.h--;
        this->DoSetChar(' ');
        x--;
        break;

    case 159:
    case 176:
    case 177:
    case 220:
    case 239:
    case 240:
    case 247:
    case 249:
    case 250:
    case 206:
    case 254:
        this->DoSetChar(ch);
        return;
        break;

    default:
        if (ch == '\0')
            ch = ' ';

//                      ASSERT(ch >= ' ');
        //                      ASSERT(ch <= '~');

        if (ch >= ' ' && ch <= '~')
            this->DoSetChar(ch);
        else
            this->DoSetChar('¿');
        return;
    }

    mCursor.h = x;
    mCursor.v = y;
}


//---------------------------------------------------------------
//
// CWindow::Print
//
//---------------------------------------------------------------
void CWindow::Print(const char *buffer)
{
    ASSERT(buffer != NULL);

    const char *p = buffer;

    while (*p != '\0')
    {
        char ch = *p++;

        this->SetChar(ch);
    }
}


//---------------------------------------------------------------
//
// CWindow::SetForeColor
//
//---------------------------------------------------------------
void CWindow::SetForeColor(const RGBColor & color)
{
    if (color.red != mForeColor.red || color.green != mForeColor.green || color.blue != mForeColor.blue)
    {
        mForeColor = color;

        this->Invalidate();
    }
}


//---------------------------------------------------------------
//
// CWindow::SetBackColor
//
//---------------------------------------------------------------
void CWindow::SetBackColor(const RGBColor & color)
{
    if (color.red != mBackColor.red || color.green != mBackColor.green || color.blue != mBackColor.blue)
    {
        mBackColor = color;

        this->Invalidate();
    }
}


//---------------------------------------------------------------
//
// CWindow::DoSetChar
//
//---------------------------------------------------------------
void CWindow::DoSetChar(unsigned char ch)
{
    ASSERT(mCursor.h < kTermWidth);
    ASSERT(mCursor.v < kTermHeight);
    ASSERT(mCursor.h >= 0);
    ASSERT(mCursor.v >= 0);

    int x = mCursor.h;

    Line & line = mLines[mCursor.v];
    ASSERT(line.size() == kTermWidth);

    SCell & cell = line[x++];
    cell.ch = ch;
    cell.color = mForeColor;

    Rect area;

    area.top = mCursor.v * mCellHeight;
    area.bottom = area.top + mCellHeight;
    area.left = mCursor.h * mCellWidth;
    area.right = area.left + mCellWidth;

    SetPort(mWindow);
    InvalRect(&area);

    if (x >= kTermWidth)
    {
        if (++mCursor.v >= kTermHeight)
        {
            mCursor.v = kTermHeight - 1;
            mCursor.h = kTermWidth - 1;
            this->DoScroll();
        }
        x = 0;
    }

    mCursor.h = x;
}


//---------------------------------------------------------------
//
// CWindow::DoClearToEOL
//
//---------------------------------------------------------------
void CWindow::DoClearToEOL()
{
    ASSERT(mCursor.h < kTermWidth);
    ASSERT(mCursor.v < kTermHeight);

    Line & line = mLines[mCursor.v];
    for (int x = mCursor.h; x < kTermWidth; ++x)
    {
        SCell & cell = line[x];
        cell.ch = ' ';
    }

    Rect area;

    area.top = mCursor.v * mCellHeight;
    area.bottom = area.top + mCellHeight;
    area.left = mCursor.h * mCellWidth;
    area.right = 16000;

    SetPort(mWindow);
    InvalRect(&area);
}


//---------------------------------------------------------------
//
// CWindow::DoScroll
//
//---------------------------------------------------------------
void CWindow::DoScroll()
{
    mLines.erase(mLines.begin());

    mLines.push_back(Line());
    mLines.back().resize(kTermWidth);

    this->Invalidate();
}


//---------------------------------------------------------------
//
// CWindow::DoSetFont
//
//---------------------------------------------------------------
void CWindow::DoSetFont(const unsigned char *name, int size)
{
    ASSERT(name != NULL);

    short fontNum;

    GetFNum(name, &fontNum);

    if (fontNum != mFontNum || size != mPointSize)
    {
        BlockMoveData(name, mFontName, name[0] + 1);

        mFontNum = fontNum;
        mPointSize = size;

        SetPort(mWindow);
        TextFont(mFontNum);
        TextSize(mPointSize);

        FontInfo info;

        GetFontInfo(&info);

        mCellHeight = info.ascent + info.descent;
        mAscent = info.ascent;

        short width = StringWidth("\pMMMMM");   // widMax is usually much too wide so we'll compute this ourselves...

        mCellWidth = width / 5 + 1;

        Rect bounds;

        bounds.top = 0;
        bounds.left = 0;
        bounds.bottom = mCellHeight * kTermHeight;
        bounds.right = mCellWidth * kTermWidth;
        SizeWindow(mWindow, bounds.right, bounds.bottom, false);

        GWorldFlags flags;

        flags = UpdateGWorld(&mPixMap, 8, &bounds, NULL, NULL, 0);
        QDErr err = (QDErr) ((flags & gwFlagErr) ? flags : noErr);

        if (err != noErr)
        {
            WarnUser("Not enough memory!");
            ExitToShell();
        }

        if (sFontMenu != NULL)
            HandleUpdateMenus(mFontName, mFontNum, mPointSize);

        this->Invalidate();
    }
}


//---------------------------------------------------------------
//
// CWindow::DoDrawCell
//
//---------------------------------------------------------------
void CWindow::DoDrawCell(const Rect & area, const SCell & cell)
{
    RGBForeColor(&cell.color);

    switch (cell.ch)
    {
    case 159:                   // fountain

        {
            char ch = '¥';

            MoveTo(area.left, area.top + mAscent);
            DrawText(&ch, 0, 1);
        }
        break;

    case 177:                   // wall

    case 176:
        PaintRect(&area);
        break;

    case 247:                   // water/lava

        PaintRect(&area);
        break;

    case 249:                   // floor

    case 250:                   // undiscovered trap?
        //                      FillRect(&area, &qd.gray);

        {
            char ch = '.';

            MoveTo(area.left, area.top + mAscent);
            DrawText(&ch, 0, 1);
        }

#if 0
        for (int y = area.top; y < area.bottom; y += 4)
        {
            for (int x = area.left; x < area.right; x += 4)
            {
              ::MoveTo(x, y);
              ::Line(0, 0);
            }
        }
#endif
        break;

    case 206:
    case 254:                   // door

        {
            Rect temp = area;

            InsetRect(&temp, 2, 2);
            PaintRect(&temp);
        }
        break;

    case 220:                   // altar

        {
            char ch = '∆';

            MoveTo(area.left, area.top + mAscent);
            DrawText(&ch, 0, 1);
        }
        break;

    case 239:                   // staircase to hell

    case 240:                   // branch staircase

        {
            char ch = '≤';

            MoveTo(area.left, area.top + mAscent);
            DrawText(&ch, 0, 1);
        }
        break;

    default:
        MoveTo(area.left, area.top + mAscent);
        DrawText(&cell.ch, 0, 1);
    }
}


//---------------------------------------------------------------
//
// CWindow::DoWritePrefs
//
//---------------------------------------------------------------
void CWindow::DoWritePrefs()
{
    // Find the prefs folder
    short vRefNum;
    long dirID;
    OSErr err = FindFolder(kOnSystemDisk, kPreferencesFolderType, true, &vRefNum, &dirID);

    if (err != noErr)
        goto done;

    // Create a spec for our pref file
    FSSpec spec;

    err = FSMakeFSSpec(vRefNum, dirID, "\pCrawl Prefs", &spec);
    if (err != noErr && err != fnfErr)
        goto done;

    // Open the file
    short refNum = -1;

    err = FSpOpenDF(&spec, fsWrPerm, &refNum);
    if (err == fnfErr)
    {
        err = FSpCreate(&spec, 'Crwl', 'pref', smSystemScript);

        if (err == noErr)
            err = FSpOpenDF(&spec, fsWrPerm, &refNum);
    }
    if (err != noErr)
        goto done;

    // Write our preferences
    long version = 1;           // major version

    long bytes = sizeof(version);

    err = FSWrite(refNum, &bytes, &version);
    if (err != noErr)
        goto fail;

    version = 1;                // minor version

    bytes = sizeof(version);
    err = FSWrite(refNum, &bytes, &version);
    if (err != noErr)
        goto fail;

    bytes = 256;                // font name

    err = FSWrite(refNum, &bytes, mFontName);
    if (err != noErr)
        goto fail;

    bytes = sizeof(mPointSize); // font size

    err = FSWrite(refNum, &bytes, &mPointSize);
    if (err != noErr)
        goto fail;

    Point loc =
    {48, 16};                   // window position

    WindowPeek wpeek = (WindowPeek) mWindow;

    if (wpeek->contRgn != NULL)
    {
        loc.h = (**wpeek->contRgn).rgnBBox.left;
        loc.v = (**wpeek->contRgn).rgnBBox.top;
    }

    bytes = sizeof(loc);
    err = FSWrite(refNum, &bytes, &loc);
    if (err != noErr)
        goto fail;

  fail:
    // Close the file
    err = FSClose(refNum);
    VERIFY(err == noErr);

  done:
    ;
}


//---------------------------------------------------------------
//
// CWindow::DoReadPrefs
//
//---------------------------------------------------------------
bool CWindow::DoReadPrefs()
{
    bool read = false;

    // Find the prefs folder
    short vRefNum;
    long dirID;
    OSErr err = FindFolder(kOnSystemDisk, kPreferencesFolderType, true, &vRefNum, &dirID);

    if (err != noErr)
        goto done;

    // Create a spec for our pref file
    FSSpec spec;

    err = FSMakeFSSpec(vRefNum, dirID, "\pCrawl Prefs", &spec);
    if (err != noErr)
        goto done;

    // Open the file
    short refNum = -1;

    err = FSpOpenDF(&spec, fsRdPerm, &refNum);
    if (err != noErr)
        goto done;

    // Read our preferences
    long version;               // major version (has to match exactly)

    long bytes = sizeof(version);

    err = FSRead(refNum, &bytes, &version);
    if (err != noErr)
        goto fail;
    if (version != 1)
        goto fail;

    bytes = sizeof(version);    // minor version (incremented as new fields are added)

    err = FSRead(refNum, &bytes, &version);
    if (err != noErr)
        goto fail;

    Str255 fontName;

    bytes = 256;                // font name

    err = FSRead(refNum, &bytes, fontName);
    if (err != noErr)
        goto fail;

    int pointSize;

    bytes = sizeof(pointSize);  // font size

    err = FSRead(refNum, &bytes, &pointSize);
    if (err != noErr)
        goto fail;

    Point loc;                  // window position

    bytes = sizeof(loc);
    err = FSRead(refNum, &bytes, &loc);
    if (err != noErr)
        goto fail;

    // Update the window's font
    this->DoSetFont(fontName, pointSize);

    // If both the window's top left and top right points
    // are outside the desktop region then offset the
    // window so that it's visible.
    RgnHandle deskTopRgn = GetGrayRgn();

    Point p2;

    p2.h = loc.h + mCellWidth * kTermWidth;
    p2.v = loc.v;

    if (!PtInRgn(loc, deskTopRgn) && !PtInRgn(p2, deskTopRgn))
    {
        Rect deskTopBounds = (**deskTopRgn).rgnBBox;

        InsetRect(&deskTopBounds, 4, 4);

        if (loc.v > deskTopBounds.bottom - mCellHeight * kTermHeight)
            loc.v = deskTopBounds.bottom - mCellHeight * kTermHeight;

        if (loc.v < deskTopBounds.top + 30)
            loc.v = deskTopBounds.top + 30;

        if (loc.h > deskTopBounds.right - mCellWidth * kTermWidth)
            loc.h = deskTopBounds.right - mCellWidth * kTermWidth;

        if (loc.h < deskTopBounds.left)
            loc.h = deskTopBounds.left;
    }

    // Restore the window to the old location.
  ::MoveWindow(mWindow, loc.h, loc.v, false);

    read = true;

  fail:
    // Close the file
    err = FSClose(refNum);
    VERIFY(err == noErr);

  done:
    return read;
}

#pragma mark -

// ========================================================================
//      Non-ANSI Functions
// ========================================================================

//---------------------------------------------------------------
//
// stricmp
//
// Case insensitive string comparison (code is from MSL which
// is why it looks so dorky).
//
//---------------------------------------------------------------
int stricmp(const char *lhs, const char *rhs)
{
    ASSERT(lhs != NULL);
    ASSERT(rhs != NULL);

    const unsigned char *p1 = (unsigned char *) lhs - 1;
    const unsigned char *p2 = (unsigned char *) rhs - 1;
    unsigned long c1, c2;

    while ((c1 = tolower(*++p1)) == (c2 = tolower(*++p2)))
        if (c1 == '\0')
            return (0);

    return c1 - c2;
}


//---------------------------------------------------------------
//
// strlwr
//
// In place conversion to lower case.
//
//---------------------------------------------------------------
char *strlwr(char *str)
{
    ASSERT(str != NULL);

    for (int i = 0; i < strlen(str); ++i)
        str[i] = tolower(str[i]);

    return str;
}


//---------------------------------------------------------------
//
// itoa
//
// Converts an integer to a string (after liblinux.cc).
//
//---------------------------------------------------------------
void itoa(int value, char *buffer, int radix)
{
    ASSERT(buffer != NULL);
    ASSERT(radix == 10 || radix == 2);

    if (radix == 10)
        sprintf(buffer, "%i", value);

    if (radix == 2)
    {                           /* int to "binary string" */
        unsigned int bitmask = 32768;
        int ctr = 0;
        int startflag = 0;

        while (bitmask)
        {
            if (value & bitmask)
            {
                startflag = 1;
                sprintf(buffer + ctr, "1");

            }
            else
            {
                if (startflag)
                    sprintf(buffer + ctr, "0");
            }

            bitmask = bitmask >> 1;
            if (startflag)
                ctr++;
        }

        if (!startflag)         /* Special case if value == 0 */
            sprintf((buffer + ctr++), "0");
        buffer[ctr] = (char) NULL;
    }
}


//---------------------------------------------------------------
//
// open
//
//---------------------------------------------------------------
int open(const char *path, int openFlags, int permissions, int mysteryFlags)
{
    (void) mysteryFlags;        // not used

    return open(path, openFlags, permissions);
}


//---------------------------------------------------------------
//
// open
//
// These file routines are not ANSI-C. They are however Posix
// routines. Unfortunately Posix leaves the permissions argument
// undefined. Crawl often uses open like this: open(path, S_IWRITE, S_IREAD).
// S_IWRITE and S_IREAD are not mentioned in my Posix book, so
// I'm assuming this is supposed to open the file for read/write
// access (although it looks ill-formed...).
//
//---------------------------------------------------------------
int open(const char *path, int flags, int permissions)
{
    ASSERT(path != NULL);
    ASSERT(flags > 0);
    (void) permissions;         // not used

    int perm;

    if (flags & O_RDONLY)
    {
        ASSERT((flags & O_CREAT) == 0);
        ASSERT((flags & O_TRUNC) == 0);
//              ASSERT((flags & O_BINARY) != 0);

        perm = fsRdPerm;

    }
    else if (flags & O_WRONLY)
    {
//              ASSERT((flags & O_BINARY) != 0);

        perm = fsWrPerm;

    }
    else if ((flags & O_RDWR) || (flags & S_IWRITE))
    {
        perm = fsRdPerm + fsWrPerm;

    }
    else
        ASSERT(false);

    FSSpec spec;
    OSErr err = __path2fss(path, &spec);

    short refNum = -1;

    err = FSpOpenDF(&spec, perm, &refNum);
    if (err == fnfErr && (flags & O_CREAT))
    {
        OSType creator = 'Crwl';
        OSType type = 'CrlF';

        int len = strlen(path);

        if (len > 4 && tolower(path[len - 4]) == '.' && tolower(path[len - 3]) == 't' && tolower(path[len - 2]) == 'x' && tolower(path[len - 1]) == 't')
        {
            creator = 'ttxt';   // TeachText

            type = 'TEXT';
        }

        err = FSpCreate(&spec, creator, type, smSystemScript);

        if (err == noErr)
            err = FSpOpenDF(&spec, perm, &refNum);

    }
    else if (err == noErr)
        if (flags & O_TRUNC)
            err = SetEOF(refNum, 0);

    if (err != noErr)
        refNum = -1;            // FSpOpenDF will set this if err == fnfErr :-(

    return refNum;
}


//---------------------------------------------------------------
//
// close
//
//---------------------------------------------------------------
int close(int desc)
{
    OSErr err = fnfErr;

    if (desc != 0 && desc != -1)
        err = FSClose(desc);

    return err == noErr ? 0 : -1;
}


//---------------------------------------------------------------
//
// read
//
//---------------------------------------------------------------
int read(int desc, void *buffer, unsigned int inBytes)
{
    ASSERT(desc != 0);
    ASSERT(desc != -1);
    ASSERT(buffer != NULL);

    long bytes = inBytes;
    OSErr err = FSRead(desc, &bytes, buffer);

    return err == noErr ? bytes : -1;
}


//---------------------------------------------------------------
//
// write
//
//---------------------------------------------------------------
int write(int desc, const void *buffer, unsigned int inBytes)
{
    ASSERT(desc != 0);
    ASSERT(desc != -1);
    ASSERT(buffer != NULL);

    long bytes = inBytes;
    OSErr err = FSWrite(desc, &bytes, buffer);

    return err == noErr ? bytes : -1;
}


//---------------------------------------------------------------
//
// unlink
//
//---------------------------------------------------------------
int unlink(const char *path)
{
    ASSERT(path != NULL);

    return remove(path);
}

#pragma mark -

// ========================================================================
//      Curses(?) Functions
// ========================================================================

//---------------------------------------------------------------
//
// window
//
//---------------------------------------------------------------
void window(int x, int y, int lx, int ly)
{
    ASSERT(lx == kTermWidth);   // CWindow hardcodes size

    ASSERT(ly == kTermHeight);
    ASSERT(sWindow != nil);

    gotoxy(x, y);

    sWindow->Show();
}


//---------------------------------------------------------------
//
// clrscr
//
//---------------------------------------------------------------
void clrscr()
{
    ASSERT(sWindow != nil);

    sWindow->Clear();
}


//---------------------------------------------------------------
//
// textcolor
//
//---------------------------------------------------------------
void textcolor(int c)
{
    ASSERT(c >= 0);
    ASSERT(c < 16);
    ASSERT(sWindow != nil);

    RGBColor color = ConvertColor(c);

    sWindow->SetForeColor(color);
}


//---------------------------------------------------------------
//
// textbackground
//
//---------------------------------------------------------------
void textbackground(int c)
{
    ASSERT(c >= 0);
    ASSERT(c < 16);
    ASSERT(sWindow != nil);

    RGBColor color = ConvertColor(c);

    sWindow->SetBackColor(color);
}


//---------------------------------------------------------------
//
// gotoxy
//
//---------------------------------------------------------------
void gotoxy(int x, int y)
{
    ASSERT(x >= 1);
    ASSERT(y >= 1);
    ASSERT(sWindow != nil);

    sWindow->SetCursor(x - 1, y - 1);
}


//---------------------------------------------------------------
//
// wherex
//
//---------------------------------------------------------------
int wherex()
{
    ASSERT(sWindow != nil);

    Point pos = sWindow->GetCursor();

    return pos.h + 1;
}


//---------------------------------------------------------------
//
// wherey
//
//---------------------------------------------------------------
int wherey()
{
    ASSERT(sWindow != nil);

    Point pos = sWindow->GetCursor();

    return pos.v + 1;
}


//---------------------------------------------------------------
//
// putch
//
//---------------------------------------------------------------
void putch(char ch)
{
    ASSERT(sWindow != nil);

    char buffer[2];

    buffer[0] = ch;
    buffer[1] = '\0';

    sWindow->SetChar(ch);
//      sWindow->Print(buffer);
}


//---------------------------------------------------------------
//
// cprintf
//
//---------------------------------------------------------------
void cprintf(const char *format,...)
{
    ASSERT(sWindow != nil);

    char buffer[2048];

    va_list argp;

    va_start(argp, format);
    vsprintf(buffer, format, argp);
    va_end(argp);

    sWindow->Print(buffer);
}


//---------------------------------------------------------------
//
// kbhit
//
//---------------------------------------------------------------
int kbhit()
{
    bool found = false;

    QHdrPtr eventQHead = LMGetEventQueue();     // this is more efficient than EventAvail since it avoids a mode switch (although it's unlikely to make much of a difference...)

    EvQElPtr eventQPtr = reinterpret_cast < EvQElPtr > (eventQHead->qHead);

    while (eventQPtr != nil && !found)
    {
        found = eventQPtr->evtQWhat == keyDown || eventQPtr->evtQWhat == autoKey;

        eventQPtr = reinterpret_cast < EvQElPtr > (eventQPtr->qLink);
    }

    return found;
}


//---------------------------------------------------------------
//
// getche
//
//---------------------------------------------------------------
char getche()
{
    char ch = getch();

    if (ch != '\r')
        putch(ch);

    return ch;
}


//---------------------------------------------------------------
//
// getstr
//
//---------------------------------------------------------------
void getstr(char *buffer, int bufferSize)
{
    ASSERT(buffer != NULL);
    ASSERT(bufferSize > 1);

    int index = 0;

    while (index < bufferSize - 1)
    {
        char ch = getche();

        if (ch == '\r')
            break;
        else if (ch == '\b' && index > 0)
            --index;
        else if (isprint(ch))
            buffer[index++] = ch;
    }

    buffer[index] = '\0';
}


//---------------------------------------------------------------
//
// _setcursortype
//
//---------------------------------------------------------------
void _setcursortype(int curstype)
{
    ASSERT(curstype == _NORMALCURSOR || curstype == _NOCURSOR);
    ASSERT(sWindow != NULL);

    sWindow->ShowCursor(curstype == _NORMALCURSOR);
}


//---------------------------------------------------------------
//
// getch
//
//---------------------------------------------------------------
int getch()
{
    ASSERT(sWindow != NULL);

    int ch = '\0';

    while (ch == '\0')
    {
        EventRecord event;
        bool gotEvent =::WaitNextEvent(everyEvent, &event, 10, NULL);

        if (gotEvent)
            ch = HandleEvent(event);
    }

    return ch;
}

#pragma mark -

// ========================================================================
//      Misc Functions
// ========================================================================

//---------------------------------------------------------------
//
// delay
//
//---------------------------------------------------------------
void delay(int ms)
{
    ASSERT(ms >= 0);

    unsigned long ticks = 60 * ms / 1000;
    unsigned long final;

    Delay(ticks, &final);
}


//---------------------------------------------------------------
//
// init_mac
//
//---------------------------------------------------------------
void init_mac()
{
    ASSERT(sWindow == NULL);

    // Initialize toolbox
    MaxApplZone();
    InitGraf((Ptr) & qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
    InitCursor();

    sHasAM = TestGestaltMask(gestaltAppearanceAttr, gestaltAppearanceExists);

    // Initialize SIOUX
#if DEBUG
    SIOUXSettings.initializeTB = false;
    SIOUXSettings.standalone = false;
    SIOUXSettings.setupmenus = false;
    SIOUXSettings.asktosaveonclose = false;
#endif

    // Add some menus
  sAppleMenu =::GetMenu(kAppleMenu);
  sFileMenu =::GetMenu(kFileMenu);
  sFontMenu =::GetMenu(kFontMenu);
  sSizeMenu =::GetMenu(kSizeMenu);
    if (sAppleMenu == NULL || sFileMenu == NULL || sFontMenu == NULL || sSizeMenu == NULL)
    {
        WarnUser("Couldn't load the menus!");
        ExitToShell();
    }

    AppendResMenu(sAppleMenu, 'DRVR');
    AppendResMenu(sFontMenu, 'FONT');

    if (sHasAM)
    {
        int numItems = CountMItems(sFontMenu);

        for (int i = 1; i <= numItems; i++)
        {
            Str255 fontName;

            GetMenuItemText(sFontMenu, i, fontName);

            short fontNum;

            GetFNum(fontName, &fontNum);

            SetMenuItemFontID(sFontMenu, i, fontNum);
        }
    }

    InsertMenu(sAppleMenu, 0);
    InsertMenu(sFileMenu, 0);
    InsertMenu(sFontMenu, 0);
    InsertMenu(sSizeMenu, 0);

  ::InvalMenuBar();

    // Read in the color table
    sColors = GetCTable(256);
    if (sColors == NULL)
    {
        WarnUser("Couldn't load the colour table!");
        ExitToShell();
    }

    // Create a window
    sWindow = new CWindow;
}


//---------------------------------------------------------------
//
// deinit_mac
//
//---------------------------------------------------------------
void deinit_mac()
{
    delete sWindow;

    sWindow = NULL;
}


#endif // macintosh
