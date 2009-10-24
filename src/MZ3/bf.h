/* 
  Copyright (C) 2005 Samuel KABAK [France] (www.codeas.net)

  This program is free software; you can redistribute it and/or modify
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
  
  If you need to use this library without being bound to the General Public
  License, or if you need support services you can purchase a commercial
  license from Codeas, www.codeas.net
*/

/*
Warning : this code was not tested on big endian systems
*/

#ifndef BF_H
#define BF_H

#include <stdio.h>

/// Blowfish
namespace bf {

#define BF_SUBKEYS  18
#define BF_SBOXES  4
#define BF_SBOXES_ENTRIES  256
#define BF_MAX_PASSWD   56  // 448bits

/** 
 * Blowfish cipher class
 * Notice that sizeof (long) must be 4 and that long must have 32 bits
 */
class bf
{
  private:
  union b32
  {
  	unsigned long word;
    unsigned char byte[4];
  };

  unsigned long PA[BF_SUBKEYS];
  unsigned long SB[BF_SBOXES][BF_SBOXES_ENTRIES];
  int bo_zero;
  int bo_one;
  int bo_two;
  int bo_three;
	void reset();
  unsigned long F(b32 *x) ;
  void wencrypt(b32 *w1,b32 *w2);
  void wdecrypt(b32 *w1,b32 *w2);

	public:
    /** constructor */
		bf();
    /** destructor */
		~bf();

    /** init blowfish with the key, returns 0 if success */
  	int init(const unsigned char *key, int len);

    /** encrypt one 64 bits block in situ */
    void encrypt(unsigned char *b64);

    /** decrypt one 64 bits block in situ */
    void decrypt(unsigned char *b64);

    /** convert long from system to big endian */
    unsigned long system2bigendian_long(unsigned long l);

    /** convert long from big endian to system */
    unsigned long bigendian2system_long(unsigned long l);

    /** test implementation with test vectors, return 0 on success */
    int autotest();

    /** stream encrypt in situ , mode is "ecb", "cbc", "cfb" or "ofb" , 
        b0 is the initial 64 bits vector for cbc & cfb modes, NULL means 0x0000000000000000
        returns 0 if success otherwize :
        -4 if data is NULL
        -1 if length is not multiple of 8
        -2 if mode is NULL
        -3 if mode is not "ecb", "cbc", "cfb" or "ofb" 

    */
    int sencrypt(unsigned char *data, int length, const char * mode, const unsigned char * b0 = NULL);

    /** stream decrypt in situ , mode is "ecb", "cbc", "cfb" or "ofb" ,
        b0 is the initial 64 bits vector for cbc & cfb modes, NULL means 0x0000000000000000
        returns 0 if success otherwize :
        -4 if data is NULL
        -1 if length is not multiple of 8
        -2 if mode is NULL
        -3 if mode is not "ecb", "cbc", "cfb" or "ofb"
    */
    int sdecrypt(unsigned char *data, int length, const char * mode,const unsigned char * b0 = NULL);
    

  private:
// no copy
//   bf& operator=(const bf& r);
//   void bf(const bf& r);  

};

}
#endif // BF_H
