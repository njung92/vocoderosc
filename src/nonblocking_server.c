#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "lo/lo.h"
#include "../headers/synth.h"

#define NUM_KEYS (12)
#define OCTAVE  (1)

int done = 0;

void error(int num, const char *m, const char *path);

void read_stdin(Synth* s);

int keyboard_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data);

int push_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data);
                
int push_handler2(const char *path, const char *types, lo_arg ** argv, 
                int argc, void *data, void *user_data);

int startLO(Synth* synth) {
    int lo_fd;
    fd_set rfds;
    #ifndef WIN32
    struct timeval tv;
    #endif
    int retval;

    /* start a new server on port 7770 */
    lo_server s = lo_server_new("7770", error);
    printf("Now listening on port 7770\n");

    /* add handlers that will match the paths /1/push* and /2/push*, with one int each */
    int i;
    for (i = 1; i <= NUM_KEYS; i++) {
        char path[10];
        char path2[10];
        snprintf(path, 10, "/1/push%d", i);
        snprintf(path2, 10, "/2/push%d", i);
        lo_server_add_method(s, path, "i", push_handler, synth);
        lo_server_add_method(s, path2, "i", push_handler2, synth);
    }
    /* add handler for MIDI keyboard */
    lo_server_add_method(s, "/keyboard", "ii", keyboard_handler, synth);

    /* get the file descriptor of the server socket, if supported */
    lo_fd = lo_server_get_socket_fd(s);

    if (lo_fd > 0) {
        /* select() on lo_server fd is supported, so we'll use select()
         * to watch both stdin and the lo_server fd. */
        do {
            FD_ZERO(&rfds);
            #ifndef WIN32
            FD_SET(0, &rfds); // stdin
            #endif
            FD_SET(lo_fd, &rfds);
            
            retval = select(lo_fd + 1, &rfds, NULL, NULL, NULL); // no timeout
            
            if (retval == -1) {
                printf("select() error\n");
                exit(1);
            } else if (retval > 0) {
                if (FD_ISSET(0, &rfds)) {
                    read_stdin(synth);
                }
                if (FD_ISSET(lo_fd, &rfds)) {
                    lo_server_recv_noblock(s, 0);
                }
            }
        } while (!done);
    } else {
        /* lo_server protocol does not support select(), so we'll watch
         * stdin while polling the lo_server. */
        #ifdef WIN32
        printf
            ("non-blocking input from stdin not supported under Windows\n");
        exit(1);
        #else
        do {
            FD_ZERO(&rfds);
            FD_SET(0, &rfds);
            tv.tv_sec = 0;
            tv.tv_usec = 10000;

            retval = select(1, &rfds, NULL, NULL, &tv); // timeout every 10ms

            if (retval == -1) {
                printf("select() error\n");
                exit(1);
            } else if (retval > 0 && FD_ISSET(0, &rfds)) {
                read_stdin(synth);
            }

            lo_server_recv_noblock(s, 0);
        } while (!done);
        #endif
    }
    return 0;
}

int push_handler(const char *path, const char *types, lo_arg ** argv,
                 int argc, void *data, void *user_data) {

    (void) types;
    (void) argc;
    (void) data;

    Synth* synth = (Synth*) user_data;
    char *keystr = (char*) malloc(2);
    strncpy(keystr, path+7, strlen(path)-7);
    int key      = atoi(keystr) + (NUM_KEYS*OCTAVE) - 1;
    int note_on  = argv[0]->i;
    
    if (note_on) synth_on(key, synth);
    else synth_off(key, synth);

    free(keystr);
    return 0;
}

int push_handler2(const char *path, const char *types, lo_arg ** argv,
                  int argc, void *data, void *user_data) {
    (void) types;
    (void) argc;
    (void) data;
    
    Synth* synth = (Synth*) user_data;
    char *keystr = (char*) malloc(2);
    strncpy(keystr, path+7, strlen(path)-7);
    int key = atoi(keystr) + (NUM_KEYS*(OCTAVE+1)) - 1;
    int note_on = argv[0]->i;
    
    if (note_on) synth_on(key, synth);
    else synth_off(key, synth);
    
    free(keystr);
    return 0;
}

int keyboard_handler(const char *path, const char *types, lo_arg ** argv,
                     int argc, void *data, void *user_data) {
    
    (void) path;
    (void) types;
    (void) argc;
    (void) data;

    Synth* synth = (Synth*) user_data;
    int key      = argv[0]->i;
    int note_on  = argv[1]->i;
    
    if (note_on) synth_on(key, synth);
    else synth_off(key, synth);
    return 0;
}

void error(int num, const char *msg, const char *path) {
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
}

void read_stdin(Synth* synth) {
    int input = getc(stdin);
    if (input==32) synth_allOff(synth);
    else printf("stdin: %d\n",input);
}

/* vi:set ts=8 sts=4 sw=4: */
