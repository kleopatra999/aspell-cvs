// This file is part of The New Aspell
// Copyright (C) 2002 by Kevin Atkinson under the GNU LGPL
// license version 2.0 or 2.1.  You should have received a copy of the
// LGPL license along with this library if you did not you can find it
// at http://www.gnu.org/.
//
// Copyright 2002 Kevin B. Hendricks, Stratford, Ontario, Canada And
// Contributors.  All rights reserved. See the file affix.license for
// details.

#ifndef _AFFIX_HXX_
#define _AFFIX_HXX_

#include "affix.hpp"
#include "data.hpp"

/* A Prefix Entry  */

namespace aspeller {

// FIXME: Eliminate pointless get/set methods by
//        just making methods public in *Entry classes

class PfxEntry : public AffEntry
{
       AffixMgr*    pmyMgr;

       PfxEntry * next;
       PfxEntry * nexteq;
       PfxEntry * nextne;
       PfxEntry * flgnxt;

public:

  PfxEntry(AffixMgr* pmgr, affentry* dp );
  ~PfxEntry();

  BasicWordInfo      check(LookupInfo, const char * word, int len);

  inline bool          allowCross() { return ((xpflg & XPRODUCT) != 0); }
  inline unsigned char getFlag()    { return achar;  }
  inline const char *  getKey()     { return appnd;  }
  char *               add(const char * word, int len);

  inline PfxEntry *    getNext()   { return next;   }
  inline PfxEntry *    getNextNE() { return nextne; }
  inline PfxEntry *    getNextEQ() { return nexteq; }
  inline PfxEntry *    getFlgNxt() { return flgnxt; }

  inline void   setNext(PfxEntry * ptr)   { next = ptr;   }
  inline void   setNextNE(PfxEntry * ptr) { nextne = ptr; }
  inline void   setNextEQ(PfxEntry * ptr) { nexteq = ptr; }
  inline void   setFlgNxt(PfxEntry * ptr) { flgnxt = ptr; }
};

/* A Suffix Entry */

class SfxEntry : public AffEntry
{
       AffixMgr*    pmyMgr;
       char *       rappnd;

       SfxEntry *   next;
       SfxEntry *   nexteq;
       SfxEntry *   nextne;
       SfxEntry *   flgnxt;

public:

  SfxEntry(AffixMgr* pmgr, affentry* dp );
  ~SfxEntry();

  BasicWordInfo   check(LookupInfo, const char * word, int len, 
			int optflags, AffEntry* ppfx);

  inline bool          allowCross() { return ((xpflg & XPRODUCT) != 0); }
  inline unsigned char getFlag()   { return achar;  }
  inline const char *  getKey()    { return rappnd; } 
  char * add(const char * word, int len);

  inline SfxEntry *    getNext()   { return next;   }
  inline SfxEntry *    getNextNE() { return nextne; }
  inline SfxEntry *    getNextEQ() { return nexteq; }
  inline SfxEntry *    getFlgNxt() { return flgnxt; }

  inline void   setNext(SfxEntry * ptr)   { next = ptr;   }
  inline void   setNextNE(SfxEntry * ptr) { nextne = ptr; }
  inline void   setNextEQ(SfxEntry * ptr) { nexteq = ptr; }
  inline void   setFlgNxt(SfxEntry * ptr) { flgnxt = ptr; }

};

}

#endif


