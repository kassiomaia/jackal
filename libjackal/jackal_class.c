#include <jackal.h>

/*
 * Methods
 */

jkl_method_t *jkl_method_new(jkl_string_t name, jkl_word_t arity,
                             jkl_word_t flags, jkl_word_t (*fn)())
{
  jkl_method_t *method = malloc(sizeof(jkl_method_t));
  if (method == NULL) {
    jkl_error("jkl_method_new", "out of memory");
  }

  method->name = name;
  method->arity = arity;
  method->flags = flags;
  method->fn = fn;
  return method;
}

void jkl_method_free(jkl_method_t *method)
{
  free(method);
}

/*
 * Class
 */

static jkl_word_t JKL_CLASS_ID = 0;
static jkl_class_t **JKL_TBL_CLASS = NULL;

jkl_class_t *jkl_class_new(jkl_string_t name)
{
  jkl_class_t *klass = malloc(sizeof(jkl_class_t));
  if (klass == NULL) {
    jkl_error("jkl_class", "failed to allocate memory for class");
  }

  JKL_CLASS_ID++;

  klass->id = JKL_CLASS_ID;
  klass->name = name;
  klass->base = NULL;
  klass->includes = NULL;
  klass->n_includes = 0;
  klass->flags = JKL_CLASS_STANDARD;
  klass->methods = NULL;
  klass->n_methods = 0;

  JKL_TBL_CLASS = realloc(JKL_TBL_CLASS, sizeof(jkl_class_t *) * JKL_CLASS_ID);
  if (JKL_TBL_CLASS == NULL) {
    jkl_error("jkl_class", "failed to allocate memory for class table");
  }

  JKL_TBL_CLASS[JKL_CLASS_ID - 1] = klass;
  return klass;
}

jkl_word_t jkl_class_set_flag(jkl_class_t *klass, jkl_class_flag_t flag)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "failed to set the flag: class is NULL");
  }

  klass->flags |= flag;
  return 0;
}

jkl_word_t jkl_class_has_flag(jkl_class_t *klass, jkl_class_flag_t flag)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "failed to check the flag: class is NULL");
  }

  return klass->flags & flag;
}

jkl_word_t jkl_class_set_base(jkl_class_t *klass, jkl_class_t *base)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "failed to set the base: class is NULL");
  }

  if (base == NULL) {
    jkl_error("jkl_class", "failed to set the base: base class is NULL");
  }

  klass->base = base;
  return 0;
}

void jkl_class_include(jkl_class_t *klass, jkl_class_t *include)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "failed to include: class is NULL");
  }

  if (include == NULL) {
    jkl_error("jkl_class", "included class is NULL");
  }

  if (klass->includes == NULL) {
    klass->includes = malloc(sizeof(jkl_class_t *));
    if (klass->includes == NULL) {
      jkl_error("jkl_class", "failed to allocate memory for included classes");
    }

    klass->includes[0] = include;
    klass->n_includes = 1;
  } else {
    klass->includes = realloc(klass->includes,
                              sizeof(jkl_class_t *) * (klass->n_includes + 1));
    if (klass->includes == NULL) {
      jkl_error("jkl_class", "failed to allocate memory for included classes");
    }

    klass->includes[klass->n_includes] = include;
    klass->n_includes++;
  }
}

jkl_word_t jkl_class_is_a(jkl_class_t *klass, jkl_class_t *base)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "class is NULL");
  }

  if (base == NULL) {
    jkl_error("jkl_class", "base class is NULL");
  }

  if (klass == base) {
    return 1;
  }

  if (klass->includes != NULL) {
    for (jkl_word_t i = 0; i < klass->n_includes; i++) {
      if (jkl_class_is_a(klass->includes[i], base)) {
        return 1;
      }
    }
  }

  return 0;
}

jkl_word_t jkl_class_is_base_of(jkl_class_t *klass, jkl_class_t *base)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "class is NULL");
  }

  if (base == NULL) {
    jkl_error("jkl_class", "base class is NULL");
  }

  if (klass->base == NULL) {
    return 0;
  }

  if (klass->base == base) {
    return 1;
  }

  return jkl_class_is_base_of(klass->base, base);
}

jkl_word_t jkl_class_is_included_in(jkl_class_t *klass, jkl_class_t *base)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "class is NULL");
  }

  if (base == NULL) {
    jkl_error("jkl_class", "base class is NULL");
  }

  if (klass->includes == NULL) {
    return 0;
  }

  for (jkl_word_t i = 0; i < klass->n_includes; i++) {
    if (klass->includes[i] == base) {
      return 1;
    }
  }

  return 0;
}

jkl_word_t jkl_class_method_exists(jkl_class_t *klass, jkl_string_t name)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "class is NULL");
  }

  if (klass->methods == NULL) {
    return 0;
  }

  for (jkl_word_t i = 0; i < klass->n_methods; i++) {
    if (strcmp(klass->methods[i]->name, name) == 0) {
      return 1;
    }
  }

  return 0;
}

jkl_word_t jkl_class_add_method(jkl_class_t *klass, jkl_method_t *method)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "class is NULL");
  }

  if (method == NULL) {
    jkl_error("jkl_class", "method is NULL");
  }

  if (jkl_class_method_exists(klass, method->name)) {
    return 1;
  }

  if (klass->methods == NULL) {
    klass->methods = malloc(sizeof(jkl_method_t *));
    if (klass->methods == NULL) {
      jkl_error("jkl_class", "failed to allocate memory for methods");
    }

    klass->methods[0] = method;
    klass->n_methods = 1;
  } else {
    klass->methods = realloc(klass->methods,
                             sizeof(jkl_method_t *) * (klass->n_methods + 1));
    if (klass->methods == NULL) {
      jkl_error("jkl_class", "failed to allocate memory for methods");
    }

    klass->methods[klass->n_methods] = method;
    klass->n_methods++;
  }

  return 0;
}

void jkl_class_destroy(jkl_class_t *klass)
{
  if (klass == NULL) {
    jkl_error("jkl_class", "class is NULL");
  }

  if (klass->includes != NULL) {
    free(klass->includes);
  }

  free(klass);
}

void jkl_class_destroy_all()
{
  if (JKL_TBL_CLASS == NULL) {
    return;
  }

  for (jkl_word_t i = 0; i < JKL_CLASS_ID; i++) {
    jkl_class_destroy(JKL_TBL_CLASS[i]);
  }

  free(JKL_TBL_CLASS);
}

jkl_class_t *jkl_class_get(jkl_word_t id)
{
  if (id > JKL_CLASS_ID) {
    jkl_error("jkl_class", "class id is out of range");
  }

  return JKL_TBL_CLASS[id - 1];
}

jkl_word_t jkl_not_implemented()
{
  jkl_error("jkl_class", "method not implemented");
}

void jkl_class_init()
{
  JKL_TBL_CLASS = malloc(sizeof(jkl_class_t *));

  // Define the base class
  jkl_class_t *Base = jkl_class_new("Object");
  jkl_class_set_flag(Base, JKL_CLASS_STANDARD);
  jkl_class_set_flag(Base, JKL_CLASS_FINAL);

  jkl_class_add_method(Base, jkl_method_new("new", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Base, jkl_method_new("to_hash", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Base, jkl_method_new("attributes", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Base, jkl_method_new("methods", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Base, jkl_method_new("base", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Base, jkl_method_new("includes", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));

  // Define the stirng class
  jkl_class_t *String = jkl_class_new("String");
  jkl_class_set_flag(String, JKL_CLASS_STANDARD);
  jkl_class_set_flag(String, JKL_CLASS_FINAL);
  jkl_class_set_base(String, Base);

  jkl_class_add_method(String, jkl_method_new("new", 1,
                       JKL_METHOD_STATIC_OVERRIDE, jkl_not_implemented));
  jkl_class_add_method(String, jkl_method_new("length", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(String, jkl_method_new("concat", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(String, jkl_method_new("split", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(String, jkl_method_new("replace", 2, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(String, jkl_method_new("reverse", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(String, jkl_method_new("at", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));

  // Define the integer class
  jkl_class_t *Integer = jkl_class_new("Integer");
  jkl_class_set_flag(Integer, JKL_CLASS_STANDARD);
  jkl_class_set_flag(Integer, JKL_CLASS_FINAL);
  jkl_class_set_base(Integer, Base);

  jkl_class_add_method(Integer, jkl_method_new("new", 1,
                       JKL_METHOD_STATIC_OVERRIDE, jkl_not_implemented));

  // Define the float class
  jkl_class_t *Float = jkl_class_new("Float");
  jkl_class_set_flag(Float, JKL_CLASS_STANDARD);
  jkl_class_set_flag(Float, JKL_CLASS_FINAL);
  jkl_class_set_base(Float, Base);

  jkl_class_add_method(Float, jkl_method_new("new", 1, JKL_METHOD_STATIC_OVERRIDE,
                       jkl_not_implemented));

  // Define the boolean class
  jkl_class_t *Boolean = jkl_class_new("Boolean");
  jkl_class_set_flag(Boolean, JKL_CLASS_STANDARD);
  jkl_class_set_flag(Boolean, JKL_CLASS_FINAL);
  jkl_class_set_base(Boolean, Base);

  jkl_class_add_method(Boolean, jkl_method_new("new", 1,
                       JKL_METHOD_STATIC_OVERRIDE, jkl_not_implemented));

  // Define the array class
  jkl_class_t *Array = jkl_class_new("Array");
  jkl_class_set_flag(Array, JKL_CLASS_STANDARD);
  jkl_class_set_flag(Array, JKL_CLASS_FINAL);
  jkl_class_set_base(Array, Base);

  jkl_class_add_method(Array, jkl_method_new("new", 1, JKL_METHOD_STATIC_OVERRIDE,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("length", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("at", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("push", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("pop", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("shift", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("unshift", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("insert", 2, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("remove", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("clear", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("each", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("map", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("filter", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("reduce", 2, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("reverse", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("sort", 0, JKL_METHOD_STATIC,
                       jkl_not_implemented));
  jkl_class_add_method(Array, jkl_method_new("join", 1, JKL_METHOD_STATIC,
                       jkl_not_implemented));


  /*
   * IO
   */

  // Define the io class

  jkl_class_t *IO = jkl_class_new("IO");
  jkl_class_set_flag(IO, JKL_CLASS_STANDARD);
  jkl_class_set_flag(IO, JKL_CLASS_FINAL);
  jkl_class_set_base(IO, Base);

  jkl_class_add_method(IO, jkl_method_new("new", 0, JKL_METHOD_STATIC_OVERRIDE,
                                          jkl_not_implemented));
  jkl_class_add_method(IO, jkl_method_new("print", 1, JKL_METHOD_STATIC,
                                          jkl_not_implemented));
  jkl_class_add_method(IO, jkl_method_new("println", 1, JKL_METHOD_STATIC,
                                          jkl_not_implemented));
  jkl_class_add_method(IO, jkl_method_new("read", 0, JKL_METHOD_STATIC,
                                          jkl_not_implemented));
  jkl_class_add_method(IO, jkl_method_new("readln", 0, JKL_METHOD_STATIC,
                                          jkl_not_implemented));
  jkl_class_add_method(IO, jkl_method_new("puts", 1, JKL_METHOD_STATIC,
                                          jkl_not_implemented));
  jkl_class_add_method(IO, jkl_method_new("gets", 0, JKL_METHOD_STATIC,
                                          jkl_not_implemented));

  atexit(jkl_class_destroy_all);
}
