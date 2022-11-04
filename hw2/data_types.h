typedef enum type {
  NUMBER,
  STRING,
  BOOLEAN,
  LIST,
  IDENTIFIER_NODE,
  NO_TYPE,
  WORD,
  NONE,
  KEYWORD_NODE,
} type;

struct value_obj {
  type t;
  void *obj;
};
