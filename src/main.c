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
        SUSPENDED_TEST
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

struct TJames_TestFunc
{
        TestFuncPtr func_ptr;
        const char *func_name;
        size_t added_on_line;
        const char *file;
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

int TJames_Run()
{
        int global_result = 0;
        for(size_t i = 0; i < GLOBAL_CORE_DATA.func_list.count; ++i)
        {
                GLOBAL_CORE_DATA.last_test_result = EMPTY_TEST;
                TJames_ClearList(&GLOBAL_CORE_DATA.error_list);

                struct TJames_TestFunc *test_func = LIST_EPTR(struct TJames_TestFunc, GLOBAL_CORE_DATA.func_list, i);

                printf("%s:%s  ", test_func->file, test_func->func_name);

                test_func->func_ptr();

                if(GLOBAL_CORE_DATA.last_test_result == SUCCESSFUL_TEST || GLOBAL_CORE_DATA.last_test_result == EMPTY_TEST) {
                        if(GLOBAL_CORE_DATA.last_test_result == EMPTY_TEST) { 
                                printf("[WARNING: Empty test!] ");
                        }
                        printf("Success!\n");
                } else {
                        printf("Failed!\n");
                        for(size_t j = 0; j < GLOBAL_CORE_DATA.error_list.count; ++j) {
                                struct TJames_Error *error = LIST_EPTR(struct TJames_Error, GLOBAL_CORE_DATA.error_list, j);
                                const char *error_type = "NORMAL";
                                if(error->type == WARNING_ERROR)
                                {
                                        error_type = "WARNING";
                                } else if(error->type == CRITICAL_ERROR)
                                {
                                        error_type = "CRITICAL";
                                }
                                printf("%s:%s:%lu [%s] %s\n", test_func->file, test_func->func_name, error->line, error_type, error->message);
                        }
                        global_result = 1;
                }
        }
        TJames_Destroy();
        return global_result;
}

int TJames_AddFunc(struct TJames_CoreData *core_data, const TestFuncPtr func_ptr, const char *func_name,
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
        test_func.func_name = func_name;
        test_func.added_on_line = added_on_line;
        test_func.file = file;
        return TJames_AddToList(&core_data->func_list, &test_func);
}

#define TJAMES_ADD_FUNC(func_ptr) TJames_AddFunc(&GLOBAL_CORE_DATA, func_ptr, #func_ptr, __LINE__, __FILE__)

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


void a()
{
        TJAMES_EQUAL(1, 1);
        TJAMES_EQUAL(3, 2);
        TJAMES_EQUAL(2, 2);
}

void b()
{
        TJAMES_NOT_EQUAL(1, 2);
        TJAMES_NOT_EQUAL(3, 3);
        TJAMES_NOT_EQUAL(2, 4);
}

void c()
{
        TJAMES_LESS(1, 2);
        TJAMES_GREATER(4, 4);
        TJAMES_LESS_EQUAL(2, 4);
        TJAMES_GREATER_EQUAL(1, 4);
}

void d()
{

}

void e()
{
        TJAMES_EQUAL(1, 1);
        TJAMES_EQUAL(3, 2);
        TJAMES_EQUAL(2, 2);
        TJAMES_NOT_EQUAL(1, 2);
        TJAMES_NOT_EQUAL(3, 3);
        TJAMES_NOT_EQUAL(2, 4);
        TJAMES_LESS(1, 2);
        TJAMES_GREATER(4, 4);
        TJAMES_LESS_EQUAL(2, 4);
        TJAMES_GREATER_EQUAL(1, 4);
}

int main()
{
        TJames_Init();

        TJAMES_ADD_FUNC(a);
        TJAMES_ADD_FUNC(b);
        TJAMES_ADD_FUNC(c);
        TJAMES_ADD_FUNC(d);
        TJAMES_ADD_FUNC(e);

        return TJames_Run();
}
