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
    
    It is minimal and specifically tuned to import the result of the famous csszengarden.com template.
    
    Just as zengarden is a demonstration, not a real-world scenario, so is this a demonstration on xsl template semantic data extraction.
    
    The zengarden content is grouped badly, so we glue it together again into a body content block.
    Sidebars are made available as a 'sidebar' field which MAY be supported in your theme/content-type or not.
    
    To really make it work, you could add a cck field called 'field_css'
    This will provide a per-page holder for the zengardens unique css files and add it to the page.
    - results are a horrid mash-up of the current theme and the styled theme, but it shows the bits coming together.
    
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
      <div id="content" xml:id="content">
        <xsl:call-template name="get-content" />
      </div>
      <xsl:call-template name="get-sidebar" />
      <xsl:call-template name="get-css" />
    </body>

    </html>
  </xsl:template>

  <xsl:template name="get-title">
    <!-- 
    For our zengarden example, the page heading is, correctly, just the H1
    -->
    <xsl:value-of select="normalize-space(//xhtml:h1)" />
  </xsl:template>

  <xsl:template name="get-content">
    <!-- 
    ZenGarden content is all over the place. Grab what we think we want.
    -->
    <xsl:apply-templates select="//xhtml:div[@id='quickSummary']"/>
    <xsl:apply-templates select="//xhtml:div[@id='preamble']"/>
    <xsl:apply-templates select="//xhtml:div[@id='supportingText']"/>
  </xsl:template>

  <xsl:template name="get-sidebar">
    <div class="sidebar">
      <xsl:apply-templates select="//xhtml:div[@id='linkList']"/>
    </div>
  </xsl:template>

  
  <xsl:template name="get-css">
    <xsl:for-each select="//xhtml:style">
      <!--  
      To get a raw element to show up as a CCK, we wrap it in a named div, as cck imports usually just import the div CONTENT 
      This is a bit backwards for Style tags, but I'm not sure of a great way to flag this special case.
      -->
      <div class="field_css">
		    <xsl:copy select=".">
		      <xsl:apply-templates select="@*|node()|text()"/>
		    </xsl:copy>
	    </div>
    </xsl:for-each>
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