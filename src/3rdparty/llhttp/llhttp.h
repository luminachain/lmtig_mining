#ifndef INCLUDE_LLHTTP_H_
#define INCLUDE_LLHTTP_H_

#define LLHTTP_VERSION_MAJOR 5
#define LLHTTP_VERSION_MINOR 1
#define LLHTTP_VERSION_PATCH 0

#ifndef LLHTTP_STRICT_MODE
# define LLHTTP_STRICT_MODE 0
#endif

#ifndef INCLUDE_LLHTTP_ITSELF_H_
#define INCLUDE_LLHTTP_ITSELF_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct llhttp__internal_s llhttp__internal_t;
struct llhttp__internal_s {
  int32_t _index;
  void* _span_pos0;
  void* _span_cb0;
  int32_t error;
  const char* reason;
  const char* error_pos;
  void* data;
  void* _current;
  uint64_t content_length;
  uint8_t type;
  uint8_t method;
  uint8_t http_major;
  uint8_t http_minor;
  uint8_t header_state;
  uint8_t lenient_flags;
  uint8_t upgrade;
  uint8_t finish;
  uint16_t flags;
  uint16_t status_code;
  void* settings;
};

int llhttp__internal_init(llhttp__internal_t* s);
int llhttp__internal_execute(llhttp__internal_t* s, const char* p, const char* endp);

#ifdef __cplusplus
}  
#endif
#endif  

#ifndef LLLLHTTP_C_HEADERS_
#define LLLLHTTP_C_HEADERS_
#ifdef __cplusplus
extern "C" {
#endif

enum llhttp_errno {
  HPE_OK = 0,
  HPE_INTERNAL = 1,
  HPE_STRICT = 2,
  HPE_LF_EXPECTED = 3,
  HPE_UNEXPECTED_CONTENT_LENGTH = 4,
  HPE_CLOSED_CONNECTION = 5,
  HPE_INVALID_METHOD = 6,
  HPE_INVALID_URL = 7,
  HPE_INVALID_CONSTANT = 8,
  HPE_INVALID_VERSION = 9,
  HPE_INVALID_HEADER_TOKEN = 10,
  HPE_INVALID_CONTENT_LENGTH = 11,
  HPE_INVALID_CHUNK_SIZE = 12,
  HPE_INVALID_STATUS = 13,
  HPE_INVALID_EOF_STATE = 14,
  HPE_INVALID_TRANSFER_ENCODING = 15,
  HPE_CB_MESSAGE_BEGIN = 16,
  HPE_CB_HEADERS_COMPLETE = 17,
  HPE_CB_MESSAGE_COMPLETE = 18,
  HPE_CB_CHUNK_HEADER = 19,
  HPE_CB_CHUNK_COMPLETE = 20,
  HPE_PAUSED = 21,
  HPE_PAUSED_UPGRADE = 22,
  HPE_PAUSED_H2_UPGRADE = 23,
  HPE_USER = 24
};
typedef enum llhttp_errno llhttp_errno_t;

enum llhttp_flags {
  F_CONNECTION_KEEP_ALIVE = 0x1,
  F_CONNECTION_CLOSE = 0x2,
  F_CONNECTION_UPGRADE = 0x4,
  F_CHUNKED = 0x8,
  F_UPGRADE = 0x10,
  F_CONTENT_LENGTH = 0x20,
  F_SKIPBODY = 0x40,
  F_TRAILING = 0x80,
  F_TRANSFER_ENCODING = 0x200
};
typedef enum llhttp_flags llhttp_flags_t;

enum llhttp_lenient_flags {
  LENIENT_HEADERS = 0x1,
  LENIENT_CHUNKED_LENGTH = 0x2,
  LENIENT_KEEP_ALIVE = 0x4
};
typedef enum llhttp_lenient_flags llhttp_lenient_flags_t;

enum llhttp_type {
  HTTP_BOTH = 0,
  HTTP_REQUEST = 1,
  HTTP_RESPONSE = 2
};
typedef enum llhttp_type llhttp_type_t;

enum llhttp_finish {
  HTTP_FINISH_SAFE = 0,
  HTTP_FINISH_SAFE_WITH_CB = 1,
  HTTP_FINISH_UNSAFE = 2
};
typedef enum llhttp_finish llhttp_finish_t;

enum llhttp_method {
  HTTP_DELETE = 0,
  HTTP_GET = 1,
  HTTP_HEAD = 2,
  HTTP_POST = 3,
  HTTP_PUT = 4,
  HTTP_CONNECT = 5,
  HTTP_OPTIONS = 6,
  HTTP_TRACE = 7,
  HTTP_COPY = 8,
  HTTP_LOCK = 9,
  HTTP_MKCOL = 10,
  HTTP_MOVE = 11,
  HTTP_PROPFIND = 12,
  HTTP_PROPPATCH = 13,
  HTTP_SEARCH = 14,
  HTTP_UNLOCK = 15,
  HTTP_BIND = 16,
  HTTP_REBIND = 17,
  HTTP_UNBIND = 18,
  HTTP_ACL = 19,
  HTTP_REPORT = 20,
  HTTP_MKACTIVITY = 21,
  HTTP_CHECKOUT = 22,
  HTTP_MERGE = 23,
  HTTP_MSEARCH = 24,
  HTTP_NOTIFY = 25,
  HTTP_SUBSCRIBE = 26,
  HTTP_UNSUBSCRIBE = 27,
  HTTP_PATCH = 28,
  HTTP_PURGE = 29,
  HTTP_MKCALENDAR = 30,
  HTTP_LINK = 31,
  HTTP_UNLINK = 32,
  HTTP_SOURCE = 33,
  HTTP_PRI = 34,
  HTTP_DESCRIBE = 35,
  HTTP_ANNOUNCE = 36,
  HTTP_SETUP = 37,
  HTTP_PLAY = 38,
  HTTP_PAUSE = 39,
  HTTP_TEARDOWN = 40,
  HTTP_GET_PARAMETER = 41,
  HTTP_SET_PARAMETER = 42,
  HTTP_REDIRECT = 43,
  HTTP_RECORD = 44,
  HTTP_FLUSH = 45
};
typedef enum llhttp_method llhttp_method_t;

#define HTTP_ERRNO_MAP(XX) \
  XX(0, OK, OK) \
  XX(1, INTERNAL, INTERNAL) \
  XX(2, STRICT, STRICT) \
  XX(3, LF_EXPECTED, LF_EXPECTED) \
  XX(4, UNEXPECTED_CONTENT_LENGTH, UNEXPECTED_CONTENT_LENGTH) \
  XX(5, CLOSED_CONNECTION, CLOSED_CONNECTION) \
  XX(6, INVALID_METHOD, INVALID_METHOD) \
  XX(7, INVALID_URL, INVALID_URL) \
  XX(8, INVALID_CONSTANT, INVALID_CONSTANT) \
  XX(9, INVALID_VERSION, INVALID_VERSION) \
  XX(10, INVALID_HEADER_TOKEN, INVALID_HEADER_TOKEN) \
  XX(11, INVALID_CONTENT_LENGTH, INVALID_CONTENT_LENGTH) \
  XX(12, INVALID_CHUNK_SIZE, INVALID_CHUNK_SIZE) \
  XX(13, INVALID_STATUS, INVALID_STATUS) \
  XX(14, INVALID_EOF_STATE, INVALID_EOF_STATE) \
  XX(15, INVALID_TRANSFER_ENCODING, INVALID_TRANSFER_ENCODING) \
  XX(16, CB_MESSAGE_BEGIN, CB_MESSAGE_BEGIN) \
  XX(17, CB_HEADERS_COMPLETE, CB_HEADERS_COMPLETE) \
  XX(18, CB_MESSAGE_COMPLETE, CB_MESSAGE_COMPLETE) \
  XX(19, CB_CHUNK_HEADER, CB_CHUNK_HEADER) \
  XX(20, CB_CHUNK_COMPLETE, CB_CHUNK_COMPLETE) \
  XX(21, PAUSED, PAUSED) \
  XX(22, PAUSED_UPGRADE, PAUSED_UPGRADE) \
  XX(23, PAUSED_H2_UPGRADE, PAUSED_H2_UPGRADE) \
  XX(24, USER, USER) \


#define HTTP_METHOD_MAP(XX) \
  XX(0, DELETE, DELETE) \
  XX(1, GET, GET) \
  XX(2, HEAD, HEAD) \
  XX(3, POST, POST) \
  XX(4, PUT, PUT) \
  XX(5, CONNECT, CONNECT) \
  XX(6, OPTIONS, OPTIONS) \
  XX(7, TRACE, TRACE) \
  XX(8, COPY, COPY) \
  XX(9, LOCK, LOCK) \
  XX(10, MKCOL, MKCOL) \
  XX(11, MOVE, MOVE) \
  XX(12, PROPFIND, PROPFIND) \
  XX(13, PROPPATCH, PROPPATCH) \
  XX(14, SEARCH, SEARCH) \
  XX(15, UNLOCK, UNLOCK) \
  XX(16, BIND, BIND) \
  XX(17, REBIND, REBIND) \
  XX(18, UNBIND, UNBIND) \
  XX(19, ACL, ACL) \
  XX(20, REPORT, REPORT) \
  XX(21, MKACTIVITY, MKACTIVITY) \
  XX(22, CHECKOUT, CHECKOUT) \
  XX(23, MERGE, MERGE) \
  XX(24, MSEARCH, M-SEARCH) \
  XX(25, NOTIFY, NOTIFY) \
  XX(26, SUBSCRIBE, SUBSCRIBE) \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE) \
  XX(28, PATCH, PATCH) \
  XX(29, PURGE, PURGE) \
  XX(30, MKCALENDAR, MKCALENDAR) \
  XX(31, LINK, LINK) \
  XX(32, UNLINK, UNLINK) \
  XX(33, SOURCE, SOURCE) \
  XX(34, PRI, PRI) \
  XX(35, DESCRIBE, DESCRIBE) \
  XX(36, ANNOUNCE, ANNOUNCE) \
  XX(37, SETUP, SETUP) \
  XX(38, PLAY, PLAY) \
  XX(39, PAUSE, PAUSE) \
  XX(40, TEARDOWN, TEARDOWN) \
  XX(41, GET_PARAMETER, GET_PARAMETER) \
  XX(42, SET_PARAMETER, SET_PARAMETER) \
  XX(43, REDIRECT, REDIRECT) \
  XX(44, RECORD, RECORD) \
  XX(45, FLUSH, FLUSH) \



#ifdef __cplusplus
}  
#endif
#endif  

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

#endif  
