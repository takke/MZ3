#pragma once

#include <vector>

// CForcedDebugDlg ダイアログ

class CForcedDebugDlg : public CDialog
{
	DECLARE_DYNAMIC(CForcedDebugDlg)

public:
	CForcedDebugDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CForcedDebugDlg();

// ダイアログ データ
	enum { IDD = IDD_DEBUG2_DIALOG };

private:

	class Object {
	public:
		int speed;
		int x, y, w, h;

		Object() : speed(1), x(0), y(0), w(20), h(10) {}
		Object( int ax, int ay ) : speed(1), x(ax), y(ay), w(20), h(10) {}
		
		bool hitTest( int tx, int ty ) {
			if( x <= tx && tx <= x+w && y <= ty && ty <= y+h )
			{
				return true;
			}
			return false;
		}

		void move( int dx, int dy ) {
			x += dx;
			y += dy;
		}
	};

	class Force : public Object {
	public:
		int dx, dy;

		Force() : dx(2), dy(2) {}

		bool init(int sw, int sh) {
			x = sw/2;
			y = sh-100;
			return true;
		}

		void moveRight() {
			Object::move(dx,0);
		}

		void moveLeft() {
			Object::move(-dx,0);
		}

		int left()   const { return x-w/2; }
		int right()  const { return x+w/2; }
		int top()    const { return y-h/2; }
		int bottom() const { return y+h/2; }
	};
	Force force;

	class Ball : public Object {
	public:
		bool alive;
		int dx, dy;

		Ball() : alive(false), dx(2), dy(2) {}

		void move() {
			Object::move(dx,dy);
		}
	};
#define BALL_MAX 100
	std::vector<Ball> balls;

	bool drawing;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

	static unsigned int Ticker( LPVOID This );
	bool m_bStop;
	afx_msg void OnDestroy();
	bool NextFrame(void);
	afx_msg void OnPaint();
protected:
	virtual void OnOK();
};
