#!/usr/bin/perl -w

require 5.004;


sub normalizePath($)
{
    my ($path) = @_;
    
    # no double //'s or /./
    0 while $path =~ s|/\.?/|/|g;
    
    # eliminate embedded ..'s
    0 while $path =~ s|[^/\.]+/\.\./||g;
    
    # strip off leading / or ./
    $path =~ s|^\.?/||;

    return $path;
}


sub doScript($) 
{
    my ($cmdfile) = @_;

#    if (system("sh", "<", $cmdfile) != 0)
    if (system("sh", $cmdfile) != 0)
    	{ warn "Errors occurred in executing script: $cmdfile\n"; }
}


use Cwd;
use Getopt::Long;

$Getopt::Long::ignorecase = 0;

# process command line options

$mode = 755;
$owner = "root";
$group = "sys";
@idbs = ();
$isDirectory = 0;
if (defined $ENV{RAWIDB})  
    { $rawidb = $ENV{RAWIDB}; }
if (defined $ENV{ROOT})  
    { $root = $ENV{ROOT}; }
else
    { $root = ""; }

GetOptions(
    "m=s" => \$mode,
    "u=s" => \$owner, 
    "g=s" => \$group, 
    "idb=s" => \@idbs, 
    "src=s" => \$srcPath, 
    "f=s" => \$dstPath, 
    "F=s" => \$dstPath, 
    "rawidb=s" => \$rawidb, 
    "root=s" => \$root, 
    "dir" => \$isDirectory);

if (!defined $dstPath) {
    if (!$isDirectory) { die "install: must supply -F\n"; }
    $dstPath = "";
}
else {
    $dstPath =~ s@/?(.*)/?$@$1@; 
}

if (defined $rawidb) {
    # working in idb mode, don't copy files

    $wd = cwd();
    if (!defined $ENV{SRC}) {
	$src = $wd;
	if (! ($src =~ s@/src(/.*?)?@/src@))
    	    { die "install: SRC not set or not under directory named 'src'\n"; }
    }
    else {
	$src = $ENV{SRC};
    }

    open(IDB, ">>$rawidb") || die "couldn't create $rawidb\n";

    # figure out path prefix

    $prefix = $wd;
    $prefix =~ s@^$src/@@;

    # process each file

    foreach $file (@ARGV) {
	$srcFile = defined $srcPath? $srcPath : $file;
	print IDB $isDirectory? "d": "f"," $mode $owner $group ",
    		  normalizePath("$dstPath/$file")," ",
    		  normalizePath("$prefix/$srcFile");
	if (@idbs) { print IDB " @idbs"; }
	print IDB "\n";
    }
}
else {
    # normal install mode, create a script to copy the files

    $cmdfile = "instcmds$$.sh";
    open(CMD, ">$cmdfile") || die "couldn't open temp file: $cmdfile\n";

    foreach $file (@ARGV) {
	$srcFile = defined $srcPath? $srcPath : $file;
	$dstFile = $root."/".normalizePath("$dstPath/$file");
	
	# make destination directory (if it's missing)
	$dstDir = $dstFile;
	if (!$isDirectory) { $dstDir =~ s|[^/]+$||; }
	if (! ($dstDir =~ m|^([a-zA-Z]:)?/$|)) 
	    { print CMD "mkdir -p $dstDir\n"; }

	# and copy the file
	if (!$isDirectory) 
	    { print CMD "cp -f $srcFile $dstFile\n"; }
    }

    close(CMD);

    # and execute the script

    doScript($cmdfile);
    unlink $cmdfile;
}
