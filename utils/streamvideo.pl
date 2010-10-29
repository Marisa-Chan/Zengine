#!/usr/bin/perl

$prog="~/Projects/unraw/zork_adpcm8";


$#a=-1;

opendir ( DIR, "./SCRIPTS" );
while( ($filename = readdir(DIR))){
     if (!($filename eq "..") and !($filename eq "."))
	{
	$a[$#a+1]=$filename;
	}
}
closedir(DIR);

$#b=-1;
for ($i=0; $i<=$#a; $i++)
{
	@z=`grep pana ./SCRIPTS/$a[$i]`;
	if ($#z>-1)
	{
	$b[$i]=1;
	}
	else
	{
	$b[$i]=0;
	}
}

$#d=-1;
@c=`find ./ | grep -i avi`;
for ($i=0; $i<=$#c; $i++)
{
chomp($c[$i]);
$ind = rindex ($c[$i],"/");
$d[$i]= uc(substr ($c[$i],$ind+1));
}



for ($i=0; $i<=$#a; $i++)
{
@z = `grep -i streamvideo ./SCRIPTS/$a[$i] | grep -i \.avi`;
if ($#z>-1)
{
for ($j=0; $j<=$#z; $j++)
{
	chomp($z[$j]);
	$z[$j]=uc ($z[$j]);
	$fl=substr ( $z[$j], index($z[$j],".AVI")-8,12);
	for ($q=0; $q<=$#d; $q++)
	{
		if ($d[$q] eq $fl)
		{
			print "$prog \"$c[$q]\" \n";
			#system("$prog \"$c[$q]\" ");
			
			$ind=rindex($c[$q],".");
			$fname=substr($c[$q],0,$ind);
			
			system("mplayer -dumpaudio \"$c[$q]\"");
			system("$prog stream.dump $fname.wav 1 22050 1");
			system("rm stream.dump");
			#system("mencoder $c[$q] -of mpeg -mpegopts format=mpeg1 -audiofile stream.wav -oac mp3lame -lameopts abr:br=128:mode=1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=3 -vf eq2=1.0:1.11:0.0,softskip,harddup -fps 15 -ofps 30 -o $fname.mpg");
			system("mencoder $c[$q] -of mpeg -mpegopts format=mpeg1 -nosound -ovc lavc -lavcopts vcodec=mpeg1video:keyint=3 -vf eq2=1.0:1.11:0.0,softskip,harddup -fps 15 -ofps 30 -o $fname.mpg");
			#system("rm stream.wav");
#			system("~/work/videoconv.pl 0 \"$c[$q]\"");

		
		last;
		}
	}
}
}
}



