#include "function.h"

primitive_function_t primitive_function_new(
        const char *name,
        unsigned int arity,
        bool precise_arity,
        ast_node_t *(*fn)(ast_list_t *args)) {
    size_t size = strlen(name) + 1;
    char *owned_name = malloc(size);
    strcpy(owned_name, name);

    return (primitive_function_t){
            .name = owned_name,
            .arity = arity,
            .precise_arity = precise_arity,
            .fn = fn,
    };
}
void primitive_function_free(primitive_function_t *fn) {
    free(fn->name);
    fn->name = NULL;
    fn->precise_arity = false;
    fn->arity = 0;
    fn->fn = NULL;
}

ast_node_t *prim_isboolean(ast_list_t *args) {
    if (args->len != 1) {
        return NULL;
    } else {
        ast_node_t *ref_node;
        bool result = ast_list_get(args, 0, &ref_node);
        bool eval = (result && ref_node->tag == TAG_BOOLEAN) ? true : false;
        return ast_node_new(TAG_BOOLEAN, &eval, sizeof(bool), 0, NULL);
    }
}

ast_node_t *prim_isnumber(ast_list_t *args) {
    if (args->len != 1) {
        return NULL;
    } else {
        ast_node_t *ref_node;
        bool result = ast_list_get(args, 0, &ref_node);
        bool eval = (result && (ref_node->tag == TAG_INTEGER || ref_node->tag == TAG_DOUBLE)) ? true : false;
        return ast_node_new(TAG_BOOLEAN, &eval, sizeof(bool), 0, NULL);
    }
}

ast_node_t *prim_issymbol(ast_list_t *args) {
    if (args->len != 1) {
        return NULL;
    } else {
        ast_node_t *ref_node;
        bool result = ast_list_get(args, 0, &ref_node);
        bool eval = (result && ref_node->tag == TAG_SYMBOL) ? true : false;
        return ast_node_new(TAG_BOOLEAN, &eval, sizeof(bool), 0, NULL);
    }
}

ast_node_t *prim_isstring(ast_list_t *args) {
    if (args->len != 1) {
        return NULL;
    } else {
        ast_node_t *ref_node;
        bool result = ast_list_get(args, 0, &ref_node);
        bool eval = (result && ref_node->tag == TAG_STRING) ? true : false;
        return ast_node_new(TAG_BOOLEAN, &eval, sizeof(bool), 0, NULL);
    }
}

ast_node_t *prim_add(ast_list_t *args) {
    int sum = 0;

    for (unsigned int i = 0; i < args->len; i += 1) {
        ast_node_t *arg;
        if (!ast_list_get_clone(args, i, &arg)) {
            return NULL;
        } else {
            ast_list_t tmp = ast_list_new();
            ast_list_push(&tmp, arg);

            ast_node_t *result_node = prim_isnumber(&tmp);
            bool is_number = *(bool *) (result_node->data);
            if (is_number) {
                sum += *(int *) arg->data;
            } else {
                ast_list_free(&tmp);
                return NULL;
            }
            ast_node_free(result_node);
            ast_list_free(&tmp);
        }
    }

    return ast_node_new(TAG_INTEGER, &sum, sizeof(int), 0, NULL);
}


ast_node_t *prim_sub(ast_list_t *args) {
    if (args->len == 0) {
        return NULL;
    } else if (args->len == 1) {
        ast_node_t *arg;
        if (!ast_list_get_clone(args, 0, &arg)) {
            return NULL;
        } else {
            ast_list_t tmp = ast_list_new();
            ast_list_push(&tmp, arg);

            ast_node_t *result_node = prim_isnumber(&tmp);
            bool is_number = *(bool *) (result_node->data);
            ast_node_free(result_node);

            if (is_number) {
                int sub = 0 - *(int *) arg->data;
                ast_list_free(&tmp);
                return ast_node_new(TAG_INTEGER, &sub, sizeof(int), 0, NULL);
            } else {
                ast_list_free(&tmp);
                return NULL;
            }
        }
    } else {
        int sub;

        ast_node_t *arg;
        if (!ast_list_get_clone(args, 0, &arg)) {
            return NULL;
        } else {
            ast_list_t tmp = ast_list_new();
            ast_list_push(&tmp, arg);

            ast_node_t *result_node = prim_isnumber(&tmp);
            bool is_number = *(bool *) (result_node->data);
            ast_node_free(result_node);

            if (is_number) {
                sub = *(int *) arg->data;
            } else {
                ast_list_free(&tmp);
                return NULL;
            }

            ast_list_free(&tmp);
        }

        for (unsigned int i = 1; i < args->len; i += 1) {
            if (!ast_list_get_clone(args, i, &arg)) {
                return NULL;
            } else {
                ast_list_t tmp = ast_list_new();
                ast_list_push(&tmp, arg);

                ast_node_t *result_node = prim_isnumber(&tmp);
                bool is_number = *(bool *) (result_node->data);
                ast_node_free(result_node);

                if (is_number) {
                    sub -= *(int *) arg->data;
                } else {
                    ast_list_free(&tmp);
                    return NULL;
                }

                ast_list_free(&tmp);
            }
        }

        return ast_node_new(TAG_INTEGER, &sub, sizeof(int), 0, NULL);
    }
}

ast_node_t *prim_mul(ast_list_t *args) {
    int prod = 1;

    for (unsigned int i = 0; i < args->len; i += 1) {
        ast_node_t *arg;
        if (!ast_list_get_clone(args, i, &arg)) {
            return NULL;
        } else {
            ast_list_t tmp = ast_list_new();
            ast_list_push(&tmp, arg);

            ast_node_t *result_node = prim_isnumber(&tmp);
            bool is_number = *(bool *) (result_node->data);
            ast_node_free(result_node);

            if (is_number) {
                prod *= *(int *) arg->data;
            } else {
                ast_list_free(&tmp);
                return NULL;
            }

            ast_list_free(&tmp);
        }
    }

    return ast_node_new(TAG_INTEGER, &prod, sizeof(int), 0, NULL);
}

ast_node_t *prim_string_append(ast_list_t *args) {
    unsigned int buf_size = 128;
    char *buf = malloc(128);
    memset(buf, 0, buf_size);
    unsigned int len = 0;

    for (unsigned int i = 0; i < args->len; i += 1) {
        ast_node_t *arg;
        if (!ast_list_get_clone(args, i, &arg)) {
            break;
        } else {
            ast_list_t tmp = ast_list_new();
            ast_list_push(&tmp, arg);

            ast_node_t *result_node = prim_isstring(&tmp);
            bool is_string = *(bool *) (result_node->data);
            ast_node_free(result_node);

            if (is_string) {
                char *str = (char *)arg->data;
                len += strlen(str);

                while (buf_size <= len) {
                    buf_size *= 2;
                    char *tmp_buf = realloc(buf, buf_size);

                    if (tmp_buf) {
                        buf = tmp_buf;
                    }

                    memset(buf + len, 0, buf_size - len);
                }

                strcat(buf, str);
            }

            ast_list_free(&tmp);
        }
    }

    ast_node_t *eval = ast_node_new(TAG_STRING, buf, len + 1, 0, NULL);
    free(buf);
    return eval;
}
