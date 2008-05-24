#ifndef MTK_OBJECT_H
#define MTK_OBJECT_H

#include <string.h>

typedef struct mtk_object mtk_object_t;
struct mtk_object {
	struct mtk_object_class *_class;
};
mtk_object_t* mtk_object_new(size_t size);
void _mtk_object_class_init();
static inline mtk_object_t* mtk_object(void *o) { return o; }
typedef struct mtk_object_class mtk_object_class;
extern struct mtk_object_class _mtk_object_class;
struct mtk_object_class {
	void *_super;
	void (*free)(void *this);
};

#define CLASS(name, parent) \
	typedef struct name name##_t; \
	struct name { \
		union { \
			struct name##_class *_class; \
			struct parent _parent; \
		};

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

#endif
