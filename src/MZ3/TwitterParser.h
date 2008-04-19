/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MZ3Parser.h"

/// Twitter �p�p�[�T
namespace twitter {

class TwitterParserBase : public mixi::MixiListParser
{
protected:
	static bool ExtractLinks( CMixiData& data_ );

	/// id ���~���ɂȂ�悤�ɒǉ�����
	static bool InsertWithOrder( CMixiDataList& out_, const CMixiData& data_ )
	{
		int id = data_.GetID();

		size_t j=0;
		for (; j<out_.size(); j++) {
			if (id > out_[j].GetID()) {
				break;
			}
		}
		out_.insert( out_.begin()+j, data_ );

		// for performance tuning
/*		if (out_.size()==1) {
			for (int k=0; k<100; k++) {
				out_.insert( out_.begin()+j, data );
			}
		}
*/
		return true;
	}

	/// �ő匏�������i�����̗]���ȃf�[�^���폜����j
	static bool EraseExtraItems( CMixiDataList& out_ )
	{
		const size_t LIST_MAX_SIZE = 1000;
		if (out_.size()>LIST_MAX_SIZE) {
			out_.erase( out_.begin()+LIST_MAX_SIZE, out_.end() );
		}

		return true;
	}
};

class TwitterFriendsTimelineXmlParser : public twitter::TwitterParserBase
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ );
};

class TwitterDirectMessagesXmlParser : public twitter::TwitterParserBase
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ );
};

}//namespace twitter
