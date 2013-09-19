<?xml version="1.0"?>

<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
> 
  <xsl:output method="xml" encoding="UTF-8" />
  <!-- 
    This is a sample template to illustrate how semantically tagged and valid
    input can be rolled back into a format suitable for import.
    
    It is minimal and specifically tuned to import the result of a 'Garland' themed page.
    We know that the content is in a div with class=content.
    That, and the title is all we care about for this example.
    
    The xsl:text nodes are just scattered around to force some pretty-printing on the output
    
    When strict namespaced XHTML is the input, 
    we must continue to use the xhtml:prefix 
    in selectors when looking for it.
    select="body" will not work, use select="xhtml:body".
    
    .dan.
  -->

  <xsl:template name="html_doc" match="/*">  
    <!-- 
    Main page layout.
    Head and body are the immediate children.
    -->
    <html>
    <head>
      <!-- META And Header extraction -->
    
      <title><xsl:call-template name="get-title" /></title>
  
      <xsl:text>
      </xsl:text>
    
      <xsl:for-each select=".//head">
        <xsl:for-each select="xhtml:meta[@name]|xhtml:rel">
          <!-- copy most of the head - but NOT metas with http-equiv -->
          <xsl:copy><xsl:for-each select="@*"><xsl:copy></xsl:copy></xsl:for-each></xsl:copy>
        </xsl:for-each>
        <xsl:text>
        </xsl:text>
      </xsl:for-each>
    
    </head>
    <xsl:text>
    </xsl:text>

    <body>
      <xsl:text>
      </xsl:text>
      <h1 id="pagetitle" xml:id="pagetitle"><xsl:call-template name="get-title" /></h1>
      <xsl:text>
      </xsl:text>
      <div id="content" xml:id="content"><xsl:call-template name="get-content" ></xsl:call-template >
      </div>
    </body>

    </html>
  </xsl:template>

  <xsl:template name="get-title">
    <!-- 
    For our Garland example, the page heading is the first H2 found inside the #center div.
    -->
    <xsl:choose>
      <xsl:when test="//xhtml:div[@id='center']//xhtml:h2">
        <xsl:value-of select="normalize-space(//xhtml:div[@id='center']//xhtml:h2)" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="normalize-space(//xhtml:title)" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="get-content">
    <!-- 
    The content we care about for Garland nodes is [div#center div.content] -->
    <xsl:apply-templates select="//xhtml:div[@id='center']//xhtml:div[@class='content']/*|//xhtml:div[@id='center']//xhtml:div[@class='content']/text()"/>
  </xsl:template>
  
  <xsl:template name="passthrough" match="@*|node()|text()">
    <!-- 
    Identity transformation template - to inline existing HTML  
    -->      
    <xsl:copy>
    <xsl:apply-templates select="@*|node()|text()"/>
    <xsl:if test="not (node()|text())"><xsl:text> </xsl:text></xsl:if>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>