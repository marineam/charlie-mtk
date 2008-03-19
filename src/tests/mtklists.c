#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <mtk.h>

int main(int argc, char *argv[])
{
	mtk_list_t* list = mtk_list_new();
	char data1[4][10] = {
		"Record A",
		"Record B",
		"Record C",
		"Record D"
		};
	char data2[4][10] = {
		"Record 1",
		"Record 2",
		"Record 3",
		"Record 4"
		};
	char *r;
	int i;

	mtk_list_append(list, data1[0]);
	mtk_list_append(list, data1[1]);
	mtk_list_append(list, data1[2]);
	mtk_list_append(list, data1[3]);

	i = 0;
	mtk_list_foreach(list, r) {
		printf("%d: %s == %s\n",i, r, data1[i]);
		if (r != data1[i])
			abort();
		i++;
	}

	printf("%d == 4\n", i);
	assert(i == 4);

	mtk_list_goto(list,0);
	mtk_list_replace(list, data2[0]);
	mtk_list_next(list);
	mtk_list_replace(list, data2[1]);
	mtk_list_next(list);
	mtk_list_replace(list, data2[2]);
	mtk_list_next(list);
	mtk_list_replace(list, data2[3]);

	i = 0;
	mtk_list_foreach(list, r) {
		printf("%d: %s == %s\n",i, r, data2[i]);
		if (r != data2[i])
			abort();
		i++;
	}

	printf("%d == 4\n", i);
	assert(i == 4);

	return 0;
}
