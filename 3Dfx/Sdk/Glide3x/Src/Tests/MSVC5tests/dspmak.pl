$dodir="";
do{
    print "Please enter the directory to process:\n";
    $dodir=<STDIN>
}while($dodir eq "");

chomp $dodir;

($dodir ne "") || die "Nothing to do";

opendir(THISDIR,'..') || die "Unable to open this dir";

(@filelist)=readdir(THISDIR);

closedir(THISDIR);

(@cfiles)=grep(/test.*\.c$/,@filelist);

if(! $#cfiles)
{ 
    print "Found no test files\n";
    exit 0;
}

print "Found ",$#cfiles+1," test programs.\n";
print "Please enter the prototype MSVC 5 project (dsp) file:\n";
$protofname="";
do{
    $protofname=<STDIN>;
    chomp $protofname;
    if($protofname !~ /.*\.dsp$/ )
    {
        print "$protofname doesn't look like a project file.\n";
        print "Would you like to exit? (YN)\n";
        $protofname=<STDIN>;
        if($protofname =~ /Y/){ exit 0 };
        $protofname="";
    }
    elsif(!(-e $protofname))
    {
        print "$protofname does not exist.\n";
        $protofname="";
    }
}while($protofname eq "");

open(PROTOFILE,"<$protofname") || die "Unable to open $protofname";
(@slurp)=<PROTOFILE>;
close(PROTOFILE);

$line=$slurp[0];
($name)=($line =~ /Name=\"([^\"]*)\"/);
print "The name in $protofname is $name\n";

foreach $file (@cfiles)
{
    ($filestub,$crap) = split(/\./,$file);

    $dspfile= $filestub . '.dsp';
    if(! -e $dspfile)
    {
        if(open(DSP,">$dspfile"))
        {
            (@slurpc)=(@slurp);
            foreach $line (@slurpc)
            {
                $line =~ s/$name/$filestub/g;
                print DSP $line;
            }
            close (DSP);
        }
        else
        {
            print "Unable to open $dspfile\n";
        }
    }
    else
    {
        print "$dspfile already exists\n";
    }
}
