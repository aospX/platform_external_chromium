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
#include "hostres_plugin_bridge.h"
#include "net/disk_cache/stat_hub.h"
#include "net/host_resolver_helper/dyn_lib_loader.h"
#include "base/logging.h"

class HostResProcessor : public stat_hub::StatProcessorPlugin<HostResProcessor> {
public:
    HostResProcessor() {
    }

    virtual ~HostResProcessor() {
    }

private:
    DISALLOW_COPY_AND_ASSIGN(HostResProcessor);
};

//Standard Interface
STAT_PROC_IF_IMPL_ON_INIT(HostResProcessor, true);
STAT_PROC_IF_IMPL_ON_FETCH_DB(HostResProcessor, true);
STAT_PROC_IF_IMPL_ON_FLUSH_DB(HostResProcessor, true);
STAT_PROC_IF_IMPL_ON_CLEAR_DB(HostResProcessor, true);
STAT_PROC_IF_IMPL_ON_UPDATE_MAIN_URL(HostResProcessor, true);
STAT_PROC_IF_IMPL_ON_UPDATE_SUB_URL(HostResProcessor, true);
STAT_PROC_IF_IMPL_ON_MMC_URL_REMOVED(HostResProcessor, false);
STAT_PROC_IF_IMPL_ON_MMC_URL_ADDED(HostResProcessor, false);
STAT_PROC_IF_IMPL_ON_MAIN_URL_LOADED(HostResProcessor, false);
STAT_PROC_IF_IMPL_ON_CMD(HostResProcessor, false);


const char* hostres_plugin_name = "libdnshostprio.so";

stat_hub::StatProcessor* StatHubCreateHostResPlugin()
{
    LOG(INFO) << "StatHubCreateHostResPlugin called";
    static bool initialized = false;
    if (!initialized) {
        LOG(INFO) << "StatHubCreateHostResPlugin initializing...";
        initialized = true;
        HostResProcessor* hp = new HostResProcessor();
        void* fh = LibraryManager::GetLibraryHandle(hostres_plugin_name);
        if (fh) {
            LOG(INFO) << "StatHubCreateHostResPlugin lib loaded";
            const char* fn = NULL;
            const char* dll_ok = NULL;

            dll_ok = hp->OpenPlugin(fh);
            if (NULL==dll_ok) {
                LOG(INFO) << "StatHubCreateHostResPlugin plugin connected";;
                return hp;
            }
            LOG(INFO) << "Method " << dll_ok << " isn't found in plugin: " << hostres_plugin_name;
        }
        else {
            LOG(INFO) << "Failed to open plugin:" << hostres_plugin_name;
        }
    }
    return NULL;

}

