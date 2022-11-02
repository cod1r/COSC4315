typedef enum type {
  NUMBER,
  STRING,
  BOOLEAN,
  LIST,
  UNKNOWN,
  NO_TYPE,
	WORD,
  NONE,
} type;

struct value_obj {
  type t;
  void *obj;
};
