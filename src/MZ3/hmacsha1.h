//
//	hmacsha1.h - Using for oauth.h
//	Copyrigth (c) 2010 zerippe All Right Reserved.
//	Blog: http://udoon.blogspot.com/
//

#ifndef _HMAC_SHA1_CLASS_H_
#define _HMAC_SHA1_CLASS_H_

#pragma comment( lib, "Crypt32.lib" )

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include < windows.h >
#include < wincrypt.h >

#include < vector >
#include < string >

#ifndef CRYPT_IPSEC_HMAC_KEY
#define CRYPT_IPSEC_HMAC_KEY 0x00000100
#endif

struct KeyContext
{
	BLOBHEADER	header;
	DWORD		length;
	BYTE		key[ 0xff ];
};

class HSHA1
{
public:

	HSHA1(){}
	~HSHA1(){}

public:

	bool digest( const std::string& key, const std::string& source, std::vector< BYTE >& dest ) const
	{
		const DWORD length = static_cast< DWORD >( key.size() );

		HCRYPTPROV prov = NULL;
		if( !::CryptAcquireContext( &prov, NULL, NULL, PROV_RSA_FULL, 0 ) )
		{
			MZ3LOGGER_ERROR(util::FormatString(L"Error in AcquireContext (0x%08x)", GetLastError()));

			// new key set ê∂ê¨ÇééÇ›ÇÈ
			if( !::CryptAcquireContext( &prov, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET ) )
			{
				MZ3LOGGER_ERROR(util::FormatString(L"Error in AcquireContext, CRYPT_NEWKEYSET (0x%08x)", GetLastError()));

				return false;
			}
		}
		
		KeyContext blobkey;
		::ZeroMemory( &blobkey, sizeof( blobkey ) );

		blobkey.header.bType =		PLAINTEXTKEYBLOB;
		blobkey.header.bVersion =	CUR_BLOB_VERSION;
		blobkey.header.reserved =	0;
		blobkey.header.aiKeyAlg =	CALG_RC2;
		blobkey.length			=	length;

		::memcpy_s( blobkey.key, sizeof( blobkey.key ), reinterpret_cast< const BYTE* >( key.c_str() ), length );

		HCRYPTKEY keyHandle = NULL;
		if( !::CryptImportKey( prov, reinterpret_cast< const BYTE* >( &blobkey ), sizeof( blobkey ), 0, CRYPT_IPSEC_HMAC_KEY, &keyHandle ) )
		{
			MZ3LOGGER_ERROR(util::FormatString(L"Error in CryptImportKey (0x%08x)", GetLastError()));
			release( prov, NULL, NULL );
			return false;
		}

		HCRYPTHASH hmacHash = NULL;
		if( !::CryptCreateHash( prov, CALG_HMAC, keyHandle, 0, &hmacHash ) )
		{
			MZ3LOGGER_ERROR(util::FormatString(L"Error in CryptCreateHash (0x%08x)", GetLastError()));
			release( prov, keyHandle, NULL );
			return false;
		}

		HMAC_INFO info;
		::ZeroMemory( &info, sizeof( info ) );
		info.HashAlgid = CALG_SHA1;
		if( !::CryptSetHashParam( hmacHash, HP_HMAC_INFO, reinterpret_cast< BYTE* >( &info ), 0 ) )
		{
			MZ3LOGGER_ERROR(util::FormatString(L"Error in CryptSetHashParam (0x%08x)", GetLastError()));
			release( prov, keyHandle, hmacHash );
			return false;
		}

		if( !::CryptHashData( hmacHash, reinterpret_cast< const BYTE* >( source.c_str() ), static_cast< DWORD >( source.size() ), 0 ) )
		{
			MZ3LOGGER_ERROR(util::FormatString(L"Error in CryptHashData (0x%08x)", GetLastError()));
			release( prov, keyHandle, hmacHash );
			return false;
		}

		DWORD size = 0;
		if( !::CryptGetHashParam( hmacHash, HP_HASHVAL, NULL, &size, 0 ) )
		{
			MZ3LOGGER_ERROR(util::FormatString(L"Error in CryptGetHashParam (0x%08x)", GetLastError()));
			release( prov, keyHandle, hmacHash );
			return false;
		}

		dest.resize( size );
		if( !::CryptGetHashParam( hmacHash, HP_HASHVAL, &dest[ 0 ], &size, 0 ) )
		{
			MZ3LOGGER_ERROR(util::FormatString(L"Error in CryptGetHashParam (0x%08x)", GetLastError()));
			release( prov, keyHandle, hmacHash );
			return false;
		}
		
		release( prov, keyHandle, hmacHash );

		return true;
	}

	bool digest( const std::string& key, const std::string& source, std::string& dest ) const
	{
		std::vector< BYTE > buffer;
		if( !digest( key, source, buffer ) )
		{
			return false;
		}

		char tmp[ 3 ];
		const std::vector< BYTE >::const_iterator& end( buffer.end() );
		for( std::vector< BYTE >::const_iterator it = buffer.begin(); it != end; ++it )
		{
			sprintf( tmp, "%2.2x", *it );
			dest += tmp;
		}

		return true;
	}

private:

	void release( HCRYPTPROV prov, HCRYPTKEY key, HCRYPTHASH hash ) const
	{
		if( hash != NULL )
		{
			::CryptDestroyHash( hash );
		}

		if( key != NULL )
		{
			::CryptDestroyKey( key );
		}

		if( prov != NULL )
		{
			::CryptReleaseContext( prov, 0 );
		}
	}
};

#endif
