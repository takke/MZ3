<div id="about">
	<p><a href="http://validator.w3.org/check?uri=referer"><img src="http://www.w3.org/Icons/valid-xhtml10" alt="Valid XHTML 1.0!" height="31" width="88" /></a></p>
</div> <!-- id="about" -->

<!-- Google Analytics Widget Code -->

<script type="text/javascript">
  var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
  document.write(unescape("%3Cscript src='" + gaJsHost + "google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E"));
</script>
<script type="text/javascript">
  try {
    jQuery(document).ready( function() {
      var pageTracker = _gat._getTracker("UA-185187-4");
      jQuery('a').each( function() {
        var url;
        if ( this.href.match("http:\/\/www.mz3.jp") ) {
          if ( this.href.match(/\.(zip|tar|tar.gz|tar.bzip|egg)$/) ) {
            url = this.pathname + this.search;
          };
        } else {
          var port = '';
          if ( this.port != '') port = ':'+this.port;
          url = '/trac_mz3_external/' + this.hostname + port + this.pathname + this.search;
        };
        if ( url ) {
          if ( ! url.match('^\/attachment\/') ) {
            jQuery(this).click( function() {
              pageTracker._trackPageview(url);
            });
          };
        };
      });
    pageTracker._trackPageview();
    });
  } catch(err) {}
</script>
<!-- Google Analytics Widget Code Ended -->
