# some defaults
USE_TAGGING?=false
GC_TYPE?=generational
CACHE_INTEGER?=true
INT_CACHE_MIN_VALUE?=-5
INT_CACHE_MAX_VALUE?=100
GENERATE_INTEGER_HISTOGRAM?=false
GENERATE_ALLOCATION_STATISTICS?=false
LOG_RECEIVER_TYPES?=false
UNSAFE_FRAME_OPTIMIZATION?=false
ADDITIONAL_ALLOCATION?=false

#
# set feature flags 
#
ifeq ($(USE_TAGGING),true)
  FEATURE_FLAGS+=-DUSE_TAGGING
endif
ifeq ($(CACHE_INTEGER),true)
  FEATURE_FLAGS+=-DCACHE_INTEGER
  FEATURE_FLAGS+=-DINT_CACHE_MIN_VALUE=$(INT_CACHE_MIN_VALUE)
  FEATURE_FLAGS+=-DINT_CACHE_MAX_VALUE=$(INT_CACHE_MAX_VALUE)
endif
ifeq ($(GC_TYPE),copying)
  FEATURE_FLAGS+=-DGC_TYPE=COPYING
endif
ifeq ($(GC_TYPE),mark_sweep)
  FEATURE_FLAGS+=-DGC_TYPE=MARK_SWEEP
endif
ifeq ($(GC_TYPE),generational)
  FEATURE_FLAGS+=-DGC_TYPE=GENERATIONAL
endif
ifeq ($(GENERATE_INTEGER_HISTOGRAM),true)
  FEATURE_FLAGS+=-DGENERATE_INTEGER_HISTOGRAM
endif
ifeq ($(GENERATE_ALLOCATION_STATISTICS),true)
  FEATURE_FLAGS+=-DGENERATE_ALLOCATION_STATISTICS
endif
ifeq ($(UNSAFE_FRAME_OPTIMIZATION),true)
  FEATURE_FLAGS+=-DUNSAFE_FRAME_OPTIMIZATION
endif
ifeq ($(LOG_RECEIVER_TYPES),true)
  FEATURE_FLAGS+=-DLOG_RECEIVER_TYPES
endif
ifeq ($(ADDITIONAL_ALLOCATION),true)
  FEATURE_FLAGS+=-DADDITIONAL_ALLOCATION
endif
