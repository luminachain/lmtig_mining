#include <stdio.h>
#ifndef LLHTTP__TEST
# include "llhttp.h"
#else
# define llhttp_t llparse_t
#endif  

int llhttp_message_needs_eof(const llhttp_t* parser);
int llhttp_should_keep_alive(const llhttp_t* parser);

int llhttp__before_headers_complete(llhttp_t* parser, const char* p,
                                    const char* endp) {
  
  if ((parser->flags & F_UPGRADE) &&
      (parser->flags & F_CONNECTION_UPGRADE)) {
    
    parser->upgrade =
        (parser->type == HTTP_REQUEST || parser->status_code == 101);
  } else {
    parser->upgrade = (parser->method == HTTP_CONNECT);
  }
  return 0;
}



int llhttp__after_headers_complete(llhttp_t* parser, const char* p,
                                   const char* endp) {
  int hasBody;

  hasBody = parser->flags & F_CHUNKED || parser->content_length > 0;
  if (parser->upgrade && (parser->method == HTTP_CONNECT ||
                          (parser->flags & F_SKIPBODY) || !hasBody)) {
    
    return 1;
  }

  if (parser->flags & F_SKIPBODY) {
    return 0;
  } else if (parser->flags & F_CHUNKED) {
    
    return 2;
  } else if (parser->flags & F_TRANSFER_ENCODING) {
    if (parser->type == HTTP_REQUEST &&
        (parser->lenient_flags & LENIENT_CHUNKED_LENGTH) == 0) {
      

      
      return 5;
    } else {
      

      
      return 4;
    }
  } else {
    if (!(parser->flags & F_CONTENT_LENGTH)) {
      if (!llhttp_message_needs_eof(parser)) {
        
        return 0;
      } else {
        
        return 4;
      }
    } else if (parser->content_length == 0) {
      
      return 0;
    } else {
      
      return 3;
    }
  }
}


int llhttp__after_message_complete(llhttp_t* parser, const char* p,
                                   const char* endp) {
  int should_keep_alive;

  should_keep_alive = llhttp_should_keep_alive(parser);
  parser->finish = HTTP_FINISH_SAFE;
  parser->flags = 0;

  
  return should_keep_alive;
}


int llhttp_message_needs_eof(const llhttp_t* parser) {
  if (parser->type == HTTP_REQUEST) {
    return 0;
  }

  
  if (parser->status_code / 100 == 1 || 
      parser->status_code == 204 ||     
      parser->status_code == 304 ||     
      (parser->flags & F_SKIPBODY)) {     
    return 0;
  }

  
  if ((parser->flags & F_TRANSFER_ENCODING) &&
      (parser->flags & F_CHUNKED) == 0) {
    return 1;
  }

  if (parser->flags & (F_CHUNKED | F_CONTENT_LENGTH)) {
    return 0;
  }

  return 1;
}


int llhttp_should_keep_alive(const llhttp_t* parser) {
  if (parser->http_major > 0 && parser->http_minor > 0) {
    
    if (parser->flags & F_CONNECTION_CLOSE) {
      return 0;
    }
  } else {
    
    if (!(parser->flags & F_CONNECTION_KEEP_ALIVE)) {
      return 0;
    }
  }

  return !llhttp_message_needs_eof(parser);
}
