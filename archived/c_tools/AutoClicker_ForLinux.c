#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "MousePointer.h"

#define KEY_F6 XK_F6

/*

sudo apt-get install libxtst-dev libx11-dev

gcc -o ./outputs/autoclicker AutoClicker_ForLinux.c -lX11 -lXtst -lXi

*/

int main() {
  Display *display = XOpenDisplay(0);
  Window root_window = XRootWindow(display, 0);
  if (display == NULL) {
    fprintf(stderr, "Unable to open X display\n");
    return -1;
  }

  printf("Type F6 to toggle the autoclicker.\n");

  // Seed the random number generator
  srand(time(NULL));

  bool autoclickerEnabled = false;

  int x, y;

  while (true) {
    // getMousePosition(&x,&y, display, &root_window);
    // printf("mouse position: x %d y %d\n", x, y);

    // Polling for F6 key press
    char keyState[32];
    XQueryKeymap(display, keyState);
    KeyCode f6Key = XKeysymToKeycode(display, KEY_F6);

    // Check if F6 is pressed
    if (keyState[f6Key / 8] & (1 << (f6Key % 8))) {
      // Toggle autoclicker state
      autoclickerEnabled = !autoclickerEnabled;

      if (autoclickerEnabled)
        printf("Autoclicker has been enabled.\n");
      else
        printf("Autoclicker has been disabled.\n");

      // Wait for F6 to be released to avoid multiple toggles
      while (keyState[f6Key / 8] & (1 << (f6Key % 8))) {
        XQueryKeymap(display, keyState);
        usleep(100000); // 100 ms
      }
    }

    // Autoclicker logic
    if (autoclickerEnabled /*&& x > 1860 && x < 2010 && y > 1235 && y < 1370*/) {
      // Simulate mouse click (left button down and up)
      XTestFakeButtonEvent(display, 1, True, CurrentTime);
      XTestFakeButtonEvent(display, 1, False, CurrentTime);
      XFlush(display); // Ensure the events are sent

      // Generate a random delay between 500ms and 1500ms
      int randomDelay =
          500000 +
          rand() % 1000000; // between 500,000 and 1,500,000 microseconds
      usleep(randomDelay);  // Sleep for the random delay    }
    }

    usleep(100000); // Sleep for 100ms to avoid high CPU usage in the loop
  }

  XCloseDisplay(display);
  return 0;
}
