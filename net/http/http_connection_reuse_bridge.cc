/*
 * Copyright (C) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "build/build_config.h"
#include <unistd.h>
#include <string>
#include <cutils/log.h>
#include "base/compiler_specific.h"
#include "http_connection_reuse_bridge.h"
#include <dlfcn.h>
#include <cutils/log.h>

static bool (*DoIsConnectionReuseEnabled)() = NULL;

static void InitOnce() {
  static bool initialized = false;
  if (!initialized) {
      initialized = true;
      void* fh = dlopen("tcp_connection_reuse.so", RTLD_LAZY);
      if (fh) {
        dlerror(); //see man dlopen
        *(void **)(&DoIsConnectionReuseEnabled) = dlsym(fh, "DoIsConnectionReuseEnabled");
        SLOGD("Succeeded to open tcp_connection_reuse library");
      }
      else {
        SLOGD("tcp_connection_reuse dl error message %s", dlerror());
      }
  }
}

bool IsConnectionReuseEnabled()
{
  InitOnce();
  if (DoIsConnectionReuseEnabled) {
      return DoIsConnectionReuseEnabled();
  }
  return false;
}
