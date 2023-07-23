#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <unistd.h>

#define KEY_F6 0x75

int main(int argc, char** argv)
{
  printf("Type F6 to toggle autoclicker.\n");
  while (true)
  {
    // spins until key down
    while (!GetAsyncKeyState(KEY_F6))
      ;

    // spins until key up
    while (GetAsyncKeyState(KEY_F6))
      ;

    printf("Autoclicker has been enabled.\n");
    // spins until key down
    while (!GetAsyncKeyState(KEY_F6))
    {
      mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
      mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
      sleep(1);
    }

    // spins until key up
    while (GetAsyncKeyState(KEY_F6))
      ;
    printf("Autoclicker has been disabled.\n");
  }
}
