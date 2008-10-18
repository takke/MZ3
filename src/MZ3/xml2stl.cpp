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
			// name="value" �������͂��B
			{
				xml2stl::XML2STL_STRING name, value;
				name.reserve(INITIAL_RESERVED_SIZE_OF_XML2STL_STRING);
				value.reserve(INITIAL_RESERVED_SIZE_OF_XML2STL_STRING);

				// name �擾
				name.push_back(c);
				reader.search_until_target('=', &name);

				// value �擾
				// 1�����ڂ� '"' �ȊO�Ȃ�ǉ�����
				if (!reader.is_eof()) {
					wint_t c1 = reader.get_char();
					if (c1!='"') {
						value.push_back(c1);
					}
					reader.search_until_target('"', &value);
				}
//				wprintf( L"parse_properties(), name[%s], value[%s]\n", name.c_str(), value.c_str());

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

/**
 * target ���������܂œǂݔ�΂��A���̒��O�܂ł� node_text �ɐݒ肷��
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
					// �����B
					node.addText( node_text );
					wprintf( L"parse_until_target(), target[%s], node_text[%s]\n", target.c_str(), node_text.c_str());
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
*/
}

bool SimpleXmlParser::parse_node( Container& node, kfm::kf_buf_reader<TCHAR>& reader )
{
	PARSE_STATE state = PARSE_STATE_SEARCHING;

	// lazy ���[�h�̏ꍇ�́A�������̃^�O�� HTML ���ɉ�͂��邽�߁A���ʉ�͂��L�����Z������B
	if (m_bLazyParseMode) {
		const xml2stl::XML2STL_STRING& node_name = node.getName();
		if (node_name==L"img") {
			return true;
		}
		if (node_name==L"script") {
			// script �^�O��p��́B
			// "</script>" �܂œǂݔ�΂�
			return parse_until_target( node, reader, L"</script>" );
		}
		if (node_name==L"style") {
			// style �^�O��p��́B
			// "</style>" �܂œǂݔ�΂�
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
				// 1�����ڂ� '<' �ȊO�Ȃ̂� '<' ���������܂œǂݔ�΂��A����܂ł̕������ node_text �Ƃ���B
				node_text.push_back(c);
				if (!reader.search_until_target('<', &node_text)) {
					return true;
				}
			} else {
				// 1�����ڂ����ł� '<' �Ȃ̂Ńm�[�h��͊J�n
			}
			// NODE START
			state = PARSE_STATE_IN_NODE_NAME;
			// �e�L�X�g������Γo�^����B
			if (!node_text.empty()) {
				node.addText( node_text );
				node_text = L"";
			}
			node_name = L"";
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
//					node_text = L"";
				}
				break;
			case '>':
				// NODE END
				{
					Node& newNode = node.addNode(node_name);

					// ���m�[�h���
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
					// </xxx> ��ԁB
					// '>' �������܂Ńp�[�X���ATEXT ��ݒ肵�ďI���B
					if (reader.search_until_target('>', NULL)) {
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
							reader.search_until_target('[', NULL);

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

}
