#include "jectlib.h"
#include <malloc.h>


/*<
	�f�o�b�O�pmalloc�J�o�[
	
	//$size �V�K�̈���m�ۂ���T�C�Y
	
	//$$return �m�ۂ����̈�
*/
void* _db_malloc(u_int size)
{
#undef malloc
#ifdef __BORLANDC__
	if(size == 0) {size++;}
#endif // __BORLANDC__
	return(malloc(size));
#define malloc(size) _db_malloc((u_int)size)
}


//%zenryaku
//@2003/07/14
