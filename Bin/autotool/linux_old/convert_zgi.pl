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

if ($#ARGV < 1)
{
	printf("Usage:\nconvert_zgi.pl   /path/to/original_game   /dest/path\n");
	exit();
}

$dirname = $ARGV[0];

$en=substr($dirname,-1,1);
if (($en eq "\\") or ($en eq "/")) 
	{chop $dirname;}

$dirname2 = $ARGV[1];
$en=substr($dirname2,-1,1);
if (($en eq "\\") or ($en eq "/")) 
	{chop $dirname2;}
	
if (! -f $dirname2)
	{mkdir $dirname2;}
	
mkdir $dirname2."/RESOURCES";

system("./to_upper.pl \"$dirname\"");

system("./converter.pl ./files_zgi.db \"$dirname\" \"$dirname2/RESOURCES\" simple");

mkdir $dirname2."/FONTS";
mkdir $dirname2."/SCRIPTS";
mkdir $dirname2."/CURSOR";
mkdir $dirname2."/SUBTITLE";
mkdir $dirname2."/ADDON";
mkdir $dirname2."/DEATH";

system("cp -f ./res/arial.ttf \"$dirname2/FONTS/arial.ttf\"");
system("cp -f ./res/cour.ttf \"$dirname2/FONTS/cour.ttf\"");
system("cp -f ./res/ZORDEATH.TTF \"$dirname2/FONTS/ZORDEATH.TTF\"");
system("cp -f ./res/ZORKNORM.TTF \"$dirname2/FONTS/ZORKNORM.TTF\"");

system("cp -f ./res/Z4TEXT10.ANM \"$dirname2/ADDON/Z4TEXT10.ANM\"");
system("cp -f ./res/Z4TEXT10.png \"$dirname2/ADDON/Z4TEXT10.png\"");

system("cp -f \"$dirname/ADDON/\"*\".scr\" \"$dirname2/ADDON/\" &>/dev/null");
system("cp -f \"$dirname/ADDON/\"*\".SCR\" \"$dirname2/ADDON/\" &>/dev/null");

system("cp -f \"$dirname/INQUIS.INI\" \"$dirname2/INQUIS.INI\"");

system("cp -f \"$dirname/R.SVR\" \"$dirname2/r.svr\"");

system("./progs/_zfs \"$dirname/SCRIPTS.ZFS\" \"$dirname2/SCRIPTS/\"");
system("./progs/_zfs \"$dirname/SUBTITLE.ZFS\" \"$dirname2/SUBTITLE/\"");
system("./progs/_zfs \"$dirname/CURSOR.ZFS\" \"$dirname2/CURSOR/\"");
system("./progs/_zfs \"$dirname/DEATH.ZFS\" \"$dirname2/DEATH/\"");


if (! -f "./temp")
	{mkdir "./temp";}
	
opendir (DIR, "$dirname2/SUBTITLE");
local @files = readdir(DIR);
closedir(DIR);

foreach(@files)
{
	$flnm = lc($_);
	if (substr($flnm,-3) eq "txt")
	{
		system("./progs/_txt \"$dirname2/SUBTITLE/$_\" ./temp/tmp.txt");
		system("mv -f ./temp/tmp.txt \"$dirname2/SUBTITLE/$_\"");
	}
}

opendir (DIR, "$dirname2/DEATH");
local @files = readdir(DIR);
closedir(DIR);

foreach(@files)
{
	$flnm = lc($_);
	if (substr($flnm,-3) eq "str")
	{
		system("./progs/_txt \"$dirname2/DEATH/$_\" ./temp/tmp.txt");
		system("sed \"s/POINT\\ 14/POINT\\ 12/g\" ./temp/tmp.txt > ./temp/tmp2.txt");
		system("mv -f ./temp/tmp2.txt ./temp/tmp.txt");
		system("mv -f ./temp/tmp.txt \"$dirname2/DEATH/$_\"");
	}
}

opendir (DIR, "$dirname2/CURSOR");
local @files = readdir(DIR);
closedir(DIR);

foreach(@files)
{
	if (-e "./temp/tmp.tga")
	{
		system("rm -f ./temp/tmp.tga");
	}
	
	$flnm = lc($_);
	if (substr($flnm,-3) eq "zcr")
	{
		$newname = Replacer($_,".zcr","");
		system("./progs/_zcr \"$dirname2/CURSOR/$_\" ./temp/tmp.tga ./temp/tmp.pnt");
		system("$imgmgk ./temp/tmp.tga ./temp/tmp.png");
		system("rm -f ./temp/tmp.tga");
		system("mv -f ./temp/tmp.png \"$dirname2/CURSOR/$newname.png\"");
		system("mv -f ./temp/tmp.pnt \"$dirname2/CURSOR/$newname.point\"");
		system("rm -f \"$dirname2/CURSOR/$_\"");
	}
	elsif (substr($flnm,-3) eq "tga")
	{
		$newname = Replacer($_,"tga","png");
		system("./progs/_tgz \"$dirname2/CURSOR/$_\" ./temp/tmp.tga");
		if (-e "./temp/tmp.tga")
		{
			system("$imgmgk ./temp/tmp.tga \"$dirname2/CURSOR/$newname\"");
			system("rm -f ./temp/tmp.tga");
		}
		system("rm -f \"$dirname2/CURSOR/$_\"");
	}
}

rmdir "./temp";

system("./progs/_txt \"$dirname/INQUIS.STR\" \"$dirname2/INQUIS.STR\"");

open FILE,">","$dirname2/Zork.dir";

print FILE "ADDON\n";
print FILE "SCRIPTS\n";
print FILE "CURSOR\n";
print FILE "SCRIPTS\n";
print FILE "SUBTITLE\n";
print FILE "RESOURCES\n";
print FILE "DEATH\n";

close FILE;

system("cp -f ./res/Zengine_zgi \"$dirname2/Zengine\"");

exit();


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
