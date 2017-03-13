#ifndef MSGRPC_THRIFT_TYPES_H_H
#define MSGRPC_THRIFT_TYPES_H_H

#include <string>
#include <vector>
#include <map>
#include <thrift/protocol/TProtocol.h>

////////////////////////////////////////////////////////////////////////////////
/*enum TType {
    T_VOID       = 1,
    T_BOOL       = 2,  -
    T_BYTE       = 3,  -
    T_I08        = 3,  -
    T_I16        = 6,  -
    T_I32        = 8,  -
    T_U64        = 9,
    T_I64        = 10, -
    T_DOUBLE     = 4,  -
    T_STRING     = 11, -
    T_UTF7       = 11,
    T_STRUCT     = 12, -
    T_MAP        = 13, -
    T_SET        = 14, -
    T_LIST       = 15, -
    T_UTF8       = 16,
    T_UTF16      = 17
};*/

struct binary : std::string {};

////////////////////////////////////////////////////////////////////////////////
template<typename T> struct TTypeT {
    enum {value = ::apache::thrift::protocol::T_STRUCT};
    static uint32_t  read(::apache::thrift::protocol::TProtocol* iprot, T& t) {
        return t.read(iprot);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const T& t) {
        return t.write(oprot);
    }
};

template<> struct TTypeT<bool> {
    enum {value = ::apache::thrift::protocol::T_BOOL};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, bool& t) {
        return iprot->readBool(t);
    }
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::vector<bool>::reference t) {
        return iprot->readBool(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const bool& t) {
        return oprot->writeBool(t);
    }
};

template<> struct TTypeT<int8_t> {
    enum {value = ::apache::thrift::protocol::T_BYTE};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, int8_t& t) {
        return iprot->readByte(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const int8_t& t) {
        return oprot->writeByte(t);
    }
};

template<> struct TTypeT<int16_t> {
    enum {value = ::apache::thrift::protocol::T_I16};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, int16_t& t) {
        return iprot->readI16(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const int16_t& t) {
        return oprot->writeI16(t);
    }
};

template<> struct TTypeT<int32_t> {
    enum {value = ::apache::thrift::protocol::T_I32};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, int32_t& t) {
        return iprot->readI32(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const int32_t& t) {
        return oprot->writeI32(t);
    }
};

template<> struct TTypeT<int64_t> {
    enum {value = ::apache::thrift::protocol::T_I64};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, int64_t& t) {
        return iprot->readI64(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const int64_t& t) {
        return oprot->writeI64(t);
    }
};

template<> struct TTypeT<double> {
    enum {value = ::apache::thrift::protocol::T_DOUBLE};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, double& t) {
        return iprot->readDouble(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const double& t) {
        return oprot->writeDouble(t);
    }
};

template<> struct TTypeT<std::string> {
    enum {value = ::apache::thrift::protocol::T_STRING};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::string& t) {
        return iprot->readString(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const std::string& t) {
        return oprot->writeString(t);
    }
};

template<> struct TTypeT<binary> {
    enum {value = ::apache::thrift::protocol::T_STRING};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, binary& t) {
        return iprot->readBinary(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const binary& t) {
        return oprot->writeString(t);
    }
};

template<typename T>
struct TTypeT<std::vector<T> > {
enum {value = ::apache::thrift::protocol::T_LIST};
static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::vector<T>& ___t) {
    uint32_t ret = 0;
    ___t.clear();
    uint32_t _size;
    ::apache::thrift::protocol::TType _etype;
    ret += iprot->readListBegin(_etype, _size);

    ___t.resize(_size);
    uint32_t _i;
    for (_i = 0; _i < _size; ++_i)
    {
        ret += TTypeT<T>::read(iprot, ___t[_i]);
    }
    ret += iprot->readListEnd();
    return ret;
}

static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const std::vector<T>& ___t) {
    uint32_t ret = 0;
    ret += oprot->writeListBegin((::apache::thrift::protocol::TType)TTypeT<T>::value, static_cast<uint32_t>(___t.size()));

    typename std::vector<T>::const_iterator _iter;
    for (_iter = ___t.begin(); _iter != ___t.end(); ++_iter)
    {
        ret += TTypeT<T>::write(oprot, (*_iter));
    }

    ret += oprot->writeListEnd();
    return ret;
}
};

template<typename T>
struct TTypeT<std::set<T> > {
enum {value = ::apache::thrift::protocol::T_SET};
static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::set<T>& ___t) {
    uint32_t ret = 0;
    uint32_t _size;
    ::apache::thrift::protocol::TType _etype;
    ret += iprot->readSetBegin(_etype, _size);

    uint32_t _i;
    for (_i = 0; _i < _size; ++_i)
    {
        T _elem;
        ret += TTypeT<T>::read(iprot, _elem);
        ___t.insert(_elem);
    }
    ret += iprot->readSetEnd();

    return ret;
}

static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const std::set<T>& ___t) {
    uint32_t ret = 0;

    ret += oprot->writeSetBegin((::apache::thrift::protocol::TType)TTypeT<T>::value, static_cast<uint32_t>(___t.size()));
    typename std::set<T>::const_iterator _iter;
    for (_iter = ___t.begin(); _iter != ___t.end(); ++_iter)
    {
        ret += TTypeT<T>::write(oprot, (*_iter));
    }
    ret += oprot->writeSetEnd();

    return ret;
}
};

template<typename _K, typename _V>
struct TTypeT<std::map<_K, _V> > {
enum {value = ::apache::thrift::protocol::T_MAP};
static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::map<_K, _V>& ___t) {
    uint32_t ret = 0;

    ___t.clear();
    uint32_t _size;
    ::apache::thrift::protocol::TType _ktype;
    ::apache::thrift::protocol::TType _vtype;
    ret += iprot->readMapBegin(_ktype, _vtype, _size);

    uint32_t _i;
    for (_i = 0; _i < _size; ++_i) {
        _K _key;
        ret += TTypeT<_K>::read(iprot, _key);
        _V &_val = ___t[_key];
        ret += TTypeT<_V>::read(iprot, _val);
    }
    ret += iprot->readMapEnd();

    return ret;
}

static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const std::map<_K, _V>& ___t) {
    uint32_t ret = 0;

    ret += oprot->writeMapBegin((::apache::thrift::protocol::TType)TTypeT<_K>::value
            , (::apache::thrift::protocol::TType)TTypeT<_V>::value
            , static_cast<uint32_t>(___t.size()));
    typename std::map<_K, _V> ::const_iterator _iter;
    for (_iter = ___t.begin(); _iter != ___t.end(); ++_iter)
    {
        ret += TTypeT<_K>::write(oprot, _iter->first);
        ret += TTypeT<_V>::write(oprot, _iter->second);
    }
    ret += oprot->writeMapEnd();

    return ret;
}
};


#endif //MSGRPC_THRIFT_TYPES_H_H
