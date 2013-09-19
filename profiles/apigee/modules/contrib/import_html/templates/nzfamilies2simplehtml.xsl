<?xml version="1.0"?>

<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
> 
  <xsl:output method="xml" encoding="UTF-8" />
  <!-- 
    Template tuned to wrap existing legacy pages from 
    nzfamilies.org 2008.
    
    Part of the site is Drupal 4.7.
    Part is more static html/php, so at least two unique templates are supported.
    
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
      <xsl:apply-templates select="//*[@class='sidebar']" />
      <xsl:apply-templates select="//*[@id='sidebarRight']" />
    </body>

    </html>
  </xsl:template>

  <xsl:template name="get-title">
    <!-- 
    The page heading is H1, or the first H2 found inside the #center div.
    -->
    <xsl:choose>
      <xsl:when test="//xhtml:h1">
        <xsl:value-of select="normalize-space(//xhtml:h1)" />
      </xsl:when>
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
    The content we care about for Garland nodes is [div#center div.content] 
    Although other pages have variations.
    Take whichever one we find
    -->
    <xsl:apply-templates select="//xhtml:div[@id='center']//xhtml:div[@class='content']/*|//xhtml:div[@id='center']//xhtml:div[@class='content']/text()"/>
    <xsl:apply-templates select="//xhtml:div[@id='contentbody']/*|//xhtml:div[@id='contentbody']/text()"/>
    <!-- 
    Some other pages have 'mainContent' as the key body element.
    Although that div inappropriately includes the breadcrumbs also. 
     -->
    <xsl:apply-templates select="//xhtml:div[@id='mainContent']/*|//xhtml:div[@id='mainContent']/text()"/>
    <!--  another type of page -->
    <xsl:apply-templates select="//xhtml:div[@class='view view-media-releases']/*|//xhtml:div[@class='view view-media-releases']/text()"/>
    <xsl:apply-templates select="//xhtml:div[@class='view view-speeches']/*|//xhtml:div[@class='view view-speeches']/text()"/>

  </xsl:template>

  <xsl:template match="*[@class='sidebar']">
    <div class="sidebar">
      <xsl:apply-templates select="*" />
    </div>
  </xsl:template>

  <xsl:template match="*[@id='sidebarRight']">
	  <div class="sidebar">
  	  <xsl:apply-templates select="*" />
	  </div>
  </xsl:template>
  

  <xsl:template match="xhtml:h1">
  <!-- remove these from the flow altogether -->
  </xsl:template>

  <!-- Adjustments specifically for the old HTML -->
  
  <xsl:template match="xhtml:div[@class = 'breadcrumb']">
  <!-- remove these from the flow altogether -->
  </xsl:template>
  
  <xsl:template match="xhtml:div[@class = 'clearfloat']">
  <!-- remove these from the flow altogether -->
  </xsl:template>

  <xsl:template match="xhtml:a[@href = '#top']">
  <!-- remove these from the flow altogether -->
  </xsl:template>
  <xsl:template match="xhtml:div[@class = 'top']">
  <!-- remove these from the flow altogether -->
  </xsl:template>
  
  <xsl:template match="xhtml:ul[@class = 'secondary-links']">
  <!-- This messes up the css. remove the class attribute -->
    <xsl:copy>
    <xsl:attribute name="class">x-secondary-links</xsl:attribute>
    <xsl:apply-templates select="node()|text()"/>
    </xsl:copy>
  </xsl:template>
  
  
  <xsl:template name="passthrough" match="@*|node()|text()">
    <!-- 
    Identity transformation template - to inline existing HTML  
    -->      
    <xsl:copy>
    <xsl:apply-templates select="@*|node()|text()"/>
    <!-- Fatten all empty tags (anorexic divs are a failure) but not known singletons -->
    <xsl:if test="not (node()|text()) and not(local-name()='img') and not(local-name()='br') and not(local-name()='hr') "><xsl:text> </xsl:text></xsl:if>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>