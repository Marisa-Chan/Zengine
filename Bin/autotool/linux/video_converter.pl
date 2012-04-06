#!/usr/bin/perl

$mencoder=`which mencoder 2>/dev/null`;
chomp($mencoder);
if (! -e $mencoder)
{
	print("Mencoder not found.\n");
	exit();
}

$mpla=`which mplayer 2>/dev/null`;
chomp($mpla);
if (! -e $mpla)
{
	print("Mplayer not found.\n");
	exit();
}

if (! -e "./progs/_sfx")
{
	print("_sfx tool not found.\n");
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
$#filarg = -1;

foreach(@txt)
{
	chomp $_;
	@b = split(/ /,$_);
	push @filname,$b[0];
	push @filarg,$b[1];	
}


print "Start conversion\n";
ConvertDirSubDir($dirname,$dirname2);
print "Conversion done\n";
exit();


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
				ConvertDirSubDir($tmp,$_[1]);
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
					if (-s $tmp > 16) #files smaller than 16 bytes - dummy
					{
						ConvertAVI($tmp,$pth2,$filarg[$j]);
					}
					
				}
			}
		}
    
	}
}

sub Replacer
{
	local $tmp = $_[0];
	local $tmp2 = lc($_[0]);
	local $tmp3 = lc($_[1]);
	local $tmp4 = $_[2];
	local $val = index($tmp2,$tmp3);
	if ($val != -1)
		{return substr($tmp,0,$val).$tmp4;}
	else
		{return $tmp;}	
}

sub GetFileName
{
	local $tmp1 = rindex($_[0],"\\");
	local $tmp2 = rindex($_[0],"/");
	if ($tmp1 < $tmp2)
		{$tmp1 = $tmp2;}
	
	return substr($_[0],$tmp1+1);
	
}

sub ConvertAVI #0 - src file, 1 - dst file, 2 - renderer 
{
	if (! -f "./temp")
		{mkdir "./temp";}
	local $dst = Replacer($_[1],"avi","mpg");
	local $dst2 = Replacer($_[1],"avi","wav");
	local $tempname = GetFileName($_[0]);
		
	if ($_[2] eq "0") # flat
	{
		system("$mencoder \"$_[0]\" -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=1 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=90,flip,rotate=90,flip -fps 15 -ofps 30 -nosound -o ./temp/$tempname.mpg &>/dev/null");
	}
	elsif ($_[2] eq "1") # pana
	{
		system("$mencoder \"$_[0]\" -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=1 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=90,flip -fps 15 -ofps 30 -nosound -o ./temp/$tempname.mpg &>/dev/null");
	}
	elsif ($_[2] eq "2") # tilt
	{
		system("$mencoder \"$_[0]\" -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=1 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=90,flip,rotate=90,flip -fps 15 -ofps 30 -nosound -o ./temp/$tempname.mpg &>/dev/null");
	}
	elsif ($_[2] eq "5") # streamvideo (with audio!)
	{
		system("$mencoder \"$_[0]\" -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=1 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=90,flip,rotate=90,flip -fps 15 -ofps 30 -nosound -o ./temp/$tempname.mpg &>/dev/null");
		system("$mpla -dumpaudio \"$_[0]\" -dumpfile ./temp/$tempname.dump &>/dev/null");
		if (-s "./temp/$tempname.dump" > 44)
		{
		system("./progs/_sfx ./temp/$tempname.dump ./temp/$tempname.wav 1 22050 1");
		}
		system("rm -f ./temp/$tempname.dump");
	}
	
	if (-e "./temp/$tempname.mpg")
		{system("mv -n ./temp/$tempname.mpg \"$dst\"");}
	if (-e "./temp/$tempname.wav")
		{system("mv -n ./temp/$tempname.wav \"$dst2\"");}
		
	system("rm -f ./temp/$tempname.wav ./temp/$tempname.mpg");
}
