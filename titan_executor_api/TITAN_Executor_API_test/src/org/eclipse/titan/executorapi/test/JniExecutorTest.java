/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
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
package org.eclipse.titan.executorapi.test;

import org.junit.Rule;
import org.junit.rules.Timeout;

/**
 * Base test class of all the classes, that tests JniExecutor
 * @see org.eclipse.titan.executorapi.JniExecutor 
 */
public abstract class JniExecutorTest {

	/** The time in milliseconds max per method tested */
	@Rule
	public Timeout globalTimeout = new Timeout(60000);
}
