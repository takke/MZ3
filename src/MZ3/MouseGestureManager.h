/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

class MouseGestureManager
{
public:
	bool	m_bInGestureMode;
	CPoint	m_posStart;

	int		m_delta;				///< ���𑜓x�z�C�[���Ή��̂��߁Adelta �l��ݐς���B
	bool	m_bProcessed;			///< �W�F�X�`���������s��ꂽ���ǂ����̃t���O�B
									///< �f�t�H���g�����i�E�N���b�N���j���[���j�̃L�����Z���̂��߁B

	MouseGestureManager()
		: m_bInGestureMode(false)
		, m_delta(0)
		, m_bProcessed(false)
	{
	}

	void StartGestureMode(const CPoint& pos) {
		m_bInGestureMode = true;
		m_posStart = pos;
		m_delta = 0;
		m_bProcessed = false;
	}

	void StopGestureMode()	{
		m_bInGestureMode = false;
	}

	bool IsGestureMode() {
		if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) {
			m_bInGestureMode = false;
		}
		return m_bInGestureMode;
	}

	bool IsProcessed() {
		if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) {
			m_bInGestureMode = false;
		}
		return m_bProcessed;
	}

	void SetProcessed() {
		m_bProcessed = true;
	}
};
