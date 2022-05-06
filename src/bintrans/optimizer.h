#pragma once 

#include "optimizer_conf.h"
#include "bintrans.h"
#include "../list/list.h"

//===============================================

struct Instr_data
{
    union
    {
        float float_value;
        int   int_value;
    };

    unsigned char  unsigned_char_value;
};

//-----------------------------------------------

struct Instr
{
    unsigned char oper_code;
    unsigned int  size;

    struct Instr_data data;

    #ifdef ADD_INSTR_NAME

        char* name_str;

    #endif 
};

//===============================================

int _binary_optimize(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS));

int _read_instructions(Input* input, List* list FOR_LOGS(, LOG_PARAMS));

int _optimize_instructions(List* list FOR_LOGS(, LOG_PARAMS));

int _count_instructions_size(List* list FOR_LOGS(, LOG_PARAMS));

int _flush_instructions_to_buf(Trans_struct* trans_struct, List* list 
                                              FOR_LOGS(, LOG_PARAMS));

int _list_free_instr(List* list FOR_LOGS(, LOG_PARAMS));

Instr* _init_instr(unsigned char oper_code, Input* input 
                                            FOR_LOGS(, LOG_PARAMS));

#ifdef ADD_INSTR_NAME

    int _add_instruction_name(Instr* instr FOR_LOGS(, LOG_PARAMS));

#endif 

//-----------------------------------------------

int _load_float_value        (Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS));

int _load_unsigned_char_value(Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS));

int _load_int_value          (Input* input, Instr* instr FOR_LOGS(, LOG_PARAMS));

int _load_instr_data         (unsigned char oper_code, Input* input, 
                                            Instr* instr FOR_LOGS(, LOG_PARAMS));

//-----------------------------------------------

int _store_int_value          (Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS));

int _store_float_value        (Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS));

int _store_unsigned_char_value(Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS));

int _store_instr_data         (Instr* instr, unsigned char* new_buf,
                                             unsigned int*  new_buf_pos FOR_LOGS(, LOG_PARAMS));

//===============================================

#define store_int_value(instr, buf, buf_pos) \
       _store_int_value(instr, buf, buf_pos FOR_LOGS(, LOG_ARGS))

#define store_float_value(instr, buf, buf_pos) \
       _store_float_value(instr, buf, buf_pos FOR_LOGS(, LOG_ARGS))

#define store_unsigned_char_value(instr, buf, buf_pos) \
       _store_unsigned_char_value(instr, buf, buf_pos FOR_LOGS(, LOG_ARGS))

#define store_instr_data(instr, buf, buf_pos) \
       _store_instr_data(instr, buf, buf_pos FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

#define load_float_value(input, instr) \
       _load_float_value(input, instr FOR_LOGS(, LOG_ARGS))

#define load_int_value(input, instr) \
       _load_int_value(input, instr FOR_LOGS(, LOG_ARGS))

#define load_unsigned_char_value(input, instr) \
       _load_unsigned_char_value(input, instr FOR_LOGS(, LOG_ARGS))

#define load_instr_data(oper_code, input, instr) \
       _load_instr_data(oper_code, input, instr FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

#define init_instr(oper_code, input) \
       _init_instr(oper_code, input FOR_LOGS(, LOG_ARGS))

#define count_instructions_size(list) \
       _count_instructions_size(list FOR_LOGS(, LOG_ARGS))

#define list_free_instr(list) \
       _list_free_instr(list FOR_LOGS(, LOG_ARGS))

#define read_instructions(input, list) \
       _read_instructions(input, list FOR_LOGS(, LOG_ARGS))

#define optimize_instructions(list) \
       _optimize_instructions(list FOR_LOGS(, LOG_ARGS))

#define flush_instructions_to_buf(trans_struct, list) \
       _flush_instructions_to_buf(trans_struct, list FOR_LOGS(, LOG_ARGS))

#define binary_optimize(trans_struct) \
       _binary_optimize(trans_struct FOR_LOGS(, LOG_ARGS))

//-----------------------------------------------

#ifdef ADD_INSTR_NAME

    #define add_instruction_name(instr) \
           _add_instruction_name(instr FOR_LOGS(, LOG_ARGS))

#endif 

       