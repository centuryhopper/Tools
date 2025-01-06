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
#define KEY_F5 XK_F5

/*

sudo apt-get install libxtst-dev libx11-dev

gcc -o autoclicker AutoClicker_ForLinux.c -lX11 -lXtst

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
  bool refreshEnabled = true;      // Enable page refresh by default
  time_t lastRefresh = time(NULL); // Track the last refresh time

  int x, y;

  while (true) {
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

      // Refresh page logic (trigger F5 keypress every 10 seconds)

      // random time between 10 and 30 seconds
      int randomRefreshDelay = 10 + rand() % 30;
      if (refreshEnabled &&
          difftime(time(NULL), lastRefresh) >= randomRefreshDelay) {
        printf("Refreshing the page by simulating F5 keypress.\n");

        KeyCode f5Key = XKeysymToKeycode(display, KEY_F5);
        XTestFakeKeyEvent(display, f5Key, True, 0);
        XTestFakeKeyEvent(display, f5Key, False, 0);
        XFlush(display); // Ensure the events are sent

        lastRefresh = time(NULL); // Update last refresh time
      }

      // Generate a random delay between 500ms and 1500ms
      int randomDelay =
          500000 +
          rand() % 1000000; // between 500,000 and 1,500,000 microseconds
      usleep(randomDelay);  // Sleep for the random delay
    }

    usleep(100000); // Sleep for 100ms to avoid high CPU usage in the loop
  }

  XCloseDisplay(display);
  return 0;
}
