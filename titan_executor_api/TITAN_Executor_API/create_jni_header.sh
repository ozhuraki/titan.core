###############################################################################
# Copyright (c) 2000-2021 Ericsson Telecom AB
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
#
# Contributors:
#   Balasko, Jeno
#   Lovassy, Arpad
#
###############################################################################

# Creates C++ header from the Java code for Java to C++ JNI function calls.
# This must run if any of the native Java methods change (in org.eclipse.titan.executor.jni.JNIMiddleWare).
# A C++ function header is generated for each native Java method.
# Native Java method: declared with native keyword without implementation
# This script generates these files into ../../JNI/ directory:
#   org_eclipse_titan_executor_jni_JNIMiddleWare_EventDispatcher.h (this is empty, it can be deleted after creation)
#   org_eclipse_titan_executor_jni_JNIMiddleWare.h
#
# NOTE: Copyright header is not added to the generated file, it must be added manually.

javah -jni -classpath bin -d ../../JNI/ org.eclipse.titan.executor.jni.JNIMiddleWare
