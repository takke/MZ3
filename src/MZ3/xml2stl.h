/**
 * Simple XML Parser, XML to STL Container library.
 *
 * Copyright (C) takke, All rights reserved.
 * Dual licenced under GPL and modified BSD licence.
 *
 * TODO:
 *   - entity decoding.
 *   - supports XPath subset, like "feed/entry/link/@href"
 */
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include "kfm_buffer.h"

namespace xml2stl {

typedef std::wstring XML2STL_STRING;

enum XML2STL_STATE
{
    XML2STL_STATE_SEARCH_TAG,
    XML2STL_STATE_SEARCH_PROPERTY,
};

enum XML2STL_TYPE
{
    XML2STL_TYPE_ROOTNODE,  ///< ROOT
    XML2STL_TYPE_NODE,      ///< NODE
    XML2STL_TYPE_TEXT,      ///< TEXT
};

class Container;
typedef Container Node;

class NodeNotFoundException
{
	XML2STL_STRING message;

public:
	NodeNotFoundException( const XML2STL_STRING& message_ )
		: message(message_)
	{
	}

	const XML2STL_STRING& getMessage() const 
	{
		return message;
	}
};

class Property
{
public:
    XML2STL_STRING name;
    XML2STL_STRING text;

	Property()
	{}

	Property(const XML2STL_STRING& name_, const XML2STL_STRING& text_)
		: name(name_), text(text_)
	{}
};

typedef std::vector<Property>	PropertyList;
typedef std::vector<Node>		NodeList;

class Container
{
private:
	XML2STL_TYPE	type;
	XML2STL_STRING  name_or_text;	///< NODE ならノード名、TEXT なら文字列

	PropertyList	properties;
	NodeList		children;

public:
	Container() : type(XML2STL_TYPE_ROOTNODE) 
	{}

	XML2STL_TYPE getType() const {
		return type;
	}

	/**
	 * ノード名の取得
	 */
	const XML2STL_STRING& getName() const {
		if (isNode()) {
			return name_or_text;
		} else {
			// NODE以外なので取得禁止
			static XML2STL_STRING empty_text = L"";
			return empty_text;
		}
	}

	const XML2STL_STRING getText1() const {
		if (isText()) {
			return name_or_text;
		} else {
			// TEXT以外なので取得禁止
			static XML2STL_STRING empty_text = L"";
			return empty_text;
		}
	}

	const XML2STL_STRING getTextAll() const {
		// 下位の全ノードを重ねてテキスト化して返す
		size_t n = children.size();
		XML2STL_STRING  text;
		for (size_t i=0; i<n; i++) {
			const Node& targetNode = children[i];
			switch (targetNode.type) {
			case XML2STL_TYPE_NODE:
				// プロパティと下位ノードを TEXT 化
				text += L"<";
				text += targetNode.name_or_text;
				// プロパティを TEXT 化
				{
					size_t n_property=targetNode.properties.size();
					for (size_t j=0; j<n_property; j++) {
						const Property& prop = targetNode.getProperty(j);
						text += L" ";
						text += prop.name;
						text += L"=\"";
						text += prop.text;
						text += L"\"";
					}
				}

				if (targetNode.children.empty()) {
					text += L" />";
				} else {
					text += L">";
					// 下位ノード(再帰)
					text += targetNode.getTextAll();
					text += L"</";
					text += targetNode.name_or_text;
					text += L">";
				}
				break;
			case XML2STL_TYPE_TEXT:
				text += targetNode.name_or_text;
				break;
			}
		}
		return text;
	}

	const NodeList& getChildren() const {
		return children;
	}

	const PropertyList& getProperties() const {
		return properties;
	}

	size_t getChildrenCount() const {
		return children.size();
	}

	bool hasChildren() const
	{
		return children.empty() ? false : true;
	}

	bool isNode() const	{ return type == XML2STL_TYPE_NODE; }
	bool isText() const	{ return type == XML2STL_TYPE_TEXT; }

	Node& addNode( const XML2STL_STRING& name )
	{
		Node node;
		node.name_or_text = name;
		node.type = XML2STL_TYPE_NODE;

		children.push_back( node );

		return children[ children.size()-1 ];
	}

	Node& addText( const XML2STL_STRING& text=_T("") )
	{
		Node node;
		node.name_or_text = text;
		node.type = XML2STL_TYPE_TEXT;

		children.push_back( node );

		return children[ children.size()-1 ];
	}

	Property& addProperty( const XML2STL_STRING& name, const XML2STL_STRING& text )
	{
		Property prop;
		prop.name = name;
		prop.text = text;

		properties.push_back( prop );

		return properties[ properties.size()-1 ];
	}

	const Property& getProperty( size_t index ) const
	{
		return properties[ index ];
	}

	const XML2STL_STRING& getProperty( const XML2STL_STRING& name ) const
	{
		size_t n = properties.size();
		for (size_t i=0; i<n; i++) {
			if (properties[i].name == name) {
				return properties[i].text;
			}
		}

		std::wostringstream stream;
		stream << L"property not found... name[" << name << L"]";
		throw NodeNotFoundException(stream.str());
		// TODO: use another exception.
	}

	const Node& getNode( size_t index ) const
	{
		if (index >= children.size()) {
			std::wostringstream stream;
			stream << L"node not found... index[" << index << L"]";
			throw NodeNotFoundException(stream.str());
		}
		return children[ index ];
	}

	const Node& getNode( const XML2STL_STRING& name, int index=0 ) const
	{
		int nFound = 0;
		size_t n = children.size();
		for (size_t i=0; i<n; i++) {
			if (children[i].type == XML2STL_TYPE_NODE &&
				children[i].name_or_text == name) 
			{
				if (nFound==index) {
					return children[i];
				} else {
					nFound ++;
				}
			}
		}

		std::wostringstream stream;
		stream << L"node not found... name[" << name << L"], index[" << index << L"]";
		_dumpChildren(stream);
		throw NodeNotFoundException(stream.str());
	}

	/**
	 * example : getNode( L"div", L"id=bodyArea" );
	 */
	const Node& getNode( const XML2STL_STRING& name, const XML2STL_STRING& prop_name_value ) const
	{
		XML2STL_STRING::size_type idxEq = prop_name_value.find( '=' );
		if (idxEq==XML2STL_STRING::npos) {
			std::wostringstream stream;
			stream << L"internal error. at getNode, name[" << name << L"], prop_name_value[" << prop_name_value << L"]";
			_dumpChildren(stream);
			throw NodeNotFoundException(stream.str());
		}

		// 委譲
		XML2STL_STRING prop_name  = prop_name_value.substr( 0, idxEq );
		XML2STL_STRING prop_value = prop_name_value.substr( idxEq+1 );
		return getNode( name, Property(prop_name, prop_value) );
	}

	const Node& getNode( const XML2STL_STRING& name, const Property& prop ) const
	{
		size_t n = children.size();
		for (size_t i=0; i<n; i++) {
			if (children[i].type == XML2STL_TYPE_NODE &&
				children[i].name_or_text == name) 
			{
				try {
					if (children[i].getProperty(prop.name) == prop.text) {
						return children[i];
					}
				} catch( NodeNotFoundException& ) {
					// not found, ok.
				}
			}
		}

		std::wostringstream stream;
		stream << L"node not found... name[" << name << L"], property[" << prop.name + L"=" << prop.text << L"]";
		_dumpChildren(stream);
		throw NodeNotFoundException(stream.str());
	}

//	void setText( const XML2STL_STRING& text )
//	{
//		this->name_or_text = text;
//	}

private:
	void _dumpChildren(std::wostringstream& stream) const
	{
		stream << L" children[";

		bool bDumpFirst = true;
		size_t n = children.size();
		for (size_t i=0; i<n; i++) {
			if (children[i].type == XML2STL_TYPE_NODE) {
				if (!bDumpFirst) {
					stream << L"\r\n";
				}
				stream << L"<";
				stream << children[i].name_or_text;
				if (!children[i].properties.empty()) {
					stream << L" ";
					size_t nProperties = children[i].properties.size();
					for (size_t j=0; j<nProperties; j++) {
						if (j>0) {
							stream << L" ";
						}
						stream << children[i].properties[j].name.c_str();
						stream << L"=\"";
						stream << children[i].properties[j].text.c_str();
						stream << L"\"";
					}
				}
				stream << L">";

				bDumpFirst = false;
			}
		}

		stream << L"]";
	}
};

inline void dump_nest( FILE* fp, int nest_level )
{
    for (int i=0; i<nest_level*2; i++) {
        fputc( ' ', fp );
    }
}

inline void dump_container( const Container& c, FILE* fp, int nest_level=0 )
{
    switch (c.getType()) {
        case XML2STL_TYPE_ROOTNODE:
            dump_nest( fp, nest_level );
            fwprintf( fp, _T("+ROOT\n"), c.getName().c_str() );
            break;
        case XML2STL_TYPE_NODE:
            dump_nest( fp, nest_level );
            fwprintf( fp, _T("+[%s]\n"), c.getName().c_str() );
            break;
		case XML2STL_TYPE_TEXT:
            dump_nest( fp, nest_level );
			fwprintf( fp, _T("=[%s]\n"), c.getText1().c_str() );
            return;
        default:
            break;
    }

    // プロパティ
    if (!c.getProperties().empty()) {
		size_t n = c.getProperties().size();
        for (size_t i=0; i<n; i++) {
            const Property& p = c.getProperty(i);
            dump_nest( fp, nest_level+1 );
            fwprintf( fp, _T("[%s] => [%s]\n"), p.name.c_str(), p.text.c_str() );
        }
    }

    // 子要素
    if (c.hasChildren()) {
		size_t n = c.getChildren().size();
        for (size_t i=0; i<n; i++) {
            dump_container( c.getNode(i), fp, nest_level+1 );
        }
    }
}

class SimpleXmlParser
{
private:
	bool m_bLazyParseMode;		///< img タグ等を閉じなくてもOKにする

public:

	SimpleXmlParser( bool bLazyParseMode ) 
		: m_bLazyParseMode( bLazyParseMode )
	{
	}

	static bool loadFromFile( Container& root, LPCTSTR filename, bool bLazyParseMode=true )
	{
		FILE* fp = NULL;

		if ((fp = _wfopen( filename, L"rt" )) == NULL) {
			return false;
		}

		std::vector<TCHAR> buffer;
		kfm::kf_buf_writer<TCHAR> writer( buffer );
		while( !feof(fp) ) {
			writer.put_char( fgetwc( fp ) );
		}
		fclose( fp );

		return loadFromText( root, buffer, bLazyParseMode );
	}

	static bool loadFromText( Container& root, const std::vector<TCHAR>& text, bool bLazyParseMode=true )
	{
		kfm::kf_buf_reader<TCHAR> reader( text );

		SimpleXmlParser parser( bLazyParseMode );
		return parser.parse_node( root, reader );
	}

private:
	bool parse_properties( Node& node, kfm::kf_buf_reader<TCHAR>& reader )
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
					xml2stl::XML2STL_STRING name;
					xml2stl::XML2STL_STRING value;
					// name 取得
					name.push_back(c);
					while( !reader.is_eof() ) {
						wint_t c = reader.get_char();

						if (c=='=') {
							break;
						}
						name.push_back(c);
					}

					// value 取得
					for( int i=0; !reader.is_eof(); i++ ) {
						wint_t c = reader.get_char();

						if (i==0) {
							if (c=='"') {
								// skip
							}
						} else {
							if (c=='"') {
								break;
							}
							value.push_back(c);
						}
					}

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

	enum PARSE_STATE
	{
		PARSE_STATE_SEARCHING,
		PARSE_STATE_IN_NODE_NAME,
	};

	bool parse_until_target( Container& node, kfm::kf_buf_reader<TCHAR>& reader, const xml2stl::XML2STL_STRING& target )
	{
		xml2stl::XML2STL_STRING node_text;
		xml2stl::XML2STL_STRING temp;
		while(! reader.is_eof() ) {
			wint_t c = reader.get_char();

			if (temp.empty()) {
				if (c=='<') {
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
	}

	bool parse_node( Container& node, kfm::kf_buf_reader<TCHAR>& reader )
	{
		PARSE_STATE state = PARSE_STATE_SEARCHING;

		// lazy モードの場合は、いくつかのタグを HTML 風に解析するため、下位解析をキャンセルする。
		if (m_bLazyParseMode) {
			if (node.getName()==L"img") {
				return true;
			}
			if (node.getName()==L"script") {
				// script タグ専用解析。
				// "</script>" まで読み飛ばす
				return parse_until_target( node, reader, L"</script>" );
			}
			if (node.getName()==L"style") {
				// style タグ専用解析。
				// "</style>" まで読み飛ばす
				return parse_until_target( node, reader, L"</style>" );
			}
		}

		xml2stl::XML2STL_STRING node_name;
		xml2stl::XML2STL_STRING node_text;

		while(! reader.is_eof() ) {
			wint_t c = reader.get_char();

			switch (state) {
			case PARSE_STATE_SEARCHING:
				switch (c) {
				case '<':
					// NODE START
					state = PARSE_STATE_IN_NODE_NAME;
					// テキストがあれば登録する。
					if (!node_text.empty()) {
						node.addText( node_text );
						node_text = L"";
					}
					node_name = L"";
					break;
				default:
					node_text.push_back(c);
					break;
				}
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
//						node_text = L"";
					}
					break;
				case '>':
					// NODE END
					{
						Node& newNode = node.addNode(node_name);

						// 次ノード解析
						parse_node( newNode, reader );
						state = PARSE_STATE_SEARCHING;
//						node_text = L"";
					}
					break;
				case '/':
					if (reader.get_char()=='>') {
						// NODE END, like <xxx/>
						node.addNode(node_name);
						state = PARSE_STATE_SEARCHING;
//						node_text = L"";
					} else {
						// </xxx> 状態。
						// '>' が現れるまでパースし、TEXT を設定して終了。
						while( !reader.is_eof() ) {
							wint_t c = reader.get_char();
							if (c=='>') {
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
						}
						return false;
					}
					break;
				case '!':
					// 1文字目であれば、DTD 宣言とみなし、読み飛ばす。
					if (node_name.size()==0) {
						// DTD 宣言。
						// '>' が現れるまでパースし、継続。
						while( !reader.is_eof() ) {
							wint_t c = reader.get_char();
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
};

}
