#!/bin/perl
#!/usr/local/bin/perl
#!/perl/perl/perl/perl
#!/lova/perl

use strict;

if($#ARGV != 0 && $#ARGV != 1)
{
	print "dumpmap.pl FILE [MAPFILE]\n";
	print " input dump file-name.";
	exit;
}

my %addr;

my $dump = @ARGV[0];
my $map  = @ARGV[1];
if($map eq "")
{
	$map  = $dump;
	$map =~ s/\.dump$/.map/;
}
if($dump eq "" || $map eq "") {print "input dump file-name."; exit}

open IN, $map;
while(<IN>)
{
	s/\x0d\x0a/\x0a/g;
	chomp;
	next if $_ !~ m/^ [0-9a-f]+:[0-9a-f]+ +(\w+(?:@\d+)?) +([0-9a-f]+)(?: f)? +(.+)/i;
	
	if($3 ne "")
	{
		%addr->{$2} = $1 . "()[" . $3 . "]";
	}
	else
	{
		%addr->{$2} = $1 . "()";
	}
}
close IN;

open IN, $dump;
open OUT, ">$dump.txt";
while(<IN>)
{
	if(m/(0x[0-9a-f]+) ---/)
	{
		my $tmp = $1;
		
		foreach my $ad(sort keys %addr)
		{
			if(hex($tmp) <= hex($ad))
			{
				print OUT "$tmp " . %addr->{$ad} . " + " . sprintf("0x%x", (hex($ad) - hex($tmp))) . "\n";
				$tmp = "";
				last;
			}
		}
		if($tmp ne "")
		{
			print OUT "$tmp, ---\n";
		}
	}
	else
	{
		print OUT $_;
	}
}
close OUT;
close IN;
