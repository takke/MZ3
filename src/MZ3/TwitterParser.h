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

class TwitterFriendsTimelineXmlParser : public mixi::MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ );
private:
	static bool ExtractLinks( CMixiData& data_ );
};

}//namespace twitter
