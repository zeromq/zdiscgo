/*  =========================================================================
    zdiscgoplugin - Plugin API

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of zdiscgo.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

/*
@header
    zdiscgoplugin - Plugin API
@discuss
@end
*/

#include "zdiscgo_classes.h"

//  Structure of our class

struct _zdiscgoplugin_t {
    int filler;     //  Declare class properties here
};


//  --------------------------------------------------------------------------
//  Create a new zdiscgoplugin

zdiscgoplugin_t *
zdiscgoplugin_new (void)
{
    zdiscgoplugin_t *self = (zdiscgoplugin_t *) zmalloc (sizeof (zdiscgoplugin_t));
    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zdiscgoplugin

void
zdiscgoplugin_destroy (zdiscgoplugin_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdiscgoplugin_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Self test of this class

void
zdiscgoplugin_test (bool verbose)
{
    printf (" * zdiscgoplugin: ");

    //  @selftest
    //  Simple create/destroy test
    zdiscgoplugin_t *self = zdiscgoplugin_new ();
    assert (self);
    zdiscgoplugin_destroy (&self);
    //  @end
    printf ("OK\n");
}
