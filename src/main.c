#include "tjames.h"

typedef enum {
    TOKEN_INT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_EOF
} TokenType;

struct Token {
    TokenType type;
    const char *text;
};

struct Token lex_next(const char **input) {
    while (**input == ' ') (*input)++;

    struct Token t;
    t.text = *input;

    switch (**input) {
        case '+': t.type = TOKEN_PLUS; (*input)++; break;
        case '-': t.type = TOKEN_MINUS; (*input)++; break;
        case '\0': t.type = TOKEN_EOF; break;
        default: t.type = TOKEN_INT; (*input)++; break;
    }
    return t;
}

void test_lexer_plus() {
    const char *input = "+";
    struct Token tok = lex_next(&input);
    TJAMES_EQUAL(tok.type, TOKEN_PLUS);
}

void test_lexer_minus() {
    const char *input = "-";
    struct Token tok = lex_next(&input);
    TJAMES_EQUAL(tok.type, TOKEN_MINUS);
}

void test_lexer_integer() {
    const char *input = "7";
    struct Token tok = lex_next(&input);
    TJAMES_EQUAL(tok.type, TOKEN_INT);
}

void test_lexer_sequence() {
    const char *input = "7 + - 3";
    struct Token tok;
    
    tok = lex_next(&input);
    TJAMES_EQUAL(tok.type, TOKEN_INT);

    tok = lex_next(&input);
    TJAMES_EQUAL(tok.type, TOKEN_PLUS);

    tok = lex_next(&input);
    TJAMES_EQUAL(tok.type, TOKEN_MINUS);

    tok = lex_next(&input);
    TJAMES_EQUAL(tok.type, TOKEN_INT);

    tok = lex_next(&input);
    TJAMES_EQUAL(tok.type, TOKEN_EOF);
}

void test_new()
{
        TJAMES_EQUAL_INT(2, 2);
        TJAMES_EQUAL_FLOAT(3.2, 3.2);
}

int main() {
        TJames_Init();

        TJAMES_ADD_GROUPED_FUNC(test_lexer_plus, "Lexer");
        TJAMES_ADD_GROUPED_FUNC(test_lexer_minus, "Lexer");
        TJAMES_ADD_GROUPED_FUNC(test_lexer_integer, "Lexer");
        TJAMES_ADD_GROUPED_FUNC(test_lexer_sequence, "Lexer");
        TJAMES_ADD_FUNC(test_new);

        return TJames_Run();
}
