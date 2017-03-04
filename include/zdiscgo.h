/*  =========================================================================
    zdiscgo - ZeroMQ Syslog Normalizer

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of zdiscgo.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZDISCGO_H_H_INCLUDED
#define ZDISCGO_H_H_INCLUDED

//  Include the project library file
#include "zdiscgo_library.h"

ZDISCGO_EXPORT void
zdiscgo_actor (zsock_t *pipe, void *args);

ZDISCGO_EXPORT void 
zdiscgo_verbose (zactor_t *self);

ZDISCGO_EXPORT int 
zdiscgo_load_plugin (zactor_t *self, char *path) ;

ZDISCGO_EXPORT char *
zdiscgo_discover (zactor_t *self, char *url, char *key) ;

void
zdiscgo_test (bool verbose);

#endif
