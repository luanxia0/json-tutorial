#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
	v->type = LEPT_NULL;
    c->json += 3;
    
    return LEPT_PARSE_OK;
}


static int lept_parse_false(lept_context* c, lept_value* v) {
	EXPECT(c, 'f');
	if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e') {
		return LEPT_PARSE_INVALID_VALUE;
	}

	v->type = LEPT_FALSE;
	c->json += 4;

	return LEPT_PARSE_OK;
}


static int lept_parse_true(lept_context* c, lept_value* v) {
	EXPECT(c, 't');
	if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e') {
		return LEPT_PARSE_INVALID_VALUE;
	}

	v->type = LEPT_TRUE;
	c->json += 3;

	return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
	case 'n':  return lept_parse_literal(c, v, "null", LEPT_NULL);//lept_parse_null(c, v);
	case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);//lept_parse_false(c, v);
	case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);//lept_parse_true(c, v);
    case '\0': return LEPT_PARSE_EXPECT_VALUE;
    default:   return LEPT_PARSE_INVALID_VALUE;
    }
}


static int lept_parse_literal(lept_context* c, lept_value* v, const char *literal_value, lept_type type) {

	EXPECT(c, *literal_value);
	size_t len = strlen(literal_value);
	for (size_t i = 0; i < len - 1; ++i) {
		if (c->json[i] != literal_value[i + 1]) {
			return LEPT_PARSE_INVALID_VALUE;
		}
	}

	c->json += (len - 1);

	v->type = type;
	return LEPT_PARSE_OK;


}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    int ret = lept_parse_value(&c, v);

	if (ret == LEPT_PARSE_OK) {
		lept_parse_whitespace(&c);
		if (*(c.json) != '\0') {
			return LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	return ret;
}
	

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}
