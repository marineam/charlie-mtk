#ifndef MTK_OBJECT_H
#define MTK_OBJECT_H

#include <string.h>

typedef struct mtk_object mtk_object_t;
struct mtk_object {
	struct mtk_object_class *_class;
};
mtk_object_t* mtk_object_new(size_t size);
void _mtk_object_class_init();
typedef struct mtk_object_class mtk_object_class;
extern struct mtk_object_class _mtk_object_class;
struct mtk_object_class {
	void *_super;
	void (*free)(mtk_object_t* obj);
};

#define MTK_OBJECT(o) ((mtk_object_t*)o)

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
#define METHOD(name,func) ((name##_class*)_class)->func
#define METHOD_TABLE_END }

#define SET_CLASS(this, class) (this->_class = &_##class##_class)

#define new(name,args...) \
	name##_new(sizeof(name##_t), ## args)
/* FIXME: super is kinda broken */
#define super(obj,name,super,func,args...) \
	_##name##_class._super->func((super##_t*)(obj), ## args)
#define call(obj,name,func,args...) \
	(obj)->_class->func((name##_t*)(obj), ## args)
#define call_defined(obj,name,func) \
	((obj)->_class->func != NULL)

#endif
