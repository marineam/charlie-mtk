#include <stdlib.h>
#include <mtk.h>

mtk_object_class mtk_object;

mtk_object_t* mtk_object_new(size_t size)
{
	mtk_object_t *this = xmalloc0(size);
	SET_CLASS(this, mtk_object);
	return this;
}

static void objfree(mtk_object_t* obj)
{
	free(obj);
}

void _mtk_object_class_init()
{
	mtk_object.free = objfree;
}
