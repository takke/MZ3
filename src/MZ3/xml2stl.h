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
};

class Property
{
public:
    XML2STL_STRING name;
    XML2STL_STRING text;
};

typedef std::vector<Property>	PropertyList;
typedef std::vector<Node>		NodeList;

class Container
{
private:
	XML2STL_TYPE type;
	XML2STL_STRING  name_or_text;	///< NODE �Ȃ�m�[�h���ATEXT �Ȃ當����

	PropertyList	properties;
	NodeList		children;

public:
	Container() : type(XML2STL_TYPE_ROOTNODE) 
	{}

	XML2STL_TYPE getType() const {
		return type;
	}

	const XML2STL_STRING& getName() const {
		return name_or_text;
	}

	const XML2STL_STRING getText1() const {
		return name_or_text;
	}

	const XML2STL_STRING getTextAll() const {
		// ���ʂ̑S�m�[�h���d�˂ăe�L�X�g�����ĕԂ�
		size_t n = children.size();
		XML2STL_STRING  text;
		for (size_t i=0; i<n; i++) {
			const Node& targetNode = children[i];
			switch (targetNode.type) {
			case XML2STL_TYPE_NODE:
				// �v���p�e�B�Ɖ��ʃm�[�h�� TEXT ��
				text += L"<";
				text += targetNode.name_or_text;
				// �v���p�e�B�� TEXT ��
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
					// ���ʃm�[�h
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
		throw NodeNotFoundException();
		// TODO: use another exception.
		// TODO: set requested "name".
	}

	const Node& getNode( size_t index ) const
	{
		if (index >= children.size()) {
			throw NodeNotFoundException();
			// TODO: set requested info.
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
		throw NodeNotFoundException();
		// TODO: set requested info.
	}

//	void setText( const XML2STL_STRING& text )
//	{
//		this->name_or_text = text;
//	}
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

    // �v���p�e�B
    if (!c.getProperties().empty()) {
		size_t n = c.getProperties().size();
        for (size_t i=0; i<n; i++) {
            const Property& p = c.getProperty(i);
            dump_nest( fp, nest_level+1 );
            fwprintf( fp, _T("[%s] => [%s]\n"), p.name.c_str(), p.text.c_str() );
        }
    }

    // �q�v�f
    if (c.hasChildren()) {
		size_t n = c.getChildren().size();
        for (size_t i=0; i<n; i++) {
            dump_container( c.getNode(i), fp, nest_level+1 );
        }
    }

    // �{���� children �� non-empty �Ȃ� text ����
//	if (!c.getText().empty()) {
//		dump_nest( fp, nest_level );
//		fwprintf( fp, _T("  => [%s]\n"), c.getText().c_str() );
//	}
}

class SimpleXmlParser
{
private:
	bool m_bLazyParseMode;		///< img �^�O������Ȃ��Ă�OK�ɂ���

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
				// name="value" �������͂��B
				{
					xml2stl::XML2STL_STRING name;
					xml2stl::XML2STL_STRING value;
					// name �擾
					name.push_back(c);
					while( !reader.is_eof() ) {
						wint_t c = reader.get_char();

						if (c=='=') {
							break;
						}
						name.push_back(c);
					}

					// value �擾
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

					// name, value ���Ƃ���1�����ȏ�ł���΃v���p�e�B�ǉ�
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
						// �����B
						node.addText( node_text );
						return true;
					} else {
						// �p��
					}
				} else {
					// �s��v�B
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

		// lazy ���[�h�̏ꍇ�́A�������̃^�O�� HTML ���ɉ�͂��邽�߁A���ʉ�͂��L�����Z������B
		if (m_bLazyParseMode) {
			if (node.getName()==L"img") {
				return true;
			}
		}

		if (node.getName()==L"script") {
			// script �^�O��p��́B
			// "</script>" �܂œǂݔ�΂�
			return parse_until_target( node, reader, L"</script>" );
		}
		if (node.getName()==L"style") {
			// style �^�O��p��́B
			// "</style>" �܂œǂݔ�΂�
			return parse_until_target( node, reader, L"</style>" );
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
					// �e�L�X�g������Γo�^����B
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
				// "<..." �̉�͏��
				switch (c) {
				case ' ':
				case '\t':
				case '\r':
				case '\n':
				case '\r\n':
					// NODE NAME END
					// �v���p�e�B��͂��s���B
					{
						Node& newNode = node.addNode(node_name);
						node_name = L"";
						bool r = parse_properties( newNode, reader );
						if (r) {
							// "/>" �ŏI���Ȃ������̂ŁA���m�[�h���
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

						// ���m�[�h���
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
						// </xxx> ��ԁB
						// '>' �������܂Ńp�[�X���ATEXT ��ݒ肵�ďI���B
						while( !reader.is_eof() ) {
							wint_t c = reader.get_char();
							if (c=='>') {
								// �q�v�f�������TEXT��ݒ肵�Ȃ��B
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

								// �K�v�ł���΁Anode.name �Ƃ���܂ł�c�l�Ƃ̕������r���s�����ƁB
								return true;
							}
						}
						return false;
					}
					break;
				case '!':
					// 1�����ڂł���΁ADTD �錾�Ƃ݂Ȃ��A�ǂݔ�΂��B
					if (node_name.size()==0) {
						// DTD �錾�B
						// '>' �������܂Ńp�[�X���A�p���B
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
						// �G���[
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
