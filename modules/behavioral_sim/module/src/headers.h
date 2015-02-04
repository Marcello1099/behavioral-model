#ifndef _BM_HEADERS_H_
#define _BM_HEADERS_H_

#include <vector>

#include "fields.h"

typedef int header_type_id_t;

class HeaderType
{
private:
  header_type_id_t type_id;
  std::vector<int> fields_bit_width;

public:
  HeaderType(header_type_id_t type_id)
    : type_id(type_id) {}

  void add_field(int bit_width) {
    fields_bit_width.push_back(bit_width);
  }

  int get_bit_width(int field_offset) const {
    return fields_bit_width[field_offset];
  }

  header_type_id_t get_type_id() const {
    return type_id;
  }

  int get_num_fields() const {
    return fields_bit_width.size();
  }
};

class Header
{
private:
  header_type_id_t header_type_id;
  std::vector<Field> fields;
  bool valid;
  int nbytes_phv;
  int nbytes_packet;

public:
 Header() : valid(false), nbytes_phv(0), nbytes_packet(0) {}

  Header(const HeaderType &header_type)
  {
    valid = false;
    nbytes_phv = 0;
    nbytes_packet = 0;
    header_type_id = header_type.get_type_id();
    for(int i = 0; i < header_type.get_num_fields(); i++) {
      fields.push_back(Field(header_type.get_bit_width(i)));
      nbytes_phv += fields.back().get_nbytes();
      nbytes_packet += fields.back().get_nbits();
    }
    assert(nbytes_packet % 8 == 0);
    nbytes_packet /= 8;
  }

  ~Header() {

  }

  int get_nbytes_packet() const {
    return nbytes_packet;
  }

  bool is_valid() const{
    return valid;
  }

  void mark_valid() {
    valid = true;
  }

  void mark_invalid() {
    valid = false;
  }

  Field &get_field(int field_offset) {
    return fields[field_offset];
  }

  const Field &get_field(int field_offset) const {
    return fields[field_offset];
  }

  Header& operator=(const Header &other) {
    if(&other == this)
      return *this;
    assert(NULL);
    // assert(header_type_id == other.header_type_id);
    // for(unsigned field_index = 0; field_index < fields.size(); field_index++) {
    //   fields[field_index] = other.fields[field_index];
    // }
    header_type_id = other.header_type_id;
    fields = other.fields;
    valid = other.valid;
    return *this;
  }

  void extract(const char *data) {
    // TODO
    int hdr_offset = 0;
    for(std::vector<Field>::iterator it = fields.begin();
	it != fields.end();
	++it) {
      hdr_offset += it->extract(data, hdr_offset);
      data += hdr_offset / 8;
      hdr_offset = hdr_offset % 8;
    }
    mark_valid();
    return;
  }
};

#endif