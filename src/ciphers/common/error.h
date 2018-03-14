#ifndef COMMON_ERROR_H
#define COMMON_ERROR_H


typedef enum EngineErrorCode {
    NO_ERROR = 0,
    OUT_OF_MEM,
} EngineErrorCode;



extern int engine_errno;


#endif
