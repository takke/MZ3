// �ȉ��� ifdef �u���b�N�� DLL ����̃G�N�X�|�[�g��e�Ղɂ���}�N�����쐬���邽�߂� 
// ��ʓI�ȕ��@�ł��B���� DLL ���̂��ׂẴt�@�C���́A�R�}���h ���C���Œ�`���ꂽ MZ3HOOKDLL_EXPORTS
// �V���{���ŃR���p�C������܂��B���̃V���{���́A���� DLL ���g���v���W�F�N�g�Œ�`���邱�Ƃ͂ł��܂���B
// �\�[�X�t�@�C�������̃t�@�C�����܂�ł��鑼�̃v���W�F�N�g�́A 
// MZ3HOOKDLL_API �֐��� DLL ����C���|�[�g���ꂽ�ƌ��Ȃ��̂ɑ΂��A���� DLL �́A���̃}�N���Œ�`���ꂽ
// �V���{�����G�N�X�|�[�g���ꂽ�ƌ��Ȃ��܂��B
#ifdef MZ3HOOKDLL_EXPORTS
#define MZ3HOOKDLL_API __declspec(dllexport)
#else
#define MZ3HOOKDLL_API __declspec(dllimport)
#endif

MZ3HOOKDLL_API int MZ3Hook_StartHook(void);
MZ3HOOKDLL_API int MZ3Hook_SetMainWindow(HWND);
MZ3HOOKDLL_API int MZ3Hook_StopHook(void);
LRESULT CALLBACK MZ3Hook_HookProc(int, WPARAM, LPARAM);
