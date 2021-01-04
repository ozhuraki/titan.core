/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   
 *   Szabo, Bence Janos
 *
 ******************************************************************************/
#include "OER.hh"
#include "Encdec.hh"
#include "BER.hh"
#include "../common/memory.h"
#include "Error.hh"


void encode_oer_length(size_t num_bytes, TTCN_Buffer& buf, boolean seof) {
  if (num_bytes < 128 && seof == FALSE) {
    buf.put_c(num_bytes);
  } else {
    size_t bytes = num_bytes;
    // Encode length in maybe more than 1 byte
    size_t needed_bytes = 0;
    do {
      bytes >>= 8;
      needed_bytes++;
    }
    while (bytes != 0);
    char c = 0;
    if (seof == FALSE) {
      c |= 1 << 7;
    }
    c+= needed_bytes;
    buf.put_c(c);
    for (int i = needed_bytes - 1; i >= 0; i--) {
      buf.put_c(static_cast<unsigned char>(num_bytes >> i*8));
    }
  }
}

size_t decode_oer_length(TTCN_Buffer& buf, boolean seof) {
  const unsigned char* uc = buf.get_read_data();
  buf.increase_pos(1);
  
  size_t num_bytes = 0;
  if (uc[0] & 0x80 || seof == TRUE) {
    size_t bytes = uc[0] & (seof == FALSE ? 0x7F : 0xFF);
    for (size_t i = 1; i < bytes+1; i++) {
      num_bytes += uc[i] << (bytes-i)*8;
    }
    buf.increase_pos(bytes);
  } else {
    // its length is encoded in the last 7 bytes
    num_bytes = *uc & 0x7F;
  }
  return num_bytes;
}

void encode_oer_tag(const ASN_BERdescriptor_t& descr, TTCN_Buffer& buf) {
  char c;
  switch(descr.tags[descr.n_tags-1].tagclass) {
    case ASN_TAG_UNIV:
      c = 0;
      break;
    case ASN_TAG_APPL:
      c = 1 << 6;
      break;
    case ASN_TAG_CONT:
      c = 1 << 7;
      break;
    case ASN_TAG_PRIV:
      c = 3 << 6;
      break;
    default:
      TTCN_error("Incorrect tagclass while encoding OER tag.");
  }
  
  unsigned int tagnum = descr.tags[descr.n_tags-1].tagnumber;
  if (tagnum < 63) {
    c += tagnum;
    buf.put_c(c);
  } else {
    c += 63; // set every bit to 1 from 6 to 1 bit
    buf.put_c(c);
    int first_bit_pos = 8*sizeof(unsigned int) - 1;
    for (; first_bit_pos != 0; first_bit_pos--) {
      if ((tagnum >> first_bit_pos) & 1) {
        break;
      }
    }
    size_t needed_bytes = first_bit_pos / 7 + 1;// todo test 0
    unsigned char * uc = (unsigned char*)Malloc(needed_bytes * sizeof(unsigned char));
    size_t buf_pos = 0;
    uc[buf_pos] = 0;
    int pos = first_bit_pos % 7;
    while (first_bit_pos > -1) {

        uc[buf_pos] += ((tagnum >> first_bit_pos) & 1) << (pos);
        pos--;
      if (pos == -1) {
        pos = 6;
        if (buf_pos != needed_bytes - 1) {
          uc[buf_pos] |= 1 << 7;
        }
        buf_pos++;
        if (buf_pos != needed_bytes) {
          uc[buf_pos] = 0;
        }
      }
      first_bit_pos--;
    }
    buf.put_s(needed_bytes, uc);
    Free(uc);
  }
}

ASN_Tag_t decode_oer_tag(TTCN_Buffer& buf) {
  ASN_Tag_t tag;
  const unsigned char * uc = buf.get_read_data();
  int tagclass = (uc[0] & 0xC0) >> 6;
  switch (tagclass) {
    case 0:
      tag.tagclass = ASN_TAG_UNIV;
      break;
    case 1:
      tag.tagclass = ASN_TAG_APPL;
      break;
    case 2:
      tag.tagclass = ASN_TAG_CONT;
      break;
    case 3:
      tag.tagclass = ASN_TAG_PRIV;
      break;
    default:
      tag.tagclass = ASN_TAG_UNDEF;
      break;
  }

  buf.increase_pos(1);
  if ((uc[0] & 0x3F) != 0x3F) {
    // Encoded on 6 bits
    tag.tagnumber = (uc[0] & 0x3F);
  } else {
    size_t tagnumber = 0;
    // Encoded on multiple bytes
    size_t buf_pos = 1;
    // If more bytes to follow
    while (uc[buf_pos] & 0x80) {
      tagnumber += uc[buf_pos] & 0x7F;
      tagnumber <<= 7;
      buf_pos++;
    }
    tagnumber += uc[buf_pos];
    tag.tagnumber = tagnumber;
    buf.increase_pos(buf_pos);
  }
  return tag;
}

const TTCN_OERdescriptor_t BOOLEAN_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t INTEGER_oer_ = { -1, TRUE, 0, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t BITSTRING_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t OCTETSTRING_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL , 0, NULL};

const TTCN_OERdescriptor_t FLOAT_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t ASN_NULL_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t IA5String_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t VisibleString_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t NumericString_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t PrintableString_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t BMPString_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t UniversalString_oer_ = { 0, TRUE, -1, FALSE , 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t UTF8String_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t TeletexString_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL , 0, NULL};

const TTCN_OERdescriptor_t VideotexString_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t GraphicString_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t GeneralString_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t OBJID_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t ASN_ROID_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t EMBEDDED_PDV_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t EXTERNAL_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };

const TTCN_OERdescriptor_t ObjectDescriptor_oer_ = { 0, TRUE, -1, FALSE, 0, 0, NULL, 0, NULL };
