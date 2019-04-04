/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Szabo, Bence Janos
 *
 ******************************************************************************/

#include "openssl_version.h"
#include <openssl/crypto.h>


// Backward compatibility with older openssl versions
#if OPENSSL_VERSION_NUMBER < 0x10100000L

const char * openssl_version_str(void) {
	return SSLeay_version(SSLEAY_VERSION);
}

#else

const char * openssl_version_str(void) {
	return OpenSSL_version(OPENSSL_VERSION);
}

#endif
