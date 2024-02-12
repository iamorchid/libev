#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int main() {
  int fd;
  int wdd, wdf;
  char buffer[BUF_LEN];

  // Initialize inotify instance
  fd = inotify_init();
  if (fd < 0) {
    perror("inotify_init");
  }

  // Add a watch for directory for all events
  wdd = inotify_add_watch(fd, "/tmp/test", IN_ALL_EVENTS);
  if (wdd < 0) {
    perror("Cannot watch directory");
    exit(EXIT_FAILURE);
  } else {
    printf("Successfully watched directory: %d\n", wdd);
  }

  // Add a watch for directory for all events
  wdf = inotify_add_watch(fd, "/tmp/hello.txt", IN_ALL_EVENTS);
  if (wdf < 0) {
    perror("Cannot watch file");
    exit(EXIT_FAILURE);
  } else {
    printf("Successfully watched file: %d\n", wdf);
  }

  // // If pathname was already being watched, then the watch descriptor 
  // // for the existing watch is returned.
  // wdf = inotify_add_watch(fd, "/tmp/hello.txt", IN_ALL_EVENTS);
  // if (wdf < 0) {
  //   perror("Cannot watch file");
  //   exit(EXIT_FAILURE);
  // } else {
  //   printf("Successfully watched file: %d\n", wdf);
  // }

  // Read events
  while(1) {
    int length, i = 0;

    length = read(fd, buffer, BUF_LEN);
    if (length < 0) {
      perror("read");
    }

    // Process each event
    while (i < length) {
      // 如果watch的是文件, 则name为空
      struct inotify_event *event = (struct inotify_event *) &buffer[i];
      printf("event from %d, ", event->wd);
      if (event->mask & IN_CREATE) {
        printf("Created file: %s\n", event->name);
      } else if (event->mask & IN_DELETE) {
        printf("Deleted file: %s\n", event->name);
      } else if (event->mask & IN_OPEN) {
        printf("Opened file: %s\n", event->name);
      } else if (event->mask & IN_CLOSE) {
        printf("Closed file: %s\n", event->name);
      } else if (event->mask & IN_MODIFY) {
        printf("Modified file: %s\n", event->name);
      } else {
        printf("other event: %x\n", event->mask);
      }
      i += EVENT_SIZE + event->len;
    }
  }

  // Remove the watch and close inotify file descriptor
  (void) inotify_rm_watch(fd, wdd);
  (void) inotify_rm_watch(fd, wdf);
  (void) close(fd);

  exit(EXIT_SUCCESS);
}
