/*  =========================================================================
    zdiscgo - Zactor for service discovery with Go plugin support

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of zdiscgo.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

/*
@header
    zdiscgo - Zactor for service discovery with Go plugin support
@discuss
@end
*/

#include "zdiscgo_classes.h"

//  Structure of our actor

struct _zdiscgo_t {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    zdiscgoplugin_t *plugin;    //  zdiscgoplugin_t reference
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
};


//  --------------------------------------------------------------------------
//  Create a new zdiscgo instance

static zdiscgo_t *
zdiscgo_new (zsock_t *pipe, void *args)
{
    zdiscgo_t *self = (zdiscgo_t *) zmalloc (sizeof (zdiscgo_t));
    assert (self);

    self->pipe = pipe;
    self->terminated = false;
    self->poller = zpoller_new (self->pipe, NULL);

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zdiscgo instance

static void
zdiscgo_destroy (zdiscgo_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdiscgo_t *self = *self_p;

        zpoller_destroy (&self->poller);
        zdiscgoplugin_destroy (&self->plugin); 

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  Here we handle incoming message from the node

static void
zdiscgo_recv_api (zdiscgo_t *self)
{
    //  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return;        //  Interrupted

    char *command = zmsg_popstr (request);
    if (streq (command, "VERBOSE")) {
        self->verbose = true;
    }
    else
    if (streq (command, "CONFIGURE")) {
        
        if (self->verbose)
            zsys_debug ("received 'CONFIGURE' command");

        char *libpath = zmsg_popstr (request);
        self->plugin = zdiscgoplugin_new (libpath);
        
        if (self->verbose)
            zsys_debug ("loaded '%s' library", libpath);

        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "DISCOVER")) {

        char *url = zmsg_popstr (request);
        char *key = zmsg_popstr (request);
        
        if (self->verbose)
            zsys_debug ("'DISCOVER', '%s', '%s'", url, key);

        const char *endpoints = zdiscgoplugin_discover_endpoints (self->plugin, url, key);

        if (self->verbose)
            zsys_debug ("'DISCOVER FOUND', '%s'", endpoints);

        zstr_send (self->pipe, endpoints);
    }
    else
    if (streq (command, "$TERM")) {
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    }
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
}


//  --------------------------------------------------------------------------
//  This is the actor which runs in its own thread.

void
zdiscgo_actor (zsock_t *pipe, void *args)
{
    zdiscgo_t * self = zdiscgo_new (pipe, args);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, 0);
        if (which == self->pipe)
            zdiscgo_recv_api (self);
       //  Add other sockets when you need them.
    }
    zdiscgo_destroy (&self);
}

//  --------------------------------------------------------------------------
//  Self test of this actor.

void
zdiscgo_test (bool verbose)
{
    printf (" * zdiscgo: ");
    //  @selftest
    zactor_t *zdiscgo = zactor_new (zdiscgo_actor, NULL);
    zstr_send (zdiscgo, "VERBOSE");
    
    zsock_send (zdiscgo, "ss", "CONFIGURE", "./go/libmockdiscgo.so");
    zsock_wait (zdiscgo);

    zsock_send (zdiscgo, "sss", "DISCOVER", "url", "key");
    
    char *endpoints = zstr_recv (zdiscgo);
    assert (streq (endpoints, "inproc://url-key"));

    zactor_destroy (&zdiscgo);
    //  @end

    printf ("OK\n");
}
