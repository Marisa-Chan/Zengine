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

$imgmgk=`which convert 2>/dev/null`;
chomp($imgmgk);
if (! -e $imgmgk)
{
	print("ImageMagick's convert not found.\n");
	exit();
}

if (! -e "./progs/_rlf")
{
	print("_rlf tool not found.\n");
	exit();
}

if (! -e "./progs/_txt")
{
	print("_txt tool not found.\n");
	exit();
}

if (! -e "./progs/_sfx")
{
	print("_sfx tool not found.\n");
	exit();
}

if (! -e "./progs/_tgz")
{
	print("_tgz tool not found.\n");
	exit();
}

if (! -e "./progs/_zcr")
{
	print("_zcr tool not found.\n");
	exit();
}

if (! -e "./progs/_zfs")
{
	print("_zfs tool not found.\n");
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

$toonedir = 0;
$simpleoutdir = $dirname2;
if ($#ARGV == 3)
{
	if ($ARGV[3] eq "simple")
		{$toonedir = 1;}
}

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
if ($toonedir == 0)
{
print "Creating directories structure ";
CreateDestDirs($dirname,$dirname2);
print "[OK]\n";
}
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
			if ($toonedir == 1)
			{$pth2 = $simpleoutdir."/".$filename;}
			else
			{$pth2 = $_[1]."/".$filename;}
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
					if (-s $tmp > 16) #files smaller than 16 bytes - dummy
					{
						if ($filtp[$j] == 0) #tgz
						{
							ConvertTGZ($tmp,$pth2,$filarg[$j]);
						}
						elsif ($filtp[$j] == 1) # rlf
						{
							ConvertRLF($tmp,$pth2,$filarg[$j]);
						}
						elsif ($filtp[$j] == 2) # avi
						{
							ConvertAVI($tmp,$pth2,$filarg[$j]);
						}
						elsif ($filtp[$j] == 4) # sound
						{
							ConvertSFX($tmp,$pth2,$filarg[$j]);
						}
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

sub ConvertTGZ #0 - src file, 1 - dst file, 2 - renderer 
{
	local $btst = "";
	if (! -f "./temp")
		{mkdir "./temp";}
	system("./progs/_tgz \"$_[0]\" ./temp/tmp.tga");
	local $dst = Replacer($_[1],"tga","png");
		
	if ($_[2] eq "0") # flat
	{
		$btst = `$imgmgk ./temp/tmp.tga ./temp/tmp.png 2>&1`;
	}
	elsif ($_[2] eq "1") # pana
	{
		$btst = `$imgmgk -transpose ./temp/tmp.tga ./temp/tmp.png 2>&1`;
	}
	elsif ($_[2] eq "2") # tilt
	{
		$btst = `$imgmgk ./temp/tmp.tga ./temp/tmp.png 2>&1`;
	}
	if ($btst ne "")
		{print "$_[0] not converted!\n";}
	system("mv -n ./temp/tmp.png \"$dst\"");
	system("rm -f ./temp/tmp.tga ./temp/tmp.png");
}

sub ConvertRLF #0 - src file, 1 - dst file, 2 - renderer 
{
	local $btst = "";
	if (! -f "./temp")
		{mkdir "./temp";}
	local $dst = Replacer($_[1],"rlf","png");
	local $dst2 = Replacer($_[1],"rlf","anm");
		
	if ($_[2] eq "0") # flat
	{
		system("./progs/_rlf 0 \"$_[0]\" ./temp/tmp.tga ./temp/tmp.anm");
	}
	elsif ($_[2] eq "1") # pana
	{
		system("./progs/_rlf 1 \"$_[0]\" ./temp/tmp.tga ./temp/tmp.anm");
	}
	elsif ($_[2] eq "2") # tilt
	{
		system("./progs/_rlf 0 \"$_[0]\" ./temp/tmp.tga ./temp/tmp.anm");
	}
	$btst = `$imgmgk ./temp/tmp.tga ./temp/tmp.png 2>&1`;
	if ($btst ne "")
		{print "$_[0] not converted!\n";}
	
	system("mv -n ./temp/tmp.png \"$dst\"");
	system("mv -n ./temp/tmp.anm \"$dst2\"");
	system("rm -f ./temp/tmp.tga ./temp/tmp.png ./temp/tmp.anm");
}

sub ConvertAVI #0 - src file, 1 - dst file, 2 - renderer 
{
	if (! -f "./temp")
		{mkdir "./temp";}
	local $dst = Replacer($_[1],"avi","mpg");
	local $dst2 = Replacer($_[1],"avi","wav");
		
	if ($_[2] eq "0") # flat
	{
		system("$mencoder \"$_[0]\" -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=1 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=90,flip,rotate=90,flip -fps 15 -ofps 30 -nosound -o ./temp/tmp.mpg &>/dev/null");
	}
	elsif ($_[2] eq "1") # pana
	{
		system("$mencoder \"$_[0]\" -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=1 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=90,flip -fps 15 -ofps 30 -nosound -o ./temp/tmp.mpg &>/dev/null");
	}
	elsif ($_[2] eq "2") # tilt
	{
		system("$mencoder \"$_[0]\" -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=1 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=90,flip,rotate=90,flip -fps 15 -ofps 30 -nosound -o ./temp/tmp.mpg &>/dev/null");
	}
	elsif ($_[2] eq "5") # streamvideo (with audio!)
	{
		system("$mencoder \"$_[0]\" -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=1 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=90,flip,rotate=90,flip -fps 15 -ofps 30 -nosound -o ./temp/tmp.mpg &>/dev/null");
		system("$mpla -dumpaudio \"$_[0]\" -dumpfile ./temp/tmp.dump &>/dev/null");
		system("./progs/_sfx ./temp/tmp.dump ./temp/tmp.wav 1 22050 1");
		system("rm -f ./temp/tmp.dump");
	}
	
	if (-e "./temp/tmp.mpg")
		{system("mv -n ./temp/tmp.mpg \"$dst\"");}
	if (-e "./temp/tmp.wav")
		{system("mv -n ./temp/tmp.wav \"$dst2\"");}
		
	system("rm -f ./temp/tmp.wav ./temp/tmp.mpg");
}

sub ConvertSFX #0 - src file, 1 - dst file, 2 - type
{
	if (! -f "./temp")
		{mkdir "./temp";}
	local $dst = Replacer($_[1],"ifp","wav");
	$dst = Replacer($dst,"raw","wav");
	$dst = Replacer($dst,"src","wav");
	
	if ($_[2] eq "0") # adpcm 22050 mono
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 22050 0");
	}
	elsif ($_[2] eq "1") # adpcm 22050 stereo
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 22050 1");
	}
	elsif ($_[2] eq "2") # adpcm 44100 stereo
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 44100 1");
	}
	elsif ($_[2] eq "3") # adpcm 11025 stereo
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 11025 1");
	}
	elsif ($_[2] eq "4") # raw 22050 stereo
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 0 22050 0");
	}
	elsif ($_[2] eq "10") # adpcm 8000 mono 8bit
	{
		#system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 8000 0");
		print("Not supported sfx type $_[2]\n");
	}
	elsif ($_[2] eq "11") # adpcm 8000 stereo 8bit
	{
		#system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 8000 1");
		print("Not supported sfx type $_[2]\n");
	}
	elsif ($_[2] eq "12") # adpcm 8000 mono 16bit
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 8000 0");
	}
	elsif ($_[2] eq "13") # adpcm 8000 stereo 16bit
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 8000 1");
	}
	elsif ($_[2] eq "14") # adpcm 11025 mono 8bit
	{
		#system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 11025 0");
		print("Not supported sfx type $_[2]\n");
	}
	elsif ($_[2] eq "15") # adpcm 11025 stereo 8bit
	{
		#system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 11025 1");
		print("Not supported sfx type $_[2]\n");
	}
	elsif ($_[2] eq "16") # adpcm 11025 mono 16bit
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 11025 0");
	}
	elsif ($_[2] eq "17") # adpcm 11025 stereo 16bit
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 11025 1");
	}
	elsif ($_[2] eq "18") # adpcm 22050 mono 8bit
	{
		#system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 22050 0");
		print("Not supported sfx type $_[2]\n");
	}
	elsif ($_[2] eq "19") # adpcm 22050 stereo 8bit
	{
		#system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 22050 1");
		print("Not supported sfx type $_[2]\n");
	}
	elsif ($_[2] eq "20") # adpcm 22050 mono 16bit
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 22050 0");
	}
	elsif ($_[2] eq "21") # adpcm 22050 stereo 16bit
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 22050 1");
	}
	elsif ($_[2] eq "22") # adpcm 44100 mono 8bit
	{
		#system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 44100 0");
		print("Not supported sfx type $_[2]\n");
	}
	elsif ($_[2] eq "23") # adpcm 44100 stereo 8bit
	{
		#system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 44100 1");
		print("Not supported sfx type $_[2]\n");
	}
	elsif ($_[2] eq "24") # adpcm 44100 mono 16bit
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 44100 0");
	}
	elsif ($_[2] eq "25") # adpcm 44100 stereo 16bit
	{
		system("./progs/_sfx \"$_[0]\" ./temp/tmp.wav 1 44100 1");
	}

	system("mv -n ./temp/tmp.wav \"$dst\"");
	system("rm -f ./temp/tmp.wav");
}