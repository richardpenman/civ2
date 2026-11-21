/*
 * macOS: the folder (or disc image, a .cue file, or a .zip of the game) for the set-up asked with
 * the system's open panel (setup_unix.c).  Compiled with ARC.
 */
#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include <stdio.h>
#include "host_lang.h"

int setup_mac_ask_folder(const char *prompt, char *out, size_t size)
{
    @autoreleasepool {
        NSOpenPanel *panel;
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];
        panel = [NSOpenPanel openPanel];
        panel.canChooseFiles = YES;
        panel.allowedContentTypes = @[[UTType typeWithFilenameExtension:@"cue"], UTTypeZIP];     /* macOS 11 */
        panel.canChooseDirectories = YES;
        panel.allowsMultipleSelection = NO;
        panel.message = [NSString stringWithUTF8String:prompt];
        panel.prompt = [NSString stringWithUTF8String:host_text("setup.choose")];
        panel.directoryURL = [NSURL fileURLWithPath:@"/Volumes"];
        if ([panel runModal] != NSModalResponseOK || !panel.URL.fileSystemRepresentation)
            return 0;
        snprintf(out, size, "%s", panel.URL.fileSystemRepresentation);
        return 1;
    }
}
