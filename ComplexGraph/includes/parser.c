#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <complex.h>

#define NDEBUG

#include <assert.h>

/* Im pretty sure the fact that these will only be interfaced with as pointers nullfies the usefulness of the `inline', but oh well. */

static inline int cisnan(complex a) { return isnan(creal(a)) || isnan(cimag(a)); }

static inline complex cadd(complex a, complex b) { return a + b; }
static inline complex csub(complex a, complex b) { return a - b; }
static inline complex cmult(complex a, complex b) { return a * b; }
static inline complex cdiv(complex a, complex b) { return a / b; }
static inline complex croot(complex a, complex b) { return pow(a, 1 / b); }
static inline complex clogs(complex a, complex b) { return clog(a) / clog(b); }
static inline complex csquare(complex a) { return a * a; }

static inline complex cfloor(complex a) { return floor(creal(a)) + I * floor(cimag(a)); }
static inline complex cround(complex a) { return round(creal(a)) + I * round(cimag(a)); }
static inline complex cceil(complex a) { return ceil(creal(a)) + I * ceil(cimag(a)); }
static inline complex cmod(complex a, complex b) { return a - b * cfloor(a / b); }

static inline complex csec(complex a) { return 1 / ccos(a); }
static inline complex ccsc(complex a) { return 1 / csin(a); }
static inline complex ccot(complex a) { return 1 / ctan(a); }

static inline complex csech(complex a) { return 1 / ccosh(a); }
static inline complex ccsch(complex a) { return 1 / csinh(a); }
static inline complex ccoth(complex a) { return 1 / ctanh(a); }

static inline complex creal_wrapper(complex a) { return (complex) creal(a); }
static inline complex cimag_wrapper(complex a) { return (complex) cimag(a); }
static inline complex carg_wrapper(complex a) { return (complex) carg(a); }
static inline complex cabs_wrapper(complex a) { return (complex) cabs(a); }

typedef enum Type {
    TYPE_NUMBER,
    TYPE_VARIABLE,
    TYPE_UNFUNCTION,
    TYPE_BIFUNCTION
} Type;

typedef union Value {
    complex number;
    char variable;
    complex (*unfunction)(complex), (*bifunction)(complex, complex);
} Value;

typedef struct Token {
    Type type;
    Value value;
} Token;

typedef struct Node Node;

struct Node {
    Token token;
    Node *next;
};

Node *Node_new(Token token)
{
    Node *ret = malloc(sizeof *ret);

    assert(ret != NULL);

    ret->token = token;
    ret->next = NULL;

    return ret;
}

void Node_delete(Node **node)
{
    assert(node != NULL && *node != NULL);

    free(*node);
    *node = NULL;
}

void Queue_delete_all(Node **queue_head)
{
    if (queue_head == NULL || *queue_head == NULL)
        return;

    Queue_delete_all(&(*queue_head)->next);

    Node_delete(queue_head);
}

void Queue_enqueue(Node **queue_tail, Token queuee_token)
{
    assert(queue_tail != NULL && *queue_tail != NULL);

    (*queue_tail)->next = Node_new(queuee_token);

    assert((*queue_tail)->next != NULL);

    *queue_tail = (*queue_tail)->next;

}

void Queue_enqueue_Node(Node **queue_tail, Node *node)
{
    assert(queue_tail != NULL && *queue_tail != NULL && node != NULL);

    (*queue_tail)->next = node;

    *queue_tail = (*queue_tail)->next;
}

void Queue_dequeue(Node **queue_head)
{
    assert(queue_head != NULL && *queue_head != NULL);

    *queue_head = (*queue_head)->next;
}

void Node_print(Node *node);

void Queue_print(Node *queue_head)
{
    Node *curr = queue_head;

    while (curr != NULL) {
        Node_print(curr);

        Queue_dequeue(&curr);
    }
}

void Stack_push(Node **stack_top, Token pushee_token)
{
    assert(stack_top != NULL && *stack_top != NULL);

    Node *pushee = Node_new(pushee_token);

    assert(pushee != NULL);

    pushee->next = *stack_top;

    *stack_top = pushee;
}

/* There technically isn't a need for this function, but oh well. */

void Stack_pop(Node **stack_top)
{
    assert(stack_top != NULL && *stack_top != NULL);

    *stack_top = (*stack_top)->next;
}

void Stack_print(Node *stack_top)
{
    Node *curr = stack_top;

    while (curr != NULL) {
        Node_print(curr);

        Stack_pop(&curr);
    }
}

/* Have a `Node_check' and a `Queue_check' function */

typedef char *String;

String String_alloc(size_t buffer_size)
{
    char *ret = malloc(2 *sizeof(size_t) + buffer_size);

    assert(ret != NULL);

    *(size_t *) &ret[0 * sizeof(size_t)] = 0;
    *(size_t *) &ret[1 * sizeof(size_t)] = buffer_size;
    ret[2 * sizeof(size_t)] = '\0';

    return ret + 2 * sizeof(size_t);
}

void String_delete(String *string)
{
    assert(string != NULL && *string != NULL);

    free((char *) (*string - 2 * sizeof(size_t)));
    *string = NULL;
}

size_t String_get_size(String string)
{
    assert(string != NULL);

    return *(size_t *) (string - 2 * sizeof(size_t));
}

size_t String_get_buffer_size(String string)
{
    assert(string != NULL);

    return *(size_t *) (string - sizeof(size_t));
}

size_t String_set_size(String string, size_t size)
{
    assert(string != NULL);

    return *(size_t *) (string - 2 * sizeof(size_t)) = size;
}

size_t String_set_buffer_size(String string, size_t buffer_size)
{
    assert(string != NULL);

    return *(size_t *) (string - sizeof(size_t)) = buffer_size;
}

char *String_get_pointer(String string)
{
    assert(string != NULL);

    return string - 2 * sizeof(size_t);
}

char *String_set_pointer(String *string, char *pointer)
{
    assert(string != NULL && *string != NULL && pointer != NULL);

    return *string = pointer + 2 * sizeof(size_t);
}

void String_resize(String *string)
{
    assert(string != NULL && *string != NULL);

    char *ret = realloc(String_get_pointer(*string), 2 * sizeof(size_t) + String_get_buffer_size(*string) * 2);

    assert(ret != NULL);

    String_set_pointer(string, ret);
    String_set_buffer_size(*string, String_get_buffer_size(*string) * 2);
}

/* Maybe this function should resize `string' to match the size of c_string */
void String_from_c_string(String string, const char *c_string)
{
    assert(string != NULL && c_string != NULL);

    size_t buffer_size = String_get_buffer_size(string);

    size_t i = 0;

    while (i < buffer_size - 1 && c_string[i] != '\0') {
        string[i] = c_string[i];

        i++;
    }

    string[i] = '\0';

    *(size_t *) (string - 2 * sizeof(size_t)) = i;
}

void String_append(String *string, char character)
{
    assert(string != NULL && *string != NULL && character != '\0');

    if (String_get_size(*string) >= String_get_buffer_size(*string) - 1)
        String_resize(string);

    (*string)[String_get_size(*string)] = character;
    (*string)[String_set_size(*string, String_get_size(*string) + 1)] = '\0';
}

void String_clear(String string)
{
    assert(string != NULL);

    String_set_size(string, 0);

    string[0] = '\0';
}

int String_equals_c_string(String string, const char *c_string)
{
    size_t i = 0;

    while (string[i] != '\0') {
        if (string[i] != c_string[i])
            return 0;

        i++;
    }

    return string[i] == c_string[i];
}

void parse_name(String *string_buffer, FILE *file)
{
    assert(string_buffer != NULL && *string_buffer != NULL && file != NULL);

    int c;

    while ('a' <= (c = fgetc(file)) && c <= 'z')
        String_append(string_buffer, c);

    ungetc(c, file);
}

void parse_upper_name(String *string_buffer, FILE *file)
{
    assert(string_buffer != NULL && *string_buffer != NULL && file != NULL);

    int c;

    while ('A' <= (c = fgetc(file)) && c <= 'Z')
        String_append(string_buffer, c);

    ungetc(c, file);
}

void parse_number(String *string_buffer, FILE *file)
{
    assert(string_buffer != NULL && *string_buffer != NULL && file != NULL);

    int c, has_decimal = 0;

    while (('0' <= (c = fgetc(file)) && c <= '9') || (c == '.' && !has_decimal)) {
        if (c == '.')
            has_decimal = 1;

        String_append(string_buffer, c);
    }

    ungetc(c, file);
}

/* Support more inverse trig functions */

typedef struct Pair {
    const char *key;
    Value value;
} Pair;

Pair complex_unfunctions_list[] = {
    (Pair) { .key = "floor", .value.unfunction = cfloor },
    (Pair) { .key = "round", .value.unfunction = cround },
    (Pair) { .key = "ceil", .value.unfunction = cceil },
    (Pair) { .key = "re", .value.unfunction = creal_wrapper },
    (Pair) { .key = "im", .value.unfunction = cimag_wrapper },
    (Pair) { .key = "arg", .value.unfunction = carg_wrapper },
    (Pair) { .key = "mag", .value.unfunction = cabs_wrapper },

    (Pair) { .key = "sin", .value.unfunction = csin },
    (Pair) { .key = "cos", .value.unfunction = ccos },
    (Pair) { .key = "tan", .value.unfunction = ctan },
    (Pair) { .key = "sec", .value.unfunction = csec },
    (Pair) { .key = "csc", .value.unfunction = ccsc },
    (Pair) { .key = "cot", .value.unfunction = ccot },
    (Pair) { .key = "sinh", .value.unfunction = csinh },
    (Pair) { .key = "cosh", .value.unfunction = ccosh },
    (Pair) { .key = "tanh", .value.unfunction = ctanh },
    (Pair) { .key = "sech", .value.unfunction = csech },
    (Pair) { .key = "csch", .value.unfunction = ccsch },
    (Pair) { .key = "coth", .value.unfunction = ccoth },

    (Pair) { .key = "arcsin", .value.unfunction = casin },
    (Pair) { .key = "arccos", .value.unfunction = cacos },
    (Pair) { .key = "arctan", .value.unfunction = catan },
    (Pair) { .key = "argsinh", .value.unfunction = casinh },
    (Pair) { .key = "argcosh", .value.unfunction = cacosh },
    (Pair) { .key = "argtanh", .value.unfunction = catanh },

    (Pair) { .key = "conj", .value.unfunction = conj },

    (Pair) { .key = "exp", .value.unfunction = cexp },
    (Pair) { .key = "ln", .value.unfunction = clog },

    (Pair) { .key = "sqrt", .value.unfunction = csqrt },
    (Pair) { .key = "square", .value.unfunction = csquare },
};

void Node_print(Node *node)
{
    assert(node != NULL);

    switch (node->token.type) {
        case TYPE_NUMBER:
            printf("%f + I * %f\n", creal(node->token.value.number), cimag(node->token.value.number));
            break;
        case TYPE_VARIABLE:
            printf("%c\n", node->token.value.variable);
            break;
        case TYPE_UNFUNCTION:
            for (int i = 0; i < sizeof complex_unfunctions_list / sizeof *complex_unfunctions_list; i++)
                if (node->token.value.unfunction == complex_unfunctions_list[i].value.unfunction) {
                    printf("%s\n", complex_unfunctions_list[i].key);
                    return;
                }

            printf("(Invalid unary function)\n");
            break;
        case TYPE_BIFUNCTION:
            if (node->token.value.bifunction == cadd)
                printf("+\n");
            else if (node->token.value.bifunction == csub)
                printf("-\n");
            else if (node->token.value.bifunction == cmult)
                printf("*\n");
            else if (node->token.value.bifunction == cdiv)
                printf("/\n");
            else if (node->token.value.bifunction == cpow)
                printf("^\n");
            else
                printf("(Invalid binary function)\n");

            break;
    }
}

complex (*String_to_unfunction(String string))(complex)
{
    for (int i = 0; i < sizeof complex_unfunctions_list / sizeof *complex_unfunctions_list; i++)
        if (String_equals_c_string(string, complex_unfunctions_list[i].key))
            return complex_unfunctions_list[i].value.unfunction;

    return NULL;
}

Node *parser(FILE *file)
{
    assert(file != NULL);

    Node *queue_tail = NULL, *queue_head = NULL, *stack = NULL;

    String string_buffer = String_alloc(100);

    int c;

    while ((c = fgetc(file)) != EOF && c != '\n' && c != '\r') {
        if (c == ' ' || c == '\t') {
            continue;
        } else if (('0' <= c && c <= '9') || c == '.' || c == 'I') {
            int is_complex = c == 'I';

            if (!is_complex)
                ungetc(c, file);

            parse_number(&string_buffer, file);

            if (!is_complex) {
                c = fgetc(file);

                is_complex = c == 'I';

                if (!is_complex)
                    ungetc(c, file);
            }

            if (queue_tail != NULL)
                Queue_enqueue(&queue_tail, (Token) { .type = TYPE_NUMBER, (is_complex * I + !is_complex) * atof(string_buffer) + (String_get_size(string_buffer) == 0 && is_complex) * I });
            else
                queue_head = queue_tail = Node_new((Token) { .type = TYPE_NUMBER, .value.number = (is_complex * I + !is_complex) * atof(string_buffer) + (String_get_size(string_buffer) == 0 && is_complex) * I });

            String_clear(string_buffer);
        } else if ('a' <= c && c <= 'z') {
            ungetc(c, file);

            parse_name(&string_buffer, file);

            complex (*unfunction)(complex) = String_to_unfunction(string_buffer);

            if (stack != NULL)
                if (unfunction != NULL)
                    Stack_push(&stack, (Token) { .type = TYPE_UNFUNCTION, .value.unfunction = unfunction });
                else
                    return NULL;
            else
                if (unfunction != NULL)
                    stack = Node_new((Token) { .type = TYPE_UNFUNCTION, .value.unfunction = unfunction });
                else
                    return NULL;

            String_clear(string_buffer);
        } else if (c == 'Z') {
            if (queue_tail != NULL)
                Queue_enqueue(&queue_tail, (Token) { .type = TYPE_VARIABLE, .value.variable = 'Z' });
            else
                queue_head = queue_tail = Node_new((Token) { .type = TYPE_VARIABLE, .value.variable = 'Z' });
        } else if (c == '+' || c == '-') {
            while (stack != NULL && (stack->token.value.bifunction == cmult || stack->token.value.bifunction == cdiv || stack->token.value.bifunction == cmod || stack->token.value.bifunction == cpow)) {
                assert(queue_tail != NULL);
                /* Will the fact that `stack' is not pointing to `NULL' cause problems? Hopefully not. If it does, I can just set it to `NULL' later. */
                Queue_enqueue_Node(&queue_tail, stack);

                Stack_pop(&stack);
            }

            if (stack != NULL)
                Stack_push(&stack, (Token) { .type = TYPE_BIFUNCTION, .value.bifunction = c == '+' ? cadd : csub });
            else
                stack = Node_new((Token) { .type = TYPE_BIFUNCTION, .value.bifunction = c == '+' ? cadd : csub });
        } else if (c == '*' || c == '/' || c == '%') {
            while (stack != NULL && stack->token.value.bifunction == cpow) {
                assert(queue_tail != NULL);

                Queue_enqueue_Node(&queue_tail, stack);

                Stack_pop(&stack);
            }

            if (stack != NULL)
                Stack_push(&stack, (Token) { .type = TYPE_BIFUNCTION, .value.bifunction = c == '*' ? cmult : c == '/' ? cdiv : cmod });
            else
                stack = Node_new((Token) { .type = TYPE_BIFUNCTION, .value.bifunction = c == '*' ? cmult : c == '/' ? cdiv : cmod });
        } else if (c == '^') {
            while (stack != NULL && stack->token.value.bifunction == cpow) {
                assert(queue_tail != NULL);

                Queue_enqueue_Node(&queue_tail, stack);

                Stack_pop(&stack);
            }

            if (stack != NULL)
                Stack_push(&stack, (Token) { .type = TYPE_BIFUNCTION, .value.bifunction = cpow });
            else
                stack = Node_new((Token) { .type = TYPE_BIFUNCTION, .value.bifunction = cpow });
        } else if (c == '(') {
            if (stack != NULL)
                Stack_push(&stack, (Token) { .type = TYPE_BIFUNCTION, .value.bifunction = NULL }); /* This is some unholy C code - it's changed but it's still pretty bad */
            else
                stack = Node_new((Token) { .type = TYPE_BIFUNCTION, .value.bifunction = NULL });
        } else if (c == ')') {
            while (stack != NULL && stack->token.value.bifunction != NULL) {
                Queue_enqueue_Node(&queue_tail, stack);

                Stack_pop(&stack);
            }

            assert(stack != NULL && stack->token.value.bifunction == NULL);

            Node *temp = stack;

            Stack_pop(&stack);

            Node_delete(&temp);

            if (stack != NULL && stack->token.type == TYPE_UNFUNCTION) {
                Queue_enqueue_Node(&queue_tail, stack);

                Stack_pop(&stack);
            }
        } else {
            return NULL;
        }
    }

    while (stack != NULL && stack->token.value.bifunction != NULL) {
        Queue_enqueue_Node(&queue_tail, stack);

        Stack_pop(&stack);
    }

    assert(stack == NULL);

    String_delete(&string_buffer);

    return queue_head;
}

/* This does no error-checking as this should all be done by the parser */
complex eval(Node *queue_head, complex variable_value)
{
    /* Does Spartan programming make this slower? Hopefully not. */

    assert(queue_head != NULL);

    complex acc = NAN, aux = NAN, anc = NAN;

    Node *curr = queue_head;

    while (curr != NULL) {
        Value curr_value = curr->token.value;
        switch (curr->token.type) {
            case TYPE_VARIABLE:
                curr_value.number = variable_value;
            case TYPE_NUMBER:
                if (cisnan(curr_value.number))
                    return NAN;

                if (cisnan(acc))
                    acc = curr_value.number;
                else if (cisnan(aux))
                    aux = curr_value.number;
                else
                    anc = curr_value.number;
                break;
            case TYPE_UNFUNCTION:
                if (cisnan(aux)) {
                    complex out = curr_value.unfunction(acc);

                    if (cisnan(out))
                        return NAN;

                    acc = out;
                } else if (cisnan(anc)) {
                    complex out = curr_value.unfunction(aux);

                    if (cisnan(out))
                        return NAN;

                    aux = out;
                } else {
                    complex out = curr_value.unfunction(anc);

                    if (cisnan(anc))
                        return NAN;

                    anc = out;
                }
                break;
            case TYPE_BIFUNCTION:
                if (cisnan(anc)) {
                    complex out = curr_value.bifunction(acc, aux);

                    if (cisnan(out))
                        return NAN;

                    acc = out;
                    aux = NAN;
                } else {
                    complex out = curr_value.bifunction(aux, anc);

                    if (cisnan(out))
                        return NAN;

                    aux = out;
                    anc = NAN;
                }
                break;
        }

        Queue_dequeue(&curr);
    }

    return acc;
}
