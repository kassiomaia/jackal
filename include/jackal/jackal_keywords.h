/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf -t -L ANSI-C -C -E -k '1,3,$' -H hash -N keyword_lookup -p -t -j1 -o -i 1 -g -G -s 2 ./defs/keywords.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
&& ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
&& (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
&& ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
&& ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
&& ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
&& ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
&& ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
&& ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
&& ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
&& ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
&& ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
&& ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
&& ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
&& ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
&& ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
&& ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
&& ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
&& ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
&& ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
&& ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
&& ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
&& ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "./defs/keywords.gperf"
struct jkl_keyword {
  char *id, int has_block;
};
enum {
  TOTAL_KEYWORDS = 7,
  MIN_WORD_LENGTH = 2,
  MAX_WORD_LENGTH = 4,
  MIN_HASH_VALUE = 4,
  MAX_HASH_VALUE = 11
};

/* maximum key range = 8, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash(register const char *str, register size_t len)
{
  static const unsigned char asso_values[] = {
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12,  3,
    12,  1,  1, 12, 12,  1, 12, 12,  2, 12,
    3,  2,  2, 12,  1,  2,  2, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12
  };
  register unsigned int hval = len;

  switch (hval) {
    default:
      hval += asso_values[(unsigned char)str[2]];
    /*FALLTHROUGH*/
    case 2:
    case 1:
      hval += asso_values[(unsigned char)str[0]];
      break;
  }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static const struct jkl_keyword wordlist[] = {
  {""}, {""}, {""}, {""},
#line 5 "./defs/keywords.gperf"
  {"if", 1},
  {""},
#line 8 "./defs/keywords.gperf"
  {"for", 1},
#line 6 "./defs/keywords.gperf"
  {"elif", 1},
#line 7 "./defs/keywords.gperf"
  {"else", 1},
#line 3 "./defs/keywords.gperf"
  {"let", 0},
#line 4 "./defs/keywords.gperf"
  {"loop", 1},
#line 9 "./defs/keywords.gperf"
  {"func", 1}
};

const struct jkl_keyword *
keyword_lookup(register const char *str, register size_t len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
    register unsigned int key = hash(str, len);

    if (key <= MAX_HASH_VALUE) {
      register const char *s = wordlist[key].name;

      if (*str == *s && !strcmp(str + 1, s + 1)) {
        return &wordlist[key];
      }
    }
  }
  return 0;
}
