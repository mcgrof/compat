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

/* Backports 091ad365, a rename */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
#define DECLARE_EVENT_CLASS TRACE_EVENT_TEMPLATE
#endif

/* Backpports 2939b0469 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
#define TP_PROTO TPPROTO
#define TP_ARGS TPARGS
#define TP_FMT TPFMT
#define TP_RAW_FMT TPRAWFMT
#endif

/* Force disabling tracing */

#undef PARAMS
#define PARAMS(args...) args

#undef TRACE_EVENT
#define TRACE_EVENT(name, proto, ...) \
DECLARE_TRACE(name, PARAMS(proto), PARAMS(args))

#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(...)
#undef DEFINE_EVENT

#define DEFINE_EVENT(template, name, proto, args)               \
	DECLARE_TRACE(name, PARAMS(proto), PARAMS(args))

#undef DECLARE_TRACE_NOARGS
#define DECLARE_TRACE_NOARGS(name)					\
		__DECLARE_TRACE(name, void, , 1, void *__data, __data)


/* Backports 38516ab59fbc5b3bb278cf5e1fe2867c70cff32e */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))

#undef DECLARE_TRACE
#define DECLARE_TRACE(name, proto, args)				\
		__DECLARE_TRACE(name, PARAMS(proto), PARAMS(args), 1,	\
				PARAMS(void *__data, proto),		\
				PARAMS(__data, args))

#undef DECLARE_TRACE_CONDITION
#define DECLARE_TRACE_CONDITION(name, proto, args, cond)		\
	__DECLARE_TRACE(name, PARAMS(proto), PARAMS(args), PARAMS(cond), \
			PARAMS(void *__data, proto),			\
			PARAMS(__data, args))

#else

#undef DECLARE_TRACE
#define DECLARE_TRACE(name, proto, args)				\
		__DECLARE_TRACE(name, PARAMS(proto), PARAMS(args), 1,	\
				PARAMS(void *__data, proto),		\
				PARAMS(__data, args))

#undef DECLARE_TRACE_CONDITION
#define DECLARE_TRACE_CONDITION(name, proto, args, cond)		\
	__DECLARE_TRACE(name, PARAMS(proto), PARAMS(args), PARAMS(cond), \
			PARAMS(void *__data, proto),			\
			PARAMS(__data, args))
#endif /* backport 38516ab59fbc5b3bb278cf5e1fe2867c70cff32e */

/* Backports 38516ab59fbc5b3bb278cf5e1fe2867c70cff32e */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))

/* Backports c420970ef476d7d68df119711700666224001f43 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))

#undef __DECLARE_TRACE
#define __DECLARE_TRACE(name, proto, args, cond, data_proto, data_args) \
	static inline void trace_##name(proto)				\
	{ }								\
	static inline void trace_##name##_rcuidle(proto)		\
	{ }								\
	static inline int						\
	register_trace_##name(void (*probe)(proto))			\
	{								\
		return -ENOSYS;						\
	}								\
	static inline void						\
	unregister_trace_##name(void (*probe)(proto))			\
	{								\
	}								\
	static inline void check_trace_callback_type_##name(void (*cb)(data_proto)) \
	{								\
	}
#else

#undef __DECLARE_TRACE
#define __DECLARE_TRACE(name, proto, args, cond, data_proto, data_args) \
	static inline void trace_##name(proto)				\
	{ }								\
	static inline void trace_##name##_rcuidle(proto)		\
	{ }								\
	static inline int						\
	register_trace_##name(void (*probe)(proto))			\
	{								\
		return -ENOSYS;						\
	}								\
	static inline int						\
	unregister_trace_##name(void (*probe)(proto))			\
	{								\
		return -ENOSYS;						\
	}								\
	static inline void check_trace_callback_type_##name(void (*cb)(data_proto)) \
	{								\
	}
#endif /* backport c420970ef476d7d68df119711700666224001f43 */

#else

#undef __DECLARE_TRACE
#define __DECLARE_TRACE(name, proto, args, cond, data_proto, data_args) \
	static inline void trace_##name(proto)				\
	{ }								\
	static inline void trace_##name##_rcuidle(proto)		\
	{ }								\
	static inline int						\
	register_trace_##name(void (*probe)(data_proto),		\
			      void *data)				\
	{								\
		return -ENOSYS;						\
	}								\
	static inline int						\
	unregister_trace_##name(void (*probe)(data_proto),		\
				void *data)				\
	{								\
		return -ENOSYS;						\
	}								\
	static inline void check_trace_callback_type_##name(void (*cb)(data_proto)) \
	{								\
	}

#endif /* backport 38516ab59fbc5b3bb278cf5e1fe2867c70cff32e */

#undef DEFINE_TRACE_FN
#define DEFINE_TRACE_FN(name, reg, unreg)

#undef DEFINE_TRACE
#define DEFINE_TRACE(name)

#undef EXPORT_TRACEPOINT_SYMBOL_GPL
#define EXPORT_TRACEPOINT_SYMBOL_GPL(name)

#undef EXPORT_TRACEPOINT_SYMBOL
#define EXPORT_TRACEPOINT_SYMBOL(name)

#ifdef CONFIG_TRACEPOINTS
#else /* CONFIG_TRACEPOINTS */
#endif /* CONFIG_TRACEPOINTS */

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

#endif /* (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,27)) */

#endif	/* _COMPAT_LINUX_TRACEPOINT_H */
