#include <jackal.h>

jkl_word_t jkl_string_len(jkl_string_t str)
{
  return strlen(str);
};

jkl_word_t jkl_string_hash(jkl_string_t str)
{
  jkl_word_t hash = 5381;

  jkl_word_t len = jkl_string_len(str);
  for (jkl_word_t i = 0; i < len; i++)
  {
    jkl_char_t c = str[i];
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return abs(hash);
}