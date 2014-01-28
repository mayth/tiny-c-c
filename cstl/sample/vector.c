#include <stdio.h>
#include <cstl/vector.h>

CSTL_VECTOR_INTERFACE(CharVector, char)
CSTL_VECTOR_IMPLEMENT(CharVector, char)

int main(void)
{
	CharVector *x = CharVector_new();
	CharVector_insert_array(x, 0, "aaaa", 4);
	CharVector_push_back(x, '\0');
	printf("%s\n", CharVector_at(x, 0));
	printf("size:%d\n", CharVector_size(x));

	CharVector_pop_back(x);
	CharVector_insert_array(x, CharVector_size(x), "bbbb", 4);
	CharVector_push_back(x, '\0');
	printf("%s\n", CharVector_at(x, 0));
	printf("size:%d\n", CharVector_size(x));

	*CharVector_at(x, 0) = 'c';
	*CharVector_at(x, 1) = 'd';
	*CharVector_at(x, 4) = 'e';
	*CharVector_at(x, 5) = 'f';
	printf("%s\n", CharVector_at(x, 0));

	CharVector_erase(x, 2, 4);
	printf("%s\n", CharVector_at(x, 0));

	CharVector_insert(x, 0, 'g');
	CharVector_insert(x, 0, 'h');
	CharVector_insert(x, 0, 'i');
	CharVector_insert_array(x, 0, "jklm", 4);
	printf("%s\n", CharVector_at(x, 0));

	CharVector_delete(x);
	return 0;
}
