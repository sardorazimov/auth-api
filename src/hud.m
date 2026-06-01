#include "agent.h"
#import <Cocoa/Cocoa.h>

@interface MiransasHudController : NSObject
@property(nonatomic, strong) NSPanel *panel;
@end

@implementation MiransasHudController

- (instancetype)initWithMetrics:(const sys_metrics_t *)metrics {
    self = [super init];
    if (!self) {
        return nil;
    }

    NSScreen *screen = [NSScreen mainScreen];
    NSRect screenFrame = [screen visibleFrame];
    CGFloat width = 360.0;
    CGFloat height = 132.0;
    NSRect frame = NSMakeRect(NSMaxX(screenFrame) - width - 24.0,
                              NSMaxY(screenFrame) - height - 24.0,
                              width,
                              height);

    self.panel = [[NSPanel alloc] initWithContentRect:frame
                                            styleMask:NSWindowStyleMaskBorderless
                                              backing:NSBackingStoreBuffered
                                                defer:NO];
    [self.panel setOpaque:NO];
    [self.panel setAlphaValue:0.94];
    [self.panel setLevel:NSFloatingWindowLevel];
    [self.panel setHidesOnDeactivate:NO];
    [self.panel setReleasedWhenClosed:NO];
    [self.panel setIgnoresMouseEvents:NO];
    [self.panel setBackgroundColor:[NSColor clearColor]];

    NSVisualEffectView *blurView = [[NSVisualEffectView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];
    [blurView setMaterial:NSVisualEffectMaterialHUDWindow];
    [blurView setBlendingMode:NSVisualEffectBlendingModeBehindWindow];
    [blurView setState:NSVisualEffectStateActive];
    [blurView setWantsLayer:YES];
    [blurView.layer setCornerRadius:18.0];
    [blurView.layer setMasksToBounds:YES];

    NSTextField *title = [self labelWithFrame:NSMakeRect(22, 88, 316, 24)
                                         text:@"Miransas Health"
                                     fontSize:18.0
                                       weight:NSFontWeightSemibold
                                        alpha:1.0];

    NSString *bodyText = [NSString stringWithFormat:@"CPU %.1f%%  |  RAM free %@ MB / %@ MB",
                          metrics->cpu_usage,
                          @(metrics->free_ram),
                          @(metrics->total_ram)];
    NSTextField *body = [self labelWithFrame:NSMakeRect(22, 56, 316, 22)
                                        text:bodyText
                                    fontSize:14.0
                                      weight:NSFontWeightRegular
                                       alpha:0.88];

    NSTextField *foot = [self labelWithFrame:NSMakeRect(22, 25, 316, 18)
                                        text:@"App health overlay prototype"
                                    fontSize:12.0
                                      weight:NSFontWeightRegular
                                       alpha:0.62];

    [blurView addSubview:title];
    [blurView addSubview:body];
    [blurView addSubview:foot];
    [self.panel setContentView:blurView];

    return self;
}

- (NSTextField *)labelWithFrame:(NSRect)frame
                           text:(NSString *)text
                       fontSize:(CGFloat)fontSize
                         weight:(NSFontWeight)weight
                          alpha:(CGFloat)alpha {
    NSTextField *label = [[NSTextField alloc] initWithFrame:frame];
    [label setStringValue:text];
    [label setBezeled:NO];
    [label setDrawsBackground:NO];
    [label setEditable:NO];
    [label setSelectable:NO];
    [label setTextColor:[[NSColor whiteColor] colorWithAlphaComponent:alpha]];
    [label setFont:[NSFont systemFontOfSize:fontSize weight:weight]];
    return label;
}

- (void)show {
    [self.panel makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (void)close {
    [self.panel close];
}

@end

void show_health_hud(const sys_metrics_t *metrics) {
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];

        MiransasHudController *controller = [[MiransasHudController alloc] initWithMetrics:metrics];
        [controller show];

        NSDate *deadline = [NSDate dateWithTimeIntervalSinceNow:4.0];
        while ([deadline timeIntervalSinceNow] > 0.0) {
            @autoreleasepool {
                NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                    untilDate:[NSDate dateWithTimeIntervalSinceNow:0.05]
                                                       inMode:NSDefaultRunLoopMode
                                                      dequeue:YES];
                if (event) {
                    [NSApp sendEvent:event];
                }
                [NSApp updateWindows];
            }
        }

        [controller close];
    }
}
