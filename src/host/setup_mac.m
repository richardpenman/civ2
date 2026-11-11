/*
 * macOS: the folder (or disc image, a .cue file) for the set-up asked with
 * the system's open panel (setup_unix.c).  Compiled with ARC.
 */
#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include <stdio.h>

int setup_mac_ask_folder(const char *prompt, char *out, size_t size)
{
    @autoreleasepool {
        NSOpenPanel *panel;
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];
        panel = [NSOpenPanel openPanel];
        panel.canChooseFiles = YES;
        panel.allowedContentTypes = @[[UTType typeWithFilenameExtension:@"cue"]];     /* macOS 11 */
        panel.canChooseDirectories = YES;
        panel.allowsMultipleSelection = NO;
        panel.message = [NSString stringWithUTF8String:prompt];
        panel.prompt = @"Choose";
        panel.directoryURL = [NSURL fileURLWithPath:@"/Volumes"];
        if ([panel runModal] != NSModalResponseOK || !panel.URL.fileSystemRepresentation)
            return 0;
        snprintf(out, size, "%s", panel.URL.fileSystemRepresentation);
        return 1;
    }
}
