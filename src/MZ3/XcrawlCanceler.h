#pragma once

/**
 * Advanced/W-ZERO3[es] �� Xcrawl ����N���X
 *
 * Xcrawl �� N �ȉ��̘A��������L�����Z������B
 *
 * �A������Ƃ݂Ȃ����Ԃ� setContinueLimitMsec() �Őݒ�ł���B
 *
 * �L�����Z������񐔂� setCancelContinueLimitCount() �Őݒ�ł���B
 */
class XcrawlCanceler
{
	bool	bEnableXcrawl_;			///< Xcrawl �L����ԁBWM_KEYDOWN, VK_F20 �� ON �ɁAWM_KEYUP, VK_F20 �� OFF �ɂ���B
	DWORD	dwLastXcrawl_;			///< �O�� Xcrawl �����삳�ꂽ�����B�܂�A�O�� bEnableXcrawl_ �� true �ɂȂ��������B
	int		nContinue_;				///< Xcrawl �ł̘A�������
	int		nXcrawlCancelLimit_;	///< Xcrawl �ł̘A������𖳌��������
	DWORD	dwContinueLimitMsec_;	///< �A������Ƃ݂Ȃ����� [msec]

public:
	/**
	 * �R���X�g���N�^
	 */
	XcrawlCanceler() : bEnableXcrawl_(false), dwLastXcrawl_(0), nContinue_(0), nXcrawlCancelLimit_(1), dwContinueLimitMsec_(300) {
	}

	void setContinueLimitMsec( DWORD msec ) {
		dwContinueLimitMsec_ = msec;
	}

	void setCancelContinueLimitCount( int count ) {
		nXcrawlCancelLimit_ = count;
	}

	/**
	 * Xcrawl �ɂ��X�N���[����Ԃ��ۂ�
	 */
	bool isXcrawlEnabled() {
		return bEnableXcrawl_;
	}

	/**
	 * WM_KEYUP ���̏����B
	 *
	 * @return true:�L�����Z�����ׂ���ԁB�Ăяo������ VK_UP, VK_DOWN ����𖳌��ɂ��邱�ƁB<br />
	 *         false: Xcrawl �̑ΏۃL�[����ł͂Ȃ����A�X���[��ԁB
	 */
	bool procKeyup( WORD key ) {
		if( !bEnableXcrawl_ ) {
			// Xcrawl �̃L�[����ł͂Ȃ��̂ŏI��
			return false;
		}

		switch (key) {
		case VK_F20:	// 0x83
			// Xcrawl ���쒆�t���O�� OFF �ɁB
			bEnableXcrawl_ = false;
			return false;

		case VK_UP:
		case VK_DOWN:
			if( nContinue_ <= nXcrawlCancelLimit_ ) {
				// N ��ڈȓ��Ȃ̂ŏ������Ȃ��i�L�����Z���L���j
//				MZ3LOGGER_INFO( L"Xcrawl canceled" );
				return true;
			}else{
				// N+1 ��ڈȏ�Ȃ̂ŁA�X���[
//				MZ3LOGGER_INFO( L"Xcrawl enable!" );
				return false;
			}

		default:
			return false;
		}
	}

	/**
	 * WM_KEYDOWN ���̏���
	 *
	 * Xcrawl �̏�����
	 *
	 * @return true:�L�����Z�����ׂ���ԁB<br />
	 *         false: Xcrawl �̑ΏۃL�[����ł͂Ȃ����A�X���[��ԁB
	 */
	bool procKeydown( WORD key ) {
		if( key == VK_F20 ) {
//			MZ3LOGGER_DEBUG( L"Xcrawl on" );

			// Xcrawl �{�����t���O�� ON �ɁB
			bEnableXcrawl_ = true;

			// �O�񂩂� N msec �ȏ�o���Ă���΁A�񐔃��Z�b�g
			DWORD dwNow = GetTickCount();
			if( dwNow >= dwLastXcrawl_ + dwContinueLimitMsec_ ) {
				nContinue_ = 1;
			}else{
				// N msec �ȓ��Ȃ̂ŘA������Ɣ���B
				nContinue_ ++;
			}

			dwLastXcrawl_ = dwNow;
			return true;
		}
		return false;
	}


};