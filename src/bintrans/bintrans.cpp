#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
//===============================================

#include "bintrans.h"
#include "../general/general.h"
#include "../../lang/proc/assembler/processor_general.h"

//===============================================

int _read_binary_input(Binary_input* binary_input, const char* input_filename 
                                                      FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 

    assert(binary_input);
    
    if (input_filename == NULL)
    {
        error_report(INPUT_INV_NAME);
        return -1;
    }

    FILE* input_file_ptr = open_file(input_filename, "rb");
    if (input_file_ptr == NULL)
        return -1;

    binary_input->file_ptr = input_file_ptr;

    int ret_val = input_load_to_buffer(binary_input);
    if (ret_val == -1) return -1;

    ret_val = close_file(input_file_ptr);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _free_binary_input(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    free(binary_input->buffer);

    return 0;
}

//-----------------------------------------------

int _input_load_to_buffer(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    long size = get_file_size(binary_input->file_ptr);
    if (size == -1) return -1;

    binary_input->size = (unsigned int)size;

    int ret_val = fill_input_buffer(binary_input);
    if (ret_val == -1) return -1;

    return 0;
}

//-----------------------------------------------

int _fill_input_buffer(Binary_input* binary_input FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(binary_input);

    char* buffer = (char*) calloc(binary_input->size, sizeof(char));
    if (!buffer)
    {
		error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    unsigned int ret_val = (unsigned int) fread(buffer, sizeof(char), binary_input->size, 
                                                                 binary_input->file_ptr);
    if (ret_val != binary_input->size)
    {
        error_report(FREAD_ERR);
        return -1;
    }

    binary_input->buffer = buffer;
    return 0;
}

//-----------------------------------------------

int _binary_translate(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    TRANS_STRUCT_VALID(trans_struct);

    int is_ok = binary_header_check(trans_struct);
    if (is_ok == -1) return -1;

    trans_struct->buffer_pos += (unsigned int)sizeof(Header);

    return 0;
}

//-----------------------------------------------

int _binary_header_check(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();

    Header header = { 0 };
    
    memcpy(&header, trans_struct->input_buffer, sizeof(Header));

    int err_num = 0;

    if (header.signature != SIGN)
    {    
        error_report(HDR_INV_SIGN);
        err_num++;
    }

    if (header.version   != VERSION)
    {
        error_report(HDR_INV_VERSION);
        err_num++;
    }

    if (header.file_size != trans_struct->input_size)
    { 
        error_report(HDR_INV_FILE_SIZE);
        err_num++;
    }

    if (err_num) return -1;

    return 0;
}


//-----------------------------------------------

int _binary_execute(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int ret_val = flush_entities_buf(trans_struct);
    if (ret_val == -1) return -1;

    //call

    // int (*func) (void) = NULL;
    // func = (int (*)(void)) call_buf;

    // __asm__("int3");

    // int ret_val = func();
    
    free(trans_struct->call_buf);

    return 0;
}

//-----------------------------------------------

int _flush_entities_buf(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report();
    assert(trans_struct);

    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1)
    {
        error_report(SYSCONF_ERR);
        return -1;
    }

    //
    printf("\n size of page %d \n", pagesize);
    //

    trans_struct->call_buf = (unsigned char*) aligned_alloc (pagesize, 4 * sizeof(unsigned char));
    if (!trans_struct->call_buf) return -1;

    trans_struct->call_buf[0] = 0x48;
    trans_struct->call_buf[1] = 0x31;
    trans_struct->call_buf[2] = 0xC0;
    trans_struct->call_buf[3] = 0xC3;



    //
    //trans_struct->call_buf = {0x90, 0x90, 0x53, 0x55, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x90, 0x90, 0x41, 0x5F, 0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x5D, 0x5B, 0x90, 0x90, 0x48, 0x31, 0xC0, 0xC3};

    int (*func) (void) = NULL;
    func = (int (*)(void)) trans_struct->call_buf;

    #ifdef DEBUG_EXEC
    
        __asm__("int3");
    
    #endif 

    fprintf(stderr, "\n address of call buf: %p \n", trans_struct->call_buf);

    // int is_ok = mprotect((void*)trans_struct->call_buf, pagesize, PROT_EXEC);
    // if (is_ok != 0)
    // {
    //     error_report(MPROTECT_ERR);
    //     return -1;
    // }


    // int ret_val = func();
    // printf("\n Process ended with exit code: %d \n", ret_val);

    

    return 0;
}

//-----------------------------------------------

int _trans_struct_ctor(Trans_struct* trans_struct, Binary_input* binary_input 
                                                      FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 

    assert(trans_struct);
    assert(binary_input);

    trans_struct->input_buffer = binary_input->buffer;
    trans_struct->input_size   = binary_input->size; 
    trans_struct->buffer_pos   = 0;

    trans_struct->entities = (Trans_entity*) calloc(Entities_init_cap, sizeof(Trans_entity));
    if (!trans_struct->entities) 
    {
        error_report(CANNOT_ALLOCATE_MEM);
        return -1;
    }

    trans_struct->cap = Entities_init_cap;
    trans_struct->num = 0;

    #ifdef BINTRANS_LISTING

        int ret_val = init_listing_file(trans_struct);
        if (ret_val == -1) return -1;

    #endif 

    return 0;
}

//-----------------------------------------------

int _init_listing_file(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    FILE* listing = open_file(Listing_filename, "ab");
    if (!listing)
    {
        error_report(LISTING_CLOSE_ERR);
        return -1;
    }

    trans_struct->listing = listing;

    fprintf(listing, "\t Listing Start: ");
    fprintf(listing, "%s \n", get_time_string());

    return 0;
}

//-----------------------------------------------

int _end_listing_file(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    fprintf(trans_struct->listing, "\t Listing End: ");
    fprintf(trans_struct->listing, "%s \n", get_time_string());

    int ret_val = close_file(trans_struct->listing);
    if (ret_val == -1)
    {
        error_report(LISTING_CLOSE_ERR);
        return -1;
    }

    return 0;
}

//-----------------------------------------------

int _trans_struct_dtor(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    // if (trans_struct->buffer_pos != trans_struct->input_size)
    // {
    //     error_report(TRANS_STRUCT_DTOR_EARLY);
    //     return -1;
    // }

    free(trans_struct->entities);

    #ifdef BINTRANS_LISTING

        int ret_val = end_listing_file(trans_struct);
        if (ret_val == -1) return -1;

    #endif 

    return 0;
}

//-----------------------------------------------

int _trans_struct_validator(Trans_struct* trans_struct FOR_LOGS(, LOG_PARAMS))
{
    bintrans_log_report(); 
    assert(trans_struct);

    int err_num = 0;

    if (!trans_struct->entities)
    {
        error_report(TRANS_STRUCT_NULL_ENTITIES);
        err_num++;
    }

    if (trans_struct->cap == 0)
    {
        error_report(TRANS_STRUCT_ZERO_CAP);
        err_num++;
    }    

    if (trans_struct->cap < trans_struct->num)
    {
        error_report(TRANS_STRUCT_CAP_LESS_NUM);
        err_num++;
    }

    if (!trans_struct->input_buffer)
    {
        error_report(TRANS_STRUCT_NULL_INPUT_BUFFER);
        err_num++;
    }

    if (trans_struct->buffer_pos > trans_struct->input_size)
    {
        error_report(TRANS_STRUCT_INV_BUFFER_POS);
        err_num++;
    }

    #ifdef BINTRANS_LISTING

        if (trans_struct->listing == NULL)
        {
            error_report(LISTING_NULL_PTR);
            err_num++;
        }

    #endif 

    if (err_num != 0)
        return -1;

    return 0;
}

//===============================================
