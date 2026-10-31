/*
 * The native window's menu bar on macOS: the game's menus in the system
 * menu bar, after the application menu SDL made.  Shortcut hints are shown
 * (right-aligned), not made key equivalents: the keys stay the game's.
 * Compiled with ARC.
 */
#import <Cocoa/Cocoa.h>
#include <SDL_syswm.h>
#include <stdio.h>
#include <stdlib.h>
#include "host.h"
#include "host_menu.h"

static SDL_Window *menu_window;
static int frameless;                       /* the window had no frame before full screen */
static __weak NSMenuItem *full_screen_item;

static NSWindow *cocoa_window(void)
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (menu_window && SDL_GetWindowWMInfo(menu_window, &info) && info.subsystem == SDL_SYSWM_COCOA)
        return info.info.cocoa.window;
    return nil;
}

@interface Civ2MenuTarget : NSObject
- (void)pick:(id)sender;
- (void)fullScreen:(id)sender;
@end

@implementation Civ2MenuTarget
- (void)pick:(id)sender
{
    host_menu_picked((int)[(NSMenuItem *)sender tag]);
}

/* View > Full Screen: the window to macOS full screen (its own Space, the
   menu bar shown at the top edge) and back, as the green button does; the
   desktop keeps its size, drawn scaled to fit.  Only a window with a title
   bar goes full screen: a window without a frame gets one meanwhile. */
- (void)fullScreen:(id)sender
{
    NSWindow *w = cocoa_window();
    (void)sender;
    if (!w)
        return;
    if (!(w.styleMask & NSWindowStyleMaskFullScreen)) {
        frameless = !(w.styleMask & NSWindowStyleMaskTitled);
        w.styleMask |= NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                       NSWindowStyleMaskResizable;
        w.collectionBehavior |= NSWindowCollectionBehaviorFullScreenPrimary;
    }
    [w toggleFullScreen:nil];
}

- (void)didEnter:(NSNotification *)n
{
    (void)n;
    full_screen_item.state = NSControlStateValueOn;
}

- (void)didExit:(NSNotification *)n
{
    NSWindow *w = n.object;
    full_screen_item.state = NSControlStateValueOff;
    if (frameless)
        dispatch_async(dispatch_get_main_queue(), ^{
            NSRect r = w.screen.visibleFrame;
            w.styleMask = NSWindowStyleMaskBorderless;
            [w setFrame:r display:YES];
        });
}
@end

static Civ2MenuTarget *target;

static void make_target(void)
{
    NSWindow *w;
    if (target)
        return;
    w = cocoa_window();
    target = [[Civ2MenuTarget alloc] init];
    /* While a menu is open (the run loop in its tracking mode, the program's
       loop held): host_modal_tick. */
    [[NSRunLoop currentRunLoop] addTimer:[NSTimer timerWithTimeInterval:0.02 repeats:YES block:^(NSTimer *t) {
                                    (void)t;
                                    host_modal_tick();
                                }]
                                 forMode:NSEventTrackingRunLoopMode];
    [[NSNotificationCenter defaultCenter] addObserver:target selector:@selector(didEnter:)
                                                 name:NSWindowDidEnterFullScreenNotification object:w];
    [[NSNotificationCenter defaultCenter] addObserver:target selector:@selector(didExit:)
                                                 name:NSWindowDidExitFullScreenNotification object:w];
}

void host_mac_full_screen(SDL_Window *window)
{
    menu_window = window;
    make_target();
    [target fullScreen:nil];
}

static NSString *string_of(const char *s)
{
    NSString *r = [NSString stringWithCString:s encoding:NSWindowsCP1252StringEncoding];
    return r ? r : @"";
}

/* "Zoom &In\tz": the title "Zoom In" and the hint "z" ("&&" is an ampersand). */
static NSString *title_of(const char *text, NSString **hint)
{
    char buf[256];
    size_t k = 0;
    const char *s = text;
    *hint = nil;
    for (; *s && *s != '\t' && k + 1 < sizeof buf; s++) {
        if (*s == '&') {
            if (s[1] != '&')
                continue;
            s++;
        }
        buf[k++] = *s;
    }
    buf[k] = 0;
    if (*s == '\t' && s[1])
        *hint = string_of(s + 1);
    return string_of(buf);
}

void host_mac_activate(void)
{
    /* The full screen item is ours (View > Full Screen), not the one
       AppKit puts in a menu called View, which a window without a frame
       could not use. */
    [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"NSFullScreenMenuItemEverywhere"];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
}

void host_menu_apply(SDL_Window *window, const HostMenuItem *items, int n)
{
    if (getenv("CIV2_LOG_MENU"))
        fprintf(stderr, "civ2: menu bar: %d items%s\n", n, n ? "" : " (none yet)");
    @autoreleasepool {
        NSMenu *old = [NSApp mainMenu];
        NSMenu *bar = [[NSMenu alloc] initWithTitle:@""];
        NSMenu *stack[4];
        NSFont *font = [NSFont menuFontOfSize:0];
        NSDictionary *measure = @{NSFontAttributeName: font};
        NSMutableParagraphStyle *para = [[NSMutableParagraphStyle alloc] init];
        CGFloat tab = 0;
        NSMenu *view = nil;
        int i;
        menu_window = window;
        make_target();
        /* The application menu (About, Hide, Quit ...) stays first. */
        if (old && [old numberOfItems] > 0) {
            NSMenuItem *app = [old itemAtIndex:0];
            [old removeItem:app];
            [bar addItem:app];
        }
        /* Hints at a tab stop past the longest title and its hint. */
        for (i = 0; i < n; i++) {
            NSString *hint, *title = title_of(items[i].text, &hint);
            if (hint) {
                CGFloat w = [title sizeWithAttributes:measure].width + 40 + [hint sizeWithAttributes:measure].width;
                if (w > tab)
                    tab = w;
            }
        }
        para.tabStops = @[[[NSTextTab alloc] initWithTextAlignment:NSTextAlignmentRight location:tab options:@{}]];
        stack[0] = bar;
        for (i = 0; i < n; i++) {
            const HostMenuItem *it = &items[i];
            NSString *hint, *title;
            NSMenuItem *mi;
            NSMenu *parent;
            if (it->level < 0 || it->level > 2)
                continue;
            parent = stack[it->level];
            if (it->flags & HOST_MENU_SEPARATOR) {
                [parent addItem:[NSMenuItem separatorItem]];
                continue;
            }
            title = title_of(it->text, &hint);
            mi = [[NSMenuItem alloc] initWithTitle:title action:nil keyEquivalent:@""];
            if (it->flags & HOST_MENU_POPUP) {
                NSMenu *sub = [[NSMenu alloc] initWithTitle:title];
                sub.autoenablesItems = NO;
                mi.submenu = sub;
                stack[it->level + 1] = sub;
                if (it->level == 0 && [title isEqualToString:@"View"])
                    view = sub;
            } else {
                mi.target = target;
                mi.action = @selector(pick:);
                mi.tag = it->id;
            }
            mi.enabled = !(it->flags & HOST_MENU_GRAYED);
            mi.state = (it->flags & HOST_MENU_CHECKED) ? NSControlStateValueOn : NSControlStateValueOff;
            if (hint)
                mi.attributedTitle = [[NSAttributedString alloc]
                    initWithString:[NSString stringWithFormat:@"%@\t%@", title, hint]
                        attributes:@{NSFontAttributeName: font, NSParagraphStyleAttributeName: para}];
            [parent addItem:mi];
        }
        if (view) {
            /* (the shortcut shown, not a key equivalent: host_sdl.c takes ctrl-cmd-F) */
            NSMenuItem *fs = [[NSMenuItem alloc] initWithTitle:@"Full Screen" action:@selector(fullScreen:)
                                                 keyEquivalent:@""];
            fs.attributedTitle = [[NSAttributedString alloc]
                initWithString:@"Full Screen\t\u2303\u2318F"
                    attributes:@{NSFontAttributeName: font, NSParagraphStyleAttributeName: para}];
            fs.target = target;
            fs.state = (cocoa_window().styleMask & NSWindowStyleMaskFullScreen) ? NSControlStateValueOn
                                                                                 : NSControlStateValueOff;
            full_screen_item = fs;
            [view addItem:[NSMenuItem separatorItem]];
            [view addItem:fs];
        }
        bar.autoenablesItems = NO;
        [NSApp setMainMenu:bar];
    }
}

int host_menu_syswm_command(const void *msg)
{
    (void)msg;
    return 0;
}
