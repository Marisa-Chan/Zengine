#!/usr/bin/perl

@a=`find ./ | grep -i "tga"`;

$prog="~/Projects/unlzss/bin/Release/unlzss";

for ($i=0; $i<=$#a; $i++)
{
	chomp($a[$i]);
        $ind=rindex($a[$i],"/");
	$subcp=substr($a[$i],$ind+1);
	$ind=rindex($a[$i],".");
	$fname=substr($a[$i],0,$ind);
	#print $subcp."\n";

	$pss=`grep -m 1 -r -i "$subcp" ./SCRIPTS/`;
	chomp($pss);
	@b=split(/:/,$pss);

	
	
	if ($#b > -1)
	{
	@c=`grep pana "$b[0]"`;
	system("$prog $a[$i] ");
	
	if ($#c >-1)
	   {
		system("convert -transpose $a[$i] $fname.png");
	   }
	else
	   {
		system("convert $a[$i] $fname.png");
	   }
	#print $b[0]." $c[1]"."\n";
	}
	else
	{
	print $subcp."\n";
	}
	
}

