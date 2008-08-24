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

	enum CMD {
		CMD_NONE  = -1,
		CMD_UP    = 'U',
		CMD_DOWN  = 'D',
		CMD_RIGHT = 'R',
		CMD_LEFT  = 'L',
	};
	std::vector<CMD> m_cmdList;		///< �R�}���h��
	CPoint	m_posLastCmd;

	MouseGestureManager()
		: m_bInGestureMode(false)
		, m_delta(0)
		, m_bProcessed(false)
	{
	}

	CMD GetLastCmd() const {
		if (m_cmdList.empty()) {
			return CMD_NONE;
		}
		return m_cmdList.back();
	}

	std::string GetCmdList() const {
		std::string s;
		for (size_t i=0; i<m_cmdList.size(); i++) {
			s.push_back(m_cmdList[i]);
		}
		return s;
	}

	void StartGestureMode(const CPoint& pos) {
		m_bInGestureMode = true;
		m_posStart = pos;
		m_posLastCmd = pos;
		m_delta = 0;
		m_bProcessed = false;
		m_cmdList.clear();
	}

	void StopGestureMode()	{
		m_bInGestureMode = false;
		m_cmdList.clear();
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
