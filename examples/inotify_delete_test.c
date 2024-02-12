#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

int main() {
    int inotifyFd = inotify_init();
    if (inotifyFd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Watch the directory containing the file for creation events
    int watchDescriptorDir = inotify_add_watch(inotifyFd, "/tmp/test", IN_CREATE);
    if (watchDescriptorDir < 0) {
        perror("inotify_add_watch on directory");
        exit(EXIT_FAILURE);
    }
    printf ("wdd: %d\n", watchDescriptorDir);

    // Watch the file for deletion events
    int watchDescriptorFile = inotify_add_watch(inotifyFd, "/tmp/test/hello.txt", IN_DELETE_SELF | IN_MODIFY);
    if (watchDescriptorFile < 0) {
        perror("inotify_add_watch on file");
        exit(EXIT_FAILURE);
    }
    printf ("wdf: %d\n", watchDescriptorFile);

    // Read and process events
    const size_t eventSize = sizeof(struct inotify_event);
    const size_t bufferSize = 1024 * (eventSize + 16);
    char buffer[bufferSize];

    while (1) {
        ssize_t length = read(inotifyFd, buffer, bufferSize);
        if (length < 0) {
            perror("read");
            break;
        }

        ssize_t i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            printf("wd: %d, name: %s, event: %d\n", event->wd, event->name, event->mask);

            if (event->wd == watchDescriptorFile && (event->mask & IN_DELETE_SELF)) {
                printf("The watched file was deleted.\n");
                // The file watch is now invalid; a new watch must be added when the file is recreated.
            }

            if (event->wd == watchDescriptorDir && (event->mask & IN_CREATE)) {
                if (strcmp(event->name, "hello.txt") == 0) {
                    printf("The watched file was re-created.\n");
                    // Remove the old watch descriptor if needed, as it's no longer valid
                    inotify_rm_watch(inotifyFd, watchDescriptorFile);
                    // Add a new watch for the new file
                    watchDescriptorFile = inotify_add_watch(inotifyFd, "/tmp/test/hello.txt", IN_DELETE_SELF | IN_MODIFY);
                    if (watchDescriptorFile < 0) {
                        perror("inotify_add_watch on re-created file");
                        exit(EXIT_FAILURE);
                    }
                    printf ("wdf: %d\n", watchDescriptorFile);
                }
            }
            i += eventSize + event->len;
        }
    }

    close(inotifyFd);
    return EXIT_SUCCESS;
}
