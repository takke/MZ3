/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MZ3Parser.h"

/// Twitter �p�p�[�T
namespace parser {

class TwitterParserBase : public parser::MZ3ParserBase
{
public:
	/**
	 * out_ �� new_ ���}�[�W����
	 *
	 * out_[in]  : 6, 5, 2, 1
	 * new_[in]  : 4, 3
	 * out_[out] : 6, 5, 4, 3, 2, 1
	 *
	 * out_[in]  : 
	 * new_[in]  : 4, 3
	 * out_[out] : 4, 3
	 *
	 * out_[in]  : 2, 1
	 * new_[in]  : 4, 3
	 * out_[out] : 4, 3, 2, 1
	 */
	static bool MergeNewList( CMixiDataList& out_, CMixiDataList& new_, const size_t LIST_MAX_SIZE = 1000 )
	{
		util::StopWatch sw_detect_insert_pos;
		util::StopWatch sw_move;

		if (new_.empty()) {
			return true;
		}

		size_t old_list_size = out_.size();
		size_t new_list_size = new_.size();

		// out_ �ɂ� new_ ���V�������̂��܂܂��ꍇ������
		// (out_ �ɂ� 1�y�[�W�ڂ�5�y�[�W�ڂ��Anew_ �ɂ� 2�y�[�W�ڂ��܂܂��悤�ȏꍇ)

		// out_, new_ ���� id �̍~���Ƀ\�[�g��

		//--- �}���ʒu�̔���
		sw_detect_insert_pos.start();
		// new_[0].id �� out_ �̒��ōŏ��ɑ傫���Ȃ�ʒu��T������
		int insert_pos = old_list_size;	// �����l�Fnew_[0].id �� out_ �̒��ōŏ��̏ꍇ�̑}���ʒu
		INT64 max_id_on_new = new_[0].GetID();
		for (size_t i=0; i<old_list_size; i++) {
			INT64 id = out_[i].GetID();
			if (max_id_on_new > id) {
				insert_pos = i;
				break;
			}
		}
		sw_detect_insert_pos.stop();
//		MZ3_TRACE(L"���}���ʒu=%d\n", insert_pos);

		//--- ���f�[�^�̂����A�V�f�[�^���Â����̂������(�V����)�ړ�����
		sw_move.start();
		// ��Ƀ������m��
		out_.resize( out_.size() + new_.size() );

		// insert_pos �����ɂ��� out_ �̃f�[�^������Ɉړ�����
		int n_move_target = old_list_size - insert_pos;
		for (int i=old_list_size-1; i>=insert_pos; i--) {
			out_[i+new_list_size] = out_[i];
		}

		//--- �V�f�[�^��}������
		for (u_int i=0; i<new_list_size; i++) {
			out_[insert_pos+i] = new_[i];
		}
		sw_move.stop();

		//--- �͂ݏo�����f�[�^���폜����
		if (out_.size() > LIST_MAX_SIZE) {
			out_.erase(out_.begin()+LIST_MAX_SIZE, out_.end());
		}

		MZ3LOGGER_DEBUG(
			util::FormatString(
				L"MergeNewList(), old[%d], new[%d], detect[%dms], move[%dms]", 
				old_list_size,
				new_list_size,
				sw_detect_insert_pos.getElapsedMilliSecUntilStoped(),
				sw_move.getElapsedMilliSecUntilStoped()));

		return true;
	}

/*	/// out_ �̐擪�� new_list ��ǉ�����
	static bool AppendNewList( CMixiDataList& out_, CMixiDataList& new_list )
	{
		const size_t LIST_MAX_SIZE = 1000;

		if (new_list.empty()) {
			return true;
		}

		size_t old_list_size = out_.size();
		size_t new_list_size = new_list.size();

		// ��Ƀ������m��
		out_.resize( out_.size() + new_list.size() );
		// ���f�[�^�̈ړ�
		for (int i=old_list_size-1; i>=0; i--) {
			out_[i+new_list_size] = out_[i];
		}
		// �V�f�[�^�̒ǉ�
		for (u_int i=0; i<new_list_size; i++) {
			out_[i] = new_list[i];
		}

		return true;
	}
*/
};

/*
class TwitterFriendsTimelineXmlParser : public parser::TwitterParserBase
{
public:
	static bool parse( CMixiData& parent, CMixiDataList& out_, const CHtmlArray& html_ );
};
*/

/*
class TwitterDirectMessagesXmlParser : public parser::TwitterParserBase
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ );
};
*/

/*
class WassrFriendsTimelineXmlParser : public parser::TwitterParserBase
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ );
};
*/

/*
class GoohomeQuoteQuotesFriendsParser : public parser::TwitterParserBase
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ );

	// �n���ꂽ�s(line)�Ɏw�肳�ꂽ�L�[�̗v�f������Ύ擾����
	static bool GetJsonValue(LPCTSTR line, LPCTSTR json_key, CString& result)
	{
		CString key;
		key.Format(L"\"%s\":", json_key);
		if (util::LineHasStringsNoCase(line, key) &&
			util::GetAfterSubString(line, L":", result)>0 &&
			util::GetBetweenSubString(result, L"\"", L"\"", result)>0)
		{
			return true;
		} else {
			return false;
		}
	}
};
*/

}//namespace parser
