#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
using namespace std;

/// INI �t�@�C��
namespace inifile {

/// INI �t�@�C���̃��R�[�h
struct Record
{
	string Comments;	///< �R�����g
	char Commented;		///< �R�����g�s�t���O(����)
	string Section;		///< �Z�N�V����
	string Key;			///< �L�[
	string Value;		///< �l
};

/// INI �t�@�C���A�N�Z�X�p�N���X
class Access
{
public:
	static bool Load(string FileName, vector<Record>& content);	
	static bool Save(string FileName, vector<Record>& content);
};

/**
 * INI �t�@�C���Ǘ��N���X(static��)
 *
 * �S�� static ���\�b�h�B
 * ��y�Ɏg���邪�p�ɂɌĂяo���ƃR�X�g�i���ԁj�������Ȃ�̂Œ��ӁB
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
 * INI �t�@�C���Ǘ��N���X
 */
class IniFile
{
private:
	vector<Record>	contents_;	///< �R���e���c

public:
	bool			bLoaded_;	///< �ǂݍ��ݍς݁H

	IniFile() : bLoaded_(false)
	{
	}

	bool Load( const wchar_t* filename )
	{
		// �t�@�C������ MBS �ɕϊ�
		static std::vector<char> filename_mbs( 256 );
		memset( &filename_mbs[0], 0, 256 );
		wcstombs( &filename_mbs[0], filename, 255);

		// ���[�h
		if( Access::Load( &filename_mbs[0], contents_ ) ) {
			bLoaded_ = true;
		}else{
			bLoaded_ = true;
			// ���݂��Ȃ���ΐV�K�쐬
			FILE* fp = fopen( &filename_mbs[0], "w" );
			fclose( fp );
		}

		return true;
	}

	bool Save( const wchar_t* filename, bool bFailOnNotLoaded=true )
	{
		// �t�@�C������ MBS �ɕϊ�
		static std::vector<char> filename_mbs( 256 );
		memset( &filename_mbs[0], 0, 256 );
		wcstombs( &filename_mbs[0], filename, 255);

		if( bFailOnNotLoaded ) {
			if( !bLoaded_ )
				return false;
		}else{
			// �V�K�쐬
			FILE* fp = fopen( &filename_mbs[0], "w" );
			fclose( fp );
		}

		// �ۑ�
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