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
