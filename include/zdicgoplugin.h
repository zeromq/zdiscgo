/*  =========================================================================
    zdicgoplugin - Plugin API

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of zdiscgo.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef ZDICGOPLUGIN_H_INCLUDED
#define ZDICGOPLUGIN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new zdicgoplugin
ZDISCGO_EXPORT zdicgoplugin_t *
    zdicgoplugin_new (void);

//  Destroy the zdicgoplugin
ZDISCGO_EXPORT void
    zdicgoplugin_destroy (zdicgoplugin_t **self_p);

//  Self test of this class
ZDISCGO_EXPORT void
    zdicgoplugin_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
