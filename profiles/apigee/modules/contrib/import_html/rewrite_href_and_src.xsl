<?xml version="1.0" encoding="UTF-8"?>

<!--
	When an XML doc is converted, it is presented to the viewer out of context.
	If the context was important (eg it was XHTML comprising a hrefs and img srcs)
	that must be repaired. 
	Using the HTML base markup is ugly. instead we'll use it as a parameter to all links.
	
	If href_base parameter is set, then all hrefs, img src etc in the document
	need to be adjusted to point back to the real data source location.
	This will have to be applied by clever re-writing
	of the links, as this template attempts.
	
	When importing or otherwise moving a site or subsite around, it may be appropriate
	to place the images, css and other 'resources' in a different place from the html content.
	This is the src_base parameter.

	When seriously renovating a site from one system to another, we may be changing or
	even discarding the suffix. If so, set replace_suffix to true, and tell me the new suffix.
	replace_suffix = TRUE and new_suffix = '' means discard suffixes altogether (just hrefs, not srcs)

  [site|src]_root are what the links are being rewritten TO
  [site|src]_base are what the links are being rewritten FROM
  Very basically, replace 'base' with 'root' and see what happens

  In the process of scanning every a href in the doc, it also discards any stray
  target attributes. Because HTML Tidy failed to :-/

	$Id$
-->


<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xhtml="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
>
<!--
	Output must be set to be XML compliant, else the meta and link tags
	(unclosed singletons in old HTML) will not validate on the next pass.
-->
  <xsl:output
    method="xml"
    encoding="UTF-8" />

  <xsl:param name="site_root">/</xsl:param>
  <xsl:param name="src_root">/files/imported</xsl:param>
  <xsl:param name="href_base">thebase</xsl:param>
  <xsl:param name="src_base">thesrc</xsl:param>
  <xsl:param name="full_url_root">http://example.com/</xsl:param>
  <xsl:param name="replace_suffix" />
  <xsl:param name="new_suffix">.newhtm</xsl:param>

  <xsl:param name="strip_script_tags" />
  <xsl:param name="default_document">index</xsl:param>

<!--	
	<xsl:include href="safe_pass_through.xsl" />
PHP4 just breaks for some reason
 -->
	<!-- Include some intelligent identity transformations for the rest of the page -->
	
<!-- 
	Rewrite everything that uses 'href' to use the corrected versions of that path.
  Prepend either href_base or src_base to it, depending on the type of target.
-->
  <xsl:template
    name="rewritehrefs"
    match="node()[@href]"
  >
	<!--
   	rewrite <xsl:value-of select="@href" /> using <xsl:value-of select="$href_base" />
	-->
    <xsl:param name="basename">
        <!-- 
          basename is just the last segment of the href. 
          We look at that to guess if it's a html pagge, resource or a folder.
          Treat folders like HTML.
         -->
      <xsl:call-template name="basename">
        <xsl:with-param name="string"><xsl:value-of select="@href" /></xsl:with-param>
      </xsl:call-template>
    </xsl:param>

    <xsl:call-template name="rewritelink">
      <xsl:with-param name="attname">href</xsl:with-param>
      <xsl:with-param name="replace_suffix">
        <xsl:value-of select="$replace_suffix" />
      </xsl:with-param>
			<!--
				Problem - hrefs that link directly to resources (eg, the big jpeg) need extra care.
				Looking for the suffix is not great, but it's a best guess.
				Should use ends-with, but sablotron doesn't play that.
				This needs work and may cause problems.
			-->
      <xsl:with-param name="linkbase">

        <xsl:choose>
          <xsl:when
            test="contains(@href,'.htm') or contains(@href,'.php') or contains(@href,'.asp') or contains(@href,'.jsp') "
          >
            <xsl:value-of select="$href_base" />
          </xsl:when>
          <xsl:when test="substring(@href, string-length(@href) ) = '/' ">
            <!-- ends-with('/') - also assumed to be a normal page -->
            <xsl:value-of select="$href_base" />
          </xsl:when>
          <!-- 
            Check the last segment (basename) of the path. 
            If it has no '.' it looks like it's a folder (so is type html) 
          -->
          <!-- 
            url "afolder" is a relative folder (match) 
            url "afolder/image.gif" has a . in the filename, no match
            url "../afolder" has . in the path but not in the last segment, a match 
            url "../afolder/resource.pdf" has a . in the last segment, no match 
          -->
          <xsl:when test="not( contains( $basename, '.') )">
            <xsl:value-of select="$href_base" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$src_base" />
          </xsl:otherwise>
        </xsl:choose>
      </xsl:with-param>

      <xsl:with-param name="linkroot">
        <xsl:choose>
          <xsl:when
            test="contains(@href,'.htm') or contains(@href,'.php') or contains(@href,'.asp') or contains(@href,'.jsp') "
          >
            <xsl:value-of select="$site_root" />
          </xsl:when>
          <xsl:when test="substring(@href, string-length(@href) ) = '/'">
            <!-- ends-with('/') -->
            <xsl:value-of select="$site_root" />
          </xsl:when>
          <xsl:when test="not( contains($basename, '.'))">
            <!--  It has no '.' in the filename. It looks like it's a folder -->
            <xsl:value-of select="$site_root" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$src_root" />
          </xsl:otherwise>
        </xsl:choose>
      </xsl:with-param>

      <xsl:with-param name="replace_suffix">
        <xsl:choose>
          <xsl:when
            test="contains(@href,'.htm') or contains(@href,'.php') or contains(@href,'.asp') or contains(@href,'.jsp') "
          >
						<!-- only these instances get suffix replaced -->
            <xsl:value-of select="$replace_suffix" />
          </xsl:when>
        </xsl:choose>
      </xsl:with-param>

    </xsl:call-template>
  </xsl:template>


<!-- 
	rewrite everything that uses 'src'
	Generally, src urls should NOT have their suffix rewritten, that's just silly.
 -->
  <xsl:template
    name="rewritesrcs"
    match="node()[@src]"
  >
    <xsl:call-template name="rewritelink">
      <xsl:with-param name="attname">src</xsl:with-param>
      <xsl:with-param name="linkbase">
        <xsl:value-of select="$src_base" />
      </xsl:with-param>
      <xsl:with-param name="linkroot">
        <xsl:value-of select="$src_root" />
      </xsl:with-param>
      <xsl:with-param name="replace_suffix" />
    </xsl:call-template>
  </xsl:template>


  <xsl:template
    name="rewritebackgroundlinks"
    match="node()[@background]"
  >
    <xsl:call-template name="rewritelink">
      <xsl:with-param name="attname">background</xsl:with-param>
      <xsl:with-param name="linkbase">
        <xsl:value-of select="$src_base" />
      </xsl:with-param>
      <xsl:with-param name="linkroot">
        <xsl:value-of select="$src_root" />
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>


  <xsl:template
    name="rewriteembeddedmedia"
    match="node()[@name='movie' and name()='param']"
  >
    	<!--
    		SWF and things with <param name='movie' value='source/file.swf' /> syntax.
    		Awkward node()[name='node-name'] xpath is used here to ignore namespaces.
    	-->
    <xsl:call-template name="rewritelink">
      <xsl:with-param name="attname">value</xsl:with-param>
      <xsl:with-param name="linkbase">
        <xsl:value-of select="$src_base" />
      </xsl:with-param>
      <xsl:with-param name="linkroot">
        <xsl:value-of select="$src_root" />
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

<!-- 
  Rewrite form actions also. Forms generally won't work, but they may at least link to the expected place.
 -->
  <xsl:template
    name="rewriteactions"
    match="node()[@action]"
  >
    <xsl:call-template name="rewritelink">
      <xsl:with-param name="attname">action</xsl:with-param>
      <xsl:with-param name="linkbase">
        <xsl:value-of select="$href_base" />
      </xsl:with-param>
      <xsl:with-param name="linkroot">
        <xsl:value-of select="$site_root" />
      </xsl:with-param>
      <xsl:with-param name="replace_suffix" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template
    name="rewriteimports"
    match="xhtml:style"
  >
    <!-- Damn. Time for some string replacement -->
    <!-- This only works on clear quoted root-relative imports right now. css is rarely fully relative? -->
    <xsl:copy>
      <xsl:call-template name="replace">
        <xsl:with-param
          name="string"
          select="text()" />
        <xsl:with-param name="search">import "/</xsl:with-param>
        <xsl:with-param name="replace">import "<xsl:value-of select="$src_root" />
        </xsl:with-param>
      </xsl:call-template>
    </xsl:copy>
  </xsl:template>

  <!-- Boring having to reinvent the wheel here. The long way. -->
  <xsl:template name="replace">
    <xsl:param name="string" />
    <xsl:param name="search" />
    <xsl:param name="replace" />

    <xsl:choose>
      <xsl:when test="contains($string, $search)">
        <xsl:value-of select="substring-before($string, $search)" />
        <xsl:value-of select="$replace" />
        <xsl:call-template name="replace">
          <xsl:with-param
            name="string"
            select="substring-after($string, $search)" />
          <xsl:with-param
            name="search"
            select="$search" />
          <xsl:with-param
            name="replace"
            select="$replace" />
        </xsl:call-template>

      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$string" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <!-- Utility function. given a path of indeterminate length, return the segment afer the last '/'. -->
  <xsl:template name="basename">
    <xsl:param name="string" />
    <xsl:param name="delimiter">/</xsl:param>
    <xsl:choose>
      <xsl:when test="contains($string, $delimiter)">
        <xsl:call-template name="basename">
          <xsl:with-param
            name="string"
            select="substring-after($string, $delimiter)" />
          <xsl:with-param
            name="delimiter"
            select="$delimiter" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$string" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!-- 
	rewrite everything that has a link (href,src, etc)
	to use a link relative to somewhere else
 -->
  <xsl:template name="rewritelink">
    <xsl:param name="attname" />
    <!-- use for relative links  -->
    <xsl:param name="linkbase"><xsl:value-of select="$src_base" /></xsl:param>
    <!-- use for root-relative links -->
    <xsl:param name="linkroot"><xsl:value-of select="$src_root" /></xsl:param>
    <xsl:param name="replace_suffix"><xsl:value-of select="$replace_suffix" /></xsl:param>

    <xsl:copy>
      <xsl:for-each select="@*">
        <xsl:choose>

          <xsl:when test="name()=$attname">

					<!-- These ARE the droids you are looking for -->
            <xsl:choose>

              <xsl:when
                test="not(contains(.,':')) and not(starts-with(.,'/')) and not(starts-with(.,'#'))"
              >
					   <!-- 
					       It has no scheme, and is doesn't start with /
					       It's a partial Url, may need help being relative
					       Rewrite it
				       -->
                <xsl:attribute name="{$attname}">
							<xsl:value-of select="$linkbase" />
              <xsl:call-template name="replace_suffix"><xsl:with-param
                  name="path"
                ><xsl:value-of select="." /></xsl:with-param><xsl:with-param
                  name="replace_suffix"
                ><xsl:value-of select="$replace_suffix" /></xsl:with-param></xsl:call-template>
							</xsl:attribute>
              </xsl:when>

              <xsl:when test="starts-with(., '/')">
              <!-- 
                This link DOES start with / . 
                Prepend the given linkroot. 
                -->
                <xsl:attribute name="{$attname}">
               <xsl:value-of select="$linkroot" />
               <xsl:call-template name="replace_suffix"><xsl:with-param
                  name="path"
                ><xsl:value-of select="substring( . ,2)" /></xsl:with-param><xsl:with-param
                  name="replace_suffix"
                ><xsl:value-of select="$replace_suffix" /></xsl:with-param></xsl:call-template>
              </xsl:attribute>
              </xsl:when>

              <xsl:when test="starts-with(.,$full_url_root)">
              <!-- remove host from self-referential URLs -->
                <xsl:attribute name="{$attname}">
               <xsl:value-of select="$linkroot" />
               <xsl:value-of select="substring-after( . , $full_url_root)" />
              </xsl:attribute>
              </xsl:when>


              <xsl:otherwise>
						<!-- 
							Full Url or unknown link type, 
              it defines its own scheme, leave as is 
              TODO: remove host from self-referential URLs
						-->
                <xsl:copy-of select="." />
                <xsl:apply-templates select=" * | text()" />
              </xsl:otherwise>

            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
					<!-- 
						It's one of the other attributes
						Copy all and carry on 
						UNLESS it's 'target' which is deprecated but html tidy still hasn't killed for us
					-->
            <xsl:if test="not( name() = 'target')">
              <xsl:copy />
            </xsl:if>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
		<!-- 
		  Done attributes, needs to carry on with the contents 
		-->
      <xsl:apply-templates />

    </xsl:copy>

  </xsl:template>

  <xsl:template name="replace_suffix">
      <!-- 
        Trim the suffix off thie given argument (file path) and 
        replace it on-the-fly, if asked to.
        Cannot really handle query strings, hashes or anything.
      -->
    <xsl:param name="path" />
    <xsl:param name="replace_suffix" />
    <xsl:choose>

      <xsl:when test="contains($path, concat('/', $default_document))">
          <!--           
            If it matches the default document, 
            remove it altogether leaving only the directory.
            Otherwise blah/index.htm becomes blah/index 
            (if trimming) or remains long (if not)
            I want blah/index.htm to become "blah"
          -->
        <xsl:value-of
          select="substring-before($path, concat('/', $default_document))" />
      </xsl:when>
        
        <!-- testing against the input $replace_suffix flag parameter still returns TRUE if given '0'. Bool got cast into string somewhere  -->
      <xsl:when test="($replace_suffix = 1) and substring-before($path, '.')">
          <!-- Trim the suffix -->
          <!-- currently broken if path starts with (or contains?) "." -->
        <xsl:value-of select="substring-before( $path, '.')" />
        <xsl:value-of select="$new_suffix" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$path" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!-- Identity transformation template - let everything else pass through  -->
  <xsl:template match=" * | comment() | processing-instruction() | text()">
    <xsl:copy>
      <xsl:copy-of select="@*" />

      <xsl:apply-templates
        select=" * | comment() | processing-instruction() | text()" />
    </xsl:copy>
  </xsl:template>

  <xsl:template match="*[local-name()='script']">
<!-- 
  General-case for script pass-through is tricky.
  Note, this match is LESS specific than the one that matches all src-ed elements
  so a script src='' thing is NOT touched here, only inline code blocks.
 -->
    <xsl:if test="not(number($strip_script_tags))">
      <xsl:copy>
        <xsl:copy-of select="@*" />
         <!-- xsl:comment-->
        <xsl:text disable-output-escaping="yes"> // &lt;![CDATA[</xsl:text>
        <xsl:value-of
          select="text()"
          disable-output-escaping="yes" />
        <xsl:text disable-output-escaping="yes"> // ]]&gt; </xsl:text>
         <!-- /xsl:comment-->
      </xsl:copy>
    </xsl:if>
  </xsl:template>
   
   <!-- in a strange turn of events, an empty noscript tag stopped half a site from rendering -->
  <xsl:template match="*[local-name()='noscript']">
    <xsl:if test="*|text()">
      <!-- Only passthrough noscripts with some content worth showing -->
      <xsl:copy>
        <xsl:copy-of select="@*|*|text()" />
      </xsl:copy>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>