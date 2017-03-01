/*  =========================================================================
    zdiscgoplugin - Plugin API

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of zdiscgo.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZDISCGOPLUGIN_H_INCLUDED
#define ZDISCGOPLUGIN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef long long go_int;
typedef struct{const char *p; go_int len;} go_str;

//  @interface
//  Create a new zdiscgoplugin
ZDISCGO_EXPORT zdiscgoplugin_t *
    zdiscgoplugin_new (char *libpath);

//  Destroy the zdiscgoplugin
ZDISCGO_EXPORT void
    zdiscgoplugin_destroy (zdiscgoplugin_t **self_p);

//  Self test of this class
ZDISCGO_EXPORT void
    zdiscgoplugin_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
