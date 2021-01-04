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
package org.eclipse.titan.executor.jni;

/**
 * Structure for storing global identifiers of TTCN-3 definitions.
 * <p>
 * The original C++ structure can be found at TTCNv3\core\Types.h
 */
public final class QualifiedName {
	public String module_name;
	public String definition_name;

	public QualifiedName(final String module_name, final String definition_name) {
		this.module_name = module_name;
		this.definition_name = definition_name;
	}
	
	@Override
	public String toString() {
		return 	"{ " + module_name + ", " + definition_name + " }";
	}
}
