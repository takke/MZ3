#!perl

use strict;

if($#ARGV<0) {print 'argv-filename'; exit;}

my(@print,$author,$date);
my($gc,$rp);

foreach my $fn(@ARGV)
{
	open IN,$fn or die "file-not-found:$fn";
	$fn =~ s/\.c$/.html/;
	open OUT,">$fn";
	
	@print = ();
	$author = 'nothing-author';
	$date = 'update?';
	$gc = '';
	my $bl = '';
	my $cm = '';
	foreach my $ln(<IN>)
	{
		$ln = $bl.$ln;
		if($ln=~m/\\$/)
		{
			$bl = $`;
			$bl=~s/^\s+//g;
			$bl=~s/\s+$//g;
			next;
		}else {$bl=''}
		
		$ln=~s/^\s+//g;
		$ln=~s/\s+$//g;
		
		if($ln=~m#^/[/*].+# || $gc ne '') {$cm=&comment($cm,$ln)}
		if($ln=~m/^(#[a-zA-Z]+)\s+(.+)/)
		{
			if($cm ne '') {&print_def($cm,$1,$2)}
			elsif($1 eq '#include')
			{
				my $inc = $2;
				$inc=~s#["<](.+?)[">]#$1#;
				&print_inc($cm,$inc);
			}
			elsif($ln=~m%^(#[a-zA-Z]+)\s+(.+?)(//.+)?$%)
			{
				my $def = $1;
				my $opt = $2;
				$cm = &comment($cm,$3);
				&print_def($cm,$def,$opt) if $cm ne '';
			}
		}
		elsif($ln=~m/^[\w]+\*?(\s+[\w<>]+)?\s+(\S+)\(.*\)$/)
		{
			&print_func($cm,$&) if $cm ne '';
		}
		if($gc eq '' && $ln!~m#\*/#) {$cm = ''}
	}
	print OUT <<__EOM__;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html lang="ja" dir="ltr">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=sjis">
<meta name="author" content="$author">
<meta name="date" content="$date">
<meta name="description" content="xxx">
<meta name="keywords" content="c-lang">
<meta http-equiv="Content-Style-Type" content="text/css">
<link rev="made"       href="mailto:xxx\@xxx.ne.jp">
<link rel="contents"   href="http://www.xxx.ne.jp/">
<link rel="stylesheet" href="./jectdoc.css" type="text/css">
<title>xxx</title>
</head>
<body><div id="TOPBST">
__EOM__
	print OUT join("\n",@print)."\n";
	print OUT <<__EOM__;
</div></body>
</html>
__EOM__
	close OUT;
	close IN;
}

sub comment($$)
{
	my($cm,$nc) = @_;
	my $mc = '';
	
	if($nc=~m#^//--#)
	{
		$' =~ m/^-*(.+?)-+$/;
		push @print,'<h1>'.$1.'</h1>'
	}
	elsif($nc=~m#^//-#) {push @print,'<h2>'.&conv_html($').'</h2>'}
	elsif($nc=~m%^//#% || ($rp eq '\\#' && $nc=~m/^$rp/))
	{
		$mc='<cite>'.&conv_html($').'</cite>';
		$rp = '\\#';
	}
	elsif($nc=~m%^/\*#%)
	{
		$mc = '<cite>'.&conv_html($');
		$gc = '</cite>';
	}
	elsif($nc=~m%^//!% || ($rp eq '!' && $nc=~m/^$rp/))
	{
		$mc='<strong>'.&conv_html($').'</strong>';
		$rp = '!';
	}
	elsif($nc=~m#^/\*!#)
	{
		$mc = '<strong>'.&conv_html($');
		$gc = '</strong>';
	}
	elsif($nc=~m%^//\+% || ($rp eq '\\+' && $nc=~m/^$rp/))
	{
		$mc='<ins>'.&conv_html($').'</ins>';
		$rp = '\\+';
	}
	elsif($nc=~m#^/\*\+#)
	{
		$mc = '<ins>'.&conv_html($');
		$gc = '</ins>';
	}
	elsif($nc=~m%^//<% || ($rp eq '<' && $nc=~m/^$rp/))
	{
		$mc='<div>'.&conv_html($').'</div>';
		$rp = '<';
	}
	elsif($nc=~m#^/\*<#)
	{
		$mc = '<div>'.&conv_html($');
		$gc = '</div>';
	}
	elsif($nc=~m#^//%#) {$mc='<addres>'.&conv_html($').'</addres>'}
	elsif($nc=~m#^//@#) {$mc=$'."<br />"}
	elsif($nc=~m#^//\$\$(\S+)(\s+(.+))?#)
	{
		if(defined($3)) {$mc='<kbd>'.&conv_html($1).'</kbd>'.&conv_html($3)}
		else {$mc='<kbd>'.&conv_html($1).'</kbd>'}
	}
	elsif($nc=~m#^//\$(\S+)(\s+(.+))?#)
	{
		if(defined($3)) {$mc='<var>'.&conv_html($1).'</var>'.&conv_html($3)}
		else {$mc='<var>'.&conv_html($1).'</var>'}
	}
	elsif($gc ne '' && $nc=~m#\*/#)
	{
		$mc .= $gc;
		$gc = '';
		$rp = '';
	}
	else {$mc.=$nc}
	
	return($cm.$mc."<br />\n") if $gc ne '';
	return($cm.$mc);
}

sub print_inc($$)
{
	my($cm,$inc) = @_;
	push @print,qq(<div class="inc"><em>$inc</em>$cm</div>);
}

sub print_def($$$)
{
	my($cm,$df,$di) = @_;
	push @print,qq(<div class="def">$cm $df,$di</div>);
}

sub print_func($$)
{
	my($cm,$fn) = @_;
	push @print,qq(<div class="func"><em>$fn</em>\n$cm</div>);
}

sub conv_html($)
{
	$_ = @_[0];
	s/^\s+//;
	s/\s+$//;
	s/\s/\&nbsp;/g;
	s/</\&lt;/g;
	s/>/\&gt;/g;
	$_;
}
