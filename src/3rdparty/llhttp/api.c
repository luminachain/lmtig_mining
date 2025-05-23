#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "llhttp.h"

#define CALLBACK_MAYBE(PARSER, NAME, ...)                                     \
  do {                                                                        \
    const llhttp_settings_t* settings;                                        \
    settings = (const llhttp_settings_t*) (PARSER)->settings;                 \
    if (settings == NULL || settings->NAME == NULL) {                         \
      err = 0;                                                                \
      break;                                                                  \
    }                                                                         \
    err = settings->NAME(__VA_ARGS__);                                        \
  } while (0)

void llhttp_init(llhttp_t* parser, llhttp_type_t type,
                 const llhttp_settings_t* settings) {
  llhttp__internal_init(parser);

  parser->type = type;
  parser->settings = (void*) settings;
}


#if defined(__wasm__)

extern int wasm_on_message_begin(llhttp_t * p);
extern int wasm_on_url(llhttp_t* p, const char* at, size_t length);
extern int wasm_on_status(llhttp_t* p, const char* at, size_t length);
extern int wasm_on_header_field(llhttp_t* p, const char* at, size_t length);
extern int wasm_on_header_value(llhttp_t* p, const char* at, size_t length);
extern int wasm_on_headers_complete(llhttp_t * p);
extern int wasm_on_body(llhttp_t* p, const char* at, size_t length);
extern int wasm_on_message_complete(llhttp_t * p);

const llhttp_settings_t wasm_settings = {
  wasm_on_message_begin,
  wasm_on_url,
  wasm_on_status,
  wasm_on_header_field,
  wasm_on_header_value,
  wasm_on_headers_complete,
  wasm_on_body,
  wasm_on_message_complete,
  NULL,
  NULL,
};


llhttp_t* llhttp_alloc(llhttp_type_t type) {
  llhttp_t* parser = malloc(sizeof(llhttp_t));
  llhttp_init(parser, type, &wasm_settings);
  return parser;
}

void llhttp_free(llhttp_t* parser) {
  free(parser);
}



uint8_t llhttp_get_type(llhttp_t* parser) {
  return parser->type;
}

uint8_t llhttp_get_http_major(llhttp_t* parser) {
  return parser->http_major;
}

uint8_t llhttp_get_http_minor(llhttp_t* parser) {
  return parser->http_minor;
}

uint8_t llhttp_get_method(llhttp_t* parser) {
  return parser->method;
}

int llhttp_get_status_code(llhttp_t* parser) {
  return parser->status_code;
}

uint8_t llhttp_get_upgrade(llhttp_t* parser) {
  return parser->upgrade;
}

#endif  


void llhttp_reset(llhttp_t* parser) {
  llhttp_type_t type = parser->type;
  const llhttp_settings_t* settings = parser->settings;
  void* data = parser->data;
  uint8_t lenient_flags = parser->lenient_flags;

  llhttp__internal_init(parser);

  parser->type = type;
  parser->settings = (void*) settings;
  parser->data = data;
  parser->lenient_flags = lenient_flags;
}


llhttp_errno_t llhttp_execute(llhttp_t* parser, const char* data, size_t len) {
  return llhttp__internal_execute(parser, data, data + len);
}


void llhttp_settings_init(llhttp_settings_t* settings) {
  memset(settings, 0, sizeof(*settings));
}


llhttp_errno_t llhttp_finish(llhttp_t* parser) {
  int err;

  
  if (parser->error != 0) {
    return 0;
  }

  switch (parser->finish) {
    case HTTP_FINISH_SAFE_WITH_CB:
      CALLBACK_MAYBE(parser, on_message_complete, parser);
      if (err != HPE_OK) return err;

    
    case HTTP_FINISH_SAFE:
      return HPE_OK;
    case HTTP_FINISH_UNSAFE:
      parser->reason = "Invalid EOF state";
      return HPE_INVALID_EOF_STATE;
    default:
      abort();
  }
}


void llhttp_pause(llhttp_t* parser) {
  if (parser->error != HPE_OK) {
    return;
  }

  parser->error = HPE_PAUSED;
  parser->reason = "Paused";
}


void llhttp_resume(llhttp_t* parser) {
  if (parser->error != HPE_PAUSED) {
    return;
  }

  parser->error = 0;
}


void llhttp_resume_after_upgrade(llhttp_t* parser) {
  if (parser->error != HPE_PAUSED_UPGRADE) {
    return;
  }

  parser->error = 0;
}


llhttp_errno_t llhttp_get_errno(const llhttp_t* parser) {
  return parser->error;
}


const char* llhttp_get_error_reason(const llhttp_t* parser) {
  return parser->reason;
}


void llhttp_set_error_reason(llhttp_t* parser, const char* reason) {
  parser->reason = reason;
}


const char* llhttp_get_error_pos(const llhttp_t* parser) {
  return parser->error_pos;
}


const char* llhttp_errno_name(llhttp_errno_t err) {
#define HTTP_ERRNO_GEN(CODE, NAME, _) case HPE_##NAME: return "HPE_" #NAME;
  switch (err) {
    HTTP_ERRNO_MAP(HTTP_ERRNO_GEN)
    default: abort();
  }
#undef HTTP_ERRNO_GEN
}


const char* llhttp_method_name(llhttp_method_t method) {
#define HTTP_METHOD_GEN(NUM, NAME, STRING) case HTTP_##NAME: return #STRING;
  switch (method) {
    HTTP_METHOD_MAP(HTTP_METHOD_GEN)
    default: abort();
  }
#undef HTTP_METHOD_GEN
}


void llhttp_set_lenient_headers(llhttp_t* parser, int enabled) {
  if (enabled) {
    parser->lenient_flags |= LENIENT_HEADERS;
  } else {
    parser->lenient_flags &= ~LENIENT_HEADERS;
  }
}


void llhttp_set_lenient_chunked_length(llhttp_t* parser, int enabled) {
  if (enabled) {
    parser->lenient_flags |= LENIENT_CHUNKED_LENGTH;
  } else {
    parser->lenient_flags &= ~LENIENT_CHUNKED_LENGTH;
  }
}


void llhttp_set_lenient_keep_alive(llhttp_t* parser, int enabled) {
  if (enabled) {
    parser->lenient_flags |= LENIENT_KEEP_ALIVE;
  } else {
    parser->lenient_flags &= ~LENIENT_KEEP_ALIVE;
  }
}




int llhttp__on_message_begin(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_message_begin, s);
  return err;
}


int llhttp__on_url(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_url, s, p, endp - p);
  return err;
}


int llhttp__on_url_complete(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_url_complete, s);
  return err;
}


int llhttp__on_status(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_status, s, p, endp - p);
  return err;
}


int llhttp__on_status_complete(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_status_complete, s);
  return err;
}


int llhttp__on_header_field(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_header_field, s, p, endp - p);
  return err;
}


int llhttp__on_header_field_complete(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_header_field_complete, s);
  return err;
}


int llhttp__on_header_value(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_header_value, s, p, endp - p);
  return err;
}


int llhttp__on_header_value_complete(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_header_value_complete, s);
  return err;
}


int llhttp__on_headers_complete(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_headers_complete, s);
  return err;
}


int llhttp__on_message_complete(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_message_complete, s);
  return err;
}


int llhttp__on_body(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_body, s, p, endp - p);
  return err;
}


int llhttp__on_chunk_header(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_chunk_header, s);
  return err;
}


int llhttp__on_chunk_complete(llhttp_t* s, const char* p, const char* endp) {
  int err;
  CALLBACK_MAYBE(s, on_chunk_complete, s);
  return err;
}





void llhttp__debug(llhttp_t* s, const char* p, const char* endp,
                   const char* msg) {
  if (p == endp) {
    fprintf(stderr, "p=%p type=%d flags=%02x next=null debug=%s\n", s, s->type,
            s->flags, msg);
  } else {
    fprintf(stderr, "p=%p type=%d flags=%02x next=%02x   debug=%s\n", s,
            s->type, s->flags, *p, msg);
  }
}
