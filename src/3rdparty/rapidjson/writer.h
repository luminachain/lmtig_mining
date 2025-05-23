













#ifndef RAPIDJSON_WRITER_H_
#define RAPIDJSON_WRITER_H_

#include "stream.h"
#include "internal/clzll.h"
#include "internal/meta.h"
#include "internal/stack.h"
#include "internal/strfunc.h"
#include "internal/dtoa.h"
#include "internal/itoa.h"
#include "stringbuffer.h"
#include <new>      

#if defined(RAPIDJSON_SIMD) && defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#endif
#ifdef RAPIDJSON_SSE42
#include <nmmintrin.h>
#elif defined(RAPIDJSON_SSE2)
#include <emmintrin.h>
#elif defined(RAPIDJSON_NEON)
#include <arm_neon.h>
#endif

#ifdef __clang__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(padded)
RAPIDJSON_DIAG_OFF(unreachable-code)
RAPIDJSON_DIAG_OFF(c++98-compat)
#elif defined(_MSC_VER)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(4127) 
#endif

RAPIDJSON_NAMESPACE_BEGIN





#ifndef RAPIDJSON_WRITE_DEFAULT_FLAGS
#define RAPIDJSON_WRITE_DEFAULT_FLAGS kWriteNoFlags
#endif


enum WriteFlag {
    kWriteNoFlags = 0,              
    kWriteValidateEncodingFlag = 1, 
    kWriteNanAndInfFlag = 2,        
    kWriteDefaultFlags = RAPIDJSON_WRITE_DEFAULT_FLAGS  
};



template<typename OutputStream, typename SourceEncoding = UTF8<>, typename TargetEncoding = UTF8<>, typename StackAllocator = CrtAllocator, unsigned writeFlags = kWriteDefaultFlags>
class Writer {
public:
    typedef typename SourceEncoding::Ch Ch;

    static const int kDefaultMaxDecimalPlaces = 324;

    
    
    explicit
    Writer(OutputStream& os, StackAllocator* stackAllocator = 0, size_t levelDepth = kDefaultLevelDepth) :
        os_(&os), level_stack_(stackAllocator, levelDepth * sizeof(Level)), maxDecimalPlaces_(kDefaultMaxDecimalPlaces), hasRoot_(false) {}

    explicit
    Writer(StackAllocator* allocator = 0, size_t levelDepth = kDefaultLevelDepth) :
        os_(0), level_stack_(allocator, levelDepth * sizeof(Level)), maxDecimalPlaces_(kDefaultMaxDecimalPlaces), hasRoot_(false) {}

#if RAPIDJSON_HAS_CXX11_RVALUE_REFS
    Writer(Writer&& rhs) :
        os_(rhs.os_), level_stack_(std::move(rhs.level_stack_)), maxDecimalPlaces_(rhs.maxDecimalPlaces_), hasRoot_(rhs.hasRoot_) {
        rhs.os_ = 0;
    }
#endif

    
    
    void Reset(OutputStream& os) {
        os_ = &os;
        hasRoot_ = false;
        level_stack_.Clear();
    }

    
    
    bool IsComplete() const {
        return hasRoot_ && level_stack_.Empty();
    }

    int GetMaxDecimalPlaces() const {
        return maxDecimalPlaces_;
    }

    
    
    void SetMaxDecimalPlaces(int maxDecimalPlaces) {
        maxDecimalPlaces_ = maxDecimalPlaces;
    }

    
    

    bool Null()                 { Prefix(kNullType);   return EndValue(WriteNull()); }
    bool Bool(bool b)           { Prefix(b ? kTrueType : kFalseType); return EndValue(WriteBool(b)); }
    bool Int(int i)             { Prefix(kNumberType); return EndValue(WriteInt(i)); }
    bool Uint(unsigned u)       { Prefix(kNumberType); return EndValue(WriteUint(u)); }
    bool Int64(int64_t i64)     { Prefix(kNumberType); return EndValue(WriteInt64(i64)); }
    bool Uint64(uint64_t u64)   { Prefix(kNumberType); return EndValue(WriteUint64(u64)); }

    
    
    bool Double(double d)       { Prefix(kNumberType); return EndValue(WriteDouble(d)); }

    bool RawNumber(const Ch* str, SizeType length, bool copy = false) {
        RAPIDJSON_ASSERT(str != 0);
        (void)copy;
        Prefix(kNumberType);
        return EndValue(WriteString(str, length));
    }

    bool String(const Ch* str, SizeType length, bool copy = false) {
        RAPIDJSON_ASSERT(str != 0);
        (void)copy;
        Prefix(kStringType);
        return EndValue(WriteString(str, length));
    }

#if RAPIDJSON_HAS_STDSTRING
    bool String(const std::basic_string<Ch>& str) {
        return String(str.data(), SizeType(str.size()));
    }
#endif

    bool StartObject() {
        Prefix(kObjectType);
        new (level_stack_.template Push<Level>()) Level(false);
        return WriteStartObject();
    }

    bool Key(const Ch* str, SizeType length, bool copy = false) { return String(str, length, copy); }

#if RAPIDJSON_HAS_STDSTRING
    bool Key(const std::basic_string<Ch>& str)
    {
      return Key(str.data(), SizeType(str.size()));
    }
#endif

    bool EndObject(SizeType memberCount = 0) {
        (void)memberCount;
        RAPIDJSON_ASSERT(level_stack_.GetSize() >= sizeof(Level)); 
        RAPIDJSON_ASSERT(!level_stack_.template Top<Level>()->inArray); 
        RAPIDJSON_ASSERT(0 == level_stack_.template Top<Level>()->valueCount % 2); 
        level_stack_.template Pop<Level>(1);
        return EndValue(WriteEndObject());
    }

    bool StartArray() {
        Prefix(kArrayType);
        new (level_stack_.template Push<Level>()) Level(true);
        return WriteStartArray();
    }

    bool EndArray(SizeType elementCount = 0) {
        (void)elementCount;
        RAPIDJSON_ASSERT(level_stack_.GetSize() >= sizeof(Level));
        RAPIDJSON_ASSERT(level_stack_.template Top<Level>()->inArray);
        level_stack_.template Pop<Level>(1);
        return EndValue(WriteEndArray());
    }
    

    
    

    
    bool String(const Ch* const& str) { return String(str, internal::StrLen(str)); }
    bool Key(const Ch* const& str) { return Key(str, internal::StrLen(str)); }

    

    
    
    bool RawValue(const Ch* json, size_t length, Type type) {
        RAPIDJSON_ASSERT(json != 0);
        Prefix(type);
        return EndValue(WriteRawValue(json, length));
    }

    
    
    void Flush() {
        os_->Flush();
    }

    static const size_t kDefaultLevelDepth = 32;

protected:
    
    struct Level {
        Level(bool inArray_) : valueCount(0), inArray(inArray_) {}
        size_t valueCount;  
        bool inArray;       
        bool inLine = false;
    };

    bool WriteNull()  {
        PutReserve(*os_, 4);
        PutUnsafe(*os_, 'n'); PutUnsafe(*os_, 'u'); PutUnsafe(*os_, 'l'); PutUnsafe(*os_, 'l'); return true;
    }

    bool WriteBool(bool b)  {
        if (b) {
            PutReserve(*os_, 4);
            PutUnsafe(*os_, 't'); PutUnsafe(*os_, 'r'); PutUnsafe(*os_, 'u'); PutUnsafe(*os_, 'e');
        }
        else {
            PutReserve(*os_, 5);
            PutUnsafe(*os_, 'f'); PutUnsafe(*os_, 'a'); PutUnsafe(*os_, 'l'); PutUnsafe(*os_, 's'); PutUnsafe(*os_, 'e');
        }
        return true;
    }

    bool WriteInt(int i) {
        char buffer[11];
        const char* end = internal::i32toa(i, buffer);
        PutReserve(*os_, static_cast<size_t>(end - buffer));
        for (const char* p = buffer; p != end; ++p)
            PutUnsafe(*os_, static_cast<typename OutputStream::Ch>(*p));
        return true;
    }

    bool WriteUint(unsigned u) {
        char buffer[10];
        const char* end = internal::u32toa(u, buffer);
        PutReserve(*os_, static_cast<size_t>(end - buffer));
        for (const char* p = buffer; p != end; ++p)
            PutUnsafe(*os_, static_cast<typename OutputStream::Ch>(*p));
        return true;
    }

    bool WriteInt64(int64_t i64) {
        char buffer[21];
        const char* end = internal::i64toa(i64, buffer);
        PutReserve(*os_, static_cast<size_t>(end - buffer));
        for (const char* p = buffer; p != end; ++p)
            PutUnsafe(*os_, static_cast<typename OutputStream::Ch>(*p));
        return true;
    }

    bool WriteUint64(uint64_t u64) {
        char buffer[20];
        char* end = internal::u64toa(u64, buffer);
        PutReserve(*os_, static_cast<size_t>(end - buffer));
        for (char* p = buffer; p != end; ++p)
            PutUnsafe(*os_, static_cast<typename OutputStream::Ch>(*p));
        return true;
    }

    bool WriteDouble(double d) {
        if (internal::Double(d).IsNanOrInf()) {
            if (!(writeFlags & kWriteNanAndInfFlag))
                return false;
            if (internal::Double(d).IsNan()) {
                PutReserve(*os_, 3);
                PutUnsafe(*os_, 'N'); PutUnsafe(*os_, 'a'); PutUnsafe(*os_, 'N');
                return true;
            }
            if (internal::Double(d).Sign()) {
                PutReserve(*os_, 9);
                PutUnsafe(*os_, '-');
            }
            else
                PutReserve(*os_, 8);
            PutUnsafe(*os_, 'I'); PutUnsafe(*os_, 'n'); PutUnsafe(*os_, 'f');
            PutUnsafe(*os_, 'i'); PutUnsafe(*os_, 'n'); PutUnsafe(*os_, 'i'); PutUnsafe(*os_, 't'); PutUnsafe(*os_, 'y');
            return true;
        }

        char buffer[25];
        char* end = internal::dtoa(d, buffer, maxDecimalPlaces_);
        PutReserve(*os_, static_cast<size_t>(end - buffer));
        for (char* p = buffer; p != end; ++p)
            PutUnsafe(*os_, static_cast<typename OutputStream::Ch>(*p));
        return true;
    }

    bool WriteString(const Ch* str, SizeType length)  {
        static const typename OutputStream::Ch hexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
        static const char escape[256] = {
#define Z16 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
            
            'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'b', 't', 'n', 'u', 'f', 'r', 'u', 'u', 
            'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 
              0,   0, '"',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
            Z16, Z16,                                                                       
              0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,'\\',   0,   0,   0, 
            Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16                                
#undef Z16
        };

        if (TargetEncoding::supportUnicode)
            PutReserve(*os_, 2 + length * 6); 
        else
            PutReserve(*os_, 2 + length * 12);  

        PutUnsafe(*os_, '\"');
        GenericStringStream<SourceEncoding> is(str);
        while (ScanWriteUnescapedString(is, length)) {
            const Ch c = is.Peek();
            if (!TargetEncoding::supportUnicode && static_cast<unsigned>(c) >= 0x80) {
                
                unsigned codepoint;
                if (RAPIDJSON_UNLIKELY(!SourceEncoding::Decode(is, &codepoint)))
                    return false;
                PutUnsafe(*os_, '\\');
                PutUnsafe(*os_, 'u');
                if (codepoint <= 0xD7FF || (codepoint >= 0xE000 && codepoint <= 0xFFFF)) {
                    PutUnsafe(*os_, hexDigits[(codepoint >> 12) & 15]);
                    PutUnsafe(*os_, hexDigits[(codepoint >>  8) & 15]);
                    PutUnsafe(*os_, hexDigits[(codepoint >>  4) & 15]);
                    PutUnsafe(*os_, hexDigits[(codepoint      ) & 15]);
                }
                else {
                    RAPIDJSON_ASSERT(codepoint >= 0x010000 && codepoint <= 0x10FFFF);
                    
                    unsigned s = codepoint - 0x010000;
                    unsigned lead = (s >> 10) + 0xD800;
                    unsigned trail = (s & 0x3FF) + 0xDC00;
                    PutUnsafe(*os_, hexDigits[(lead >> 12) & 15]);
                    PutUnsafe(*os_, hexDigits[(lead >>  8) & 15]);
                    PutUnsafe(*os_, hexDigits[(lead >>  4) & 15]);
                    PutUnsafe(*os_, hexDigits[(lead      ) & 15]);
                    PutUnsafe(*os_, '\\');
                    PutUnsafe(*os_, 'u');
                    PutUnsafe(*os_, hexDigits[(trail >> 12) & 15]);
                    PutUnsafe(*os_, hexDigits[(trail >>  8) & 15]);
                    PutUnsafe(*os_, hexDigits[(trail >>  4) & 15]);
                    PutUnsafe(*os_, hexDigits[(trail      ) & 15]);
                }
            }
            else if ((sizeof(Ch) == 1 || static_cast<unsigned>(c) < 256) && RAPIDJSON_UNLIKELY(escape[static_cast<unsigned char>(c)]))  {
                is.Take();
                PutUnsafe(*os_, '\\');
                PutUnsafe(*os_, static_cast<typename OutputStream::Ch>(escape[static_cast<unsigned char>(c)]));
                if (escape[static_cast<unsigned char>(c)] == 'u') {
                    PutUnsafe(*os_, '0');
                    PutUnsafe(*os_, '0');
                    PutUnsafe(*os_, hexDigits[static_cast<unsigned char>(c) >> 4]);
                    PutUnsafe(*os_, hexDigits[static_cast<unsigned char>(c) & 0xF]);
                }
            }
            else if (RAPIDJSON_UNLIKELY(!(writeFlags & kWriteValidateEncodingFlag ?
                Transcoder<SourceEncoding, TargetEncoding>::Validate(is, *os_) :
                Transcoder<SourceEncoding, TargetEncoding>::TranscodeUnsafe(is, *os_))))
                return false;
        }
        PutUnsafe(*os_, '\"');
        return true;
    }

    bool ScanWriteUnescapedString(GenericStringStream<SourceEncoding>& is, size_t length) {
        return RAPIDJSON_LIKELY(is.Tell() < length);
    }

    bool WriteStartObject() { os_->Put('{'); return true; }
    bool WriteEndObject()   { os_->Put('}'); return true; }
    bool WriteStartArray()  { os_->Put('['); return true; }
    bool WriteEndArray()    { os_->Put(']'); return true; }

    bool WriteRawValue(const Ch* json, size_t length) {
        PutReserve(*os_, length);
        GenericStringStream<SourceEncoding> is(json);
        while (RAPIDJSON_LIKELY(is.Tell() < length)) {
            RAPIDJSON_ASSERT(is.Peek() != '\0');
            if (RAPIDJSON_UNLIKELY(!(writeFlags & kWriteValidateEncodingFlag ?
                Transcoder<SourceEncoding, TargetEncoding>::Validate(is, *os_) :
                Transcoder<SourceEncoding, TargetEncoding>::TranscodeUnsafe(is, *os_))))
                return false;
        }
        return true;
    }

    void Prefix(Type type) {
        (void)type;
        if (RAPIDJSON_LIKELY(level_stack_.GetSize() != 0)) { 
            Level* level = level_stack_.template Top<Level>();
            if (level->valueCount > 0) {
                if (level->inArray)
                    os_->Put(','); 
                else  
                    os_->Put((level->valueCount % 2 == 0) ? ',' : ':');
            }
            if (!level->inArray && level->valueCount % 2 == 0)
                RAPIDJSON_ASSERT(type == kStringType);  
            level->valueCount++;
        }
        else {
            RAPIDJSON_ASSERT(!hasRoot_);    
            hasRoot_ = true;
        }
    }

    
    bool EndValue(bool ret) {
        if (RAPIDJSON_UNLIKELY(level_stack_.Empty()))   
            Flush();
        return ret;
    }

    OutputStream* os_;
    internal::Stack<StackAllocator> level_stack_;
    int maxDecimalPlaces_;
    bool hasRoot_;

private:
    
    Writer(const Writer&);
    Writer& operator=(const Writer&);
};



template<>
inline bool Writer<StringBuffer>::WriteInt(int i) {
    char *buffer = os_->Push(11);
    const char* end = internal::i32toa(i, buffer);
    os_->Pop(static_cast<size_t>(11 - (end - buffer)));
    return true;
}

template<>
inline bool Writer<StringBuffer>::WriteUint(unsigned u) {
    char *buffer = os_->Push(10);
    const char* end = internal::u32toa(u, buffer);
    os_->Pop(static_cast<size_t>(10 - (end - buffer)));
    return true;
}

template<>
inline bool Writer<StringBuffer>::WriteInt64(int64_t i64) {
    char *buffer = os_->Push(21);
    const char* end = internal::i64toa(i64, buffer);
    os_->Pop(static_cast<size_t>(21 - (end - buffer)));
    return true;
}

template<>
inline bool Writer<StringBuffer>::WriteUint64(uint64_t u) {
    char *buffer = os_->Push(20);
    const char* end = internal::u64toa(u, buffer);
    os_->Pop(static_cast<size_t>(20 - (end - buffer)));
    return true;
}

template<>
inline bool Writer<StringBuffer>::WriteDouble(double d) {
    if (internal::Double(d).IsNanOrInf()) {
        
        if (!(kWriteDefaultFlags & kWriteNanAndInfFlag))
            return false;
        if (internal::Double(d).IsNan()) {
            PutReserve(*os_, 3);
            PutUnsafe(*os_, 'N'); PutUnsafe(*os_, 'a'); PutUnsafe(*os_, 'N');
            return true;
        }
        if (internal::Double(d).Sign()) {
            PutReserve(*os_, 9);
            PutUnsafe(*os_, '-');
        }
        else
            PutReserve(*os_, 8);
        PutUnsafe(*os_, 'I'); PutUnsafe(*os_, 'n'); PutUnsafe(*os_, 'f');
        PutUnsafe(*os_, 'i'); PutUnsafe(*os_, 'n'); PutUnsafe(*os_, 'i'); PutUnsafe(*os_, 't'); PutUnsafe(*os_, 'y');
        return true;
    }

    char *buffer = os_->Push(25);
    char* end = internal::dtoa(d, buffer, maxDecimalPlaces_);
    os_->Pop(static_cast<size_t>(25 - (end - buffer)));
    return true;
}

#if defined(RAPIDJSON_SSE2) || defined(RAPIDJSON_SSE42)
template<>
inline bool Writer<StringBuffer>::ScanWriteUnescapedString(StringStream& is, size_t length) {
    if (length < 16)
        return RAPIDJSON_LIKELY(is.Tell() < length);

    if (!RAPIDJSON_LIKELY(is.Tell() < length))
        return false;

    const char* p = is.src_;
    const char* end = is.head_ + length;
    const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
    const char* endAligned = reinterpret_cast<const char*>(reinterpret_cast<size_t>(end) & static_cast<size_t>(~15));
    if (nextAligned > end)
        return true;

    while (p != nextAligned)
        if (*p < 0x20 || *p == '\"' || *p == '\\') {
            is.src_ = p;
            return RAPIDJSON_LIKELY(is.Tell() < length);
        }
        else
            os_->PutUnsafe(*p++);

    
    static const char dquote[16] = { '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"', '\"' };
    static const char bslash[16] = { '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\' };
    static const char space[16]  = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F };
    const __m128i dq = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&dquote[0]));
    const __m128i bs = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&bslash[0]));
    const __m128i sp = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&space[0]));

    for (; p != endAligned; p += 16) {
        const __m128i s = _mm_load_si128(reinterpret_cast<const __m128i *>(p));
        const __m128i t1 = _mm_cmpeq_epi8(s, dq);
        const __m128i t2 = _mm_cmpeq_epi8(s, bs);
        const __m128i t3 = _mm_cmpeq_epi8(_mm_max_epu8(s, sp), sp); 
        const __m128i x = _mm_or_si128(_mm_or_si128(t1, t2), t3);
        unsigned short r = static_cast<unsigned short>(_mm_movemask_epi8(x));
        if (RAPIDJSON_UNLIKELY(r != 0)) {   
            SizeType len;
#ifdef _MSC_VER         
            unsigned long offset;
            _BitScanForward(&offset, r);
            len = offset;
#else
            len = static_cast<SizeType>(__builtin_ffs(r) - 1);
#endif
            char* q = reinterpret_cast<char*>(os_->PushUnsafe(len));
            for (size_t i = 0; i < len; i++)
                q[i] = p[i];

            p += len;
            break;
        }
        _mm_storeu_si128(reinterpret_cast<__m128i *>(os_->PushUnsafe(16)), s);
    }

    is.src_ = p;
    return RAPIDJSON_LIKELY(is.Tell() < length);
}
#elif defined(RAPIDJSON_NEON)
template<>
inline bool Writer<StringBuffer>::ScanWriteUnescapedString(StringStream& is, size_t length) {
    if (length < 16)
        return RAPIDJSON_LIKELY(is.Tell() < length);

    if (!RAPIDJSON_LIKELY(is.Tell() < length))
        return false;

    const char* p = is.src_;
    const char* end = is.head_ + length;
    const char* nextAligned = reinterpret_cast<const char*>((reinterpret_cast<size_t>(p) + 15) & static_cast<size_t>(~15));
    const char* endAligned = reinterpret_cast<const char*>(reinterpret_cast<size_t>(end) & static_cast<size_t>(~15));
    if (nextAligned > end)
        return true;

    while (p != nextAligned)
        if (*p < 0x20 || *p == '\"' || *p == '\\') {
            is.src_ = p;
            return RAPIDJSON_LIKELY(is.Tell() < length);
        }
        else
            os_->PutUnsafe(*p++);

    
    const uint8x16_t s0 = vmovq_n_u8('"');
    const uint8x16_t s1 = vmovq_n_u8('\\');
    const uint8x16_t s2 = vmovq_n_u8('\b');
    const uint8x16_t s3 = vmovq_n_u8(32);

    for (; p != endAligned; p += 16) {
        const uint8x16_t s = vld1q_u8(reinterpret_cast<const uint8_t *>(p));
        uint8x16_t x = vceqq_u8(s, s0);
        x = vorrq_u8(x, vceqq_u8(s, s1));
        x = vorrq_u8(x, vceqq_u8(s, s2));
        x = vorrq_u8(x, vcltq_u8(s, s3));

        x = vrev64q_u8(x);                     
        uint64_t low = vgetq_lane_u64(vreinterpretq_u64_u8(x), 0);   
        uint64_t high = vgetq_lane_u64(vreinterpretq_u64_u8(x), 1);  

        SizeType len = 0;
        bool escaped = false;
        if (low == 0) {
            if (high != 0) {
                uint32_t lz = internal::clzll(high);
                len = 8 + (lz >> 3);
                escaped = true;
            }
        } else {
            uint32_t lz = internal::clzll(low);
            len = lz >> 3;
            escaped = true;
        }
        if (RAPIDJSON_UNLIKELY(escaped)) {   
            char* q = reinterpret_cast<char*>(os_->PushUnsafe(len));
            for (size_t i = 0; i < len; i++)
                q[i] = p[i];

            p += len;
            break;
        }
        vst1q_u8(reinterpret_cast<uint8_t *>(os_->PushUnsafe(16)), s);
    }

    is.src_ = p;
    return RAPIDJSON_LIKELY(is.Tell() < length);
}
#endif 

RAPIDJSON_NAMESPACE_END

#if defined(_MSC_VER) || defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#endif 
