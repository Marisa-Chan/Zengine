#!/usr/bin/perl

$mencoder=`which mencoder 2>/dev/null`;
chomp($mencoder);
if (! -e $mencoder)
{
	print("Mencoder not found.\n");
	exit();
}

$imgmgk=`which convert 2>/dev/null`;
chomp($imgmgk);
if (! -e $imgmgk)
{
	print("ImageMagick's convert not found.\n");
	exit();
}

if ($#ARGV < 2)
{
	printf("Usage:\nscript.pl   file.db   /path/to/resources   /dest/path\n");
	exit();
}

$dirname = $ARGV[1];

$en=substr($dirname,-1,1);
if (($en eq "\\") or ($en eq "/")) 
	{chop $dirname;}

$dirname2 = $ARGV[2];
$en=substr($dirname2,-1,1);
if (($en eq "\\") or ($en eq "/")) 
	{chop $dirname2;}
	
open (file,"<",$ARGV[0]);
@txt = <file>;
close file;

$#filname  = -1;
$#filtp    = -1;
$#filarg = -1;

foreach(@txt)
{
	chomp $_;
	@b = split(/ /,$_);
	push @filname,$b[0];
	push @filtp,$b[1];
	push @filarg,$b[2];	
}
	
print "Creating directories structure ";
CreateDestDirs($dirname,$dirname2);
print "[OK]\n";
print "Start conversion\n";
ConvertDirSubDir($dirname,$dirname2);
print "Conversion is done\n";
exit();


sub CreateDestDirs
{
	local $tmp;
	local $tmp2;
	local $filename;
	
	opendir (DIR, $_[0] ) || die "Error in opening dir ".$_[0]."\n";
	local @filesdirs = readdir(DIR);
	closedir(DIR);
	
	foreach(@filesdirs)
	{
		$filename = $_;
		if (($filename ne ".") and ($filename ne ".."))
		{
			$tmp = $_[0]."/".$filename;
			$tmp2 = $_[1]."/".$filename;
			if (-d $tmp)
			{
				if (not(-d $tmp2))
				{
					mkdir $tmp2; 
				}
				CreateDestDirs($tmp,$tmp2);
			}
		}
    
	}
	
}


sub ConvertDirSubDir
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
			$tmp2 = lc($filename); 
			$tmp = $_[0]."/".$filename;
			$pth2 = $_[1]."/".$filename;
			if (-d $tmp)
			{
				ConvertDirSubDir($tmp,$pth2);
			}
			if (-f $tmp)
			{
				local $j=-1;
				for($i=0; $i <= $#filname; $i++)
				{
					if ($tmp2 eq $filname[$i])
					{
						$j = $i;
						last;
					}
				}
				if ($j != -1)
				{
					system("cp $tmp $pth2");
				}
			}
		}
    
	}
}