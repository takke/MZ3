#!/bin/perl

# 削除するファイルの拡張子リスト
@ahi = (
		'\.aps',
		'\.aux',
		'\.idb',
		'\.ilk',
		'\.ncb', 
		'\.obj',
		'\.opt', 
		'\.pch',
		'\.plg',
		'\.pdb',
		'\.res',
		'\.lib',
		'\.exp',
		'\.htm',
		'\.enc',
		'\.stackdump',
		'semantic\.cache',
		'Debug/MZ3.CAB',
		'Debug/MZ3.exe',
		);

$n = &findFiles( 'find' );	# find mode
if( $n > 0 ){
	print "削除しますか？ (y/n) -> ";
	$g = getc;
	if( $g eq 'y' || $g eq 'Y' ){
		print "yes ($g)\n";
		$n = &findFiles( 'clean' );	# clean mode
		print "$n 個のファイルを削除しました。\n";
	}
}else{
	print "既にクリーンです。\n";
}
exit;

# findFiles
#	引数１に 'find' を指定すると、@ahi が末尾にくるファイルを表示する。
#	引数１に 'clean' を指定すると、@ahi が末尾にくるファイルを削除する。
sub findFiles($)
{
	my($var) = @_;
	my($find) = 0;
	my($clean) = 0;
	
	if( $var eq 'find' ){
#		print "find mode\n";
		$find = 1;
	}elsif( $var eq 'clean' ){
#		print "clean mode\n";
		$clean = 1;
	}
	
	my($n) = 0;
	foreach $filename ( `find` )
	{
		chomp $filename;
		foreach $ext ( @ahi ){
			if( $filename =~ /$ext\Z/ ){
				if( $find ){
					print "found ($ext) : $filename\n";
				}
				$n ++;
				if( $clean && (unlink $filename) > 0 ){
					print "unlinked ($ext) : ${filename} \n";
				}
			}
		}
	}
	
	return $n;
}
