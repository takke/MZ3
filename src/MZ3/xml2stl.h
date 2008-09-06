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
#pragma once
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <list>
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

typedef std::list<Property>	PropertyList;
typedef std::list<Node>		NodeList;

// like iterator
class NodeRef;

class Container
{
private:
	XML2STL_TYPE	type;
	XML2STL_STRING  name_or_text;	///< NODE �Ȃ�m�[�h���ATEXT �Ȃ當����

	PropertyList	properties;
	NodeList		children;

public:
	Container() : type(XML2STL_TYPE_ROOTNODE) 
	{}

	XML2STL_TYPE getType() const {
		return type;
	}

	NodeRef getChildrenNodeRef() const;

	/**
	 * �m�[�h���̎擾
	 */
	const XML2STL_STRING& getName() const {
		if (isNode()) {
			return name_or_text;
		} else {
			// NODE�ȊO�Ȃ̂Ŏ擾�֎~
			static XML2STL_STRING empty_text = L"";
			return empty_text;
		}
	}

	const XML2STL_STRING getText1() const {
		if (isText()) {
			return name_or_text;
		} else {
			// TEXT�ȊO�Ȃ̂Ŏ擾�֎~
			static XML2STL_STRING empty_text = L"";
			return empty_text;
		}
	}

	const XML2STL_STRING getTextAll() const {
		// ���ʂ̑S�m�[�h���d�˂ăe�L�X�g�����ĕԂ�
		size_t n = children.size();
		XML2STL_STRING  text;
		for (NodeList::const_iterator it=children.begin(); it!=children.end(); it++) {
			const Node& targetNode = *it;

			switch (targetNode.type) {
			case XML2STL_TYPE_NODE:
				// �v���p�e�B�Ɖ��ʃm�[�h�� TEXT ��
				text += L"<";
				text += targetNode.name_or_text;
				// �v���p�e�B�� TEXT ��
				{
					for (PropertyList::const_iterator pit=targetNode.properties.begin(); 
						 pit!=targetNode.properties.end();
						 pit++)
					{
						const Property& prop = (*pit);
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
					// ���ʃm�[�h(�ċA)
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

		return children.back();
	}

	Node& addText( const XML2STL_STRING& text=_T("") )
	{
		Node node;
		node.name_or_text = text;
		node.type = XML2STL_TYPE_TEXT;

		children.push_back( node );

		return children.back();
	}

	Property& addProperty( const XML2STL_STRING& name, const XML2STL_STRING& text )
	{
		Property prop;
		prop.name = name;
		prop.text = text;

		properties.push_back( prop );

		return properties.back();
	}

	const Property& getProperty( size_t index ) const
	{
///		return properties[ index ];

		// very slow implementation
		size_t i=0;
		for (PropertyList::const_iterator it=properties.begin(); it!=properties.end(); it++, i++) {
			if (i==index) {
				return *it;
			}
		}

		std::wostringstream stream;
		stream << L"property not found... index[" << index << L"]";
		throw NodeNotFoundException(stream.str());
	}

	const XML2STL_STRING& getProperty( const XML2STL_STRING& name ) const
	{
		size_t n = properties.size();
		for (PropertyList::const_iterator it=properties.begin(); it!=properties.end(); it++) {
			if ((*it).name == name) {
				return (*it).text;
			}
		}

		std::wostringstream stream;
		stream << L"property not found... name[" << name << L"]";
		throw NodeNotFoundException(stream.str());
		// TODO: use another exception.
	}

	/**
	 *
	 * @notice very slow impl. use "getChildrenNodeRef" instead.
	 */
	const Node& getNode( size_t index ) const
	{
		if (index >= children.size()) {
			std::wostringstream stream;
			stream << L"node not found... index[" << index << L"]";
			throw NodeNotFoundException(stream.str());
		}

///		return children[ index ];

		// very slow implementation
		size_t i=0;
		for (NodeList::const_iterator it=children.begin(); it!=children.end(); it++, i++) {
			if (i==index) {
				return *it;
			}
		}

		std::wostringstream stream;
		stream << L"node not found... index[" << index << L"]";
		throw NodeNotFoundException(stream.str());
	}

	const Node& getNode( const XML2STL_STRING& name, int index=0 ) const
	{
		int nFound = 0;
		for (NodeList::const_iterator it=children.begin(); it!=children.end(); it++) {
			if ((*it).type == XML2STL_TYPE_NODE &&
				(*it).name_or_text == name) 
			{
				if (nFound==index) {
					return (*it);
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

		// �Ϗ�
		XML2STL_STRING prop_name  = prop_name_value.substr( 0, idxEq );
		XML2STL_STRING prop_value = prop_name_value.substr( idxEq+1 );
		return getNode( name, Property(prop_name, prop_value) );
	}

	const Node& getNode( const XML2STL_STRING& name, const Property& prop ) const
	{
		for (NodeList::const_iterator it=children.begin(); it!=children.end(); it++) {
			if ((*it).type == XML2STL_TYPE_NODE && (*it).name_or_text == name) {
				try {
					if ((*it).getProperty(prop.name) == prop.text) {
						return (*it);
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

	/**
	 * like getElementById on js.
	 *
	 * search node recursively.
	 *
	 * example : findNode( L"id=bodyArea" );
	 * example : findNode( L"class=archiveList" );
	 */
	const Node& findNode( const XML2STL_STRING& prop_name_value ) const
	{
		XML2STL_STRING::size_type idxEq = prop_name_value.find( '=' );
		if (idxEq==XML2STL_STRING::npos) {
			std::wostringstream stream;
			stream << L"internal error. at findNode, prop_name_value[" << prop_name_value << L"]";
			_dumpChildren(stream);
			throw NodeNotFoundException(stream.str());
		}

		XML2STL_STRING prop_name  = prop_name_value.substr( 0, idxEq );
		XML2STL_STRING prop_value = prop_name_value.substr( idxEq+1 );

		// Property prop_name=prop_value �ł���v�f��T������
		return findNode( Property(prop_name, prop_value) );
	}

	const Node& findNode( const Property& prop ) const
	{
		size_t n = children.size();
		for (NodeList::const_iterator it=children.begin(); it!=children.end(); it++) {
			if ((*it).type == XML2STL_TYPE_NODE) {
				try {
					if ((*it).getProperty(prop.name) == prop.text) {
						return (*it);
					}
				} catch( NodeNotFoundException& ) {
					// not found, continue...
				}

				// not found. search recursively...
				try {
					return (*it).findNode(prop);
				} catch( NodeNotFoundException& ) {
					// not found, continue...
				}
			}
		}
		std::wostringstream stream;
		stream << L"node not found... property[" << prop.name + L"=" << prop.text << L"]";
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
		for (NodeList::const_iterator it=children.begin(); it!=children.end(); it++) {
			if ((*it).type == XML2STL_TYPE_NODE) {
				if (!bDumpFirst) {
					stream << L"\r\n";
				}
				stream << L"<";
				stream << (*it).name_or_text;
				if (!(*it).properties.empty()) {
					stream << L" ";
					size_t nProperties = (*it).properties.size();
					for (PropertyList::const_iterator pit=(*it).properties.begin(); 
						 pit!=(*it).properties.end(); pit++) {
						if (pit!=(*it).properties.begin()) {
							stream << L" ";
						}
						stream << (*pit).name.c_str();
						stream << L"=\"";
						stream << (*pit).text.c_str();
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

/// like iterator
class NodeRef {
	NodeList::const_iterator iterator;
	const NodeList&	     target_node_list;

public:
	NodeRef(const NodeList& node_list) : target_node_list(node_list) {
		iterator = node_list.begin();
	}

	bool isEnd() const {
		return iterator==target_node_list.end();
	}

	void next() {
		iterator++;
	}

	const Node& getCurrentNode() const {
		return *iterator;
	}
};

inline NodeRef Container::getChildrenNodeRef() const {

	return NodeRef(children);
}

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
		for (PropertyList::const_iterator it=c.getProperties().begin(); it!=c.getProperties().end(); it++) {
            const Property& p = (*it);
            dump_nest( fp, nest_level+1 );
            fwprintf( fp, _T("[%s] => [%s]\n"), p.name.c_str(), p.text.c_str() );
        }
    }

    // �q�v�f
    if (c.hasChildren()) {
		for (NodeList::const_iterator it=c.getChildren().begin();
			 it!=c.getChildren().end();
			 it++)
		{
			dump_container( *it, fp, nest_level+1 );
		}
	}
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
				if (c==target[0]) {
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
					// 1�����ڂł���΁ADTD �錾or CDATA �Ƃ݂Ȃ��A�ǂݔ�΂��B
					/*
<!DOCTYPE html
  PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<![CDATA[  
<center>...</center>
]]>
					*/
					if (node_name.size()==0) {
						// DTD �錾 or CDATA
						// DTD : '>' �������܂Ńp�[�X���A�p���B
						// CDATA : ]]> �������܂Ńp�[�X���A�v�f���B
						for (int xpos=0; !reader.is_eof(); xpos++) {
							wint_t c = reader.get_char();

							if (xpos==0 && c=='[') {
								// CDATA
								node_text = L"";
								node_name = L"";
								// [ �܂œǂݔ�΂�
								while (!reader.is_eof()) {
									wint_t c = reader.get_char();
									if (c=='[') {
										break;
									}
								}

								// ]]> �܂œǂݔ�΂�
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
