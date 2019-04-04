/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Lovassy, Arpad
 *
 ******************************************************************************/
package org.eclipse.titan.executor.jni;

/**
 * For representing the global state of MC.
 * <p>
 * The original C++ structure can be found at TTCNv3\mctr2\mctr\MainController.h
 */
public enum McStateEnum {

	MC_INACTIVE,              //  0
	MC_LISTENING,             //  1
	MC_LISTENING_CONFIGURED,  //  2
	MC_HC_CONNECTED,          //  3
	MC_CONFIGURING,           //  4

	MC_ACTIVE,                //  5
	MC_SHUTDOWN,              //  6
	MC_CREATING_MTC,          //  7
	MC_READY,                 //  8
	MC_TERMINATING_MTC,       //  9

	MC_EXECUTING_CONTROL,     // 10
	MC_EXECUTING_TESTCASE,    // 11
	MC_TERMINATING_TESTCASE,  // 12
	MC_PAUSED;                // 13
}
