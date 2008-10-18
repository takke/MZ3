/**
 * Simple XML Parser, XML to STL Container library.
 *
 * Copyright (C) takke, All rights reserved.
 * Dual licenced under GPL and modified BSD licence.
 */
#include <tchar.h>
#include "xml2stl.h"

namespace xml2stl {

bool SimpleXmlParser::parse_properties( Node& node, kfm::kf_buf_reader<TCHAR>& reader )
{
	while( !reader.is_eof() ) {
		wint_t c = reader.get_char();

		switch( c ) {
		case '/':
			if (reader.get_char() == '>') {
				// NODE END
				return false;
			}
			break;

		case '?':
			if (reader.get_char() == '>') {
				// XML DTD END
				return false;
			}
			break;
			
		case '>':
			// NODE END
			return true;

		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\r\n':
			break;

		default:
			// name="value" が続くはず。
			{
				xml2stl::XML2STL_STRING name, value;
				name.reserve(INITIAL_RESERVED_SIZE_OF_XML2STL_STRING);
				value.reserve(INITIAL_RESERVED_SIZE_OF_XML2STL_STRING);

				// name 取得
				name.push_back(c);
				reader.search_until_target('=', &name);

				// value 取得
				// 1文字目は '"' 以外なら追加する
				if (!reader.is_eof()) {
					wint_t c1 = reader.get_char();
					if (c1!='"') {
						value.push_back(c1);
					}
					reader.search_until_target('"', &value);
				}
//				wprintf( L"parse_properties(), name[%s], value[%s]\n", name.c_str(), value.c_str());

				// name, value がともに1文字以上であればプロパティ追加
				if (!name.empty() && !value.empty()) {
					node.addProperty( name, value );
				}
			}
			break;
		}
	}
	return false;
}

/**
 * target があらわれるまで読み飛ばし、その直前までを node_text に設定する
 */
bool SimpleXmlParser::parse_until_target(Container& node, kfm::kf_buf_reader<TCHAR>& reader, const xml2stl::XML2STL_STRING& target )
{
	xml2stl::XML2STL_STRING node_text;
	reader.search_until_target( target.c_str(), &node_text );
	if (!node_text.empty()) {
		node.addText(node_text);
	}
//	wprintf( L"parse_until_target(), target[%s], node_text[%s]\n", target.c_str(), node_text.c_str());
	return true;
/*
	xml2stl::XML2STL_STRING node_text, temp;
	node_text.reserve(INITIAL_RESERVED_SIZE_OF_XML2STL_STRING);
	temp.reserve(INITIAL_RESERVED_SIZE_OF_XML2STL_STRING);

	while(! reader.is_eof() ) {
		wint_t c = reader.get_char();

		if (temp.empty()) {
			if (c==target[0]) {
				temp.push_back(c);
			} else {
				node_text.push_back( c );
			}
		} else {
			temp.push_back( c );

			if (target.compare( 0, temp.size(), temp ) == 0) {
				if (temp.size() == target.size()) {
					// 完了。
					node.addText( node_text );
					wprintf( L"parse_until_target(), target[%s], node_text[%s]\n", target.c_str(), node_text.c_str());
					return true;
				} else {
					// 継続
				}
			} else {
				// 不一致。
				node_text += temp;
				temp = L"";
			}
		}
	}

	return true;
*/
}

bool SimpleXmlParser::parse_node( Container& node, kfm::kf_buf_reader<TCHAR>& reader )
{
	PARSE_STATE state = PARSE_STATE_SEARCHING;

	// lazy モードの場合は、いくつかのタグを HTML 風に解析するため、下位解析をキャンセルする。
	if (m_bLazyParseMode) {
		const xml2stl::XML2STL_STRING& node_name = node.getName();
		if (node_name==L"img") {
			return true;
		}
		if (node_name==L"script") {
			// script タグ専用解析。
			// "</script>" まで読み飛ばす
			return parse_until_target( node, reader, L"</script>" );
		}
		if (node_name==L"style") {
			// style タグ専用解析。
			// "</style>" まで読み飛ばす
			return parse_until_target( node, reader, L"</style>" );
		}
	}

	xml2stl::XML2STL_STRING node_name, node_text;
	node_name.reserve(INITIAL_RESERVED_SIZE_OF_XML2STL_STRING);
	node_text.reserve(INITIAL_RESERVED_SIZE_OF_XML2STL_STRING);

	while(! reader.is_eof() ) {
		wint_t c = reader.get_char();

		switch (state) {
		case PARSE_STATE_SEARCHING:
			if (c!='<') {
				// 1文字目が '<' 以外なので '<' があらわれるまで読み飛ばし、それまでの文字列を node_text とする。
				node_text.push_back(c);
				if (!reader.search_until_target('<', &node_text)) {
					return true;
				}
			} else {
				// 1文字目がすでに '<' なのでノード解析開始
			}
			// NODE START
			state = PARSE_STATE_IN_NODE_NAME;
			// テキストがあれば登録する。
			if (!node_text.empty()) {
				node.addText( node_text );
				node_text = L"";
			}
			node_name = L"";
			break;

		case PARSE_STATE_IN_NODE_NAME:
			// "<..." の解析状態
			switch (c) {
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\r\n':
				// NODE NAME END
				// プロパティ解析を行う。
				{
					Node& newNode = node.addNode(node_name);
					node_name = L"";
					bool r = parse_properties( newNode, reader );
					if (r) {
						// "/>" で終わらなかったので、次ノード解析
						parse_node( newNode, reader );
					}
					state = PARSE_STATE_SEARCHING;
//					node_text = L"";
				}
				break;
			case '>':
				// NODE END
				{
					Node& newNode = node.addNode(node_name);

					// 次ノード解析
					parse_node( newNode, reader );
					state = PARSE_STATE_SEARCHING;
//					node_text = L"";
				}
				break;
			case '/':
				if (reader.get_char()=='>') {
					// NODE END, like <xxx/>
					node.addNode(node_name);
					state = PARSE_STATE_SEARCHING;
//					node_text = L"";
				} else {
					// </xxx> 状態。
					// '>' が現れるまでパースし、TEXT を設定して終了。
					if (reader.search_until_target('>', NULL)) {
						// 子要素があればTEXTを設定しない。
						if (m_bLazyParseMode) {
							if (!node_text.empty()) {
								node.addText(node_text);
							}
						} else {
							if (!node.hasChildren()) {
								if (!node_text.empty()) {
									node.addText(node_text);
								}
							}
						}

						// 必要であれば、node.name とこれまでのc値との文字列比較を行うこと。
						return true;
					}

					return false;
				}
				break;
			case '!':
				// 1文字目であれば、DTD 宣言or CDATA とみなし、読み飛ばす。
				/*
<!DOCTYPE html
PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<![CDATA[  
<center>...</center>
]]>
				*/
				if (node_name.size()==0) {
					// DTD 宣言 or CDATA
					// DTD : '>' が現れるまでパースし、継続。
					// CDATA : ]]> が現れるまでパースし、要素化。
					for (int xpos=0; !reader.is_eof(); xpos++) {
						wint_t c = reader.get_char();

						if (xpos==0 && c=='[') {
							// CDATA
							node_text = L"";
							node_name = L"";
							// [ まで読み飛ばす
							reader.search_until_target('[', NULL);

							// ]]> まで読み飛ばす
							parse_until_target( node, reader, L"]]>" );

							// clear state
							state = PARSE_STATE_SEARCHING;
							node_name = L"";
							node_text = L"";

							break;
						}
						if (c=='>') {
							state = PARSE_STATE_SEARCHING;
							node_name = L"";
							node_text = L"";
							break;
						}
					}
				} else {
					// エラー
				}
				break;
			default:
				node_name.push_back( c );
				break;
			}
			break;
		}
	}

	return true;
}

}
