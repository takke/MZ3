/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once
#include <vector>

/// 慣性スクロール情報
struct MouseMoveInfo {
	DWORD	tick;	///< 時刻
	CPoint	pt;		///< 位置

	MouseMoveInfo( DWORD tick_, const CPoint& pt_ )
		: tick(tick_)
		, pt(pt_)
	{
	}
};

/// 慣性スクロール管理クラス
class AutoScrollManager {
public:
	std::vector<MouseMoveInfo> m_moveInfoList;	///< 移動情報のログ

	/// 移動情報クリア
	void clear() {
		m_moveInfoList.clear();
	}

	/// 位置追加
	void push( DWORD tick, const CPoint& pt ) {
		m_moveInfoList.push_back( MouseMoveInfo(tick,pt) );
		while (m_moveInfoList.size()>4) {
			m_moveInfoList.erase( m_moveInfoList.begin() );
		}
	}

	/// 縦方向速度算出
	double calcMouseMoveSpeedY() {
		double speed = 0.0;

		if (m_moveInfoList.size()>=2) {
			MouseMoveInfo& p1 = m_moveInfoList[0];
			MouseMoveInfo& p2 = m_moveInfoList[m_moveInfoList.size()-1];

			int dt = p2.tick - p1.tick;
			if (dt>0) {
				speed = (double)(p2.pt.y - p1.pt.y) / dt;
			}
		}

		return speed;
	}

	/// 縦方向加速度算出
	double calcMouseMoveAccelY() {
		double accel = 0.0;

		if (m_moveInfoList.size()>=2) {
			MouseMoveInfo& p1 = m_moveInfoList[0];
			MouseMoveInfo& p2 = m_moveInfoList[m_moveInfoList.size()-1];

			int dt = p2.tick - p1.tick;
			if (dt>0) {
				accel = (double)(p2.pt.y - p1.pt.y) / dt / dt;
			}
		}

		return accel;
	}

	/// 最終位置
	CPoint getLastPoint()
	{
		if (m_moveInfoList.empty()) {
			return CPoint(0,0);
		} else {
			return m_moveInfoList[ m_moveInfoList.size()-1 ].pt;
		}
	}
};
