// This file is part of The New Aspell
// Copyright (C) 2002 by Kevin Atkinson under the GNU LGPL
// license version 2.0 or 2.1.  You should have received a copy of the
// LGPL license along with this library if you did not you can find it
// at http://www.gnu.org/.
//
// Copyright 2002 Kevin B. Hendricks, Stratford, Ontario, Canada And
// Contributors.  All rights reserved. See the file affix.license for
// details.

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "iostream.hpp"

#include "affentry.hpp"
#include "affix.hpp"
#include "errors.hpp"
#include "getdata.hpp"
#include "parm_string.hpp"

using namespace std;

namespace aspeller {

// First some base level utility routines
  char * mystrdup(const char * s);                   // duplicate string
  char * myrevstrdup(const char * s);                // duplicate reverse of string
  char * mystrsep(const char ** sptr, const char delim); // parse into tokens with char delimiter
  bool   isSubset(const char * s1, const char * s2); // is affix s1 is a "subset" affix s2


  PosibErr<void> AffixMgr::setup(ParmString affpath)
  {
  
    printf("In AffixMgr!\n");

    // register hash manager and load affix data from aff file
    cpdmin = 3;  // default value
    for (int i=0; i < SETSIZE; i++) {
      pStart[i] = NULL;
      sStart[i] = NULL;
      pFlag[i] = NULL;
      sFlag[i] = NULL;
    }
    return parse_file(affpath);
  }


  AffixMgr::~AffixMgr() 
  {
 
    // pass through linked prefix entries and clean up
    for (int i=0; i < SETSIZE ;i++) {
      pFlag[i] = NULL;
      PfxEntry * ptr = (PfxEntry *)pStart[i];
      PfxEntry * nptr = NULL;
      while (ptr) {
	nptr = ptr->next;
	delete(ptr);
	ptr = nptr;
	nptr = NULL;
      }  
    }

    // pass through linked suffix entries and clean up
    for (int j=0; j < SETSIZE ; j++) {
      sFlag[j] = NULL;
      SfxEntry * ptr = (SfxEntry *)sStart[j];
      SfxEntry * nptr = NULL;
      while (ptr) {
	nptr = ptr->next;
	delete(ptr);
	ptr = nptr;
	nptr = NULL;
      }  
    }

    cpdmin = 0;
  }


// read in aff file and build up prefix and suffix entry objects 
  PosibErr<void> AffixMgr::parse_file(const char * affpath)
  {
    // io buffers
    String key, data;
 
    // affix type
    char ft;

    // open the affix file
    affix_file = affpath;
    FStream afflst;
    RET_ON_ERR(afflst.open(affpath,"r"));

    // step one is to parse the affix file building up the internal
    // affix data structures


    // read in each line ignoring any that do not
    // start with a known line type indicator

    while (getdata_pair(afflst,key,data)) {

      ft = ' ';

      /* parse in the name of the character set used by the .dict and .aff */

      if (key == "SET") 
        encoding = data;

      /* parse in the flag used by the controlled compound words */
      else if (key == "COMPOUNDFLAG")
        compound = data;

      /* parse in the flag used by the controlled compound words */
      else if (key == "COMPOUNDMIN")
        cpdmin = atoi(data.c_str()); // FiXME

      else if (key == "TRY" || key == "REP");

      else if (key == "PFX" || key == "SFX")
        ft = key[0];

      // parse this affix: P - prefix, S - suffix
      if (ft != ' ')
	RET_ON_ERR(parse_affix(data, ft, afflst));

    }
    afflst.close();

    // now we can speed up performance greatly taking advantage of the 
    // relationship between the affixes and the idea of "subsets".

    // View each prefix as a potential leading subset of another and view
    // each suffix (reversed) as a potential trailing subset of another.

    // To illustrate this relationship if we know the prefix "ab" is
    // found in the word to examine, only prefixes that "ab" is a
    // leading subset of need be examined.  Furthermore is "ab" is not
    // present then none of the prefixes that "ab" is is a subset need
    // be examined.

    // The same argument goes for suffix string that are reversed.

    // Then to top this off why not examine the first char of the word
    // to quickly limit the set of prefixes to examine (i.e. the
    // prefixes to examine must be leading supersets of the first
    // character of the word (if they exist)
 
    // To take advantage of this "subset" relationship, we need to add
    // two links from entry.  One to take next if the current prefix
    // is found (call it nexteq) and one to take next if the current
    // prefix is not found (call it nextne).

    // Since we have built ordered lists, all that remains is to
    // properly intialize the nextne and nexteq pointers that relate
    // them

    process_pfx_order();
    process_sfx_order();

    return no_err;
  }


  // we want to be able to quickly access prefix information
  // both by prefix flag, and sorted by prefix string itself
  // so we need to set up two indexes

  PosibErr<void> AffixMgr::build_pfxlist(AffEntry* pfxptr)
  {
    PfxEntry * ptr;
    PfxEntry * pptr;
    PfxEntry * ep = (PfxEntry*) pfxptr;

    // get the right starting point 
    const char * key = ep->key();
    const unsigned char flg = ep->flag();

    // first index by flag which must exist
    ptr = (PfxEntry*)pFlag[flg];
    ep->flag_next = ptr;
    pFlag[flg] = (AffEntry *) ep;

    // next index by affix string

    // handle the special case of null affix string
    if (strlen(key) == 0) {
      // always inset them at head of list at element 0
      ptr = (PfxEntry*)pStart[0];
      ep->next = ptr;
      pStart[0] = (AffEntry*)ep;
      return no_err;
    }

    // now handle the general case
    unsigned char sp = *((const unsigned char *)key);
    ptr = (PfxEntry*)pStart[sp];
  
    /* handle the insert at top of list case */
    if ((!ptr) || ( strcmp( ep->key() , ptr->key() ) <= 0)) {
      ep->next = ptr;
      pStart[sp] = (AffEntry*)ep;
      return no_err;
    }

    /* otherwise find where it fits in order and insert it */
    pptr = NULL;
    for (; ptr != NULL; ptr = ptr->next) {
      if (strcmp( ep->key() , ptr->key() ) <= 0) break;
      pptr = ptr;
    }
    pptr->next = ep;
    ep->next = ptr;
    return no_err;
  }


  // we want to be able to quickly access suffix information
  // both by suffix flag, and sorted by the reverse of the
  // suffix string itself; so we need to set up two indexes

  PosibErr<void> AffixMgr::build_sfxlist(AffEntry* sfxptr)
  {
    SfxEntry * ptr;
    SfxEntry * pptr;
    SfxEntry * ep = (SfxEntry *) sfxptr;

    /* get the right starting point */
    const char * key = ep->key();
    const unsigned char flg = ep->flag();


    // first index by flag which must exist
    ptr = (SfxEntry*)sFlag[flg];
    ep->flag_next = ptr;
    sFlag[flg] = (AffEntry *) ep;


    // next index by affix string
    
    // handle the special case of null affix string
    if (strlen(key) == 0) {
      // always inset them at head of list at element 0
      ptr = (SfxEntry*)sStart[0];
      ep->next = ptr;
      sStart[0] = (AffEntry*)ep;
      return no_err;
    }

    // now handle the normal case
    unsigned char sp = *((const unsigned char *)key);
    ptr = (SfxEntry*)sStart[sp];
  
    /* handle the insert at top of list case */
    if ((!ptr) || ( strcmp( ep->key() , ptr->key() ) <= 0)) {
      ep->next = ptr;
      sStart[sp] = (AffEntry*)ep;
      return no_err;
    }

    /* otherwise find where it fits in order and insert it */
    pptr = NULL;
    for (; ptr != NULL; ptr = ptr->next) {
      if (strcmp( ep->key(), ptr->key() ) <= 0) break;
      pptr = ptr;
    }
    pptr->next = ep;
    ep->next = ptr;
    return no_err;
  }



// initialize the PfxEntry links NextEQ and NextNE to speed searching
  PosibErr<void> AffixMgr::process_pfx_order()
  {
    PfxEntry* ptr;

    // loop through each prefix list starting point
    for (int i=1; i < SETSIZE; i++) {

      ptr = (PfxEntry*)pStart[i];

      // look through the remainder of the list
      //  and find next entry with affix that 
      // the current one is not a subset of
      // mark that as destination for NextNE
      // use next in list that you are a subset
      // of as NextEQ

      for (; ptr != NULL; ptr = ptr->next) {

	PfxEntry * nptr = ptr->next;
	for (; nptr != NULL; nptr = nptr->next) {
	  if (! isSubset( ptr->key() , nptr->key() )) break;
	}
	ptr->next_ne = nptr;
	ptr->next_eq = NULL;
	if ((ptr->next) && isSubset(ptr->key() , 
					 (ptr->next)->key())) 
	  ptr->next_eq = ptr->next;
      }

      // now clean up by adding smart search termination strings:
      // if you are already a superset of the previous prefix
      // but not a subset of the next, search can end here
      // so set NextNE properly

      ptr = (PfxEntry *) pStart[i];
      for (; ptr != NULL; ptr = ptr->next) {
	PfxEntry * nptr = ptr->next;
	PfxEntry * mptr = NULL;
	for (; nptr != NULL; nptr = nptr->next) {
	  if (! isSubset(ptr->key(),nptr->key())) break;
	  mptr = nptr;
	}
	if (mptr) mptr->next_ne = NULL;
      }
    }
    return no_err;
  }



// initialize the SfxEntry links NextEQ and NextNE to speed searching
  PosibErr<void> AffixMgr::process_sfx_order()
  {
    SfxEntry* ptr;

    // loop through each prefix list starting point
    for (int i=1; i < SETSIZE; i++) {

      ptr = (SfxEntry *) sStart[i];

      // look through the remainder of the list
      //  and find next entry with affix that 
      // the current one is not a subset of
      // mark that as destination for NextNE
      // use next in list that you are a subset
      // of as NextEQ

      for (; ptr != NULL; ptr = ptr->next) {
	SfxEntry * nptr = ptr->next;
	for (; nptr != NULL; nptr = nptr->next) {
	  if (! isSubset(ptr->key(),nptr->key())) break;
	}
	ptr->next_ne = nptr;
	ptr->next_eq = NULL;
	if ((ptr->next) && isSubset(ptr->key(),(ptr->next)->key())) 
	  ptr->next_eq = ptr->next;
      }


      // now clean up by adding smart search termination strings:
      // if you are already a superset of the previous suffix
      // but not a subset of the next, search can end here
      // so set NextNE properly

      ptr = (SfxEntry *) sStart[i];
      for (; ptr != NULL; ptr = ptr->next) {
	SfxEntry * nptr = ptr->next;
	SfxEntry * mptr = NULL;
	for (; nptr != NULL; nptr = nptr->next) {
	  if (! isSubset(ptr->key(),nptr->key())) break;
	  mptr = nptr;
	}
	if (mptr) mptr->next_ne = NULL;
      }
    }
    return no_err;
  }



// takes aff file condition string and creates the
// conds array - please see the appendix at the end of the
// file affentry.cxx which describes what is going on here
// in much more detail

  void AffixMgr::encodeit(struct affentry * ptr, char * cs)
  {
    unsigned char c;
    int i, j, k;
    unsigned char mbr[MAXLNLEN];

    // now clear the conditions array */
    for (i=0;i<SETSIZE;i++) ptr->conds[i] = (unsigned char) 0;

    // now parse the string to create the conds array */
    int nc = strlen(cs);
    int neg = 0;   // complement indicator
    int grp = 0;   // group indicator
    int n = 0;     // number of conditions
    int ec = 0;    // end condition indicator
    int nm = 0;    // number of member in group

    // if no condition just return
    if (strcmp(cs,".")==0) {
      ptr->numconds = 0;
      return;
    }

    i = 0;
    while (i < nc) {
      c = *((unsigned char *)(cs + i));

      // start group indicator
      if (c == '[') {
	grp = 1;
	c = 0;
      }

      // complement flag
      if ((grp == 1) && (c == '^')) {
	neg = 1;
	c = 0;
      }

      // end goup indicator
      if (c == ']') {
	ec = 1;
	c = 0;
      }

      // add character of group to list
      if ((grp == 1) && (c != 0)) {
	*(mbr + nm) = c;
	nm++;
	c = 0;
      }

      // end of condition 
      if (c != 0) {
	ec = 1;
      }

    
      if (ec) {
	if (grp == 1) {
	  if (neg == 0) {
	    // set the proper bits in the condition array vals for those chars
	    for (j=0;j<nm;j++) {
	      k = (unsigned int) mbr[j];
	      ptr->conds[k] = ptr->conds[k] | (1 << n);
	    }
	  } else {
	    // complement so set all of them and then unset indicated ones
	    for (j=0;j<SETSIZE;j++) ptr->conds[j] = ptr->conds[j] | (1 << n);
	    for (j=0;j<nm;j++) {
	      k = (unsigned int) mbr[j];
	      ptr->conds[k] = ptr->conds[k] & ~(1 << n);
	    }
	  }
	  neg = 0;
	  grp = 0;   
	  nm = 0;
	} else {
	  // not a group so just set the proper bit for this char
	  // but first handle special case of . inside condition
	  if (c == '.') {
	    // wild card character so set them all
            for (j=0;j<SETSIZE;j++) ptr->conds[j] = ptr->conds[j] | (1 << n);
	  } else {  
	    ptr->conds[(unsigned int) c] = ptr->conds[(unsigned int)c] | (1 << n);
	  }
	}
	n++;
	ec = 0;
      }


      i++;
    }
    ptr->numconds = n;
    return;
  }


// check word for prefixes
  BasicWordInfo AffixMgr::prefix_check (LookupInfo linf, ParmString word, 
                                        CheckInfo & ci, GuessInfo * gi) const
  {
    BasicWordInfo wordinfo;
 
    // first handle the special case of 0 length prefixes
    PfxEntry * pe = (PfxEntry *) pStart[0];
    while (pe) {
      wordinfo = pe->check(linf,word,ci,gi);
      if (wordinfo) return wordinfo;
      pe = pe->next;
    }
  
    // now handle the general case
    unsigned char sp = *reinterpret_cast<const unsigned char *>(word.str());
    PfxEntry * pptr = (PfxEntry *)pStart[sp];

    while (pptr) {
      if (isSubset(pptr->key(),word)) {
	wordinfo = pptr->check(linf,word,ci,gi);
	if (wordinfo) return wordinfo;
	pptr = pptr->next_eq;
      } else {
	pptr = pptr->next_ne;
      }
    }
    
    return BasicWordInfo();
  }


// check word for suffixes
  BasicWordInfo AffixMgr::suffix_check (LookupInfo linf, ParmString word, 
                                        CheckInfo & ci, GuessInfo * gi,
					int sfxopts, AffEntry * ppfx) const
  {
    BasicWordInfo wordinfo;

    // first handle the special case of 0 length suffixes
    SfxEntry * se = (SfxEntry *) sStart[0];
    while (se) {
      wordinfo = se->check(linf, word, ci, gi, sfxopts, ppfx);
      if (wordinfo) return wordinfo;
      se = se->next;
    }
  
    // now handle the general case
    char * tmpword = myrevstrdup(word);
    unsigned char sp = *((const unsigned char *)tmpword);
    SfxEntry * sptr = (SfxEntry *) sStart[sp];

    while (sptr) {
      if (isSubset(sptr->key(),tmpword)) {
	wordinfo = sptr->check(linf, word, ci, gi, sfxopts, ppfx);
	if (wordinfo) {
	  free(tmpword);
	  return wordinfo;
	}
	sptr = sptr->next_eq;
      } else {
	sptr = sptr->next_ne;
      }
    }
    
    free(tmpword);
    return NULL;
  }





// check if word with affixes is correctly spelled
  BasicWordInfo AffixMgr::affix_check(LookupInfo linf, ParmString word, 
                                      CheckInfo & ci, GuessInfo * gi) const
  {
    BasicWordInfo wordinfo;

    // check all prefixes (also crossed with suffixes if allowed) 
    wordinfo = prefix_check(linf, word, ci, gi);
    if (wordinfo) return wordinfo;

    // if still not found check all suffixes
    wordinfo = suffix_check(linf, word, ci, gi, 0, NULL);
    return wordinfo;
  }

int AffixMgr::expand_rootword(struct guessword * wlst, int maxn, 
                              const char * ts, int wl, const char * ap, int al) const
{

    int nh=0;

    // first add root word to list

    if (nh < maxn) {
       wlst[nh].word = mystrdup(ts);
       wlst[nh].allow = (1 == 0);
       nh++;
    }

    // handle suffixes
    for (int i = 0; i < al; i++) {
       unsigned char c = (unsigned char) ap[i];
       SfxEntry * sptr = (SfxEntry *)sFlag[c];
       while (sptr) {
	 char * newword = sptr->add(ts, wl);
         if (newword) {
           if (nh < maxn) {
	      wlst[nh].word = newword;
              wlst[nh].allow = sptr->allow_cross();
              nh++;
	   } else {
	      free(newword);
           }
	 }
         sptr = (SfxEntry *)sptr ->flag_next;
       }
    }

    int n = nh;

    // handle cross products of prefixes and suffixes
    for (int j=1;j<n ;j++)
       if (wlst[j].allow) {
          for (int k = 0; k < al; k++) {
             unsigned char c = (unsigned char) ap[k];
             PfxEntry * cptr = (PfxEntry *) pFlag[c];
             while (cptr) {
                if (cptr->allow_cross()) {
	            int l1 = strlen(wlst[j].word);
	            char * newword = cptr->add(wlst[j].word, l1);
                    if (newword) {
		       if (nh < maxn) {
	                  wlst[nh].word = newword;
                          wlst[nh].allow = cptr->allow_cross();
                          nh++;
		       } else {
			  free(newword);
                       }
	            }
                }
                cptr = (PfxEntry *)cptr->flag_next;
             }
	  }
       }


    // now handle pure prefixes
    for (int m = 0; m < al; m ++) {
       unsigned char c = (unsigned char) ap[m];
       PfxEntry * ptr = (PfxEntry *) pFlag[c];
       while (ptr) {
	 char * newword = ptr->add(ts, wl);
         if (newword) {
	     if (nh < maxn) {
	        wlst[nh].word = newword;
                wlst[nh].allow = ptr->allow_cross();
                nh++;
             } else {
	        free(newword);
	     } 
	 }
         ptr = (PfxEntry *)ptr ->flag_next;
       }
    }

    return nh;
}

// strip strings into token based on single char delimiter
// acts like strsep() but only uses a delim char and not 
// a delim string

  char * mystrsep(const char * * stringp, const char delim)
  {
    char * rv = NULL;
    const char * mp = *stringp;
    int n = strlen(mp);
    if (n > 0) {
      char * dp = (char *)memchr(mp,(int)((unsigned char)delim),n);
      if (dp) {
        *stringp = dp+1;
        int nc = (int)((unsigned long)dp - (unsigned long)mp); 
        rv = (char *) malloc(nc+1);
        memcpy(rv,mp,nc);
        *(rv+nc) = '\0';
        return rv;
      } else {
	rv = (char *) malloc(n+1);
	memcpy(rv, mp, n);
	*(rv+n) = '\0';
	*stringp = mp + n;
	return rv;
      }
    }
    return NULL;
  }


  char * mystrdup(const char * s)
  {
    char * d = NULL;
    if (s) {
      int sl = strlen(s);
      d = (char *) malloc(((sl+1) * sizeof(char)));
      if (d) memcpy(d,s,((sl+1)*sizeof(char)));
    }
    return d;
  }


  char * myrevstrdup(const char * s)
  {
    char * d = NULL;
    if (s) {
      int sl = strlen(s);
      d = (char *) malloc((sl+1) * sizeof(char));
      if (d) {
	const char * p = s + sl - 1;
	char * q = d;
	while (p >= s) *q++ = *p--;
	*q = '\0';
      }
    }
    return d; 
  }


/* return 1 if s1 is subset of s2 */
  bool isSubset(const char * s1, const char * s2)
  {
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    if (l1 > l2) return 0;
    if (strncmp(s2,s1,l1) == 0) return true;
    return false;
  }

  PosibErr<void> AffixMgr::parse_affix(String & data, 
				       const char at, FStream & af)
  {
    int numents = 0;      // number of affentry structures to parse
    char achar='\0';      // affix char identifier
    short ff=0;
    struct affentry * ptr= NULL;
    struct affentry * nptr= NULL;

    const char * tp = data.c_str();
    const char * nl = data.c_str();
    char * piece;
    int i = 0;

    // split affix header line into pieces

    int np = 0;
    while ((piece=mystrsep(&tp,' '))) {
      if (*piece != '\0') {
	switch(i) {
	  // piece 2 - is affix char
	case 0: { np++; achar = *piece; break; }

	  // piece 3 - is cross product indicator 
	case 1: { np++; if (*piece == 'Y') ff = XPRODUCT; break; }

	  // piece 4 - is number of affentries
	case 2: { 
	  np++;
	  numents = atoi(piece); 
	  ptr = (struct affentry *) malloc(numents * sizeof(struct affentry));
	  ptr->xpflg = ff;
	  ptr->achar = achar;
	  break;
	}

	default: break;
	}
	i++;
      }
      free(piece);
    }
    // check to make sure we parsed enough pieces
    if (np != 3) {
      free(ptr);
      String msg;
      msg << "affix " << achar << "header has insufficient data in line" << nl;
      return make_err(bad_file_format, affix_file, msg);
    }
 
    // store away ptr to first affentry
    nptr = ptr;

    String key;
    // now parse numents affentries for this affix
    for (int j=0; j < numents; j++) {
      getdata_pair(af, key, data);
      tp = data.c_str();
      i = 0;
      np = 0;

      if (nptr != ptr) nptr->xpflg = ptr->xpflg;
      
      // split line into pieces
      while ((piece=mystrsep(&tp,' '))) {
	if (*piece != '\0') {
	  switch(i) {

	    // piece 2 - is affix char
	  case 0: { 
	    np++;
	    if (*piece != achar) {
	      free(piece);
	      String msg;
	      msg << "affix "<< achar << "is corrupt near line " << nl 
		  << ", possible incorrect count";
	      return make_err(bad_file_format, affix_file, msg);

	    }
	    if (nptr != ptr) nptr->achar = ptr->achar;
	    break;
	  }

	    // piece 3 - is string to strip or 0 for null 
	  case 1: { 
	    np++;
	    nptr->strip = mystrdup(piece);
	    nptr->stripl = strlen(nptr->strip);
	    if (strcmp(nptr->strip,"0") == 0) {
	      free(nptr->strip);
	      nptr->strip=mystrdup("");
	      nptr->stripl = 0;
	    }   
	    break; 
	  }

	    // piece 4 - is affix string or 0 for null
	  case 2: { 
	    np++;
	    nptr->appnd = mystrdup(piece);
	    nptr->appndl = strlen(nptr->appnd);
	    if (strcmp(nptr->appnd,"0") == 0) {
	      free(nptr->appnd);
	      nptr->appnd=mystrdup("");
	      nptr->appndl = 0;
	    }   
	    break; 
	  }

	    // piece 5 - is the conditions descriptions
	  case 3: { np++; encodeit(nptr,piece); }

	  default: break;
	  }
	  i++;
	}
	free(piece);
      }
      // check to make sure we parsed enough pieces
      if (np != 4) {
	free(ptr);
	String msg;
	msg << "affix "<< achar << "is corrupt near line " << nl;
	return make_err(bad_file_format, affix_file, msg);
      }
      nptr++;
    }
         
    // now create SfxEntry or PfxEntry objects and use links to
    // build an ordered (sorted by affix string) list
    nptr = ptr;
    for (int k = 0; k < numents; k++) {
      if (at == 'P') {
	PfxEntry * pfxptr = new PfxEntry(this,nptr);
	build_pfxlist((AffEntry *)pfxptr);
      } else {
	SfxEntry * sfxptr = new SfxEntry(this,nptr);
	build_sfxlist((AffEntry *)sfxptr); 
      }
      nptr++;
    }      
    free(ptr);
    return no_err;
  }


  PosibErr<AffixMgr *> new_affix_mgr(ParmString name, 
                                     const Language * lang)
  {
    CERR << "NEW AFFIX MGR\n";
    if (name == "none")
      return 0;
    CERR << "USING AFFIX FILE\n";
    String file;
    file += lang->data_dir();
    file += '/';
    file += lang->name();
    file += "_affix.dat";
    AffixMgr * affix;
    affix = new AffixMgr();
    PosibErrBase pe = affix->setup(file);
    if (pe.has_err()) {
      delete affix;
      return pe;
    } else {
      return affix;
    }
  }

}

