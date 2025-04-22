













#ifndef RAPIDJSON_URI_H_
#define RAPIDJSON_URI_H_

#include "internal/strfunc.h"

#if defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(c++98-compat)
#elif defined(_MSC_VER)
RAPIDJSON_DIAG_OFF(4512) 
#endif

RAPIDJSON_NAMESPACE_BEGIN




template <typename ValueType, typename Allocator=CrtAllocator>
class GenericUri {
public:
    typedef typename ValueType::Ch Ch;
#if RAPIDJSON_HAS_STDSTRING
    typedef std::basic_string<Ch> String;
#endif

    
    GenericUri(Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
    }

    GenericUri(const Ch* uri, SizeType len, Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        Parse(uri, len);
    }

    GenericUri(const Ch* uri, Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        Parse(uri, internal::StrLen<Ch>(uri));
    }

    
    template<typename T> GenericUri(const T& uri, Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        const Ch* u = uri.template Get<const Ch*>(); 
        Parse(u, internal::StrLen<Ch>(u));
    }

#if RAPIDJSON_HAS_STDSTRING
    GenericUri(const String& uri, Allocator* allocator = 0) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        Parse(uri.c_str(), internal::StrLen<Ch>(uri.c_str()));
    }
#endif

    
    GenericUri(const GenericUri& rhs) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(), ownAllocator_() {
        *this = rhs;
    }

    
    GenericUri(const GenericUri& rhs, Allocator* allocator) : uri_(), base_(), scheme_(), auth_(), path_(), query_(), frag_(), allocator_(allocator), ownAllocator_() {
        *this = rhs;
    }

    
    ~GenericUri() {
        Free();
        RAPIDJSON_DELETE(ownAllocator_);
    }

    
    GenericUri& operator=(const GenericUri& rhs) {
        if (this != &rhs) {
            
            Free();
            Allocate(rhs.GetStringLength());
            auth_ = CopyPart(scheme_, rhs.scheme_, rhs.GetSchemeStringLength());
            path_ = CopyPart(auth_, rhs.auth_, rhs.GetAuthStringLength());
            query_ = CopyPart(path_, rhs.path_, rhs.GetPathStringLength());
            frag_ = CopyPart(query_, rhs.query_, rhs.GetQueryStringLength());
            base_ = CopyPart(frag_, rhs.frag_, rhs.GetFragStringLength());
            uri_ = CopyPart(base_, rhs.base_, rhs.GetBaseStringLength());
            CopyPart(uri_, rhs.uri_, rhs.GetStringLength());
        }
        return *this;
    }

    
    
    template<typename T> void Get(T& uri, Allocator& allocator) {
        uri.template Set<const Ch*>(this->GetString(), allocator); 
    }

    const Ch* GetString() const { return uri_; }
    SizeType GetStringLength() const { return uri_ == 0 ? 0 : internal::StrLen<Ch>(uri_); }
    const Ch* GetBaseString() const { return base_; }
    SizeType GetBaseStringLength() const { return base_ == 0 ? 0 : internal::StrLen<Ch>(base_); }
    const Ch* GetSchemeString() const { return scheme_; }
    SizeType GetSchemeStringLength() const { return scheme_ == 0 ? 0 : internal::StrLen<Ch>(scheme_); }
    const Ch* GetAuthString() const { return auth_; }
    SizeType GetAuthStringLength() const { return auth_ == 0 ? 0 : internal::StrLen<Ch>(auth_); }
    const Ch* GetPathString() const { return path_; }
    SizeType GetPathStringLength() const { return path_ == 0 ? 0 : internal::StrLen<Ch>(path_); }
    const Ch* GetQueryString() const { return query_; }
    SizeType GetQueryStringLength() const { return query_ == 0 ? 0 : internal::StrLen<Ch>(query_); }
    const Ch* GetFragString() const { return frag_; }
    SizeType GetFragStringLength() const { return frag_ == 0 ? 0 : internal::StrLen<Ch>(frag_); }

#if RAPIDJSON_HAS_STDSTRING
    static String Get(const GenericUri& uri) { return String(uri.GetString(), uri.GetStringLength()); }
    static String GetBase(const GenericUri& uri) { return String(uri.GetBaseString(), uri.GetBaseStringLength()); }
    static String GetScheme(const GenericUri& uri) { return String(uri.GetSchemeString(), uri.GetSchemeStringLength()); }
    static String GetAuth(const GenericUri& uri) { return String(uri.GetAuthString(), uri.GetAuthStringLength()); }
    static String GetPath(const GenericUri& uri) { return String(uri.GetPathString(), uri.GetPathStringLength()); }
    static String GetQuery(const GenericUri& uri) { return String(uri.GetQueryString(), uri.GetQueryStringLength()); }
    static String GetFrag(const GenericUri& uri) { return String(uri.GetFragString(), uri.GetFragStringLength()); }
#endif

    
    bool operator==(const GenericUri& rhs) const {
        return Match(rhs, true);
    }

    bool operator!=(const GenericUri& rhs) const {
        return !Match(rhs, true);
    }

    bool Match(const GenericUri& uri, bool full = true) const {
        Ch* s1;
        Ch* s2;
        if (full) {
            s1 = uri_;
            s2 = uri.uri_;
        } else {
            s1 = base_;
            s2 = uri.base_;
        }
        if (s1 == s2) return true;
        if (s1 == 0 || s2 == 0) return false;
        return internal::StrCmp<Ch>(s1, s2) == 0;
    }

    
    
    
    
    GenericUri Resolve(const GenericUri& baseuri, Allocator* allocator = 0) {
        GenericUri resuri;
        resuri.allocator_ = allocator;
        
        resuri.Allocate(GetStringLength() + baseuri.GetStringLength() + 1); 

        if (!(GetSchemeStringLength() == 0)) {
            
            resuri.auth_ = CopyPart(resuri.scheme_, scheme_, GetSchemeStringLength());
            resuri.path_ = CopyPart(resuri.auth_, auth_, GetAuthStringLength());
            resuri.query_ = CopyPart(resuri.path_, path_, GetPathStringLength());
            resuri.frag_ = CopyPart(resuri.query_, query_, GetQueryStringLength());
            resuri.RemoveDotSegments();
        } else {
            
            resuri.auth_ = CopyPart(resuri.scheme_, baseuri.scheme_, baseuri.GetSchemeStringLength());
            if (!(GetAuthStringLength() == 0)) {
                
                resuri.path_ = CopyPart(resuri.auth_, auth_, GetAuthStringLength());
                resuri.query_ = CopyPart(resuri.path_, path_, GetPathStringLength());
                resuri.frag_ = CopyPart(resuri.query_, query_, GetQueryStringLength());
                resuri.RemoveDotSegments();
            } else {
                
                resuri.path_ = CopyPart(resuri.auth_, baseuri.auth_, baseuri.GetAuthStringLength());
                if (GetPathStringLength() == 0) {
                    
                    resuri.query_ = CopyPart(resuri.path_, baseuri.path_, baseuri.GetPathStringLength());
                    if (GetQueryStringLength() == 0) {
                        
                        resuri.frag_ = CopyPart(resuri.query_, baseuri.query_, baseuri.GetQueryStringLength());
                    } else {
                        
                        resuri.frag_ = CopyPart(resuri.query_, query_, GetQueryStringLength());
                    }
                } else {
                    if (path_[0] == '/') {
                        
                        resuri.query_ = CopyPart(resuri.path_, path_, GetPathStringLength());
                        resuri.RemoveDotSegments();
                    } else {
                        
                        size_t pos = 0;
                        if (!(baseuri.GetAuthStringLength() == 0) && baseuri.GetPathStringLength() == 0) {
                            resuri.path_[pos] = '/';
                            pos++;
                        }
                        size_t lastslashpos = baseuri.GetPathStringLength();
                        while (lastslashpos > 0) {
                            if (baseuri.path_[lastslashpos - 1] == '/') break;
                            lastslashpos--;
                        }
                        std::memcpy(&resuri.path_[pos], baseuri.path_, lastslashpos * sizeof(Ch));
                        pos += lastslashpos;
                        resuri.query_ = CopyPart(&resuri.path_[pos], path_, GetPathStringLength());
                        resuri.RemoveDotSegments();
                    }
                    
                    resuri.frag_ = CopyPart(resuri.query_, query_, GetQueryStringLength());
                }
            }
        }
        
        resuri.base_ = CopyPart(resuri.frag_, frag_, GetFragStringLength());

        
        resuri.SetBase();
        resuri.uri_ = resuri.base_ + resuri.GetBaseStringLength() + 1;
        resuri.SetUri();
        return resuri;
    }

    
    Allocator& GetAllocator() { return *allocator_; }

private:
    
    
    std::size_t Allocate(std::size_t len) {
        
        if (!allocator_)
            ownAllocator_ =  allocator_ = RAPIDJSON_NEW(Allocator)();

        
        
        size_t total = (3 * len + 7) * sizeof(Ch);
        scheme_ = static_cast<Ch*>(allocator_->Malloc(total));
        *scheme_ = '\0';
        auth_ = scheme_ + 1;
        *auth_ = '\0';
        path_ = auth_ + 1;
        *path_ = '\0';
        query_ = path_ + 1;
        *query_ = '\0';
        frag_ = query_ + 1;
        *frag_ = '\0';
        base_ = frag_ + 1;
        *base_ = '\0';
        uri_ = base_ + 1;
        *uri_ = '\0';
        return total;
    }

    
    void Free() {
        if (scheme_) {
            Allocator::Free(scheme_);
            scheme_ = 0;
        }
    }

    
    
    
    void Parse(const Ch* uri, std::size_t len) {
        std::size_t start = 0, pos1 = 0, pos2 = 0;
        Allocate(len);

        
        if (start < len) {
            while (pos1 < len) {
                if (uri[pos1] == ':') break;
                pos1++;
            }
            if (pos1 != len) {
                while (pos2 < len) {
                    if (uri[pos2] == '/') break;
                    if (uri[pos2] == '?') break;
                    if (uri[pos2] == '#') break;
                    pos2++;
                }
                if (pos1 < pos2) {
                    pos1++;
                    std::memcpy(scheme_, &uri[start], pos1 * sizeof(Ch));
                    scheme_[pos1] = '\0';
                    start = pos1;
                }
            }
        }
        
        auth_ = scheme_ + GetSchemeStringLength() + 1;
        *auth_ = '\0';
        if (start < len - 1 && uri[start] == '/' && uri[start + 1] == '/') {
            pos2 = start + 2;
            while (pos2 < len) {
                if (uri[pos2] == '/') break;
                if (uri[pos2] == '?') break;
                if (uri[pos2] == '#') break;
                pos2++;
            }
            std::memcpy(auth_, &uri[start], (pos2 - start) * sizeof(Ch));
            auth_[pos2 - start] = '\0';
            start = pos2;
        }
        
        path_ = auth_ + GetAuthStringLength() + 1;
        *path_ = '\0';
        if (start < len) {
            pos2 = start;
            while (pos2 < len) {
                if (uri[pos2] == '?') break;
                if (uri[pos2] == '#') break;
                pos2++;
            }
            if (start != pos2) {
                std::memcpy(path_, &uri[start], (pos2 - start) * sizeof(Ch));
                path_[pos2 - start] = '\0';
                if (path_[0] == '/')
                    RemoveDotSegments();   
                start = pos2;
            }
        }
        
        query_ = path_ + GetPathStringLength() + 1;
        *query_ = '\0';
        if (start < len && uri[start] == '?') {
            pos2 = start + 1;
            while (pos2 < len) {
                if (uri[pos2] == '#') break;
                pos2++;
            }
            if (start != pos2) {
                std::memcpy(query_, &uri[start], (pos2 - start) * sizeof(Ch));
                query_[pos2 - start] = '\0';
                start = pos2;
            }
        }
        
        frag_ = query_ + GetQueryStringLength() + 1;
        *frag_ = '\0';
        if (start < len && uri[start] == '#') {
            std::memcpy(frag_, &uri[start], (len - start) * sizeof(Ch));
            frag_[len - start] = '\0';
        }

        
        base_ = frag_ + GetFragStringLength() + 1;
        SetBase();
        uri_ = base_ + GetBaseStringLength() + 1;
        SetUri();
    }

    
    void SetBase() {
        Ch* next = base_;
        std::memcpy(next, scheme_, GetSchemeStringLength() * sizeof(Ch));
        next+= GetSchemeStringLength();
        std::memcpy(next, auth_, GetAuthStringLength() * sizeof(Ch));
        next+= GetAuthStringLength();
        std::memcpy(next, path_, GetPathStringLength() * sizeof(Ch));
        next+= GetPathStringLength();
        std::memcpy(next, query_, GetQueryStringLength() * sizeof(Ch));
        next+= GetQueryStringLength();
        *next = '\0';
    }

    
    void SetUri() {
        Ch* next = uri_;
        std::memcpy(next, base_, GetBaseStringLength() * sizeof(Ch));
        next+= GetBaseStringLength();
        std::memcpy(next, frag_, GetFragStringLength() * sizeof(Ch));
        next+= GetFragStringLength();
        *next = '\0';
    }

    
    
    Ch* CopyPart(Ch* to, Ch* from, std::size_t len) {
        RAPIDJSON_ASSERT(to != 0);
        RAPIDJSON_ASSERT(from != 0);
        std::memcpy(to, from, len * sizeof(Ch));
        to[len] = '\0';
        Ch* next = to + len + 1;
        return next;
    }

    
    
    
    void RemoveDotSegments() {
        std::size_t pathlen = GetPathStringLength();
        std::size_t pathpos = 0;  
        std::size_t newpos = 0;   

        
        while (pathpos < pathlen) {
            
            size_t slashpos = 0;
            while ((pathpos + slashpos) < pathlen) {
                if (path_[pathpos + slashpos] == '/') break;
                slashpos++;
            }
            
            if (slashpos == 2 && path_[pathpos] == '.' && path_[pathpos + 1] == '.') {
                
                
                RAPIDJSON_ASSERT(newpos == 0 || path_[newpos - 1] == '/');
                size_t lastslashpos = newpos;
                
                if (lastslashpos > 1) {
                    
                    lastslashpos--;
                    while (lastslashpos > 0) {
                        if (path_[lastslashpos - 1] == '/') break;
                        lastslashpos--;
                    }
                    
                    newpos = lastslashpos;
                }
            } else if (slashpos == 1 && path_[pathpos] == '.') {
                
            } else {
                
                RAPIDJSON_ASSERT(newpos <= pathpos);
                std::memmove(&path_[newpos], &path_[pathpos], slashpos * sizeof(Ch));
                newpos += slashpos;
                
                if ((pathpos + slashpos) < pathlen) {
                    path_[newpos] = '/';
                    newpos++;
                }
            }
            
            pathpos += slashpos + 1;
        }
        path_[newpos] = '\0';
    }

    Ch* uri_;    
    Ch* base_;   
    Ch* scheme_; 
    Ch* auth_;   
    Ch* path_;   
    Ch* query_;  
    Ch* frag_;   

    Allocator* allocator_;      
    Allocator* ownAllocator_;   
};


typedef GenericUri<Value> Uri;

RAPIDJSON_NAMESPACE_END

#if defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#endif 
