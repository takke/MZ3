#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
using namespace std;

/// INI ファイル
namespace inifile {

/// INI ファイルのレコード
struct Record
{
	string Comments;	///< コメント
	char Commented;		///< コメント行フラグ(文字)
	string Section;		///< セクション
	string Key;			///< キー
	string Value;		///< 値
};

/// INI ファイルアクセス用クラス
class Access
{
public:
	static bool Load(string FileName, vector<Record>& content);	
	static bool Save(string FileName, vector<Record>& content);
};

/**
 * INI ファイル管理クラス(static版)
 *
 * 全て static メソッド。
 * 手軽に使えるが頻繁に呼び出すとコスト（時間）が高くなるので注意。
 */
class StaticMethod
{
	enum CommentChar
	{
		Pound = '#',
		SemiColon = ';'
	};

public:

	static bool AddSection(string SectionName, string FileName);
	static bool CommentRecord(CommentChar cc, string KeyName,string SectionName,string FileName);
	static bool CommentSection(char CommentChar, string SectionName, string FileName);
	static string Content(string FileName);
	static bool Create(string FileName);
	static bool DeleteRecord(string KeyName, string SectionName, string FileName);
	static bool DeleteSection(string SectionName, string FileName);
	static vector<Record> GetRecord(string KeyName, string SectionName, string FileName);
	static vector<Record> GetSection(string SectionName, string FileName);
	static vector<string> GetSectionNames(string FileName);
	static string GetValue(string KeyName, string SectionName, string FileName);
	static bool RecordExists(string KeyName, string SectionName, string FileName);
	static bool RenameSection(string OldSectionName, string NewSectionName, string FileName);
	static bool SectionExists(string SectionName, string FileName);
	static bool SetRecordComments(string Comments, string KeyName, string SectionName, string FileName);
	static bool SetSectionComments(string Comments, string SectionName, string FileName);
	static bool SetValue(string KeyName, string Value, string SectionName, string FileName);
	static bool Sort(string FileName, bool Descending);
	static bool UnCommentRecord(string KeyName,string SectionName,string FileName);
	static bool UnCommentSection(string SectionName, string FileName);

private:
	static vector<Record> GetSections(string FileName);

public:

	struct RecordSectionIs : std::unary_function<Record, bool>
	{
		std::string section_;

		RecordSectionIs(const std::string& section): section_(section){}

		bool operator()( const Record& rec ) const
		{
			return rec.Section == section_;
		}
	};

	struct RecordSectionKeyIs : std::unary_function<Record, bool>
	{
		std::string section_;
		std::string key_;

		RecordSectionKeyIs(const std::string& section, const std::string& key): section_(section),key_(key){}

		bool operator()( const Record& rec ) const
		{
			return ((rec.Section == section_)&&(rec.Key == key_));
		}
	};

	struct AscendingSectionSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Section < End.Section;
		}
	};

	struct DescendingSectionSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Section > End.Section;
		}
	};

	struct AscendingRecordSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Key < End.Key;
		}
	};

	struct DescendingRecordSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Key > End.Key;
		}
	};
};

/**
 * INI ファイル管理クラス
 */
class IniFile
{
private:
	vector<Record>	contents_;	///< コンテンツ

public:
	bool			bLoaded_;	///< 読み込み済み？

	IniFile() : bLoaded_(false)
	{
	}

	bool Load( const wchar_t* filename )
	{
		// ファイル名を MBS に変換
		static std::vector<char> filename_mbs( 256 );
		memset( &filename_mbs[0], 0, 256 );
		wcstombs( &filename_mbs[0], filename, 255);

		// ロード
		if( Access::Load( &filename_mbs[0], contents_ ) ) {
			bLoaded_ = true;
		}else{
			bLoaded_ = true;
			// 存在しなければ新規作成
			FILE* fp = fopen( &filename_mbs[0], "w" );
			fclose( fp );
		}

		return true;
	}

	bool Save( const wchar_t* filename, bool bFailOnNotLoaded=true )
	{
		// ファイル名を MBS に変換
		static std::vector<char> filename_mbs( 256 );
		memset( &filename_mbs[0], 0, 256 );
		wcstombs( &filename_mbs[0], filename, 255);

		if( bFailOnNotLoaded ) {
			if( !bLoaded_ )
				return false;
		}else{
			// 新規作成
			FILE* fp = fopen( &filename_mbs[0], "w" );
			fclose( fp );
		}

		// 保存
		return Access::Save( &filename_mbs[0], contents_ );
	}

	string GetValue(string KeyName, string SectionName);
	bool   SetValue(string KeyName, string Value, string SectionName);
	bool   SetValue(const wchar_t* wKeyName, const wchar_t* wValue, const char* SectionName) {

		static std::vector<char> value( 256 );
		memset(&value[0], 0x00, sizeof(char) * 256);
		wcstombs(&value[0], wValue, 255);

		static std::vector<char> keyname( 256 );
		memset(&keyname[0], 0x00, sizeof(char) * 256);
		wcstombs(&keyname[0], wKeyName, 255);

		return SetValue( &keyname[0], &value[0], SectionName );
	}

	bool   RecordExists(string KeyName, string SectionName);

	vector<string> GetSectionNames();
	bool SectionExists(string SectionName);
private:
	vector<Record> GetRecord(string KeyName, string SectionName);

};


}// namespace util