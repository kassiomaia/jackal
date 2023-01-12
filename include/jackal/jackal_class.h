#ifndef JACKAL_CLASS_H
#define JACKAL_CLASS_H

#include <jackal.h>

/*
 * Methods
 */

typedef enum
{
  JKL_METHOD_STATIC,
  JKL_METHOD_STATIC_OVERRIDE,
  JKL_METHOD_INSTANCE,
  JKL_METHOD_INSTANCE_OVERRIDE,
} jkl_method_flag_t;

typedef struct
{
  jkl_word_t id;
  jkl_string_t name;
  jkl_word_t arity;
  jkl_word_t flags;
  jkl_word_t (*fn)();
} jkl_method_t;

jkl_method_t *jkl_method_new(jkl_string_t name, jkl_word_t arity, jkl_word_t flags, jkl_word_t (*fn)(void *));
void jkl_method_free(jkl_method_t *method);

/*
 * Class
 */

typedef enum
{
  JKL_CLASS_STANDARD = 0x00,
  JKL_CLASS_ABSTRACT = 0x01,
  JKL_CLASS_FINAL = 0x02,
  JKL_CLASS_NATIVE = 0x04,
} jkl_class_flag_t;

struct jkl_class
{
  jkl_word_t id;
  jkl_string_t name;
  struct jkl_class *base;
  struct jkl_class **includes;
  jkl_word_t n_includes;
  jkl_method_t **methods;
  jkl_word_t n_methods;
  jkl_word_t flags;
};

typedef struct jkl_class jkl_class_t;

jkl_class_t *jkl_class_new(jkl_string_t name);
jkl_word_t jkl_class_set_flag(jkl_class_t *klass, jkl_class_flag_t flag);
jkl_word_t jkl_class_has_flag(jkl_class_t *klass, jkl_class_flag_t flag);
jkl_word_t jkl_class_set_base(jkl_class_t *klass, jkl_class_t *base);
void jkl_class_include(jkl_class_t *klass, jkl_class_t *include);
jkl_word_t jkl_class_is_a(jkl_class_t *klass, jkl_class_t *base);
jkl_word_t jkl_class_is_base_of(jkl_class_t *klass, jkl_class_t *base);
jkl_word_t jkl_class_is_included_in(jkl_class_t *klass, jkl_class_t *base);
jkl_word_t jkl_class_method_exists(jkl_class_t *klass, jkl_string_t name);
jkl_word_t jkl_class_add_method(jkl_class_t *klass, jkl_method_t *method);
void jkl_class_destroy(jkl_class_t *klass);
void jkl_class_destroy_all();
jkl_class_t *jkl_class_get(jkl_word_t id);
jkl_word_t jkl_not_implemented();
void jkl_class_init();

#endif
