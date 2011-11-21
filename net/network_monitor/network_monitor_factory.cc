/*
* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Code Aurora Forum, Inc. nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
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
*/

#include "net/network_monitor/network_monitor_factory.h"
#include "net/base/host_resolver_impl.h"
#include <dlfcn.h>
#include <cutils/log.h>

namespace network
{

 base::Lock NetworkMonitorFactory::m_mutex;

 NetworkMonitorFactory* NetworkMonitorFactory::s_pFactory = NULL;

 NetworkMonitorFactory* NetworkMonitorFactory::GetMonitorFactoryInstance()
 {
   base::AutoLock myLock(NetworkMonitorFactory::m_mutex);
   if (s_pFactory == NULL)
   {
     s_pFactory = new NetworkMonitorFactory();
   }
   return s_pFactory;
 }

 NetworkMonitorFactory::NetworkMonitorFactory() :
   m_pMonitor(NULL)
 {
   InitNetworkTransactionMonitor();
 }

 void NetworkMonitorFactory::InitNetworkTransactionMonitor()
 {
   void* libHandle = dlopen("libnetmonitor.so", RTLD_NOW);

   if (libHandle)
   {
     monitor_create_* monitorCreate = (monitor_create_*) dlsym(libHandle,
         "createNetworkMonitor");
     if (monitorCreate)
     {
       m_pMonitor = (INetworkTransactionMonitor*) monitorCreate();
       return;
     }
     ::dlclose(libHandle);
     SLOGD("%s: Failed to construct network monitor : %s", __FILE__, ::dlerror());
   }
   SLOGD("%s: Failed to open libnetmonitor.so : %s", __FILE__, ::dlerror());
 }

 net::HostResolver* NetworkMonitorFactory::CreateHostResolver(
                          size_t max_concurrent_resolves, net::HostResolverProc* resolver_proc,
                          net::NetLog* net_log, MessageLoop* net_notification_messageloop)
 {
    if (NULL != m_pMonitor)
    {
      return m_pMonitor->CreateHostResolver(max_concurrent_resolves,
          resolver_proc, net_log, net_notification_messageloop);
    }
    return net::CreateSystemHostResolver(max_concurrent_resolves, resolver_proc,
        net_log, net_notification_messageloop);
  }
  }; //end namespace network
