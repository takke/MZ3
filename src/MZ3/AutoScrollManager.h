/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once
#include <vector>

/// �����X�N���[�����
struct MouseMoveInfo {
	DWORD	tick;	///< ����
	CPoint	pt;		///< �ʒu

	MouseMoveInfo( DWORD tick_, const CPoint& pt_ )
		: tick(tick_)
		, pt(pt_)
	{
	}
};

/// �����X�N���[���Ǘ��N���X
class AutoScrollManager {
public:
	std::vector<MouseMoveInfo> m_moveInfoList;	///< �ړ����̃��O

	/// �ړ����N���A
	void clear() {
		m_moveInfoList.clear();
	}

	/// �ʒu�ǉ�
	void push( DWORD tick, const CPoint& pt ) {
		m_moveInfoList.push_back( MouseMoveInfo(tick,pt) );
		while (m_moveInfoList.size()>4) {
			m_moveInfoList.erase( m_moveInfoList.begin() );
		}
	}

	/// �c�������x�Z�o
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

	/// �c���������x�Z�o
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

	/// �ŏI�ʒu
	CPoint getLastPoint()
	{
		if (m_moveInfoList.empty()) {
			return CPoint(0,0);
		} else {
			return m_moveInfoList[ m_moveInfoList.size()-1 ].pt;
		}
	}
};
