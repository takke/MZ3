#pragma once

/**
 * Advanced/W-ZERO3[es] の Xcrawl 制御クラス
 *
 * Xcrawl の N 以下の連続操作をキャンセルする。
 *
 * 連続操作とみなす時間を setContinueLimitMsec() で設定できる。
 *
 * キャンセルする回数を setCancelContinueLimitCount() で設定できる。
 */
class XcrawlCanceler
{
	bool	bEnableXcrawl_;			///< Xcrawl 有効状態。WM_KEYDOWN, VK_F20 で ON に、WM_KEYUP, VK_F20 で OFF にする。
	DWORD	dwLastXcrawl_;			///< 前回 Xcrawl が操作された時刻。つまり、前回 bEnableXcrawl_ が true になった時刻。
	int		nContinue_;				///< Xcrawl での連続操作回数
	int		nXcrawlCancelLimit_;	///< Xcrawl での連続操作を無効化する回数
	DWORD	dwContinueLimitMsec_;	///< 連続操作とみなす時間 [msec]

public:
	/**
	 * コンストラクタ
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
	 * Xcrawl によるスクロール状態か否か
	 */
	bool isXcrawlEnabled() {
		return bEnableXcrawl_;
	}

	/**
	 * WM_KEYUP 時の処理。
	 *
	 * @return true:キャンセルすべき状態。呼び出し元で VK_UP, VK_DOWN 操作を無効にすること。<br />
	 *         false: Xcrawl の対象キー操作ではないか、スルー状態。
	 */
	bool procKeyup( WORD key ) {
		if( !bEnableXcrawl_ ) {
			// Xcrawl のキー操作ではないので終了
			return false;
		}

		switch (key) {
		case VK_F20:	// 0x83
			// Xcrawl 操作中フラグを OFF に。
			bEnableXcrawl_ = false;
			return false;

		case VK_UP:
		case VK_DOWN:
			if( nContinue_ <= nXcrawlCancelLimit_ ) {
				// N 回目以内なので処理しない（キャンセラ有効）
//				MZ3LOGGER_INFO( L"Xcrawl canceled" );
				return true;
			}else{
				// N+1 回目以上なので、スルー
//				MZ3LOGGER_INFO( L"Xcrawl enable!" );
				return false;
			}

		default:
			return false;
		}
	}

	/**
	 * WM_KEYDOWN 時の処理
	 *
	 * Xcrawl の処理が
	 *
	 * @return true:キャンセルすべき状態。<br />
	 *         false: Xcrawl の対象キー操作ではないか、スルー状態。
	 */
	bool procKeydown( WORD key ) {
		if( key == VK_F20 ) {
//			MZ3LOGGER_DEBUG( L"Xcrawl on" );

			// Xcrawl 捜査中フラグを ON に。
			bEnableXcrawl_ = true;

			// 前回から N msec 以上経っていれば、回数リセット
			DWORD dwNow = GetTickCount();
			if( dwNow >= dwLastXcrawl_ + dwContinueLimitMsec_ ) {
				nContinue_ = 1;
			}else{
				// N msec 以内なので連続操作と判定。
				nContinue_ ++;
			}

			dwLastXcrawl_ = dwNow;
			return true;
		}
		return false;
	}


};