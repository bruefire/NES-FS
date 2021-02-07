/************************************************************************************

Filename    :   OVR_RefCount.cpp
Content     :   Reference counting implementation
Created     :   September 19, 2012
Notes       :

Copyright   :   Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

Licensed under the Oculus Master SDK License Version 1.0 (the "License");
you may not use the Oculus VR Rift SDK except in compliance with the License,
which is provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

https://developer.oculus.com/licenses/oculusmastersdk-1.0

Unless required by applicable law or agreed to in writing, the Oculus VR SDK
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "OVR_RefCount.h"
#include "OVR_Atomic.h"

namespace OVR {

// ***** Reference Count Base implementation

RefCountImplCore::~RefCountImplCore() {
  // RefCount can be either 1 or 0 here.
  //  0 if Release() was properly called.
  //  1 if the object was declared on stack or as an aggregate.
  OVR_ASSERT(RefCount <= 1);
}

#ifdef OVR_BUILD_DEBUG
void RefCountImplCore::reportInvalidDelete(void* /*pmem*/) {
  // Invalid delete call on ref-counted object. Please use Release()
  OVR_FAIL();
}
#endif

RefCountNTSImplCore::~RefCountNTSImplCore() {
  // RefCount can be either 1 or 0 here.
  //  0 if Release() was properly called.
  //  1 if the object was declared on stack or as an aggregate.
  OVR_ASSERT(RefCount <= 1);
}

#ifdef OVR_BUILD_DEBUG
void RefCountNTSImplCore::reportInvalidDelete(void* /*pmem*/) {
  // Invalid delete call on ref-counted object. Please use Release()
  OVR_FAIL();
}
#endif

// *** Thread-Safe RefCountImpl

void RefCountImpl::AddRef() {
  RefCount.fetch_add(1, std::memory_order_relaxed);
}
void RefCountImpl::Release() {
  if (RefCount.fetch_add(-1, std::memory_order_relaxed) - 1 == 0)
    delete this;
}

// *** Thread-Safe RefCountVImpl w/virtual AddRef/Release

void RefCountVImpl::AddRef() {
  RefCount.fetch_add(1, std::memory_order_relaxed);
}
void RefCountVImpl::Release() {
  if (RefCount.fetch_add(-1, std::memory_order_relaxed) - 1 == 0)
    delete this;
}

// *** NON-Thread-Safe RefCountImpl

void RefCountNTSImpl::Release() const {
  RefCount--;
  if (RefCount == 0)
    delete this;
}

} // namespace OVR
