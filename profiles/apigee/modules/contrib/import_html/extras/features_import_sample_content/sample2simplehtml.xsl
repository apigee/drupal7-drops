<?xml version="1.0"?>

<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
> 
  <xsl:output method="xml" encoding="UTF-8" />
  <!-- 
    Analyse a totally simple raw web page and return a vanilla-as-possible
    pure XHTML, sematically-tagged version.
    
    This is a minimal example, intended as a baseline demo.
    Build your own special cases from this if the catch-all html2simplehtml.xsl logic is too hard to follow.

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
    Returns the H1, or if unavailable, the <title>
    -->
    <xsl:choose>
      <xsl:when test="//xhtml:h1">
        <xsl:value-of select="normalize-space(//xhtml:h1)" />
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
    <xsl:apply-templates select="xhtml:body/*|xhtml:body/text()"/>
  </xsl:template>
  
  
  <xsl:template match="xhtml:h1">
    <!-- 
    Remove the found H1 from the flow altogether.
    -->
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