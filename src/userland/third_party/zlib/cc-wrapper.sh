#!/bin/sh
exec x86_64-elf-gcc \
  -U__INT_FAST8_TYPE__ \
  -D__INT_FAST8_TYPE__="signed char" \
  -U__UINT_FAST8_TYPE__ \
  -D__UINT_FAST8_TYPE__="unsigned char" \
  -U__INT_FAST16_TYPE__ \
  -D__INT_FAST16_TYPE__="long int" \
  -U__UINT_FAST16_TYPE__ \
  -D__UINT_FAST16_TYPE__="long unsigned int" \
  -U__INT_FAST32_TYPE__ \
  -D__INT_FAST32_TYPE__="long int" \
  -U__UINT_FAST32_TYPE__ \
  -D__UINT_FAST32_TYPE__="long unsigned int" \
  "$@"
