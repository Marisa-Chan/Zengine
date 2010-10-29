#!/usr/bin/perl

$b=`echo -n $ARGV[1] | sed "s/\.AVI//g"`;
chomp($b);

if ($ARGV[0] eq 0)
{
system("mencoder $ARGV[1] -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=3 -vf eq2=1.0:1.11:0.0,softskip,harddup -fps 15 -ofps 30 -o $b.mpg");
}
else
{
system("mencoder $ARGV[1] -of mpeg -mpegopts format=mpeg1 -ovc lavc -lavcopts vcodec=mpeg1video:keyint=3 -vf eq2=1.0:1.11:0.0,softskip,harddup,rotate=270,flip -fps 15 -ofps 30 -o $b.mpg");
}
