// This file is part of BOINC.
// http://boinc.berkeley.edu
// Copyright (C) 2008 University of California
//
// BOINC is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// BOINC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PARSE_H
#define PARSE_H

#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "miofile.h"
#include "str_util.h"
#include "cl_boinc.h"

class XML_PARSER {
    bool scan_nonws(int&);
    int scan_comment();
    int scan_tag(char*, int, char* ab=0, int al=0);
    int scan_cdata(char*, int);
    bool copy_until_tag(char*, int);
public:
    char parsed_tag[4096];
    bool is_tag;
    MIOFILE* f;
    XML_PARSER(MIOFILE*);
    bool get(char*, int, bool&, char* ab=0, int al=0);
    inline bool get_tag(char* ab=0, int al=0) {
        return get(parsed_tag, sizeof(parsed_tag), is_tag, ab, al);
    }
    inline bool match_tag(const char* tag) {
        return !strcmp(parsed_tag, tag);
    }
    int get_aux(char* buf, int len, char* attr_buf, int attr_len);
    bool parse_start(const char*);
    bool parse_str(const char*, char*, int);
    bool parse_string(const char*, std::string&);
    bool parse_int(const char*, int&);
    bool parse_double(const char*, double&);
    bool parse_bool(const char*, bool&);
	int element_contents(const char*, char*, int);
    int copy_element(std::string&);
    void skip_unexpected(const char*, bool verbose, const char*);
    void skip_unexpected(bool verbose=false, const char* msg="") {
        skip_unexpected(parsed_tag, verbose, msg);
    }
};

extern bool boinc_is_finite(double);

/////////////// START DEPRECATED XML PARSER
// Deprecated because it makes assumptions about
// the format of the XML being parsed
///////////////

// return true if the tag appears in the line
//
inline bool match_tag(const char* buf, const char* tag) {
    if (strstr(buf, tag)) return true;
    return false;
}

inline bool match_tag(const std::string &s, const char* tag) {
    return match_tag(s.c_str(), tag);
}

#ifdef _WIN32
#define boinc_strtoull _strtoui64
#else
#ifdef HAVE_STRTOULL
#define boinc_strtoull strtoull
#else
inline unsigned long long boinc_strtoull(const char *s, char **, int) {
    char buf[64];
    char *p;
    unsigned long long y;
    strncpy(buf, s, sizeof(buf)-1);
    strip_whitespace(buf);
    p = strstr(buf, "0x");
    if (!p) p = strstr(buf, "0X");
    if (p) {
        sscanf(p, "%llx", &y);
    } else {
        sscanf(buf, "%llu", &y);
    }
    return y;
}
#endif
#endif

// parse an integer of the form <tag>1234</tag>
// return true if it's there
// Note: this doesn't check for the end tag
//
inline bool parse_int(const char* buf, const char* tag, int& x) {
    const char* p = strstr(buf, tag);
    if (!p) return false;
    int y = strtol(p+strlen(tag), 0, 0);        // this parses 0xabcd correctly
    if (errno == ERANGE) return false;
    x = y;
    return true;
}

// Same, for doubles
//
inline bool parse_double(const char* buf, const char* tag, double& x) {
    double y;
    const char* p = strstr(buf, tag);
    if (!p) return false;
    y = atof(p+strlen(tag));
    if (!boinc_is_finite(y)) {
        return false;
    }
    x = y;
    return true;
}

// Same, for unsigned long
//
inline bool parse_ulong(const char* buf, const char* tag, unsigned long& x) {
    const char* p = strstr(buf, tag);
    if (!p) return false;
    unsigned long y = strtoul(p+strlen(tag), NULL, 0);  // this parses 0xabcd correctly
    if (errno == ERANGE) return false;
    x = y;
    return true;
}

// Same, for unsigned long long 
// 
inline bool parse_cl_ulong(const char* buf, const char* tag, cl_ulong& x) { 
    const char* p = strstr(buf, tag); 
    if (!p) return false; 
    unsigned long long y = boinc_strtoull(p+strlen(tag), NULL, 0);    // this parses 0xabcd correctly 
    if (errno == ERANGE) return false; 
    x = y;
    return true; 
    } 
    
extern bool parse(char* , char* );
extern bool parse_str(const char*, const char*, char*, int);
extern bool parse_str(const char* buf, const char* tag, std::string& dest);
extern void parse_attr(const char* buf, const char* attrname, char* out, int len);
extern bool parse_bool(const char*, const char*, bool&);

/////////////// END DEPRECATED XML PARSER

extern int copy_stream(FILE* in, FILE* out);
extern int strcatdup(char*& p, char* buf);
extern int dup_element_contents(FILE* in, const char* end_tag, char** pp);
extern int dup_element(FILE* in, const char* end_tag, char** pp);
extern int copy_element_contents(FILE* in, const char* end_tag, char* p, int len);
extern int copy_element_contents(FILE* in, const char* end_tag, std::string&);
extern void replace_element_contents(
    char* buf, const char* start, const char* end, const char* replacement
);
extern bool remove_element(char* buf, const char* start, const char* end);
extern bool str_replace(char* str, const char* old, const char* neww);
extern char* sgets(char* buf, int len, char* &in);
extern void non_ascii_escape(const char*, char*, int len);
extern void xml_escape(const char*, char*, int len);
extern void xml_unescape(std::string&);
extern void xml_unescape(const char*, char*, int len);
extern void extract_venue(const char*, const char*, char*);
extern int skip_unrecognized(char* buf, MIOFILE&);

#endif
