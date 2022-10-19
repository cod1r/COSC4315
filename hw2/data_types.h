typedef enum type
{
	NUMBER,
	FLOAT,
	STRING,
	BOOLEAN,
	LIST,
} type;
typedef struct value_obj
{
	void *bytes;
} value_obj;
typedef struct list
{
	value_obj *values;
} list;
