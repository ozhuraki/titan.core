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
package org.eclipse.titan.executorapi.demo;

import java.io.File;

public class CommonData {

    // default values
	private static final String WORKSPACE = "../../titan/Install/";
	private static final String CFG_FILE = "demo/MyExample.cfg";
	static final String LOCALHOST = "NULL";
	private static final String WORKINGDIR = "demo";
	static final String EXECUTABLE = "MyExample";
	static final String MODULE = EXECUTABLE;
	static final String TESTCASE = "HelloW2";

	private static String getDefaultWorkspaceDir() {
		String ttcn3_dir = System.getenv().get("TTCN3_DIR");
		String workspace = WORKSPACE;
	    if (ttcn3_dir != null && ttcn3_dir.length() > 0) {
	    	workspace = ttcn3_dir;
	    }
	    workspace += ( workspace.endsWith( File.separator ) ? "" : File.separator );
    	return workspace;
	}
	
	public static String getDefaultWorkingDir() {
		return getDefaultWorkspaceDir() + WORKINGDIR;
	}
	
	public static String getDefaultCfgFile() {
		return getDefaultWorkspaceDir() + CFG_FILE;
	}


}
