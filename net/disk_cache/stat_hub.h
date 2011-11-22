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

#ifndef NET_STAT_HUB_H_
#define NET_STAT_HUB_H_
#pragma once

#include "googleurl/src/gurl.h"
#include "app/sql/connection.h"
#include "app/sql/init_status.h"
#include <dlfcn.h>

#include "stat_hub_api.h"

class MessageLoop;

namespace base {
    class Time;
    class Thread;
}

namespace stat_hub {

typedef void(* event_cb)(int fd, short event, void* arg);

#define STAT_PROC_IF_IMPL_0(owner, name, on, ret) \
    bool owner::name##_ok_ = on; \
    ret (*owner::Do##name)() = NULL;

#define STAT_PROC_IF_IMPL_1(owner, name, on , ret, type1, param1) \
    bool owner::name##_ok_ = on; \
    ret (*owner::Do##name)(type1 param1) = NULL;

#define STAT_PROC_IF_IMPL_2(owner, name, on , ret, type1, param1, type2, param2) \
    bool owner::name##_ok_ = on; \
    ret (*owner::Do##name)(type1 param1, type2 param2) = NULL;

#define STAT_PROC_IF_IMPL_3(owner, name, on , ret, type1, param1, type2, param2, type3, param3) \
    bool owner::name##_ok_ = on; \
    ret (*owner::Do##name)(type1 param1, type2 param2, type3 param3) = NULL;

#define STAT_PROC_IF_IMPL_TP_0(owner, name, on, ret) \
    template<> bool stat_hub::StatProcessorPlugin<owner>::name##_ok_ = on; \
    template<> ret (*stat_hub::StatProcessorPlugin<owner>::Do##name)() = NULL;

#define STAT_PROC_IF_IMPL_TP_1(owner, name, on , ret, type1, param1) \
    template<> bool stat_hub::StatProcessorPlugin<owner>::name##_ok_ = on; \
    template<> ret (*stat_hub::StatProcessorPlugin<owner>::Do##name)(type1 param1) = NULL;

#define STAT_PROC_IF_IMPL_TP_2(owner, name, on , ret, type1, param1, type2, param2) \
    template<> bool stat_hub::StatProcessorPlugin<owner>::name##_ok_ = on; \
    template<> ret (*stat_hub::StatProcessorPlugin<owner>::Do##name)(type1 param1, type2 param2) = NULL;

#define STAT_PROC_IF_IMPL_TP_3(owner, name, on , ret, type1, param1, type2, param2, type3, param3) \
    template<> bool stat_hub::StatProcessorPlugin<owner>::name##_ok_ = on; \
    template<> ret (*stat_hub::StatProcessorPlugin<owner>::Do##name)(type1 param1, type2 param2, type3 param3) = NULL;

#define STAT_PROC_IF_IMPL_ON_INIT(owner, on)                STAT_PROC_IF_IMPL_TP_2(owner, OnInit, on, bool, sql::Connection*, db, MessageLoop*, message_loop)
#define STAT_PROC_IF_IMPL_ON_FETCH_DB(owner, on)            STAT_PROC_IF_IMPL_TP_1(owner, OnFetchDb, on, bool, sql::Connection*, db)
#define STAT_PROC_IF_IMPL_ON_FLUSH_DB(owner, on)            STAT_PROC_IF_IMPL_TP_1(owner, OnFlushDb, on, bool, sql::Connection*, db)
#define STAT_PROC_IF_IMPL_ON_CLEAR_DB(owner, on)            STAT_PROC_IF_IMPL_TP_1(owner, OnClearDb, on, bool, sql::Connection*, db)
#define STAT_PROC_IF_IMPL_ON_UPDATE_MAIN_URL(owner, on)     STAT_PROC_IF_IMPL_TP_1(owner, OnUpdateMainUrl, on, bool, const char*, main_url)
#define STAT_PROC_IF_IMPL_ON_UPDATE_SUB_URL(owner, on)      STAT_PROC_IF_IMPL_TP_2(owner, OnUpdateSubUrl, on, bool, const char*, main_url, const char*, sub_url)
#define STAT_PROC_IF_IMPL_ON_MMC_URL_REMOVED(owner, on)     STAT_PROC_IF_IMPL_TP_1(owner, OnUrlRemovedFromMMCache, on, bool, unsigned int, hash)
#define STAT_PROC_IF_IMPL_ON_MMC_URL_ADDED(owner, on)       STAT_PROC_IF_IMPL_TP_1(owner, OnUrlAddedToMMCache, on, bool, unsigned int, hash)
#define STAT_PROC_IF_IMPL_ON_MAIN_URL_LOADED(owner, on)     STAT_PROC_IF_IMPL_TP_0(owner, OnMainUrlLoaded, on, bool)
#define STAT_PROC_IF_IMPL_ON_CMD(owner, on)                 STAT_PROC_IF_IMPL_TP_3(owner, OnCmd, on, bool, unsigned short, cmd, const char*, param1, const char*, param2)

#define STAT_BRIDGE_METHOD_0(name, ret) \
    static bool name##_ok_; \
    static ret (*Do##name)(); \
    virtual ret name() { if(NULL!=Do##name) return Do##name(); return (ret)0;} \

#define STAT_BRIDGE_METHOD_1(name, ret, type1, param1) \
    static bool name##_ok_; \
    static ret (*Do##name)(type1 param1); \
    virtual ret name(type1 param1) { if(NULL!=Do##name) return Do##name(param1); return (ret)0;} \

#define STAT_BRIDGE_METHOD_2(name, ret, type1, param1, type2, param2) \
    static bool name##_ok_; \
    static ret (*Do##name)(type1 param1, type2 param2); \
    virtual ret name(type1 param1,type2 param2) { if(NULL!=Do##name) return Do##name(param1, param2); return (ret)0;} \

#define STAT_BRIDGE_METHOD_3(name, ret, type1, param1, type2, param2, type3, param3) \
    static bool name##_ok_; \
    static ret (*Do##name)(type1 param1, type2 param2, type3 param3); \
    virtual ret name(type1 param1,type2 param2,type3 param3) { if(NULL!=Do##name) return Do##name(param1, param2, param3); return (ret)0;} \

#define STAT_PROC_IMPORT(handle, name) \
    if(name##_ok_) { \
        *(void **)(&Do##name) = dlsym(handle, #name); \
        if (NULL==Do##name) { \
            dll_ok = #name; \
        } \
    }

class StatProcessor {
public:
    StatProcessor(): next_(NULL){
    }

virtual ~StatProcessor() {
    }

    //Events
virtual bool OnInit(sql::Connection* db, MessageLoop* message_loop)=0;
virtual bool OnFetchDb(sql::Connection* db)=0;
virtual bool OnFlushDb(sql::Connection* db)=0;
virtual bool OnClearDb(sql::Connection* db)=0;
virtual bool OnUpdateMainUrl(const char* main_url) {return true;}
virtual bool OnUpdateSubUrl(const char* main_url, const char* sub_url) {return true;}
virtual bool OnUrlRemovedFromMMCache(unsigned int hash) {return true;}
virtual bool OnUrlAddedToMMCache(unsigned int hash) {return true;}
virtual bool OnMainUrlLoaded() {return false; }
virtual bool OnCmd(unsigned short cmd, const char* param1, const char* param2) {return false; }

private:
    friend class StatHub;

    StatProcessor* next_;
};

template <class T>
class StatProcessorPlugin : public StatProcessor {
public:
    StatProcessorPlugin(){
    }

virtual ~StatProcessorPlugin() {
    }

    STAT_BRIDGE_METHOD_2(OnInit, bool, sql::Connection*, db, MessageLoop*, message_loop)
    STAT_BRIDGE_METHOD_1(OnFetchDb, bool, sql::Connection*, db)
    STAT_BRIDGE_METHOD_1(OnFlushDb, bool, sql::Connection*, db)
    STAT_BRIDGE_METHOD_1(OnClearDb, bool, sql::Connection*, db)
    STAT_BRIDGE_METHOD_1(OnUpdateMainUrl, bool, const char*, main_url)
    STAT_BRIDGE_METHOD_2(OnUpdateSubUrl, bool, const char*, main_url, const char*, sub_url)
    STAT_BRIDGE_METHOD_1(OnUrlRemovedFromMMCache, bool, unsigned int, hash)
    STAT_BRIDGE_METHOD_1(OnUrlAddedToMMCache, bool, unsigned int, hash)
    STAT_BRIDGE_METHOD_0(OnMainUrlLoaded, bool)
    STAT_BRIDGE_METHOD_3(OnCmd, bool, unsigned short, cmd, const char*, param1, const char*, param2)

virtual const char* OpenPlugin(void* fh) {
        const char* dll_ok = NULL;
        if (fh) {
            STAT_PROC_IMPORT(fh, OnInit)
            STAT_PROC_IMPORT(fh, OnFetchDb)
            STAT_PROC_IMPORT(fh, OnFlushDb)
            STAT_PROC_IMPORT(fh, OnClearDb)
            STAT_PROC_IMPORT(fh, OnUpdateMainUrl)
            STAT_PROC_IMPORT(fh, OnUpdateSubUrl)
            STAT_PROC_IMPORT(fh, OnUrlRemovedFromMMCache)
            STAT_PROC_IMPORT(fh, OnUrlAddedToMMCache)
            STAT_PROC_IMPORT(fh, OnMainUrlLoaded)
            STAT_PROC_IMPORT(fh, OnCmd)
        }
        return dll_ok;
    }
};

class StatHub {
public:

virtual ~StatHub();

static StatHub* GetInstance();

    void RegisterProcessor(StatProcessor* processor);

    bool Init(const std::string& db_path, MessageLoop* message_loop);
    void Release();

    void UpdateMainUrl(const char* main_url);
    void UpdateSubUrl(const char* main_url,const char* sub_url);
    void UrlRemovedFromMMCache(unsigned int hash);
    void UrlAddedToMMCache(unsigned int hash);
    void MainUrlLoaded();
    void Cmd(unsigned short cmd, const char* param1, const char* param2);

    void FlushDBrequest();
    bool FlushDB();

    bool GetDBmetaData(const char* key, std::string& val);
    bool SetDBmetaData(const char* key, const char* val);

    bool IsReady() {
        return ready_;
    }

    MessageLoop* GetMessageLoop() {
        return message_loop_;
    }


    sql::Connection* GetDb() {
        return db_;
    }

    base::Thread* GetThread() {
        return thread_;
    }

    bool IsVerboseEnabled() {
        return (verbose_level_!=STAT_HUB_VERBOSE_LEVEL_DISABLED);
    }

    StatHubVerboseLevel GetVerboseLevel() {
        return verbose_level_;
    }

 private:

    StatHub();

    #if defined(NOT_NOW)
        // Vacuums the database. This will cause sqlite to defragment and collect
        // unused space in the file. It can be VERY SLOW.
        void Vacuum();
    #endif //defined(NOT_NOW)

    // Creates tables, returning true if the table already exists
    // or was successfully created.
    bool InitTables();

    sql::Connection* db_;

    std::string db_path_;
    bool ready_;
    bool flush_db_required_;
    bool flush_db_scheduled_;
    base::Time flush_db_request_time_;

    MessageLoop* message_loop_;

    std::string enabled_app_name_;

    StatProcessor*  first_processor_;

    // Separate thread on which we run blocking read for notify events.
    base::Thread* thread_;
    int flush_delay_;
    StatHubVerboseLevel verbose_level_;

    DISALLOW_COPY_AND_ASSIGN(StatHub);
};

}  // namespace stat_hub

#endif  // NET_STAT_HUB_H_
