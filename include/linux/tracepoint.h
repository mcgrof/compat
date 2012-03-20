#ifndef _COMPAT_LINUX_TRACEPOINT_H
#define _COMPAT_LINUX_TRACEPOINT_H 1

#include <linux/version.h>

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,27))

#include_next <linux/tracepoint.h>

/* Backports f42c85e7 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
#undef TP_STRUCT__entry
#define TP_STRUCT__entry(args...) args
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
/* Backports 091ad365, a rename */
#define DECLARE_EVENT_CLASS TRACE_EVENT_TEMPLATE

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
#define TP_PROTO TPPROTO
#define TP_ARGS TPARGS
#define TP_FMT TPFMT
#define TP_RAW_FMT TPRAWFMT
#endif

#endif

#else /* just disable tracing */

/* Disable all tracing */
#undef TRACE_EVENT
#define TRACE_EVENT(name, proto, ...) \
static inline void trace_ ## name(proto) {}
#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(...)
#undef DEFINE_EVENT
#define DEFINE_EVENT(evt_class, name, proto, ...) \
static inline void trace_ ## name(proto) {}

#define TP_PROTO(args...)  args
#define TP_ARGS(args...)   args
#define TP_CONDITION(args...)      args

struct tracepoint_iter {
};

#endif /* (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,27)) */

#endif	/* _COMPAT_LINUX_TRACEPOINT_H */
