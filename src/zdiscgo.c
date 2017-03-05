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
//  Set verbose mode

void
zdiscgo_verbose (zactor_t *self) {
    zstr_send (self, "VERBOSE");
}


//  --------------------------------------------------------------------------
//  Load a plugin

int 
zdiscgo_load_plugin (zactor_t *self, char *path) {
    zstr_sendx (self, "CONFIGURE", "./go/libmockdiscgo.so", NULL);
    int rc;
    zsock_recv (self, "i", &rc);
    return rc;
}


//  --------------------------------------------------------------------------
//  Perform a discovery lookup

char * 
zdiscgo_discover (zactor_t *self, char *url, char *key) {
    char *endpoints;
    zpoller_t *poller = zpoller_new (self, NULL);
    assert (poller);
    zstr_sendx (self, "DISCOVER", url, key, NULL);
    zsock_t *which = (zsock_t *) zpoller_wait (poller, 1000);
    if (zpoller_expired (poller)) {
        endpoints = "";
    }
    else {
        endpoints = zstr_recv (which);
    }
    zpoller_destroy (&poller);
    return endpoints;
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
        int rc; 
        self->plugin = zdiscgoplugin_new (libpath);
        if (self->plugin) {
            rc = 0;
            if (self->verbose)
                zsys_debug ("loaded plugin: '%s'", libpath);
        } 
        else {
            rc = 1;
            if (self->verbose)
                zsys_error ("could not load plugin: '%s'", libpath);
        }

        zsock_send (self->pipe, "i", rc);
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

    //  Create a zdiscgo instance. This will spin up a new OS level
    //  thread that will handle service discovery requests.

    zactor_t *zdiscgo = zactor_new (zdiscgo_actor, NULL);

    //  We communicate with the service discovery thread over
    //  a ZMQ_PAIR socket. You can pass the zdisgco instance
    //  to any CZMQ methods that accept zsock_t *. 
    //  Let's set the service to verbose mode.

    zdiscgo_verbose (zdiscgo);

    //  Next, let's configure the service by telling it to load 
    //  our go shared library. The zstr_sendx command will send
    //  multiple string frames. A NULL terminates the message.
    
    int rc = zdiscgo_load_plugin (zdiscgo, "./go/libmockdiscgo.so");
    assert (rc == 0);

    //  Now let's get some endpoints! We send a DISCOVER command
    //  that consists of the url of a service discovery service,
    //  and the identifer the service should use to find the 
    //  endpoints we want.

    char *endpoints = zdiscgo_discover (zdiscgo, "url", "key");

    //  Check that we have the correct response

    assert (streq (endpoints, "inproc://url-key"));

    //  Shut down the zdisgco instance and clean up memory.

    zactor_destroy (&zdiscgo);

    //  @end

    printf ("OK\n");
}
