#include <stdlib.h>

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

extern int TJames_AddFunc(const TestFuncPtr func_ptr,
        const char *func_name,
        const char *group_name,
        const size_t added_on_line,
        const char* file);

extern void TJames_Init();
extern int  TJames_Run();
extern void TJames_Destroy();

extern void TJames_PushError(const char* message, const enum TJames_ErrorType type, const size_t line);
extern void TJames_SetTestFuncResult(const enum TJames_TestResult result);

#define TJAMES_ADD_GROUPED_FUNC(func_ptr, group) TJames_AddFunc(func_ptr, #func_ptr, group, __LINE__, __FILE__)
#define TJAMES_ADD_FUNC(func_ptr) TJAMES_ADD_GROUPED_FUNC(func_ptr, NULL)

#define TJAMES_SUCCESS()        do { \
                                        TJames_SetTestFuncResult(SUCCESSFUL_TEST); \
                                } while(0)

#define TJAMES_FAILURE(message) do { \
                                        TJames_SetTestFuncResult(FAILED_TEST); \
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
