#pragma once

namespace util {

/**
 * XML の分解ユーティリティ
 */
class XmlParser
{
public:
	/// 要素抜きだし
	static inline CString GetElement(const CString& str, int num)
	{
		int pos = 0;
		CString buf;
		int count = 0;
		buf = str.Tokenize(_T("<>"), pos);
		while (buf != "") {
			count++;
			if (buf != "" && count == num) {
				break;
			}
			buf = str.Tokenize(_T("<>"), pos);
		}

		return buf;
	}
	/// 指定属性取りだし
	static inline CString GetAttribute(const CString& str, CString attr)
	{
		CString strRet;
		CString buf;

		int sIndex = 0;
		int eIndex = 0;
		sIndex = str.Find(attr) + attr.GetLength();
		eIndex = str.GetLength();

		buf = str.Mid(sIndex, (eIndex - sIndex));

		// 最初の空白を探し出して、そこまでを抜き出す
		if (buf.Find(_T(" ")) == -1) {
			strRet = buf;
		}
		else {
			strRet = buf.Left(buf.Find(_T(" ")));
		}

		return strRet;
	}
};

}