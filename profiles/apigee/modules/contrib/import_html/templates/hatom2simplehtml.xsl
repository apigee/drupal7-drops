<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml" xmlns:xhtml="http://www.w3.org/1999/xhtml" xmlns:exsl="http://exslt.org/common">
	<xsl:output method="xml" encoding="UTF-8" />
	<!-- 
		Most of the classnames described in the hAtom microformat
		http://microformats.org/wiki/hatom
		can be transferred directly to Drupal semantics.
		
		Trying this transformation on examples from 
		http://microformats.org/wiki/hatom-examples-in-wild
		Should be pretty good.
		
		The xsl:text nodes are just scattered around to force some pretty-printing on the output
		
		When strict namespaced XHTML is the input, 
		we must continue to use the xhtml:prefix 
		in selectors when looking for it.
		select="body" will not work, use select="xhtml:body".
		
		.dan.
	-->

	<!-- an HTML input may contain multiple hentries -->
	<xsl:template match="/">
		<!-- 
			This document contains sub documents. Process them individually.
		-->
		<exsl:documents>
			<xsl:for-each select="descendant::*[contains(@class,'hentry')]">
        <xsl:text>
        </xsl:text>

				<exsl:document>
					<xsl:copy-of select="@*" />
					<xsl:call-template name="html_doc" />
				</exsl:document>
        <xsl:text>
        </xsl:text>


			</xsl:for-each>
		</exsl:documents>
	</xsl:template>


	<xsl:template name="html_doc" match="*[contains(@class,'hentry')]">
		<!-- 
			Main page layout.
			Head and body are the immediate children.
		-->
		<html>
			<head>
        <xsl:text>
        </xsl:text>

				<!-- META And Header extraction -->
				<title>
					<xsl:call-template name="get-title" />
				</title>

        <xsl:text>
        </xsl:text>

				<!-- document headers get copied into each hentry -->
				<xsl:for-each select=".//head">
					<xsl:for-each select="xhtml:meta[@name]|xhtml:rel">
						<!-- copy most of the head - but NOT metas with http-equiv -->
						<xsl:copy>
							<xsl:for-each select="@*">
								<xsl:copy />
							</xsl:for-each>
						</xsl:copy>
					</xsl:for-each>
		       <xsl:text>
		       </xsl:text>

				</xsl:for-each>

				<!-- import post meta here -->
				<meta name="author">
					<xsl:attribute name="content"><xsl:value-of select="normalize-space(descendant::*[contains(@class,'author')])" />
					</xsl:attribute>
				</meta>
        <xsl:text>
        </xsl:text>

				<meta name="created">
					<xsl:attribute name="content"><xsl:value-of select="normalize-space(descendant::*[contains(@class,'published')])" />
					</xsl:attribute>
				</meta>
        <xsl:text>
        </xsl:text>

				<meta name="description">
					<xsl:attribute name="content"><xsl:value-of select="normalize-space(descendant::*[contains(@class,'entry-summary')])" />
					</xsl:attribute>
				</meta>

			</head>
        <xsl:text>
        </xsl:text>


			<body>
        <xsl:text>
        </xsl:text>

				<h1 id="pagetitle" xml:id="pagetitle">
					<xsl:call-template name="get-title" />
				</h1>
        <xsl:text>
        </xsl:text>

				<div id="content" xml:id="content">
					<xsl:call-template name="get-content" />
				</div>
			</body>

		</html>
	</xsl:template>



	<xsl:template name="get-title">
		<!-- 
			Returns the H1, entry-title, or if unavailable, the <title>
		-->
		<xsl:choose>
			<xsl:when test="descendant::*[contains(@class,'entry-title')]">
				<xsl:value-of select="normalize-space(descendant::*[contains(@class,'entry-title')])" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="normalize-space(//xhtml:title)" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template name="get-content">
		<!-- 
			By using apply-templates, the h1 can be discarded when it's found 
		-->
		<!-- According to the microformat spec, 
			the entry content is a concatenation of all entry-content items.
			Also the for-each is used to strip the outer tag off.
		-->
    <xsl:choose>
      <xsl:when test="descendant::*[contains(@class,'entry-content')]">
        <xsl:for-each select="descendant::*[contains(@class,'entry-content')]">
          <xsl:apply-templates />
            <xsl:text>
            </xsl:text>
        </xsl:for-each>
      </xsl:when>
      <!-- If no entry-content, then use the entry-summary, even though that's probably not enough, it's what we've got -->
      <xsl:when test="descendant::*[contains(@class,'entry-summary')]">
        <xsl:for-each select="descendant::*[contains(@class,'entry-summary')]">
          <xsl:apply-templates />
            <xsl:text>
            </xsl:text>
        </xsl:for-each>
      </xsl:when>
		</xsl:choose>

	</xsl:template>



	<xsl:template name="passthrough" match="@*|node()|text()">
		<!-- 
			Identity transformation template - to inline existing HTML  
		-->
		<xsl:copy>
			<xsl:apply-templates select="@*|node()|text()" />
			<xsl:if test="not (node()|text())">
        <xsl:text>
        </xsl:text>
			</xsl:if>
		</xsl:copy>
	</xsl:template>

</xsl:stylesheet>