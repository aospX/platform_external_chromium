/** ---------------------------------------------------------------------------
Copyright (c) 2011, Code Aurora Forum. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of Code Aurora Forum, Inc. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------**/

#include <unistd.h>
#include <dlfcn.h>
#include <string>

#include "base/compiler_specific.h"
#include "build/build_config.h"
#include "net/disk_cache/stat_hub.h"
#include "net/disk_cache/pp_proc_plugin_bridge.h"

class PpProc : public stat_hub::StatProcessorPlugin<PpProc> {
public:
    PpProc() {
    }

    virtual ~PpProc() {
    };

    //Additional Interface
    STAT_BRIDGE_METHOD_1(CreatePpProc, bool, net::HttpCache*, cache);

virtual const char* OpenPlugin(void* fh) {
        const char* dll_ok = NULL;
        if (fh) {
            dll_ok = stat_hub::StatProcessorPlugin<PpProc>::OpenPlugin(fh);
            STAT_PROC_IMPORT(fh, CreatePpProc);
        }
        return dll_ok;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(PpProc);
};

//Standard Interface
STAT_PROC_IF_IMPL_ON_INIT(PpProc, true);
STAT_PROC_IF_IMPL_ON_FETCH_DB(PpProc, true);
STAT_PROC_IF_IMPL_ON_FLUSH_DB(PpProc, true);
STAT_PROC_IF_IMPL_ON_CLEAR_DB(PpProc, true);
STAT_PROC_IF_IMPL_ON_UPDATE_MAIN_URL(PpProc, true);
STAT_PROC_IF_IMPL_ON_UPDATE_SUB_URL(PpProc, true);
STAT_PROC_IF_IMPL_ON_MMC_URL_REMOVED(PpProc, true);
STAT_PROC_IF_IMPL_ON_MMC_URL_ADDED(PpProc, true);
STAT_PROC_IF_IMPL_ON_MAIN_URL_LOADED(PpProc, true);
STAT_PROC_IF_IMPL_ON_CMD(PpProc, true);

//Additional Interface
STAT_PROC_IF_IMPL_1(PpProc, CreatePpProc, true, bool, net::HttpCache*, cache);

const char* pp_proc_plugin_name = "pp_proc_plugin.so";

stat_hub::StatProcessor* StatHubCreatePpProc(net::HttpCache* cache) {
    static bool stat_hub_initialized = false;
    if (!stat_hub_initialized) {
        stat_hub_initialized = true;
        PpProc* pp = new PpProc();
        void* fh = dlopen(pp_proc_plugin_name, RTLD_NOW);
        if (fh) {
            const char* fn = NULL;
            const char* dll_ok = NULL;

            dll_ok = pp->OpenPlugin(fh);
            if (NULL==dll_ok) {
                pp->CreatePpProc(cache);
                LOG(INFO) << "Plugin is ready:" << pp_proc_plugin_name;
                return pp;
            }
            LOG(INFO) << "Method " << dll_ok << " isn't found in plugin: " << pp_proc_plugin_name;
        }
        else {
            LOG(INFO) << "Failed to open plugin:" << pp_proc_plugin_name;
        }
    }
    return NULL;
}


// ======================================= Exports ==============================================
