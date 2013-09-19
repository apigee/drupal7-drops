<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 xmlns="http://www.w3.org/1999/xhtml"
 xmlns:xhtml="http://www.w3.org/1999/xhtml"
>
  <!-- 
    See if we can find any 'InstanceBeginEditable' tags put in there by, um, DreamWeaver  
    Convert them into tagged div wrappers!
    - This is rude as I'm abusing the XML tree with literals to pretend to be tags.
    - this is REALLY rude, and could even break what used to be valid, 
    as it's possible that the end tag isn' t there.
    But we are inserting structure where there was none. 
    This may involve breaking the tree.
    
    {!- InstanceBeginEditable name="Content" -}BLAH{!- InstanceEndEditable -}
    becomes
    <div class="InstanceBeginEditable" id="Content" >BLAH</div>
    !!
      
    This is one of the tidy-up processes that can be run on the source before analysis
    To be paranoid, maybe you should run 'tidy' again after it!
    
		$Id$
  -->

  <xsl:template match="comment()[contains(.,'InstanceBeginEditable')]">
    <!-- Man, I miss standard string routines. This extracts the lowercase name of the instance -->
    <xsl:text disable-output-escaping="yes">&#60;</xsl:text>div class="InstanceBeginEditable" 
      id="<xsl:value-of select="substring-before(substring-after(translate(.,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'),'name=&quot;'), '&quot;')" />" <xsl:text disable-output-escaping="yes">&#62;</xsl:text>
  </xsl:template>

  <xsl:template match="comment()[contains(.,'InstanceEndEditable')]">
    <xsl:text disable-output-escaping="yes">&#60;</xsl:text>/div<xsl:text disable-output-escaping="yes">&#62;</xsl:text>
  </xsl:template>

<!-- Identity transformation template - let everything else pass through  -->
   <xsl:template match=" * | comment() | processing-instruction() | text()">
      <xsl:copy>
         <xsl:copy-of select="@*" />
         <xsl:apply-templates
         select=" * | comment() | processing-instruction() | text()" />
      </xsl:copy>
   </xsl:template>


</xsl:stylesheet>

