#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*TestFuncPtr)();

enum TJames_ErrorType
{
        WARNING_ERROR = 0,
        NORMAL_ERROR,
        CRITICAL_ERROR
};

enum TJames_TestResult
{
        EMPTY_TEST = 0,
        SUCCESSFUL_TEST,
        FAILED_TEST,
        SKIPED_TEST
};

struct TJames_List
{
        void *data;
        size_t size_of_element;
        size_t count;
        size_t reserved;
};

#define LIST_EPTR(element_type, list, index) ((element_type*)list.data + index)
#define LIST_E(element_type, list, index) (*LIST_EPTR(element_type, list, index))

struct TJames_TestFuncData
{
        const char *func_name;
        const char *group_name;
        size_t added_on_line;
        const char *file;
        const char *file_name;
};

struct TJames_TestFunc
{
        TestFuncPtr func_ptr;
        struct TJames_TestFuncData data;
};

struct TJames_Error
{
        const char *message;
        enum TJames_ErrorType type; 
        size_t line;
};

typedef struct TJames_List TJames_TestFuncList;
typedef struct TJames_List TJames_ErrorList;

struct TJames_CoreData
{
        TJames_TestFuncList func_list;
        TJames_ErrorList error_list;
        enum TJames_TestResult last_test_result;
};


static struct TJames_CoreData GLOBAL_CORE_DATA;

struct TJames_List TJames_CreateList(const size_t size_of_element)
{
        struct TJames_List list;
        list.data = NULL;
        list.size_of_element = size_of_element;
        list.count = 0;
        list.reserved = 0;
        return list;
}

void TJames_DestroyList(struct TJames_List *list)
{
        free(list->data);
        list->size_of_element = 0;
        list->count = 0;
        list->reserved = 0;
}

int TJames_AddToList(struct TJames_List *list, const void *data)
{
        if(list->reserved <= list->count) {
                if(list->data == NULL || list->reserved == 0){
                        list->reserved = 10;
                        list->count = 0;
                        list->data = malloc(list->reserved * list->size_of_element);
                } else {
                        list->reserved *= 2;
                        list->data = realloc(list->data, list->reserved * list->size_of_element);
                }
                if(list->data == NULL)
                {
                        printf("TJames Error: Memory allocation failed, while adding a element to a list!\n");
                        return -1;
                }
        }
        
        size_t element_byte_offset = list->count * list->size_of_element; 
        void* element_ptr = (char*)list->data + element_byte_offset; 
        memcpy(element_ptr, data, list->size_of_element);
        ++list->count;
        return 0;
}

void TJames_ClearList(struct TJames_List *list)
{
        list->count = 0;
}


const char *GetFileName(const char *path)
{
        size_t last_slash = 0;
        for(size_t i = 0; path[i] != '\0'; ++i) {
                if(path[i] == '/') {
                        last_slash = i;
                }
        }
        return path + last_slash + 1;
}

void TJames_PushError(const char* message, const enum TJames_ErrorType type, const size_t line)
{
        struct TJames_Error error;
        error.message = message;
        error.type = type;
        error.line = line;
        TJames_AddToList(&GLOBAL_CORE_DATA.error_list, &error);
}

void TJames_Init()
{
        GLOBAL_CORE_DATA.func_list = TJames_CreateList(sizeof(struct TJames_TestFunc));
        GLOBAL_CORE_DATA.error_list = TJames_CreateList(sizeof(struct TJames_Error));
        GLOBAL_CORE_DATA.last_test_result = EMPTY_TEST;
        printf("Initilized TJames!\n");
}

void TJames_Destroy()
{
        TJames_DestroyList(&GLOBAL_CORE_DATA.func_list);
        TJames_DestroyList(&GLOBAL_CORE_DATA.error_list);
        printf("Destroyed TJames!\n");
}


void TJames_DumpTestFunc(const struct TJames_TestFuncData* func)
{
        printf("%s: [GROUP: %s] [FUNC: %s] ", func->file_name, func->group_name, func->func_name);
}

void TJames_DumpLineTestFunc(const struct TJames_TestFuncData* func)
{
        TJames_DumpTestFunc(func);
        printf("\n");
}

const char *TJames_ErrorTypeString(const enum TJames_ErrorType type)
{
        switch(type)
        {
        case WARNING_ERROR:
                return "WARNING";
        case NORMAL_ERROR:
                return "ERROR";
        case CRITICAL_ERROR:
                return "CRITICAL";
        }
        return "ERROR IN ERROR TYPE STRING";
}

void TJames_ReportErrors(const struct TJames_TestFuncData* func)
{
        for(size_t j = 0; j < GLOBAL_CORE_DATA.error_list.count; ++j) {
                struct TJames_Error *error = LIST_EPTR(struct TJames_Error, GLOBAL_CORE_DATA.error_list, j);
                const char *error_type = TJames_ErrorTypeString(error->type);
                printf("%s:%lu (%s) [%s] %s\n", func->file, error->line, func->func_name, error_type, error->message);
        }
}

void TJames_ReportTestFuncResult(const struct TJames_TestFuncData* func)
{
        switch(GLOBAL_CORE_DATA.last_test_result)
        {
        default:
        case FAILED_TEST:
                printf("- Failed!\n");
                break;
        case EMPTY_TEST:
                TJames_PushError("Empty Test", WARNING_ERROR, 0);
        case SUCCESSFUL_TEST:
                printf("- Success!\n");
                break;
        case SKIPED_TEST:
                printf("- Skipped!\n");
                break;
        }
        TJames_ReportErrors(func);
}

int TJames_RunTestFunc(const size_t index)
{
        GLOBAL_CORE_DATA.last_test_result = EMPTY_TEST;
        TJames_ClearList(&GLOBAL_CORE_DATA.error_list);

        struct TJames_TestFunc *func = LIST_EPTR(struct TJames_TestFunc, GLOBAL_CORE_DATA.func_list, index);

        TJames_DumpTestFunc(&func->data);

        func->func_ptr();

        TJames_ReportTestFuncResult(&func->data);

        return GLOBAL_CORE_DATA.last_test_result == SUCCESSFUL_TEST || GLOBAL_CORE_DATA.last_test_result == EMPTY_TEST;
}

int TJames_Run()
{
        size_t test_count = GLOBAL_CORE_DATA.func_list.count;
        size_t failed_tests = 0;
        for(size_t i = 0; i < GLOBAL_CORE_DATA.func_list.count; ++i)
        {
                printf("\n");
                int result = TJames_RunTestFunc(i);
                if(!result){
                        failed_tests += 1;
                }
        }
        printf("\nRun a total of %lu tests, with a successrate of %lu/%lu\n",
                test_count, test_count - failed_tests, test_count);
        TJames_Destroy();
        return failed_tests != 0;
}

int TJames_AddFunc(struct TJames_CoreData *core_data,
        const TestFuncPtr func_ptr,
        const char *func_name,
        const char *group_name,
        const size_t added_on_line,
        const char* file)
{
        if(core_data == NULL)
        {
                printf("TJames Error: core_data is a nullptr!\n");
                return -1;
        }

        struct TJames_TestFunc test_func;
        test_func.func_ptr = func_ptr;
        test_func.data.func_name = func_name;
        test_func.data.group_name = (group_name) ? group_name : "Default";
        test_func.data.added_on_line = added_on_line;
        test_func.data.file = file;
        test_func.data.file_name = GetFileName(file);
        return TJames_AddToList(&core_data->func_list, &test_func);
}



#define TJAMES_ADD_GROUPED_FUNC(func_ptr, group) TJames_AddFunc(&GLOBAL_CORE_DATA, func_ptr, #func_ptr, group, __LINE__, __FILE__)
#define TJAMES_ADD_FUNC(func_ptr) TJAMES_ADD_GROUPED_FUNC(func_ptr, NULL)

int last_result = 0;

#define TJAMES_SUCCESS()        do { \
                                        if(GLOBAL_CORE_DATA.last_test_result == EMPTY_TEST) { \
                                                GLOBAL_CORE_DATA.last_test_result = SUCCESSFUL_TEST; \
                                        } \
                                } while(0)

#define TJAMES_FAILURE(message) do { \
                                        GLOBAL_CORE_DATA.last_test_result = FAILED_TEST; \
                                        TJames_PushError(message, NORMAL_ERROR, __LINE__); \
                                } while(0)

#define TJAMES_FAILURE_EXIT(message)    do { \
                                                TJAMES_FAILURE(message); \
                                                return; \
                                        } while(0)

#define TJAMES_CMP(a, op, b, message)   do { \
                                                if (a op b) { \
                                                        TJAMES_SUCCESS(); \
                                                } else { \
                                                        TJAMES_FAILURE_EXIT(message); \
                                                } \
                                        } while(0)

#define TJAMES_EQUAL(a, b) TJAMES_CMP(a, ==, b, "Failed equal comparison!") 
#define TJAMES_NOT_EQUAL(a, b) TJAMES_CMP(a, !=, b, "Failed not equal comparison!") 
#define TJAMES_LESS(a, b) TJAMES_CMP(a, <, b, "Failed less comparison!") 
#define TJAMES_GREATER(a, b) TJAMES_CMP(a, >, b, "Failed greater comparison!") 
#define TJAMES_LESS_EQUAL(a, b) TJAMES_CMP(a, <=, b, "Failed less equal comparison!") 
#define TJAMES_GREATER_EQUAL(a, b) TJAMES_CMP(a, >=, b, "Failed greater equal comparison!") 

#define TJAMES_WARNING(message) do { \
                                        TJames_PushError(message, WARNING_ERROR, __LINE__); \
                                } while(0)

#define TJAMES_SKIP()   do { \
                                GLOBAL_CORE_DATA.last_test_result = SKIPED_TEST; \
                                return; \
                        } while(0)

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

int main() {
    TJames_Init();

    TJAMES_ADD_GROUPED_FUNC(test_lexer_plus, "Lexer");
    TJAMES_ADD_GROUPED_FUNC(test_lexer_minus, "Lexer");
    TJAMES_ADD_GROUPED_FUNC(test_lexer_integer, "Lexer");
    TJAMES_ADD_GROUPED_FUNC(test_lexer_sequence, "Lexer");

    return TJames_Run();
}
