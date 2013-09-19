<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!--
    Used to TRY and pass html, including script blocks, through unmolested.
    If the parameter 'strip_script_tags' is set, they will be discarded. 
    Include this template in other structural ones that are not supposed to wreck incidental HTML.

		$Id$
-->
	<xsl:param name="strip_script_tags"></xsl:param>


<!-- Identity transformation template - let everything else pass through  -->
   <xsl:template match=" * | comment() | processing-instruction() | text()">
      <xsl:copy>
         <xsl:copy-of select="@*" />

         <xsl:apply-templates
         select=" * | comment() | processing-instruction() | text()" />
      </xsl:copy>
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


   <xsl:template match="*[local-name()='script']">
<!-- 
	general-case for script pass-through is tricky.
	Note, this match is LESS specific than the one that matches all src-ed elements
	so a script src='' thing is NOT touched here, only inline code blocks.
 -->
	<xsl:if test="not($strip_script_tags)">
      <xsl:copy>
         <xsl:copy-of select="@*" />
         <!-- xsl:comment-->
         <xsl:text disable-output-escaping="yes"> // &lt;![CDATA[</xsl:text>
         <xsl:value-of select="text()" disable-output-escaping="yes" />
         <xsl:text disable-output-escaping="yes"> // ]]&gt; </xsl:text>
         <!-- /xsl:comment-->
      </xsl:copy>
      </xsl:if>
   </xsl:template>
</xsl:stylesheet>

