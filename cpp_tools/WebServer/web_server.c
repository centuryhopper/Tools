#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 4096

char clipboard[BUFFER_SIZE] = "Shared clipboard is empty.";

void send_response(int client_fd, const char *status, const char *content_type,
                   const char *body) {
  char response[BUFFER_SIZE * 2];
  int content_length = strlen(body);

  snprintf(response, sizeof(response),
           "HTTP/1.1 %s\r\n"
           "Content-Type: %s\r\n"
           "Content-Length: %d\r\n"
           "Connection: close\r\n"
           "\r\n"
           "%s",
           status, content_type, content_length, body);

  write(client_fd, response, strlen(response));
}

int main() {
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  // Create socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Bind socket
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  printf("Clipboard sharing server listening on port %d\n", PORT);

  while (1) {
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address,
                            (socklen_t *)&addrlen)) < 0) {
      perror("accept");
      continue;
    }

    char buffer[BUFFER_SIZE] = {0};
    int read_size = read(client_fd, buffer, sizeof(buffer) - 1);

    if (read_size > 0) {
      // Very basic parsing to identify GET or POST and the path

      if (strncmp(buffer, "GET /clipboard", 14) == 0) {
        // Return current clipboard text
        send_response(client_fd, "200 OK", "text/plain", clipboard);
      } else if (strncmp(buffer, "POST /clipboard", 15) == 0) {
        // Find start of body (after \r\n\r\n)
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
          body += 4; // skip \r\n\r\n
          // Copy new clipboard content safely
          strncpy(clipboard, body, BUFFER_SIZE - 1);
          clipboard[BUFFER_SIZE - 1] = '\0';

          send_response(client_fd, "200 OK", "text/plain",
                        "Clipboard updated.");
        } else {
          send_response(client_fd, "400 Bad Request", "text/plain",
                        "No clipboard data received.");
        }
      } else {
        send_response(client_fd, "404 Not Found", "text/plain",
                      "Route not found.");
      }
    }
    close(client_fd);
  }

  return 0;
}
