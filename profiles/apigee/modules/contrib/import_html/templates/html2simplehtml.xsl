<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:exsl="http://exslt.org/common"
> 
<!--  xmlns="http://www.w3.org/1999/xhtml" causes bogus namespaces on imported elements -->

  <xsl:output method="xml" encoding="UTF-8" />
  <!-- 
    Analyse a generic web page and return a vanilla-as-possible
    pure XHTML version.

    Although this is supposed to be 'simple' it will 
    also support multiple documents in one file, if the input
    has them in a 'exsl:document' wrapper.
    
    This doc is more complex than is needed for any one site 
    as it contains many possible cases in one.
    - see simplehtml2html as a starting point to build your own, 
    then refer to back to this one for XSL tricks if needed.
    

    To find the nodes by local name without namespaces handicapping me,
    Input html must either have no namespace ( select="html" )
    or be in the xhtml namespace ( select="xhtml:html" ).
    Use these two patterns at all times when matching nodes by name :-(

    Another work-around is ( select="*[local-name()='html']" )

    ... This may be avoided slightly by adjusting the input namespace 
    during pre-processing ... although the syntax will still require 
    the prefix - a doc with any namespace requires the prefix to
    be used from then on.

    The xsl:text nodes are just scattered around to force some pretty-printing on the output
    
    Note the use of xml:id for items we intend to find later.
    This IS significant and important for performance.

    .dan.
  -->

  <!-- 
    Caller can define the id of the 'content' block to be extracted 
  -->
  <xsl:param name='contentid'>bodyCopy</xsl:param>

  <!--
    If $xmlid is set, this document will insert xml:id along with normal html:id
    THESE ARE DIFFERENT THINGS. 
    xml:id is much better for the DOM,
    html:id is just another attribute.
    http://php5.bitflux.org/xmlonspeed/slide_20.php
    Normally these are good, but need to be turned off if doing multiple 
    documents as duplicate ids are errors.
  -->
  <xsl:param name="xmlid"></xsl:param>
  
  <xsl:template match="/*[descendant::exsl:document]">
    <!-- 
      This document contains sub documents. Process them individually.
    -->  
    <exsl:documents>
    <xsl:for-each select = "descendant::exsl:document">
      <xsl:text>
      </xsl:text>
        <exsl:document>
        <xsl:copy-of select="@*" />
        <xsl:call-template name="html_doc"/>
        </exsl:document>
      <xsl:text>
      
      </xsl:text>
    </xsl:for-each>
    </exsl:documents>
  </xsl:template>


  <xsl:template name="html_doc" match="/*[not(descendant::exsl:document)]">  
  <!-- 
  Main page layout.
  Current context node is either exsl:document or the html tag.
  Either way, head and body are the immediate children.
  -->
    <html>
    <head>
    <!-- META And Header extraction -->
    
      <title>
      <xsl:call-template name="get-title" />
      </title>

    <xsl:text>
    </xsl:text>

    <meta name="description" id="teaser" >
      <xsl:if test="$xmlid"><xsl:attribute name="xml:id">teaser</xsl:attribute></xsl:if>
      <xsl:attribute name="value">
        <xsl:call-template name="get-description" />
      </xsl:attribute>
    </meta>
    
    <xsl:if test="@href">
      <!-- copy the container exsl:document@href into our meta information -->
      <meta name="path" id="path" >
        <xsl:if test="$xmlid"><xsl:attribute name="xml:id">teaser</xsl:attribute></xsl:if>
        <xsl:value-of select="@href" />
      </meta>
    </xsl:if>


    <xsl:for-each select=".//head|.//xhtml:head">
      <xsl:for-each select="meta[@name]|xhtml:meta[@name]|rel|xhtml:rel|style|xhtml:style">
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
      <h1 id="pagetitle">
        <xsl:if test="$xmlid"><xsl:attribute name="xml:id">pagetitle</xsl:attribute></xsl:if>
        <xsl:call-template name="get-title" />
      </h1>

      <!-- content writes its own div. May be null? -->
      <xsl:call-template name="get-content" >
        <xsl:with-param name="contentid" select="$contentid" />
      </xsl:call-template >


      <!-- There's a chance we can find and use these commonly tagged elements -->
      <xsl:call-template name="get-sidebar" />
      <xsl:call-template name="get-blocks" />
      <!-- 
        Note that the destination content type 
        must have a way of dealing with these 
        - eg by making a cck field 
        called field_sidebar or something 
      -->
  
    </body>

    </html>
  </xsl:template>




  <xsl:template name="get-title">
    <!-- 
      Note: TODO 
      Need to deal with the possible difference between 
      document title and H1 display title .
      Look for H1 or anything id=pagetitle first
      If that fails, use the meta title.
      Some exceptions to do so are defined later.
      
      Sometimes need to remove the header from the body once i've found it?
    -->
      <xsl:choose>
  
        <xsl:when test="descendant::*[local-name()='h1']">
            <xsl:value-of select="normalize-space(descendant::*[local-name()='h1'])" />
        </xsl:when>
        
        <xsl:when test="//*[@id='pagetitle']">
          <!-- 
            Nice sites set the id of their title.
            May add other #ids here as I find them
          -->
            <xsl:value-of select="normalize-space(descendant::*[@id='pagetitle'])" />
        </xsl:when>

        <xsl:when test="count(descendant::h2|descendant::xhtml:h2)=1">
          <!-- 
            Maybe it used an h2 because h1 was ugly.
            If there is ONLY ONE h2, use that
          -->
            <xsl:value-of select="normalize-space(descendant::h2|descendant::xhtml:h2)" />
        </xsl:when>
        
        <xsl:otherwise>
          <!-- 
            In practice, the html head title is often more verbose than we want
            and too often it's the same across whole sections, but it'll have to do.
          -->
            <xsl:value-of select="normalize-space(descendant::*[name() = 'title'])" />
        </xsl:otherwise>
        <!--
          Any of these steps may fail if the heading contains only html, eg
          <h1><img src='header.gif' /></h1>
        -->
      </xsl:choose>
  </xsl:template>
  
  <xsl:template match="*[local-name()='h1']|*[@id='pagetitle']">
  <!-- remove these from the flow altogether if they were in the content -->
  </xsl:template>


  <xsl:template name="get-content">
      <!--  
        Attempt to find any defined content divs. 
        A set of names is scanned for, and if any elements are 
        found with those ids or classes, that is used as the content.
        We go one-by-one through {user-defined contentid} followed
        by 'content', 'bodyCopy' and 'main'.
        If more than one match is found at the same level of precedence,
        the results are concatenated.
        
        Failing that, include everything found in the body.
        
        A note of the precision used to get a result is passed back
        as an attribute of the containing element. This value can be
        inspected to provide a confidence estimate to the calling function
        if it wants to try using heuristics to make a better guess.
        precision=1 means perfect match, 
        precision=0 means no match, just a fallback.
        Everything else is decimal between the two.
      -->

    <xsl:param name="contentid"/>

    <div id="content">
      <xsl:if test="$xmlid"><xsl:attribute name="xml:id">content</xsl:attribute></xsl:if>
    
      <xsl:choose>
        <!--
          Need to detect them before using them to avoid 
          duplication of content if more than one match is valid.
          So do a look-see test before select.
          Cannot group these tests, because we need to ensure 
          these checks happen in order of desired precedence.
          
          When we've found our content, we discard its type and return its naked contents.
        -->

        <xsl:when test="descendant::*[@id=$contentid or @class='$contentid' ]">
          <!-- user-defined id tag -->
          <xsl:attribute name="precision" >1</xsl:attribute>
          <xsl:attribute name="title" >Found the named content ID first time!</xsl:attribute>
          <xsl:for-each select="descendant::*[@id=$contentid or @class=$contentid]" >
            <xsl:apply-templates />
            <xsl:text>
            </xsl:text>
          </xsl:for-each>
        </xsl:when>

        <!-- 
          Didn't find what we were told to look for - try a random handful of guesses.
          Place these in order of most uncommon/specific to possibly re-used ones
          to avoid premature false positives.
        -->

        <!-- Start thinking about some HTML5 or microformat elements -->

        <xsl:when test="count(descendant::*[local-name()='article']) = 1">
          <!-- 
            If there is only ONE 'article' element, we'll take that.
            It's pretty damn likely to be what you want.
           -->
          <xsl:attribute name="precision" >.90</xsl:attribute>
          <xsl:attribute name="title" >Imported From the HTML5 element 'article' element.</xsl:attribute>
          <xsl:for-each select="descendant::*[local-name()='article']" >
            <xsl:apply-templates />
            <xsl:text>
            </xsl:text>
          </xsl:for-each>
        </xsl:when>

        <xsl:when test="count(descendant::*[@id='article']) = 1">
          <!-- 
            If there is only ONE '#article' element, that's pretty good too.
          -->
          <xsl:attribute name="precision" >.85</xsl:attribute>
          <xsl:attribute name="title" >Imported From the #article ID element.</xsl:attribute>
          <xsl:for-each select="descendant::*[@id='article']" >
            <xsl:apply-templates />
            <xsl:text>
            </xsl:text>
          </xsl:for-each>
        </xsl:when>

        <xsl:when test="descendant::*[@id='bodyCopy' or @class='bodyCopy' or @id='contentbody' or @class='contentbody']">
          <xsl:attribute name="precision" >.75</xsl:attribute>
          <xsl:attribute name="title" >Found a generically named element that's probably the body copy.</xsl:attribute>
          <xsl:comment>Imported From the element called bodyCopy</xsl:comment>
          <xsl:for-each select="descendant::*[@id='bodyCopy' or @class='bodyCopy' or @id='contentbody' or @class='contentbody']" >
            <xsl:apply-templates />
          </xsl:for-each>
        </xsl:when>
        
        <xsl:when test="//*[@id='maincontent']">
          <xsl:attribute name="precision" >.66</xsl:attribute>
          <xsl:attribute name="title" >Found an element named 'maincontent' that may be the body copy.</xsl:attribute>
          <xsl:comment>Imported From the element called maincontent</xsl:comment>
          <xsl:for-each select="//*[@id='maincontent']" >
            <xsl:apply-templates />
          </xsl:for-each>
        </xsl:when>

        <xsl:when test="descendant::*[@id='main' or @class='main']">
          <xsl:attribute name="precision" >.5</xsl:attribute>
          <xsl:attribute name="title" >Found an element named 'main' that may be the body copy.</xsl:attribute>
          <xsl:comment>Imported From the element called main</xsl:comment>
          <xsl:for-each select="descendant::*[@id='main' or @class='main']" >
            <xsl:apply-templates />
          </xsl:for-each>
        </xsl:when>

        <xsl:when test="descendant::*[@id='content']">
          <!-- 
            'content' is a little bit generic, and may sometimes get more 
            than we wanted, or something totally irrelevant.
            Really, if you are going to tag something in your page with the ID
            'content' MAKE IT the meat! Not the outside of a wrapper div :-(
            However, I'll skip the class=content stuff, as that could be anything.
            Just get #content by ID.
           -->
          <xsl:attribute name="precision" >.33</xsl:attribute>
          <xsl:attribute name="title" >Found an element named 'content' that may be the body copy. Could be anything though.</xsl:attribute>
          <xsl:comment>Imported From the element called content</xsl:comment>
          <xsl:for-each select="descendant::*[@id='content']" >
            <xsl:apply-templates />
            <xsl:text>
            </xsl:text>
          </xsl:for-each>
        </xsl:when>

        <xsl:when test="count(descendant::*[@class='content']) = 1">
          <!-- 
            OK, so if there is only ONE class=content, we'll take that.
           -->
          <xsl:comment>Imported From the element classed 'content'</xsl:comment>
          <xsl:for-each select="descendant::*[@class='content']" >
            <xsl:apply-templates />
            <xsl:text>
            </xsl:text>
          </xsl:for-each>
        </xsl:when>

        
        <xsl:when test="descendant::*[@class='reference']">
        <!--
          This is a domain-specific example that grabs content from php.net documentation
          Just another example really
        -->
          <xsl:attribute name="precision" >.25</xsl:attribute>
          <xsl:attribute name="title" >Found an element named 'reference' that may be the body copy. Just a guess really.</xsl:attribute>
          <xsl:comment>Imported From the element called reference</xsl:comment>
          <xsl:for-each select="descendant::*[@class='reference']" >
            <xsl:apply-templates />
          </xsl:for-each>
        </xsl:when>

        <xsl:otherwise>
          <xsl:attribute name="precision" >0</xsl:attribute>
          <xsl:attribute name="title" >Couldn't selectively extract content, Imported Full Body :( May need to used a more carefully tuned import template.</xsl:attribute>
          <xsl:comment>Couldn't selectively extract content, Imported Full Body :( May need to used a more carefully tuned import template.</xsl:comment>
          <xsl:text>
          
          </xsl:text>
          <!-- Absolutely no magic here - dump it ALL in -->
          <xsl:apply-templates select="descendant::body/*|descendant::xhtml:body/*|descendant::body/text()|descendant::xhtml:body/text()"/>
        </xsl:otherwise>

      </xsl:choose>
    </div>
  </xsl:template>
  
  <xsl:template name="get-description">
    <!-- 
      Look for 
      a defined meta tag, 
      a 'description' id, 
      a 'description' class, 
      or the first paragraph. 
      In that order.
      Description text is a flat string with no formatting!
    -->
    <xsl:choose>
      <xsl:when test="descendant::meta[@name='description']">
          <xsl:value-of select="descendant::meta[@name='description']" />
      </xsl:when>
      <xsl:when test="descendant::*[@id='description']">
          <xsl:value-of select="descendant::*[@id='description']" />
      </xsl:when>
      <xsl:when test="descendant::*[@class='description']">
          <xsl:value-of select="descendant::*[@class='description']" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:variable name="set" select="descendant::p|descendant::xhtml:p"/>
        <xsl:for-each select="$set[1]">
          <xsl:value-of select="descendant::*[@class='description']" />
        </xsl:for-each>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <!-- 
    Make some more generic guesses. 
    'sidebar's and 'block's can probably be abstracted 
  -->

  <xsl:template name="get-sidebar">
    <xsl:for-each select="//*[@class='sidebar']|//*[@id='sidebar']">
      <div class="sidebar"><xsl:apply-templates select="*" /></div>
    </xsl:for-each>
  </xsl:template>

  <xsl:template name="get-blocks">
    <xsl:for-each select="//*[@class='block']">
      <div class="block">
        <xsl:apply-templates select="*" />
      </div>
    </xsl:for-each>
    
    <!-- TODO see if this hits too many false positives? anything with the string 'block' in it? -->
    <!-- Yes it did, was firing on all class="clear-block". Scrap it, it's no help. -->
    <!-- 
    <xsl:for-each select="//*[contains(@class,'block')]">
      <div class="block">
        <xsl:apply-templates select="*" />
      </div>
    </xsl:for-each>
    -->

  </xsl:template>

<!-- therefore they don't show up in the body of the page. Nullify them if they are found elsewhere -->
  <xsl:template match="*[@class='sidebar']|*[@id='sidebar']">
  </xsl:template>
  <xsl:template match="*[@class='block']">
  </xsl:template>
  

<!--  
  Anorexic anchor tags are no good, 
  as they compress into singletons 
  and probably end up ruining the rendering. 
-->
   <xsl:template name="fattentags" match="a[@name and (not(*|text()|comment()))]">
      <xsl:copy>
         <xsl:copy-of select="@*" />
         <xsl:apply-templates />
         <xsl:comment>Empty Anchor tags are non-semantic and should be deprecated</xsl:comment>
       </xsl:copy>
   </xsl:template>

<!-- 
  Although I'd like to be generic, 
  generically the following patterns are usually meta info 
  that I want to strip or approach differently. Discard it
-->
   <xsl:template match="xhtml:*[@class = 'timestamp']"></xsl:template>
   <xsl:template match="xhtml:*[@id = 'comments']"></xsl:template>
   <xsl:template match="xhtml:*[@class = 'commentlist']"></xsl:template>
   <xsl:template match="xhtml:form"><xsl:comment>Form element discarded on import. It will need special attention</xsl:comment></xsl:template>
  
  
  <!-- Identity transformation template - to inline existing HTML  -->      
  <xsl:template name="passthrough" match="@*|node()|text()">
    <xsl:copy>
    <xsl:apply-templates select="@*|node()|text()"/>
    <!-- Fatten all empty tags (anorexic divs are a failure) but not known singletons -->
    <xsl:if test="not (node()|text()) and not(local-name()='img') and not(local-name()='br') and not(local-name()='hr') "><xsl:text> </xsl:text></xsl:if>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>