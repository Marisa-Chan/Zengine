#!/usr/bin/perl

if ($#ARGV != 0)
{exit();}

$dirname = $ARGV[0];

$en=substr($dirname,-1,1);
if (($en eq "\\") or ($en eq "/")) 
	{chop $dirname;}

RenameDirSubDir($dirname);

exit();

sub RenameDirSubDir
{
	local $tmp;
	local $filename;
	local $i;
	local $tmp2;
	local $pth2;
	
	opendir (DIR, $_[0] ) || die "Error in opening dir ".$_[0]."\n";
	local @filesdirs = readdir(DIR);
	closedir(DIR);
	
	foreach(@filesdirs)
	{
		$filename = $_;
		if (($filename ne ".") and ($filename ne ".."))
		{
			$tmp2 = $_[0]."/".uc($filename); 
			$tmp = $_[0]."/".$filename;
			if (-d $tmp)
				{RenameDirSubDir($tmp);}
				
			system("mv \"$tmp\" \"$tmp2\" &>/dev/null"); 
		}
    
	}
}