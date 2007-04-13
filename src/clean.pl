#!/bin/perl

# �폜����t�@�C���̊g���q���X�g
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
	print "�폜���܂����H (y/n) -> ";
	$g = getc;
	if( $g eq 'y' || $g eq 'Y' ){
		print "yes ($g)\n";
		$n = &findFiles( 'clean' );	# clean mode
		print "$n �̃t�@�C�����폜���܂����B\n";
	}
}else{
	print "���ɃN���[���ł��B\n";
}
exit;

# findFiles
#	�����P�� 'find' ���w�肷��ƁA@ahi �������ɂ���t�@�C����\������B
#	�����P�� 'clean' ���w�肷��ƁA@ahi �������ɂ���t�@�C�����폜����B
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
