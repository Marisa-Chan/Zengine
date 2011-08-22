#!/usr/bin/perl

if ($#ARGV < 0)
{
	printf("Usage:\nscript.pl   path/scripts_dir\n");
	exit();
}

for ($ii=0;$ii<=$#ARGV; $ii++)
{
$dirname = $ARGV[$ii];

$en=substr($dirname,length($dirname)-1,1);
if (($en eq "\\") or ($en eq "/")) 
	{chop $dirname;}

ReadDirSubDir($dirname);
}
	
exit();




sub ReadDirSubDir
{
	local $tmp;
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
			if (-d $tmp)
			{
				ReadDirSubDir($tmp);
			}
			if (-f $tmp)
			{
				if (index(lc($filename),"scr") != -1)
					{ParseScriptFile($tmp);}
			}
		}
    
	}
}

sub PrepareString
{
	local $str = $_[0];
	local $i;
	chomp($str);
	$str = lc($str);
	for ($i=0; $i<length($str);$i++)
	{
		local $chr = substr($str,$i,1);
		if ($chr ne " " && $chr ne "\t")
		{
			$str = substr($str,$i);
			last;
		}
	}
	local $indx = 	index($str,"#");
	$str = substr($str,0,$indx);
	return $str;
}

sub GetParams
{
	local $str = $_[0];
	local $i;
	for ($i=0; $i<length($str);$i++)
	{
		local $chr = substr($str,$i,1);
		if ($chr eq "(")
		{
			$str = substr($str,$i+1);
			last;
		}
	}
	for ($i=0; $i<length($str);$i++)
	{
		local $chr = substr($str,$i,1);
		if ($chr eq ")")
		{
			$str = substr($str,0,$i);
			last;
		}
	}
	return $str;
}

sub ParseScriptFile
{
	open(handle,"<",$_[0]);
	local @txt = <handle>;
	close(handle);
	local $inwhat = 0;
	local $render = 0; #0 - flat / 1 - pana / 2 - tilt 
	
	local $#f_files = -1;
	local $#m_files = -1;
	
	foreach(@txt)
	{
		$cur = PrepareString($_);
		if ($inwhat == 0) # Main
		{
			if (substr($cur,0,length("puzzle")) eq "puzzle")
			{
				$inwhat = 1;
			}
			elsif (substr($cur,0,length("control")) eq "control")
			{
				local $in = index($cur,"flat");
				if ($in != -1)
				{
					$render = 0;
				}
				local $in = index($cur,"pana");
				if ($in != -1)
				{
					$render = 1;
				}
				local $in = index($cur,"tilt");
				if ($in != -1)
				{
					$render = 2;
				}
				$inwhat = 2;
			}
		}
		elsif ($inwhat == 1) #In puzzle
		{
			if (substr($cur,0,length("criteria")) eq "criteria")
			{
				$inwhat = 3;
			}
			elsif (substr($cur,0,length("results")) eq "results")
			{
				$inwhat = 4;
			}
			elsif (substr($cur,0,length("flags")) eq "flags")
			{
				$inwhat = 5;
			}
			elsif (substr($cur,0,length("}")) eq "}")
			{
				$inwhat = 0;
			}
		}
		elsif ($inwhat == 2) #In control
		{
			if (substr($cur,0,length("}")) eq "}")
			{
				$inwhat = 0;
			}
		}
		elsif ($inwhat == 3) #In criteria
		{
			if (substr($cur,0,length("}")) eq "}")
			{
				$inwhat = 1;
			}
		}
		elsif ($inwhat == 4) #In results
		{
			if (substr($cur,0,length("}")) eq "}")
			{
				$inwhat = 1;
			}
			else
			{
				if (index($cur,"set_screen") != -1)
				{
					push (@f_files,GetParams($cur));
				}
				elsif(index($cur,"set_partial_screen") != -1)
				{
					local @a = split(/ /,GetParams($cur));
					push (@f_files,$a[2]);
				}
				elsif(index($cur,"animplay") != -1)
				{
					local @a = split(/ /,GetParams($cur));
					push (@f_files,$a[0]);
				}
				elsif(index($cur,"streamvideo") != -1)
				{
					local @a = split(/ /,GetParams($cur));
					push (@f_files,$a[0]."_strm");
				}
				elsif(index($cur,"animpreload") != -1)
				{
					local @a = split(/ /,GetParams($cur));
					push (@f_files,$a[0]);
				}
				elsif(index($cur,"universe_music") != -1)
				{
					local @a = split(/ /,GetParams($cur));
					push (@m_files,$a[1]);
				}
				elsif(index($cur,"music") != -1)
				{
					local @a = split(/ /,GetParams($cur));
					push (@m_files,$a[1]);
				}
				elsif(index($cur,"syncsound") != -1)
				{
					local @a = split(/ /,GetParams($cur));
					push (@m_files,$a[2]);
				}
			}
		}
		elsif ($inwhat == 5) #In flags
		{
			if (substr($cur,0,length("}")) eq "}")
			{
				$inwhat = 1;
			}
		}
		else
		{
			die "SOS! in file ".$_[0]."\n";
		}
	}
	
	foreach(@f_files)
	{
		local $dig = substr($_,-3); 
		if (substr($_,-8) eq "avi_strm")
		{
			print substr($_,0,-5)." 2 5\n";
		}
		elsif ($dig eq "avi")
		{
			print "$_ 2 $render\n";
		}
		elsif ($dig eq "tga")
		{
			print "$_ 0 $render\n";
		}
		elsif ($dig eq "rlf")
		{
			print "$_ 1 $render\n";
		}
		else
		{
			print "$_ 3 $render\n";
		}
	}
	foreach(@m_files)
	{
		local $dig = substr($_,-5,1); 
		if ($dig eq "p")
		{
			print "$_ 4 0\n";
		}
		elsif ($dig eq "q")
		{
			print "$_ 4 1\n";
		}
		elsif ($dig eq "u")
		{
			print "$_ 4 2\n";
		}
		elsif ($dig eq "k")
		{
			print "$_ 4 3\n";
		}
		elsif ($dig eq "a")
		{
			print "$_ 4 4\n";
		}
		else
		{
			print "$_ 4 5\n";
		}
	}
}