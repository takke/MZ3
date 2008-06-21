/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MZ3Parser.h"

/// Twitter 用パーサ
namespace twitter {

class TwitterParserBase : public mixi::MixiListParser
{
protected:
	static bool ExtractLinks( CMixiData& data_ );

	/// out_ の先頭に new_list を追加する
	static bool AppendNewList( CMixiDataList& out_, CMixiDataList& new_list )
	{
		const size_t LIST_MAX_SIZE = 1000;

		if (new_list.empty()) {
			return true;
		}

		size_t old_list_size = out_.size();
		size_t new_list_size = new_list.size();

		// 先にメモリ確保
		out_.resize( out_.size() + new_list.size() );
		// 旧データの移動
		for (int i=old_list_size-1; i>=0; i--) {
			out_[i+new_list_size] = out_[i];
		}
		// 新データの追加
		for (u_int i=0; i<new_list_size; i++) {
			out_[i] = new_list[i];
		}

/*		size_t old_list_size = out_.size();
		new_list.reserve(new_list.size() + old_list_size);
		for (u_int i=0; i<old_list_size && new_list.size()<=LIST_MAX_SIZE; i++) {
			new_list.push_back(out_[i]);
		}
		out_ = new_list;
*/
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
