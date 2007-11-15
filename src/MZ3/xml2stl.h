/**
 * Simple XML Parser, XML to STL Container library.
 *
 * Copyright (C) takke, All rights reserved.
 * Dual licenced under GPL and modified BSD licence.
 *
 * TODO:
 *   - entity decoding.
 *   - supports input string.
 */
#include <stdio.h>
#include <string>
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
};

class Container;
typedef Container Node;

class NodeNotFoundException
{
};

class Property
{
public:
    XML2STL_STRING name;
    XML2STL_STRING text;
};

class Container
{
private:
    XML2STL_TYPE type;
    XML2STL_STRING  name;
    XML2STL_STRING  text;

    std::vector<Property> properties;
    std::vector<Node>     children;

public:
    Container() : type(XML2STL_TYPE_ROOTNODE) 
    {}

	XML2STL_TYPE getType() const {
		return type;
	}

	const XML2STL_STRING& getName() const {
		return name;
	}

	const XML2STL_STRING& getText() const {
		return text;
	}

	const std::vector<Node>& getChildren() const {
		return children;
	}

	const std::vector<Property>& getProperties() const {
		return properties;
	}

	size_t getChildrenCount() const {
		return children.size();
	}

	bool hasChildren() const
	{
		return children.empty() ? false : true;
	}

	Node& addNode( const XML2STL_STRING& name, const XML2STL_STRING& text=_T("") )
    {
        Node node;
        node.name = name;
        node.text = text;
        node.type = XML2STL_TYPE_NODE;

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

	const Node& getNode( size_t index ) const
	{
		if (index >= children.size()) {
	        throw NodeNotFoundException();
		}
		return children[ index ];
	}

    const Node& getNode( const XML2STL_STRING& name, int index=0 ) const
    {
        int nFound = 0;
		size_t n = children.size();
        for (size_t i=0; i<n; i++) {
            if (children[i].name == name) {
                if (nFound==index) {
                    return children[i];
                } else {
                    nFound ++;
                }
            }
        }
        throw NodeNotFoundException();
    }

	void setText( const XML2STL_STRING& text )
	{
		this->text = text;
	}
};

void dump_nest( FILE* fp, int nest_level )
{
    for (int i=0; i<nest_level*2; i++) {
        fputc( ' ', fp );
    }
}

void dump_container( const Container& c, FILE* fp, int nest_level=0 )
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

    // 本来は children が non-empty なら text 無効
    if (!c.getText().empty()) {
        dump_nest( fp, nest_level );
        fwprintf( fp, _T("  => [%s]\n"), c.getText().c_str() );
    }
}

class SimpleXmlParser
{
public:
	static bool loadFromFile( Container& root, LPCTSTR filename )
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

		return loadFromText( root, buffer );
	}

	static bool loadFromText( Container& root, const std::vector<TCHAR>& text )
	{
		kfm::kf_buf_reader<TCHAR> reader( text );
		return parse_node( root, reader );
	}

private:
	static bool parse_properties( Node& node, kfm::kf_buf_reader<TCHAR>& reader )
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

	static bool parse_node( Container& node, kfm::kf_buf_reader<TCHAR>& reader )
	{
		PARSE_STATE state = PARSE_STATE_SEARCHING;

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
						node_text = L"";
					}
					break;
				case '>':
					// NODE END
					{
						Node& newNode = node.addNode(node_name);
						parse_node( newNode, reader );
						state = PARSE_STATE_SEARCHING;
						node_text = L"";
					}
					break;
				case '/':
					if (reader.get_char()=='>') {
						// NODE END, like <xxx/>
						node.addNode(node_name);
						state = PARSE_STATE_SEARCHING;
						node_text = L"";
					} else {
						// </xxx> 状態。
						// '>' が現れるまでパースし、TEXT を設定して終了。
						while( !reader.is_eof() ) {
							wint_t c = reader.get_char();
							if (c=='>') {
								// 子要素があればTEXTを設定しない。
								if (!node.hasChildren()) {
									node.setText(node_text);
								}
								// 必要であれば、node.name とこれまでのc値との文字列比較を行うこと。
								return true;
							}
						}
						return false;
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
