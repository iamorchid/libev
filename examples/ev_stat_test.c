#include <ev.h>
#include <stdio.h>

// Callback function that is called when the file changes
static void stat_cb(struct ev_loop *loop, ev_stat *w, int revents) {
    printf("File change detected! Revent: %d\n", revents);
}

int main() {
    // Create the default event loop
    struct ev_loop *loop = EV_DEFAULT;

    // Initialize an ev_stat watcher
    ev_stat stat_watcher;

    // Initialize the stat watcher with the callback and the path to watch
    ev_stat_init(&stat_watcher, stat_cb, "/tmp/test/hello.txt", 0);

    // Start the stat watcher
    ev_stat_start(loop, &stat_watcher);

    // Start the event loop
    ev_run(loop, 0);

    // Normally you would have some code here to stop the watcher when you are done
    // ev_stat_stop(loop, &stat_watcher);

    return 0;
}