// This file is part of The New Aspell
// Copyright (C) 2001 by Kevin Atkinson under the GNU LGPL license
// version 2.0 or 2.1.  You should have received a copy of the LGPL
// license along with this library if you did not you can find
// it at http://www.gnu.org/.

#include "asc_ctype.hpp"
#include "config.hpp"
#include "string.hpp"
#include "indiv_filter.hpp"
#include "mutable_container.hpp"
#include "string_map.hpp"
#include "clone_ptr-t.hpp"
#include "vector.hpp"
#include "errors.hpp"
#include "loadable-filter-API.hpp"
#include <stdio.h>
#include <cstdio>

namespace acommon {


  class TexFilter : public IndividualFilter 
  {
    FILTER("tex",filter_name,TexFilter);
    ALIAS("latex","tex");
    SET_DESCRIPTION("tex","Filter for recognizing TeX/LaTeX commands","tex");
    ASPELL_VERSION("tex",">=0.51");
  private:
    enum InWhat {Name, Opt, Parm, Other, Swallow};
    struct Command {
      InWhat in_what;
      String name;
      const char * do_check;
      Command() {}
      Command(InWhat w) : in_what(w), do_check("P") {}
    };

    bool in_comment;
    bool prev_backslash;
    Vector<Command> stack;

    class Commands : public StringMap {
    public:
      PosibErr<bool> add(ParmString to_add);
      PosibErr<bool> remove(ParmString to_rem);
    };
    
    Commands commands;
    bool check_comments;
    
    inline void push_command(InWhat);
    inline void pop_command();

    bool end_option(char u, char l);

    inline bool process_char(FilterChar::Chr c);
    
  public:
    PosibErr<bool> setup(Config *);
    void reset();
    void process(FilterChar * &, FilterChar * &);
  };

  //
  //
  //

  inline void TexFilter::push_command(InWhat w) {
    stack.push_back(Command(w));
  }

  inline void TexFilter::pop_command() {
    stack.pop_back();
    if (stack.empty())
      push_command(Parm);
  }

  //
  //
  //

  PosibErr<bool> TexFilter::setup(Config * opts) 
  {
    name_ = filter_name();
    order_num_ = 0.35;
    commands.clear();
    LIST_OPTION(opts,"command",&commands,"tex","filter");
    SET_DESCRIPTION("command","TeX commands","tex");
    // counters
    SET_DEFAULT("command","addtocounter pp","tex");
    ADD_DEFAULT("command","addtolength pp","tex")
    ADD_DEFAULT("command","alpha p","tex")
    ADD_DEFAULT("command","arabic p","tex")
    ADD_DEFAULT("command","fnsymbol p","tex")
    ADD_DEFAULT("command","roman p","tex")
    ADD_DEFAULT("command","stepcounter p","tex")
    ADD_DEFAULT("command","setcounter pp","tex")
    ADD_DEFAULT("command","usecounter p","tex")
    ADD_DEFAULT("command","value p","tex")
    ADD_DEFAULT("command","newcounter po","tex")
    ADD_DEFAULT("command","refstepcounter p","tex")
    // cross ref
    ADD_DEFAULT("command","label p","tex")
    ADD_DEFAULT("command","pageref p","tex")
    ADD_DEFAULT("command","ref p","tex")
    // Definitions
    ADD_DEFAULT("command","newcommand poOP","tex")
    ADD_DEFAULT("command","renewcommand poOP","tex")
    ADD_DEFAULT("command","newenvironment poOPP","tex")
    ADD_DEFAULT("command","renewenvironment poOPP","tex")
    ADD_DEFAULT("command","newtheorem poPo","tex")
    ADD_DEFAULT("command","newfont pp","tex")
    // Document Classes
    ADD_DEFAULT("command","documentclass op","tex")
    ADD_DEFAULT("command","usepackage op","tex")
    // Environments
    ADD_DEFAULT("command","begin po","tex")
    ADD_DEFAULT("command","end p","tex")
    // Lengths
    ADD_DEFAULT("command","setlength pp","tex")
    ADD_DEFAULT("command","addtolength pp","tex")
    ADD_DEFAULT("command","settowidth pp","tex")
    ADD_DEFAULT("command","settodepth pp","tex")
    ADD_DEFAULT("command","settoheight pp","tex")
    // Line & Page Breaking
    ADD_DEFAULT("command","enlargethispage p","tex")
    ADD_DEFAULT("command","hyphenation p","tex")
    // Page Styles
    ADD_DEFAULT("command","pagenumbering p","tex")
    ADD_DEFAULT("command","pagestyle p","tex")
    // Spaces & Boxes
    ADD_DEFAULT("command","addvspace p","tex")
    ADD_DEFAULT("command","framebox ooP","tex")
    ADD_DEFAULT("command","hspace p","tex")
    ADD_DEFAULT("command","vspace p","tex")
    ADD_DEFAULT("command","makebox ooP","tex")
    ADD_DEFAULT("command","parbox ooopP","tex")
    ADD_DEFAULT("command","raisebox pooP","tex")
    ADD_DEFAULT("command","rule opp","tex")
    ADD_DEFAULT("command","sbox pO","tex")
    ADD_DEFAULT("command","savebox pooP","tex")
    ADD_DEFAULT("command","usebox p","tex")
    // Splitting the Input
    ADD_DEFAULT("command","include p","tex")
    ADD_DEFAULT("command","includeonly p","tex")
    ADD_DEFAULT("command","input p","tex")
    // Table of Contents
    ADD_DEFAULT("command","addcontentsline ppP","tex")
    ADD_DEFAULT("command","addtocontents pP","tex")
    // Typefaces
    ADD_DEFAULT("command","fontencoding p","tex")
    ADD_DEFAULT("command","fontfamily p","tex")
    ADD_DEFAULT("command","fontseries p","tex")
    ADD_DEFAULT("command","fontshape p","tex")
    ADD_DEFAULT("command","fontsize pp","tex")
    ADD_DEFAULT("command","usefont pppp","tex")
    // Misc
    ADD_DEFAULT("command","documentstyle op","tex")
    ADD_DEFAULT("command","cite p","tex")
    ADD_DEFAULT("command","nocite p","tex")
    ADD_DEFAULT("command","psfig p","tex")
    ADD_DEFAULT("command","selectlanguage p","tex")
    ADD_DEFAULT("command","includegraphics op","tex")
    ADD_DEFAULT("command","bibitem op","tex")
    // Geometry Package
    ADD_DEFAULT("command","geometry p","tex")

    BOOL_OPTION(opts,"check-comments",checkcomments,"tex","filter");
    SET_DESCRIPTION("check-comments","check TeX comments","tex");
    SET_DEFAULT("check-comments","false","tex");
    check_comments = checkcomments;

//  Unused
//  LIST_OPTION(opts,"extention",&tex_extentions,"tex","filter");
//  SET_DESCTIPTION("extention","TeX file extensions","tex");
//  SET_DEFAULT("extention","tex","tex");
    reset();
    return true;
  }
  
  void TexFilter::reset() 
  {
    in_comment = false;
    prev_backslash = false;
    stack.resize(0);
    push_command(Parm);
  }

#  define top stack.back()

  // yes this should be inlined, it is only called once
  inline bool TexFilter::process_char(FilterChar::Chr c) 
  {
    // deal with comments
    if (c == '%' && !prev_backslash) in_comment = true;
    if (in_comment && c == '\n')     in_comment = false;
    if (in_comment)                  return !check_comments;

    if (top.in_what == Name) {
      if (asc_isalpha(c)) {

	top.name += c;
	return true;

      } else {

	if (top.name.empty() && (c == '@')) {
	  top.name += c;
	  return true;
	}
	  
	top.in_what = Other;

	if (top.name.empty()) {
	  top.name.clear();
	  top.name += c;
	  top.do_check = commands.lookup(top.name.c_str());
	  if (top.do_check == 0) top.do_check = "";
	  return !asc_isspace(c);
	}

	top.do_check = commands.lookup(top.name.c_str());
	if (top.do_check == 0) top.do_check = "";

	if (asc_isspace(c)) { // swallow extra spaces
	  top.in_what = Swallow;
	  return true;
	} else if (c == '*') { // ignore * at end of commands
	  return true;
	}
	
	// continue o...
      }

    } else if (top.in_what == Swallow) {

      if (asc_isspace(c))
	return true;
      else
	top.in_what = Other;
    }

    if (c == '{')
      while (*top.do_check == 'O' || *top.do_check == 'o') 
	++top.do_check;

    if (*top.do_check == '\0')
      pop_command();

    if (c == '{') {

      if (top.in_what == Parm || top.in_what == Opt || top.do_check == '\0')
	push_command(Parm);

      top.in_what = Parm;
      return true;
    }

    if (top.in_what == Other) {

      if (c == '[') {

	top.in_what = Opt;
	return true;

      } else if (asc_isspace(c)) {

	return true;

      } else {
	
	pop_command();

      }

    } 

    if (c == '\\') {
      push_command(Name);
      return true;
    }

    if (top.in_what == Parm) {

      if (c == '}')
	return end_option('P','p');
      else
	return *top.do_check == 'p';

    } else if (top.in_what == Opt) {

      if (c == ']')
	return end_option('O', 'o');
      else
	return *top.do_check == 'o';

    }

    return false;
  }

  void TexFilter::process(FilterChar * & str, FilterChar * & stop)
  {
    FilterChar * cur = str;

    while (cur != stop) {
      if (process_char(*cur))
	*cur = ' ';
      ++cur;
    }
  }

  bool TexFilter::end_option(char u, char l) {
    top.in_what = Other;
    if (*top.do_check == u || *top.do_check == l)
      ++top.do_check;
    return true;
  }

  //
  // TexFilter::Commands
  //

  PosibErr<bool> TexFilter::Commands::add(ParmString value) {
    int p1 = 0;
    while (!asc_isspace(value[p1])) {
      if (value[p1] == '\0') 
	return make_err(bad_value, value,"","a string of o,O,p, or P");
      ++p1;
    }
    int p2 = p1 + 1;
    while (asc_isspace(value[p2])) {
      if (value[p2] == '\0') 
	return make_err(bad_value, value,"","a string of o,O,p, or P");
      ++p2;
    }
    String t1; t1.assign(value,p1);
    String t2; t2.assign(value+p2);
    return StringMap::replace(t1, t2);
  }
  
  PosibErr<bool> TexFilter::Commands::remove(ParmString value) {
    int p1 = 0;
    while (!asc_isspace(value[p1]) && value[p1] != '\0') ++p1;
    String temp; temp.assign(value,p1);
    return StringMap::remove(temp);
  }
  
  //
  //
  //

}
ACTIVATE_FILTER(acommon,TexFilter,"tex");
