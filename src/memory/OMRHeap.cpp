#include "OMRHeap.h"

#include <string.h>

#include "OMRCollector.h"
#include "../vmobjects/AbstractObject.h"
#include "../vm/DispatchTable.h"
#include "../vm/Universe.h"
#if GC_TYPE == OMR_GARBAGE_COLLECTION
#include "../../omr/include_core/omrvm.h"
#include "../../omr/gc/include/omrgc.h"
#include "../../omr/gc/base/GCExtensionsBase.hpp"
#include "MarkingDelegate.hpp"

OMRHeap::OMRHeap(long objectSpaceSize) : Heap<OMRHeap>(new OMRCollector(this), objectSpaceSize) {
    collectionLimit = objectSpaceSize * 0.9;
    spcAlloc = 0;
    heapSize = objectSpaceSize;
}

AbstractVMObject* OMRHeap::AllocateObject(size_t size) {
    MM_GCExtensionsBase * extensions = MM_GCExtensionsBase::getExtensions(vm.omrVM);
    uintptr_t allocSize = extensions->objectModel.adjustSizeInBytes(size);
    AbstractVMObject* object = NULL;

#if defined(OMR_GC_THREAD_LOCAL_HEAP)
    uint8_t *heapAlloc = thread.omrTlh.heapAlloc;
    uintptr_t afterAlloc = (uintptr_t)heapAlloc + allocSize;

    if (afterAlloc <= (uintptr_t)thread.omrTlh.heapTop) {
        thread.omrTlh.heapAlloc = (uint8_t *)afterAlloc;
#if defined(OMR_GC_TLH_PREFETCH_FTA)
        thread.omrTlh.tlhPrefetchFTA -= allocSize;
#endif  /* OMR_GC_TLH_PREFETCH_FTA */
        object = (AbstractVMObject *)heapAlloc;
        goto finish;
    }
#endif  /* OMR_GC_THREAD_LOCAL_HEAP */

    object = (AbstractVMObject*)OMR_GC_AllocateObject(thread.omrVMThread, 0, allocSize, OMR_GC_ALLOCATE_OBJECT_NO_GC);
    if (NULL == object) {
        Universe::ErrorPrint("Failed to allocate " + to_string(allocSize) + " Bytes.\n");
        GetUniverse()->Quit(-1);
    }

finish:
    spcAlloc += allocSize;
    if (spcAlloc >= collectionLimit) {
        triggerGC();
    }
    return object;
}

DispatchTable<256>* OMRHeap::AllocateDispatchTable(size_t size)
{
    MM_GCExtensionsBase * extensions = MM_GCExtensionsBase::getExtensions(vm.omrVM);
    uintptr_t allocSize = extensions->objectModel.adjustSizeInBytes(size);
    DispatchTable<256>* object = NULL;

#if defined(OMR_GC_THREAD_LOCAL_HEAP)
    uint8_t *heapAlloc = thread.omrTlh.heapAlloc;
    uintptr_t afterAlloc = (uintptr_t)heapAlloc + allocSize;

    if (afterAlloc <= (uintptr_t)thread.omrTlh.heapTop) {
        thread.omrTlh.heapAlloc = (uint8_t *)afterAlloc;
#if defined(OMR_GC_TLH_PREFETCH_FTA)
        thread.omrTlh.tlhPrefetchFTA -= allocSize;
#endif  /* OMR_GC_TLH_PREFETCH_FTA */
        object = (DispatchTable<256> *)heapAlloc;
        goto finish;
    }
#endif  /* OMR_GC_THREAD_LOCAL_HEAP */

    object = (DispatchTable<256>*) OMR_GC_AllocateObject(thread.omrVMThread, 0, allocSize, OMR_GC_ALLOCATE_OBJECT_NO_GC);
    if (NULL == object) {
        Universe::ErrorPrint("Failed to allocate " + to_string(allocSize) + " Bytes.\n");
        GetUniverse()->Quit(-1);
    }

finish:
    spcAlloc += allocSize;
    if (spcAlloc >= collectionLimit) {
        triggerGC();
    }
    return object;
}

#endif
