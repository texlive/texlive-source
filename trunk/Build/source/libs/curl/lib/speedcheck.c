/*****************************************************************************
 *                                  _   _ ____  _     
 *  Project                     ___| | | |  _ \| |    
 *                             / __| | | | |_) | |    
 *                            | (__| |_| |  _ <| |___ 
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 2000, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * In order to be useful for every potential user, curl and libcurl are
 * dual-licensed under the MPL and the MIT/X-derivate licenses.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the MPL or the MIT/X-derivate
 * licenses. You may pick one of these licenses.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * $Id: speedcheck.c,v 1.13 2001/10/12 12:32:20 bagder Exp $
 *****************************************************************************/

#include "setup.h"

#include <stdio.h>
#include <string.h>
#if defined(__MINGW32__)
#include <winsock.h>
#endif

#include <curl/curl.h>
#include "urldata.h"
#include "sendf.h"
#include "speedcheck.h"

void Curl_speedinit(struct SessionHandle *data)
{
  memset(&data->state.keeps_speed, 0, sizeof(struct timeval));
}

CURLcode Curl_speedcheck(struct SessionHandle *data,
                         struct timeval now)
{
  if((data->progress.current_speed >= 0) &&
     data->set.low_speed_time &&
     (Curl_tvlong(data->state.keeps_speed) != 0) &&
     (data->progress.current_speed < data->set.low_speed_limit)) {

    /* We are now below the "low speed limit". If we are below it
       for "low speed time" seconds we consider that enough reason
       to abort the download. */
    
    if( (Curl_tvdiff(now, data->state.keeps_speed)/1000) >
        data->set.low_speed_time) {
      /* we have been this slow for long enough, now die */
      failf(data,
	    "Operation too slow. "
	    "Less than %d bytes/sec transfered the last %d seconds",
	    data->set.low_speed_limit,
	    data->set.low_speed_time);
      return CURLE_OPERATION_TIMEOUTED;
    }
  }
  else {
    /* we keep up the required speed all right */
    data->state.keeps_speed = now;
  }
  return CURLE_OK;
}

/*
 * local variables:
 * eval: (load-file "../curl-mode.el")
 * end:
 * vim600: fdm=marker
 * vim: et sw=2 ts=2 sts=2 tw=78
 */
