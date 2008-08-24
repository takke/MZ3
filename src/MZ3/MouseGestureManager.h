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

	int		m_delta;				///< 高解像度ホイール対応のため、delta 値を累積する。
	bool	m_bProcessed;			///< ジェスチャ処理が行われたかどうかのフラグ。
									///< デフォルト処理（右クリックメニュー等）のキャンセルのため。

	enum CMD {
		CMD_NONE  = -1,
		CMD_UP    = 'U',
		CMD_DOWN  = 'D',
		CMD_RIGHT = 'R',
		CMD_LEFT  = 'L',
	};
	std::vector<CMD> m_cmdList;		///< コマンド列
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
