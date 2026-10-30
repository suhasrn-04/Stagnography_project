#ifndef COMMON_H
#define COMMON_H

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

/* Status will be used in fn. return type */
typedef enum
{
    failure, //0
    success
} Status;

typedef enum
{
    unsupported, //0
    encode,  //1
    decode  //2
} Opr_type;

#endif
