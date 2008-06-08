#include <stdlib.h>
#include <mtk.h>

mtk_object_class _mtk_object_class;

mtk_object_t* mtk_object_new(size_t size)
{
	mtk_object_t *this = xmalloc0(size);
	SET_CLASS(this, mtk_object);
	return this;
}

/* Record the location of signal lists of child objects
 * so they can be automatically freed when the object is freed */
static void register_signal(void *vthis, mtk_list_t *siglist) {
	mtk_object_t *this = vthis;

	if (!this->signals)
		this->signals = mtk_list_new();

	if (!mtk_list_contains(this->signals, siglist))
		mtk_list_append(this->signals, siglist);
}

static void objfree(void* vthis)
{
	mtk_object_t *this = vthis;
	mtk_list_t *siglist;
	void *sigdata;

	if (this->signals) {
		mtk_list_foreach(this->signals, siglist) {
			mtk_list_foreach(siglist, sigdata)
				free(sigdata);
			mtk_list_free(siglist);
		}
		mtk_list_free(this->signals);
	}

	free(this);
}

void _mtk_object_class_init()
{
	_mtk_object_class.free = objfree;
	_mtk_object_class._register_signal = register_signal;
}
