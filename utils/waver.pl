#!/usr/bin/perl

@a=`find ./ | grep -i "raw"`;

for ($i=0; $i<=$#a; $i++)
{
	chomp($a[$i]);
        $ind=rindex($a[$i],".");
	$subcp=substr($a[$i],0,$ind);
	#print $subcp."\n";
	$later=uc(substr($subcp,length($subcp)-1,1));
	if ($later eq "P")
	{
	#	print "mono\n";
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 22050 0");
	}
	elsif ($later eq "Q")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 22050 1");
	#	print "stereo\n";
	}
	elsif ($later eq "U")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 44100 1");
	#	print "stereo\n";
	}
	elsif ($later eq "K")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 11025 1");
	#	print "stereo\n";
	}
	elsif ($later eq "A")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 0 22050 0");
	#	print "stereo\n";
	}
	else 
	{
		print "$a[$i]\n";
	}
};

@a=`find ./ | grep -i "src"`;

for ($i=0; $i<=$#a; $i++)
{
	chomp($a[$i]);
        $ind=rindex($a[$i],".");
	$subcp=substr($a[$i],0,$ind);
	#print $subcp."\n";
	$later=uc(substr($subcp,length($subcp)-1,1));
	if ($later eq "P")
	{
	#	print "mono\n";
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 22050 0");
	}
	elsif ($later eq "Q")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 22050 1");
	#	print "stereo\n";
	}
	elsif ($later eq "U")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 44100 1");
	#	print "stereo\n";
	}
	elsif ($later eq "K")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 11025 1");
	#	print "stereo\n";
	}
	elsif ($later eq "A")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 0 22050 1");
	#	print "stereo\n";
	}
	else 
	{
		print "$a[$i]\n";
	}
};

@a=`find ./ | grep -i "ifp"`;

for ($i=0; $i<=$#a; $i++)
{
	chomp($a[$i]);
        $ind=rindex($a[$i],".");
	$subcp=substr($a[$i],0,$ind);
	#print $subcp."\n";
	$later=uc(substr($subcp,length($subcp)-1,1));
	if ($later eq "P")
	{
	#	print "mono\n";
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 22050 0");
	}
	elsif ($later eq "Q")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 22050 1");
	#	print "stereo\n";
	}
	elsif ($later eq "U")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 44100 1");
	#	print "stereo\n";
	}
	elsif ($later eq "K")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 1 11025 1");
	#	print "stereo\n";
	}
	elsif ($later eq "A")
	{
		system("~/Projects/unraw/zork_adpcm8 \"$a[$i]\" \"$subcp.wav\" 0 22050 1");
	#	print "stereo\n";
	}
	else 
	{
		print "$a[$i]\n";
	}
};
