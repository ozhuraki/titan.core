<?xml version="1.0" encoding="ISO-8859-1"?>
<!--
 Copyright (c) 2000-2021 Ericsson Telecom AB
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html

 Contributors:
  Balasko, Jeno
-->
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ttcn3="http://ttcn.ericsson.se/TitanSim/Perftest">
	<xsl:template match="/">
		<html>
			<head>
				<link href="testresults.css" rel="stylesheet" type="text/css" />
				<title>
					Test Results of
					<xsl:value-of select="//ttcn3:testresult/ttcn3:name" />
				</title>
			</head>
			<body>
				<xsl:for-each select="//ttcn3:testresult">
				<h2>
					Test Results of
					<xsl:value-of select="./ttcn3:name" />
				</h2>
				<h3>
					<xsl:value-of select="./ttcn3:purpose" />
				</h3>
				<table class="ExecutionTime">
					<tr>
						<td>Start time:</td>
						<td>
							<xsl:value-of select="./ttcn3:start_time" />
						</td>
					</tr>
					<tr>
						<td>End time:</td>
						<td>
							<xsl:value-of select="./ttcn3:end_time" />
						</td>
					</tr>
				</table>
				<div class="EnvironmentTable">
				<p class="tableTitle">Environment settings</p>
				<table>
					<tr class="Header">
						<th>Environment setting</th>
						<th>Value</th>
					</tr>
					<xsl:for-each select="./ttcn3:environment/*">
						<tr class="EnvItems">
							<td>
								<xsl:value-of select="local-name()" />
							</td>
							<td>
								<xsl:value-of select="." />
							</td>
						</tr>
					</xsl:for-each>
				</table>
				</div>
				<div class="ParamTable">
				<p class="tableTitle">Parameters</p>
				<table>
					<tr class="Header">
						<th>Parameter</th>
						<th>Value [Unit]</th>
					</tr>
					<xsl:for-each select="./ttcn3:parameter">
						<tr class="Params">
							<td>
								<xsl:value-of select="./@name" />
							</td>
							<td>
								<xsl:value-of select="." /> [<xsl:value-of select="./@unit" />]
							</td>
						</tr>
					</xsl:for-each>
				</table>
				</div>
				<div class="ResultTable">
				<p class="tableTitle">Test Results</p>
				<table>
					<tr class="Header">
						<th>Result</th>
						<th>Value [Unit]</th>
					</tr>
					<xsl:for-each select="./ttcn3:result">
						<tr class="ResItems">
							<td>
								<xsl:value-of select="./@name" />
							</td>
							<td>
								<xsl:value-of select="." /> [<xsl:value-of select="./@unit" />]
							</td>
						</tr>
					</xsl:for-each>
				</table>
				</div>
                                </xsl:for-each>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
