#ifndef INCLUDE_LLHTTP_API_H_
#define INCLUDE_LLHTTP_API_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

#if defined(__wasm__)
#define LLHTTP_EXPORT __attribute__((visibility("default")))
#else
#define LLHTTP_EXPORT
#endif

typedef llhttp__internal_t llhttp_t;
typedef struct llhttp_settings_s llhttp_settings_t;

typedef int (*llhttp_data_cb)(llhttp_t*, const char *at, size_t length);
typedef int (*llhttp_cb)(llhttp_t*);

struct llhttp_settings_s {
  
  llhttp_cb      on_message_begin;

  llhttp_data_cb on_url;
  llhttp_data_cb on_status;
  llhttp_data_cb on_header_field;
  llhttp_data_cb on_header_value;

  
  llhttp_cb      on_headers_complete;

  llhttp_data_cb on_body;

  
  llhttp_cb      on_message_complete;

  
  llhttp_cb      on_chunk_header;
  llhttp_cb      on_chunk_complete;

  llhttp_cb      on_url_complete;
  llhttp_cb      on_status_complete;
  llhttp_cb      on_header_field_complete;
  llhttp_cb      on_header_value_complete;
};


LLHTTP_EXPORT
void llhttp_init(llhttp_t* parser, llhttp_type_t type,
                 const llhttp_settings_t* settings);

#if defined(__wasm__)

LLHTTP_EXPORT
llhttp_t* llhttp_alloc(llhttp_type_t type);

LLHTTP_EXPORT
void llhttp_free(llhttp_t* parser);

LLHTTP_EXPORT
uint8_t llhttp_get_type(llhttp_t* parser);

LLHTTP_EXPORT
uint8_t llhttp_get_http_major(llhttp_t* parser);

LLHTTP_EXPORT
uint8_t llhttp_get_http_minor(llhttp_t* parser);

LLHTTP_EXPORT
uint8_t llhttp_get_method(llhttp_t* parser);

LLHTTP_EXPORT
int llhttp_get_status_code(llhttp_t* parser);

LLHTTP_EXPORT
uint8_t llhttp_get_upgrade(llhttp_t* parser);

#endif  


LLHTTP_EXPORT
void llhttp_reset(llhttp_t* parser);


LLHTTP_EXPORT
void llhttp_settings_init(llhttp_settings_t* settings);


LLHTTP_EXPORT
llhttp_errno_t llhttp_execute(llhttp_t* parser, const char* data, size_t len);


LLHTTP_EXPORT
llhttp_errno_t llhttp_finish(llhttp_t* parser);


LLHTTP_EXPORT
int llhttp_message_needs_eof(const llhttp_t* parser);


LLHTTP_EXPORT
int llhttp_should_keep_alive(const llhttp_t* parser);


LLHTTP_EXPORT
void llhttp_pause(llhttp_t* parser);


LLHTTP_EXPORT
void llhttp_resume(llhttp_t* parser);


LLHTTP_EXPORT
void llhttp_resume_after_upgrade(llhttp_t* parser);


LLHTTP_EXPORT
llhttp_errno_t llhttp_get_errno(const llhttp_t* parser);


LLHTTP_EXPORT
const char* llhttp_get_error_reason(const llhttp_t* parser);


LLHTTP_EXPORT
void llhttp_set_error_reason(llhttp_t* parser, const char* reason);


LLHTTP_EXPORT
const char* llhttp_get_error_pos(const llhttp_t* parser);


LLHTTP_EXPORT
const char* llhttp_errno_name(llhttp_errno_t err);


LLHTTP_EXPORT
const char* llhttp_method_name(llhttp_method_t method);



LLHTTP_EXPORT
void llhttp_set_lenient_headers(llhttp_t* parser, int enabled);



LLHTTP_EXPORT
void llhttp_set_lenient_chunked_length(llhttp_t* parser, int enabled);



void llhttp_set_lenient_keep_alive(llhttp_t* parser, int enabled);

#ifdef __cplusplus
}  
#endif
#endif  
