#ifndef MTK_OBJECT_H
#define MTK_OBJECT_H

#include <string.h>
#include "mtk_util.h"

typedef struct mtk_object mtk_object_t;
struct mtk_object {
	struct mtk_object_class *_class;
	mtk_list_t *signals;
};
mtk_object_t* mtk_object_new(size_t size);
void _mtk_object_class_init();
static inline mtk_object_t* mtk_object(void *o) { return o; }
typedef struct mtk_object_class mtk_object_class;
extern struct mtk_object_class _mtk_object_class;
struct mtk_object_class {
	void *_super;
	void (*free)(void *this);
	/* used in connect macro only */
	void (*_register_signal)(void *this, mtk_list_t *siglist);
};

#define CLASS(name, parent) \
	typedef struct name name##_t; \
	struct name { \
		union { \
			struct name##_class *_class; \
			struct parent _parent; \
		};

#define SIGNAL(signame, sigargs...) \
		union { \
			void (*o)(void *this, ## sigargs); \
			void (*f)(sigargs); \
		} _##signame; \
		mtk_list_t *_##signame##_list;

#define METHODS(name, parent, newargs...) \
	}; \
	name##_t* name##_new(size_t size, ## newargs); \
	void _##name##_class_init(); \
	static inline name##_t* name(void *o) { return o; } \
	typedef struct name##_class name##_class; \
	extern name##_class _##name##_class; \
	struct name##_class { \
		union { \
			struct parent##_class *_super; \
			parent##_class; \
		};

#define END };

#define METHOD_TABLE_INIT(name, parent) \
	name##_class _##name##_class; \
	void _##name##_class_init() { \
		name##_class *_class = &_##name##_class; \
		memcpy(_class, &_##parent##_class, sizeof(parent##_class)); \
		_class->_super = &_##parent##_class;
#define _METHOD(method,func) _class->method = func
#define METHOD(method) _METHOD(method,method)
#define METHOD_TABLE_END }

#define SET_CLASS(this, class) (this->_class = &_##class##_class)

#define new(name,args...) \
	name##_new(sizeof(name##_t), ## args)
#define super(obj,name,func,args...) \
	_##name##_class._super->func((obj), ## args)
#define call(obj,func,args...) \
	(obj)->_class->func((obj), ## args)
#define call_defined(obj,func) \
	((obj)->_class->func != NULL)

struct _signalcall {
	void *object;
	void *function;
};

#define emit(obj,signal,args...) \
	do { \
		struct _signalcall *_data; \
		if (!(obj)->_##signal##_list) \
			break; \
		mtk_list_foreach((obj)->_##signal##_list, _data) { \
			(obj)->_##signal.f = _data->function; \
			if (_data->object) \
				(obj)->_##signal.o(_data->object, ## args); \
			else \
				(obj)->_##signal.f(args); \
		} \
	} while(0)

#define _connect(sobj,signal,data) \
	if (!(sobj)->_##signal##_list) { \
		(sobj)->_##signal##_list = mtk_list_new(); \
		/* _register_signal hands mtk_object the list so all \
		 * _signalcall structs can be freed later */ \
		call((sobj),_register_signal, (sobj)->_##signal##_list); \
	} \
	mtk_list_append((sobj)->_##signal##_list, _data); \

/* Connect a signal to an object's method */
#define connect(sobj,signal,dobj,func) \
	do { \
		struct _signalcall *_data = \
			xmalloc(sizeof(struct _signalcall)); \
		/* assignment is just needed as a type check */ \
		(sobj)->_##signal.o = (dobj)->_class->func; \
		_data->object = (dobj); \
		_data->function = (dobj)->_class->func; \
		_connect(sobj,signal,_data) \
	} while(0)

/* Connect a signal to a function */
#define connectf(sobj,signal,func) \
	do { \
		struct _signalcall *_data = \
			xmalloc(sizeof(struct _signalcall)); \
		/* assignment is just needed as a type check */ \
		(sobj)->_##signal.f = func; \
		_data->object = NULL; \
		_data->function = func; \
		_connect(sobj,signal,_data) \
	} while(0)

#endif
