/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "api_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>

namespace org { namespace example { namespace msgrpc { namespace thrift {


EmbeddedStruct::~EmbeddedStruct() throw() {
}


void EmbeddedStruct::__set_es_i8(const int8_t val) {
  this->es_i8 = val;
}

void EmbeddedStruct::__set_es_i16(const int16_t val) {
  this->es_i16 = val;
}

uint32_t EmbeddedStruct::read(::apache::thrift::protocol::TProtocol* iprot) {

  apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_BYTE) {
          xfer += iprot->readByte(this->es_i8);
          this->__isset.es_i8 = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I16) {
          xfer += iprot->readI16(this->es_i16);
          this->__isset.es_i16 = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t EmbeddedStruct::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("EmbeddedStruct");

  xfer += oprot->writeFieldBegin("es_i8", ::apache::thrift::protocol::T_BYTE, 1);
  xfer += oprot->writeByte(this->es_i8);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("es_i16", ::apache::thrift::protocol::T_I16, 2);
  xfer += oprot->writeI16(this->es_i16);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(EmbeddedStruct &a, EmbeddedStruct &b) {
  using ::std::swap;
  swap(a.es_i8, b.es_i8);
  swap(a.es_i16, b.es_i16);
  swap(a.__isset, b.__isset);
}

EmbeddedStruct::EmbeddedStruct(const EmbeddedStruct& other0) {
  es_i8 = other0.es_i8;
  es_i16 = other0.es_i16;
  __isset = other0.__isset;
}
EmbeddedStruct& EmbeddedStruct::operator=(const EmbeddedStruct& other1) {
  es_i8 = other1.es_i8;
  es_i16 = other1.es_i16;
  __isset = other1.__isset;
  return *this;
}
void EmbeddedStruct::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "EmbeddedStruct(";
  out << "es_i8=" << to_string(es_i8);
  out << ", " << "es_i16=" << to_string(es_i16);
  out << ")";
}


ResponseData::~ResponseData() throw() {
}


void ResponseData::__set_pet_id(const int32_t val) {
  this->pet_id = val;
}

void ResponseData::__set_pet_name(const std::string& val) {
  this->pet_name = val;
}

void ResponseData::__set_pet_weight(const int32_t val) {
  this->pet_weight = val;
}

void ResponseData::__set_pet_i8_value(const int8_t val) {
  this->pet_i8_value = val;
}

void ResponseData::__set_pet_i16_value(const int16_t val) {
  this->pet_i16_value = val;
}

void ResponseData::__set_pet_i64_value(const int64_t val) {
  this->pet_i64_value = val;
}

void ResponseData::__set_pet_double_value(const double val) {
  this->pet_double_value = val;
}

void ResponseData::__set_pet_bool_value(const bool val) {
  this->pet_bool_value = val;
}

void ResponseData::__set_pet_binary_value(const std::string& val) {
  this->pet_binary_value = val;
}

void ResponseData::__set_pet_embedded_struct(const EmbeddedStruct& val) {
  this->pet_embedded_struct = val;
}

uint32_t ResponseData::read(::apache::thrift::protocol::TProtocol* iprot) {

  apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->pet_id);
          this->__isset.pet_id = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->pet_name);
          this->__isset.pet_name = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->pet_weight);
          this->__isset.pet_weight = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_BYTE) {
          xfer += iprot->readByte(this->pet_i8_value);
          this->__isset.pet_i8_value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_I16) {
          xfer += iprot->readI16(this->pet_i16_value);
          this->__isset.pet_i16_value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 6:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->pet_i64_value);
          this->__isset.pet_i64_value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 7:
        if (ftype == ::apache::thrift::protocol::T_DOUBLE) {
          xfer += iprot->readDouble(this->pet_double_value);
          this->__isset.pet_double_value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 8:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->pet_bool_value);
          this->__isset.pet_bool_value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 9:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readBinary(this->pet_binary_value);
          this->__isset.pet_binary_value = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 10:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->pet_embedded_struct.read(iprot);
          this->__isset.pet_embedded_struct = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t ResponseData::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("ResponseData");

  xfer += oprot->writeFieldBegin("pet_id", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->pet_id);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_name", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->pet_name);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_weight", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32(this->pet_weight);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_i8_value", ::apache::thrift::protocol::T_BYTE, 4);
  xfer += oprot->writeByte(this->pet_i8_value);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_i16_value", ::apache::thrift::protocol::T_I16, 5);
  xfer += oprot->writeI16(this->pet_i16_value);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_i64_value", ::apache::thrift::protocol::T_I64, 6);
  xfer += oprot->writeI64(this->pet_i64_value);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_double_value", ::apache::thrift::protocol::T_DOUBLE, 7);
  xfer += oprot->writeDouble(this->pet_double_value);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_bool_value", ::apache::thrift::protocol::T_BOOL, 8);
  xfer += oprot->writeBool(this->pet_bool_value);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_binary_value", ::apache::thrift::protocol::T_STRING, 9);
  xfer += oprot->writeBinary(this->pet_binary_value);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("pet_embedded_struct", ::apache::thrift::protocol::T_STRUCT, 10);
  xfer += this->pet_embedded_struct.write(oprot);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(ResponseData &a, ResponseData &b) {
  using ::std::swap;
  swap(a.pet_id, b.pet_id);
  swap(a.pet_name, b.pet_name);
  swap(a.pet_weight, b.pet_weight);
  swap(a.pet_i8_value, b.pet_i8_value);
  swap(a.pet_i16_value, b.pet_i16_value);
  swap(a.pet_i64_value, b.pet_i64_value);
  swap(a.pet_double_value, b.pet_double_value);
  swap(a.pet_bool_value, b.pet_bool_value);
  swap(a.pet_binary_value, b.pet_binary_value);
  swap(a.pet_embedded_struct, b.pet_embedded_struct);
  swap(a.__isset, b.__isset);
}

ResponseData::ResponseData(const ResponseData& other2) {
  pet_id = other2.pet_id;
  pet_name = other2.pet_name;
  pet_weight = other2.pet_weight;
  pet_i8_value = other2.pet_i8_value;
  pet_i16_value = other2.pet_i16_value;
  pet_i64_value = other2.pet_i64_value;
  pet_double_value = other2.pet_double_value;
  pet_bool_value = other2.pet_bool_value;
  pet_binary_value = other2.pet_binary_value;
  pet_embedded_struct = other2.pet_embedded_struct;
  __isset = other2.__isset;
}
ResponseData& ResponseData::operator=(const ResponseData& other3) {
  pet_id = other3.pet_id;
  pet_name = other3.pet_name;
  pet_weight = other3.pet_weight;
  pet_i8_value = other3.pet_i8_value;
  pet_i16_value = other3.pet_i16_value;
  pet_i64_value = other3.pet_i64_value;
  pet_double_value = other3.pet_double_value;
  pet_bool_value = other3.pet_bool_value;
  pet_binary_value = other3.pet_binary_value;
  pet_embedded_struct = other3.pet_embedded_struct;
  __isset = other3.__isset;
  return *this;
}
void ResponseData::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "ResponseData(";
  out << "pet_id=" << to_string(pet_id);
  out << ", " << "pet_name=" << to_string(pet_name);
  out << ", " << "pet_weight=" << to_string(pet_weight);
  out << ", " << "pet_i8_value=" << to_string(pet_i8_value);
  out << ", " << "pet_i16_value=" << to_string(pet_i16_value);
  out << ", " << "pet_i64_value=" << to_string(pet_i64_value);
  out << ", " << "pet_double_value=" << to_string(pet_double_value);
  out << ", " << "pet_bool_value=" << to_string(pet_bool_value);
  out << ", " << "pet_binary_value=" << to_string(pet_binary_value);
  out << ", " << "pet_embedded_struct=" << to_string(pet_embedded_struct);
  out << ")";
}

}}}} // namespace
