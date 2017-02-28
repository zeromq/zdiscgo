/*  =========================================================================
    zdicgoplugin - Plugin API

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of zdiscgo.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

/*
@header
    zdicgoplugin - Plugin API
@discuss
@end
*/

#include "zdiscgo_classes.h"

//  Structure of our class

struct _zdicgoplugin_t {
    int filler;     //  Declare class properties here
};


//  --------------------------------------------------------------------------
//  Create a new zdicgoplugin

zdicgoplugin_t *
zdicgoplugin_new (void)
{
    zdicgoplugin_t *self = (zdicgoplugin_t *) zmalloc (sizeof (zdicgoplugin_t));
    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zdicgoplugin

void
zdicgoplugin_destroy (zdicgoplugin_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdicgoplugin_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Self test of this class

void
zdicgoplugin_test (bool verbose)
{
    printf (" * zdicgoplugin: ");

    //  @selftest
    //  Simple create/destroy test
    zdicgoplugin_t *self = zdicgoplugin_new ();
    assert (self);
    zdicgoplugin_destroy (&self);
    //  @end
    printf ("OK\n");
}
