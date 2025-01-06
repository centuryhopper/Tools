// Try this C program as the basis of what you want todo..

#include <stdio.h>
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

/*
gcc -o mousepointer MousePointer.c -lX11 -lXi
 */

void getMousePosition(int* x, int* y, Display* display, Window* root_window)
{
    /* check XInput */
    int xi_opcode, event, error;
    if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error)) {
        fprintf(stderr, "Error: XInput extension is not supported!\n");
        return;
    }

    /* Check XInput 2.0 */
    int major = 2;
    int minor = 0;
    int retval = XIQueryVersion(display, &major, &minor);
    if (retval != Success) {
        fprintf(stderr, "Error: XInput 2.0 is not supported (ancient X11?)\n");
        return;
    }

    /*
     * Set mask to receive XI_RawMotion events. Because it's raw,
     * XWarpPointer() events are not included, you can use XI_Motion
     * instead.
     */
    unsigned char mask_bytes[(XI_LASTEVENT + 7) / 8] = {0};  /* must be zeroed! */
    XISetMask(mask_bytes, XI_RawMotion);

    /* Set mask to receive events from all master devices */
    XIEventMask evmasks[1];
    /* You can use XIAllDevices for XWarpPointer() */
    evmasks[0].deviceid = XIAllMasterDevices;
    evmasks[0].mask_len = sizeof(mask_bytes);
    evmasks[0].mask = mask_bytes;
    XISelectEvents(display, *root_window, evmasks, 1);

    XEvent xevent;
    XNextEvent(display, &xevent);

    if (xevent.xcookie.type != GenericEvent || xevent.xcookie.extension != xi_opcode) {
        /* not an XInput event */
        return;
    }
    XGetEventData(display, &xevent.xcookie);
    if (xevent.xcookie.evtype != XI_RawMotion) {
        /*
            * Not an XI_RawMotion event (you may want to detect
            * XI_Motion as well, see comments above).
            */
        XFreeEventData(display, &xevent.xcookie);
        return;
    }
    XFreeEventData(display, &xevent.xcookie);

    Window root_return, child_return;
    int root_x_return, root_y_return;
    int win_x_return, win_y_return;
    unsigned int mask_return;
    /*
        * We need:
        *     child_return - the active window under the cursor
        *     win_{x,y}_return - pointer coordinate with respect to root window
        */
    retval = XQueryPointer(display, *root_window, &root_return, &child_return,
                                &root_x_return, &root_y_return,
                                &win_x_return, &win_y_return,
                                &mask_return);
    if (!retval) {
        /* pointer is not in the same screen, ignore */
        return;
    }

    /* We used root window as its reference, so both should be the same */
    assert(root_x_return == win_x_return);
    assert(root_y_return == win_y_return);

    // printf("root: x %d y %d\n", root_x_return, root_y_return);

    *x = root_x_return;
    *y = root_y_return;
        
}

// int main(int argc, char **argv)
// {
//     Display *display;
//     Window root_window;

//     /* Initialize (FIXME: no error checking). */
//     display = XOpenDisplay(0);
//     root_window = XRootWindow(display, 0);

//     /* check XInput */
//     int xi_opcode, event, error;
//     if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error)) {
//         fprintf(stderr, "Error: XInput extension is not supported!\n");
//         return 1;
//     }

//     /* Check XInput 2.0 */
//     int major = 2;
//     int minor = 0;
//     int retval = XIQueryVersion(display, &major, &minor);
//     if (retval != Success) {
//         fprintf(stderr, "Error: XInput 2.0 is not supported (ancient X11?)\n");
//         return 1;
//     }

//     /*
//      * Set mask to receive XI_RawMotion events. Because it's raw,
//      * XWarpPointer() events are not included, you can use XI_Motion
//      * instead.
//      */
//     unsigned char mask_bytes[(XI_LASTEVENT + 7) / 8] = {0};  /* must be zeroed! */
//     XISetMask(mask_bytes, XI_RawMotion);

//     /* Set mask to receive events from all master devices */
//     XIEventMask evmasks[1];
//     /* You can use XIAllDevices for XWarpPointer() */
//     evmasks[0].deviceid = XIAllMasterDevices;
//     evmasks[0].mask_len = sizeof(mask_bytes);
//     evmasks[0].mask = mask_bytes;
//     XISelectEvents(display, root_window, evmasks, 1);

//     XEvent xevent;
//     while (1) {
//         XNextEvent(display, &xevent);

//         if (xevent.xcookie.type != GenericEvent || xevent.xcookie.extension != xi_opcode) {
//             /* not an XInput event */
//             continue;
//         }
//         XGetEventData(display, &xevent.xcookie);
//         if (xevent.xcookie.evtype != XI_RawMotion) {
//             /*
//              * Not an XI_RawMotion event (you may want to detect
//              * XI_Motion as well, see comments above).
//              */
//             XFreeEventData(display, &xevent.xcookie);
//             continue;
//         }
//         XFreeEventData(display, &xevent.xcookie);

//         Window root_return, child_return;
//         int root_x_return, root_y_return;
//         int win_x_return, win_y_return;
//         unsigned int mask_return;
//         /*
//          * We need:
//          *     child_return - the active window under the cursor
//          *     win_{x,y}_return - pointer coordinate with respect to root window
//          */
//         int retval = XQueryPointer(display, root_window, &root_return, &child_return,
//                                    &root_x_return, &root_y_return,
//                                    &win_x_return, &win_y_return,
//                                    &mask_return);
//         if (!retval) {
//             /* pointer is not in the same screen, ignore */
//             continue;
//         }

//         /* We used root window as its reference, so both should be the same */
//         assert(root_x_return == win_x_return);
//         assert(root_y_return == win_y_return);

//         printf("root: x %d y %d\n", root_x_return, root_y_return);

//         // if (child_return) {
//         //     int local_x, local_y;
//         //     XTranslateCoordinates(display, root_window, child_return,
//         //                           root_x_return, root_y_return,
//         //                           &local_x, &local_y, &child_return);
//         //     printf("local: x %d y %d\n\n", local_x, local_y);
//         // }
        
//     }

//     XCloseDisplay(display);

//     return 0;
// }