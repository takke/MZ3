#include "jectlib.h"
#include <malloc.h>


/*<
	デバッグ用mallocカバー
	
	//$size 新規領域を確保するサイズ
	
	//$$return 確保した領域
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
