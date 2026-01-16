<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:wix="http://wixtoolset.org/schemas/v4/wxs">
  <!-- Explicitly handle the root element -->
  <xsl:template match="/wix:Wix">
    <xsl:copy>
      <xsl:copy-of select="@*" />
      <xsl:apply-templates select="node()" />
    </xsl:copy>
  </xsl:template>

  <!-- Identity template for everything else -->
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()" />
    </xsl:copy>
  </xsl:template>

  <!-- Add DiskId to Files under specific DirectoryRef contexts -->
  <xsl:template match="wix:Component[ancestor::wix:DirectoryRef/@Id='WindowsSDK_usr_lib_swift_shims']">
    <xsl:copy>
      <xsl:attribute name="DiskId">5</xsl:attribute>
      <xsl:apply-templates select="@* | node()" />
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
