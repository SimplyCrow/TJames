#include <stdlib.h>
#include <math.h> // needed in floating point comparision

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

void TJames_PushError(const enum TJames_ErrorType type, const size_t line, const char* message, ...);
extern void TJames_SetTestFuncResult(const enum TJames_TestResult result);

// *--------------------------*
// |                          |
// |   FUNCTION REGISTERING   |
// |                          |
// *--------------------------*

#define TJAMES_ADD_GROUPED_FUNC(func_ptr, group) TJames_AddFunc(func_ptr, #func_ptr, group, __LINE__, __FILE__)
#define TJAMES_ADD_FUNC(func_ptr) TJAMES_ADD_GROUPED_FUNC(func_ptr, NULL)

// *-----------------------*
// |                       |
// |   BASIS TEST MACROS   |
// |                       |
// *-----------------------*

#define TJAMES_FLOAT_DELTA      (1e-6)
#define TJAMES_DOUBLE_DELTA     (1e-12)
#define TJAMES_LONGDOUBLE_DELTA (1e-15)

#define TJAMES_SUCCESS()        do { \
                                        TJames_SetTestFuncResult(SUCCESSFUL_TEST); \
                                } while(0)

#define TJAMES_FAILURE(message) do { \
                                        TJames_SetTestFuncResult(FAILED_TEST); \
                                        TJames_PushError(NORMAL_ERROR, __LINE__, message); \
                                } while(0)

#define TJAMES_FAILURE_FMT(message, ...) do { \
                                        TJames_SetTestFuncResult(FAILED_TEST); \
                                        TJames_PushError(NORMAL_ERROR, __LINE__, message, __VA_ARGS__); \
                                } while(0)

#define TJAMES_FAILURE_EXIT(message)    do { \
                                                TJAMES_FAILURE(message); \
                                                return; \
                                        } while(0)

#define TJAMES_FAILURE_EXIT_FMT(message, ...) do { \
                                                TJAMES_FAILURE_FMT(message, __VA_ARGS__); \
                                                return; \
                                        } while(0)

#define TJAMES_WARNING(message) do { \
                                        TJames_PushError(WARNING_ERROR, __LINE__, message); \
                                } while(0)

#define TJAMES_WARNING_FMT(message, ...) do { \
                                        TJames_PushError(WARNING_ERROR, __LINE__, message, __VA_ARGS__); \
                                } while(0)

#define TJAMES_SKIP()   do { \
                                TJames_SetTestFuncResult(SKIPED_TEST); \
                                return; \
                        } while(0)

// *-------------------------*
// |                         |
// |   GENERIC TEST MACROS   |
// |                         |
// *-------------------------*

#define TJAMES_CMP_BASE(comparision, message)   do { \
                                                if (comparision) { \
                                                        TJAMES_SUCCESS(); \
                                                } else { \
                                                        TJAMES_FAILURE_EXIT(message); \
                                                } \
                                        } while(0)

#define TJAMES_CMP_BASE_FMT(comparision, message, ...)   do { \
                                                if (comparision) { \
                                                        TJAMES_SUCCESS(); \
                                                } else { \
                                                        TJAMES_FAILURE_EXIT_FMT(message, __VA_ARGS__); \
                                                } \
                                        } while(0)

#define TJAMES_CMP(a, op, b, message) TJAMES_CMP_BASE(a op b, message)
#define TJAMES_CMP_FMT(a, op, b, message, ...) TJAMES_CMP_BASE_FMT(a op b, message, __VA_ARGS__)

#define         TJAMES_EQUAL(a, b) TJAMES_CMP(a, ==, b, "Failed generic equal comparison!") 
#define     TJAMES_NOT_EQUAL(a, b) TJAMES_CMP(a, !=, b, "Failed generic not equal comparison!") 
#define          TJAMES_LESS(a, b) TJAMES_CMP(a, <,  b, "Failed generic less comparison!") 
#define       TJAMES_GREATER(a, b) TJAMES_CMP(a, >,  b, "Failed generic greater comparison!") 
#define    TJAMES_LESS_EQUAL(a, b) TJAMES_CMP(a, <=, b, "Failed generic less equal comparison!") 
#define TJAMES_GREATER_EQUAL(a, b) TJAMES_CMP(a, >=, b, "Failed generic greater equal comparison!") 

#define         TJAMES_EQUAL_FMT(a, b, a_fc, b_fc) TJAMES_CMP_FMT(a, ==, b, "Failed \"" #a_fc "\" == \"" #b_fc "\"!", a, b) 
#define     TJAMES_NOT_EQUAL_FMT(a, b, a_fc, b_fc) TJAMES_CMP_FMT(a, !=, b, "Failed \"" #a_fc "\" != \"" #b_fc "\"!", a, b) 
#define          TJAMES_LESS_FMT(a, b, a_fc, b_fc) TJAMES_CMP_FMT(a, <,  b, "Failed \"" #a_fc "\" <  \"" #b_fc "\"!", a, b) 
#define       TJAMES_GREATER_FMT(a, b, a_fc, b_fc) TJAMES_CMP_FMT(a, >,  b, "Failed \"" #a_fc "\" >  \"" #b_fc "\"!", a, b) 
#define    TJAMES_LESS_EQUAL_FMT(a, b, a_fc, b_fc) TJAMES_CMP_FMT(a, <=, b, "Failed \"" #a_fc "\" <= \"" #b_fc "\"!", a, b) 
#define TJAMES_GREATER_EQUAL_FMT(a, b, a_fc, b_fc) TJAMES_CMP_FMT(a, >=, b, "Failed \"" #a_fc "\" >= \"" #b_fc "\"!", a, b) 

#define         TJAMES_FLOATING_EQUAL_CONDITION(a, b, delta) (fabs((a) - (b)) <= (delta))
#define     TJAMES_FLOATING_NOT_EQUAL_CONDITION(a, b, delta) (fabs((a) - (b)) >  (delta))
#define          TJAMES_FLOATING_LESS_CONDITION(a, b, delta) ((a) <  (b) - (delta))
#define       TJAMES_FLOATING_GREATER_CONDITION(a, b, delta) ((a) >  (b) + (delta))
#define    TJAMES_FLOATING_LESS_EQUAL_CONDITION(a, b, delta) ((a) <= (b) + (delta))
#define TJAMES_FLOATING_GREATER_EQUAL_CONDITION(a, b, delta) ((a) >= (b) - (delta))

#define         TJAMES_FLOATING_EQUAL(a, b, delta) TJAMES_CMP_BASE(TJAMES_FLOATING_EQUAL_CONDITION(a, b, delta), \
                                                                        "Failed generic floating equal comparison!") 
#define     TJAMES_FLOATING_NOT_EQUAL(a, b, delta) TJAMES_CMP_BASE(TJAMES_FLOATING_NOT_EQUAL_CONDITION(a, b, delta), \
                                                                        "Failed generic floating not equal comparison!") 
#define          TJAMES_FLOATING_LESS(a, b, delta) TJAMES_CMP_BASE(TJAMES_FLOATING_LESS_CONDITION(a, b, delta), \
                                                                        "Failed generic floating less comparison!") 
#define       TJAMES_FLOATING_GREATER(a, b, delta) TJAMES_CMP_BASE(TJAMES_FLOATING_GREATER_CONDITION(a, b, delta), \
                                                                        "Failed generic floating greater comparison!") 
#define    TJAMES_FLOATING_LESS_EQUAL(a, b, delta) TJAMES_CMP_BASE(TJAMES_FLOATING_LESS_EQUAL_CONDITION(a, b, delta), \
                                                                        "Failed generic floating less equal comparison!") 
#define TJAMES_FLOATING_GREATER_EQUAL(a, b, delta) TJAMES_CMP_BASE(TJAMES_FLOATING_GREATER_EQUAL_CONDITION(a, b, delta), \
                                                                        "Failed generic floating greater equal comparison!") 

#define         TJAMES_FLOATING_EQUAL_FMT(a, b, delta, a_fc, b_fc) TJAMES_CMP_BASE_FMT(TJAMES_FLOATING_EQUAL_CONDITION(a, b, delta), \
                                                                        "Failed \"" #a_fc "\" == \"" #b_fc "\"!", a, b)
#define     TJAMES_FLOATING_NOT_EQUAL_FMT(a, b, delta, a_fc, b_fc) TJAMES_CMP_BASE_FMT(TJAMES_FLOATING_NOT_EQUAL_CONDITION(a, b, delta), \
                                                                        "Failed \"" #a_fc "\" != \"" #b_fc "\"!", a, b)
#define          TJAMES_FLOATING_LESS_FMT(a, b, delta, a_fc, b_fc) TJAMES_CMP_BASE_FMT(TJAMES_FLOATING_LESS_CONDITION(a, b, delta), \
                                                                        "Failed \"" #a_fc "\" <  \"" #b_fc "\"!", a, b)
#define       TJAMES_FLOATING_GREATER_FMT(a, b, delta, a_fc, b_fc) TJAMES_CMP_BASE_FMT(TJAMES_FLOATING_GREATER_CONDITION(a, b, delta), \
                                                                        "Failed \"" #a_fc "\" >  \"" #b_fc "\"!", a, b)
#define    TJAMES_FLOATING_LESS_EQUAL_FMT(a, b, delta, a_fc, b_fc) TJAMES_CMP_BASE_FMT(TJAMES_FLOATING_LESS_EQUAL_CONDITION(a, b, delta), \
                                                                        "Failed \"" #a_fc "\" <= \"" #b_fc "\"!", a, b)
#define TJAMES_FLOATING_GREATER_EQUAL_FMT(a, b, delta, a_fc, b_fc) TJAMES_CMP_BASE_FMT(TJAMES_FLOATING_GREATER_EQUAL_CONDITION(a, b, delta), \
                                                                        "Failed \"" #a_fc "\" >= \"" #b_fc "\"!", a, b)

// *-------------------------------*
// |                               |
// |   BUILT-IN TYPE TEST MACROS   |
// |                               |
// *-------------------------------*

#define         TJAMES_EQUAL_PTR(a, b)         TJAMES_EQUAL_FMT(a, b, %p, %p) 
#define     TJAMES_NOT_EQUAL_PTR(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %p, %p)
#define          TJAMES_LESS_PTR(a, b)          TJAMES_LESS_FMT(a, b, %p, %p)
#define       TJAMES_GREATER_PTR(a, b)       TJAMES_GREATER_FMT(a, b, %p, %p)
#define    TJAMES_LESS_EQUAL_PTR(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %p, %p)
#define TJAMES_GREATER_EQUAL_PTR(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %p, %p)

#define         TJAMES_EQUAL_CHAR(a, b)         TJAMES_EQUAL_FMT(a, b, %c, %c) 
#define     TJAMES_NOT_EQUAL_CHAR(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %c, %c)
#define          TJAMES_LESS_CHAR(a, b)          TJAMES_LESS_FMT(a, b, %c, %c)
#define       TJAMES_GREATER_CHAR(a, b)       TJAMES_GREATER_FMT(a, b, %c, %c)
#define    TJAMES_LESS_EQUAL_CHAR(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %c, %c)
#define TJAMES_GREATER_EQUAL_CHAR(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %c, %c)

#define         TJAMES_EQUAL_STRING(a, b)         TJAMES_EQUAL_FMT(a, b, %s, %s) 
#define     TJAMES_NOT_EQUAL_STRING(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %s, %s)
#define          TJAMES_LESS_STRING(a, b)          TJAMES_LESS_FMT(a, b, %s, %s)
#define       TJAMES_GREATER_STRING(a, b)       TJAMES_GREATER_FMT(a, b, %s, %s)
#define    TJAMES_LESS_EQUAL_STRING(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %s, %s)
#define TJAMES_GREATER_EQUAL_STRING(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %s, %s)

#define         TJAMES_EQUAL_SHORT(a, b)         TJAMES_EQUAL_FMT(a, b, %hd, %hd) 
#define     TJAMES_NOT_EQUAL_SHORT(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %hd, %hd)
#define          TJAMES_LESS_SHORT(a, b)          TJAMES_LESS_FMT(a, b, %hd, %hd)
#define       TJAMES_GREATER_SHORT(a, b)       TJAMES_GREATER_FMT(a, b, %hd, %hd)
#define    TJAMES_LESS_EQUAL_SHORT(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %hd, %hd)
#define TJAMES_GREATER_EQUAL_SHORT(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %hd, %hd)

#define         TJAMES_EQUAL_USHORT(a, b)         TJAMES_EQUAL_FMT(a, b, %hu, %hu) 
#define     TJAMES_NOT_EQUAL_USHORT(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %hu, %hu)
#define          TJAMES_LESS_USHORT(a, b)          TJAMES_LESS_FMT(a, b, %hu, %hu)
#define       TJAMES_GREATER_USHORT(a, b)       TJAMES_GREATER_FMT(a, b, %hu, %hu)
#define    TJAMES_LESS_EQUAL_USHORT(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %hu, %hu)
#define TJAMES_GREATER_EQUAL_USHORT(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %hu, %hu)

#define         TJAMES_EQUAL_INT(a, b)         TJAMES_EQUAL_FMT(a, b, %d, %d) 
#define     TJAMES_NOT_EQUAL_INT(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %d, %d)
#define          TJAMES_LESS_INT(a, b)          TJAMES_LESS_FMT(a, b, %d, %d)
#define       TJAMES_GREATER_INT(a, b)       TJAMES_GREATER_FMT(a, b, %d, %d)
#define    TJAMES_LESS_EQUAL_INT(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %d, %d)
#define TJAMES_GREATER_EQUAL_INT(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %d, %d)

#define         TJAMES_EQUAL_UINT(a, b)         TJAMES_EQUAL_FMT(a, b, %u, %u) 
#define     TJAMES_NOT_EQUAL_UINT(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %u, %u)
#define          TJAMES_LESS_UINT(a, b)          TJAMES_LESS_FMT(a, b, %u, %u)
#define       TJAMES_GREATER_UINT(a, b)       TJAMES_GREATER_FMT(a, b, %u, %u)
#define    TJAMES_LESS_EQUAL_UINT(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %u, %u)
#define TJAMES_GREATER_EQUAL_UINT(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %u, %u)

#define         TJAMES_EQUAL_LONG(a, b)         TJAMES_EQUAL_FMT(a, b, %ld, %ld) 
#define     TJAMES_NOT_EQUAL_LONG(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %ld, %ld)
#define          TJAMES_LESS_LONG(a, b)          TJAMES_LESS_FMT(a, b, %ld, %ld)
#define       TJAMES_GREATER_LONG(a, b)       TJAMES_GREATER_FMT(a, b, %ld, %ld)
#define    TJAMES_LESS_EQUAL_LONG(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %ld, %ld)
#define TJAMES_GREATER_EQUAL_LONG(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %ld, %ld)

#define         TJAMES_EQUAL_ULONG(a, b)         TJAMES_EQUAL_FMT(a, b, %lu, %lu) 
#define     TJAMES_NOT_EQUAL_ULONG(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %lu, %lu)
#define          TJAMES_LESS_ULONG(a, b)          TJAMES_LESS_FMT(a, b, %lu, %lu)
#define       TJAMES_GREATER_ULONG(a, b)       TJAMES_GREATER_FMT(a, b, %lu, %lu)
#define    TJAMES_LESS_EQUAL_ULONG(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %lu, %lu)
#define TJAMES_GREATER_EQUAL_ULONG(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %lu, %lu)

#define         TJAMES_EQUAL_LONGLONG(a, b)         TJAMES_EQUAL_FMT(a, b, %lld, %lld) 
#define     TJAMES_NOT_EQUAL_LONGLONG(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %lld, %lld)
#define          TJAMES_LESS_LONGLONG(a, b)          TJAMES_LESS_FMT(a, b, %lld, %lld)
#define       TJAMES_GREATER_LONGLONG(a, b)       TJAMES_GREATER_FMT(a, b, %lld, %lld)
#define    TJAMES_LESS_EQUAL_LONGLONG(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %lld, %lld)
#define TJAMES_GREATER_EQUAL_LONGLONG(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %lld, %lld)

#define         TJAMES_EQUAL_ULONGLONG(a, b)         TJAMES_EQUAL_FMT(a, b, %llu, %llu) 
#define     TJAMES_NOT_EQUAL_ULONGLONG(a, b)     TJAMES_NOT_EQUAL_FMT(a, b, %llu, %llu)
#define          TJAMES_LESS_ULONGLONG(a, b)          TJAMES_LESS_FMT(a, b, %llu, %llu)
#define       TJAMES_GREATER_ULONGLONG(a, b)       TJAMES_GREATER_FMT(a, b, %llu, %llu)
#define    TJAMES_LESS_EQUAL_ULONGLONG(a, b)    TJAMES_LESS_EQUAL_FMT(a, b, %llu, %llu)
#define TJAMES_GREATER_EQUAL_ULONGLONG(a, b) TJAMES_GREATER_EQUAL_FMT(a, b, %llu, %llu)


#define         TJAMES_EQUAL_FLOAT(a, b)         TJAMES_FLOATING_EQUAL_FMT(a, b, TJAMES_FLOAT_DELTA, %f, %f) 
#define     TJAMES_NOT_EQUAL_FLOAT(a, b)     TJAMES_FLOATING_NOT_EQUAL_FMT(a, b, TJAMES_FLOAT_DELTA, %f, %f)
#define          TJAMES_LESS_FLOAT(a, b)          TJAMES_FLOATING_LESS_FMT(a, b, TJAMES_FLOAT_DELTA, %f, %f)
#define       TJAMES_GREATER_FLOAT(a, b)       TJAMES_FLOATING_GREATER_FMT(a, b, TJAMES_FLOAT_DELTA, %f, %f)
#define    TJAMES_LESS_EQUAL_FLOAT(a, b)    TJAMES_FLOATING_LESS_EQUAL_FMT(a, b, TJAMES_FLOAT_DELTA, %f, %f)
#define TJAMES_GREATER_EQUAL_FLOAT(a, b) TJAMES_FLOATING_GREATER_EQUAL_FMT(a, b, TJAMES_FLOAT_DELTA, %f, %f)

#define         TJAMES_EQUAL_DOUBLE(a, b)         TJAMES_FLOATING_EQUAL_FMT(a, b, TJAMES_DOUBLE_DELTA, %lf, %lf) 
#define     TJAMES_NOT_EQUAL_DOUBLE(a, b)     TJAMES_FLOATING_NOT_EQUAL_FMT(a, b, TJAMES_DOUBLE_DELTA, %lf, %lf)
#define          TJAMES_LESS_DOUBLE(a, b)          TJAMES_FLOATING_LESS_FMT(a, b, TJAMES_DOUBLE_DELTA, %lf, %lf)
#define       TJAMES_GREATER_DOUBLE(a, b)       TJAMES_FLOATING_GREATER_FMT(a, b, TJAMES_DOUBLE_DELTA, %lf, %lf)
#define    TJAMES_LESS_EQUAL_DOUBLE(a, b)    TJAMES_FLOATING_LESS_EQUAL_FMT(a, b, TJAMES_DOUBLE_DELTA, %lf, %lf)
#define TJAMES_GREATER_EQUAL_DOUBLE(a, b) TJAMES_FLOATING_GREATER_EQUAL_FMT(a, b, TJAMES_DOUBLE_DELTA, %lf, %lf)

#define         TJAMES_EQUAL_LONGDOUBLE(a, b)         TJAMES_FLOATING_EQUAL_FMT(a, b, TJAMES_LONGDOUBLE_DELTA, %Lf, %Lf) 
#define     TJAMES_NOT_EQUAL_LONGDOUBLE(a, b)     TJAMES_FLOATING_NOT_EQUAL_FMT(a, b, TJAMES_LONGDOUBLE_DELTA, %Lf, %Lf)
#define          TJAMES_LESS_LONGDOUBLE(a, b)          TJAMES_FLOATING_LESS_FMT(a, b, TJAMES_LONGDOUBLE_DELTA, %Lf, %Lf)
#define       TJAMES_GREATER_LONGDOUBLE(a, b)       TJAMES_FLOATING_GREATER_FMT(a, b, TJAMES_LONGDOUBLE_DELTA, %Lf, %Lf)
#define    TJAMES_LESS_EQUAL_LONGDOUBLE(a, b)    TJAMES_FLOATING_LESS_EQUAL_FMT(a, b, TJAMES_LONGDOUBLE_DELTA, %Lf, %Lf)
#define TJAMES_GREATER_EQUAL_LONGDOUBLE(a, b) TJAMES_FLOATING_GREATER_EQUAL_FMT(a, b, TJAMES_LONGDOUBLE_DELTA, %Lf, %Lf)
