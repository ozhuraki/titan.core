###############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Raduly, Csaba
#
###############################################################################
#! /usr/bin/perl -w
use strict;

my $is_enum = 0;

print <<HEAD;
#ifndef TITAN_LOGGER_API_SIMPLE_HH
#define TITAN_LOGGER_API_SIMPLE_HH
namespace TitanLoggerApiSimple {

HEAD

while (<>) {
  if ($is_enum) { # already found the start of an enum
    if (/enum\s*enum_type\s*{\s*([^}]+)};/) {
      print $_; # copy enum_type
    }
    elsif (/^};$/) {
      print $_; # copy the closing brace
      $is_enum = 0;
    }
  }
  else {
    if (my ($class, $base) = /class\s*(\w+)\s*:\s*public\s*(\w+)\s*{\s*\/\/\s*enum/) {
      print "\nclass $class {\npublic:\n";
      $is_enum = 1;
    }
  }
}

print <<TAIL;
} // end namespace
#endif
TAIL
