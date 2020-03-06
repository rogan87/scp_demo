#!c:/perl/bin/perl.exe
##############################################################################
# Copyright (C) 2008, MDS Technology Co., Ltd. All rights reserved.
#
# MDS Technology Co., Ltd.
# 15F., Kolon Digital Tower Bilant, Guro3-dong, Guro-gu, 
# Seoul,  Korea, 152-777
# 
##############################################################################

##############################################################################
# required modules
##############################################################################
# find the location of the script
my @homePath = split /\\|\//, $0;
my $scriptName = pop @homePath;
my $scriptPath = join '/', @homePath;

# look for modules where the script lives
push @INC, $scriptPath;

use strict;
require NewOpts;
use Carp;

##############################################################################
# define the command line options and their attributes
##############################################################################
my $options = <<OPTS
printObjNames    is_boolean
idName           is_boolean
srcDir           is_filespec must_exist has_default(./Source)
objDir           is_filespec must_exist has_default(./Object)
suiteName        is_filespec has_default(not_a_suite)
arglist          arg_count(>0) is_filespec has_description(testName.c)
OPTS
;

# parse the command line
my $opts = new NewOpts($options);

printf "SRCDIR: %s\n", $opts->srcDir    if $opts->verbose(2);
printf "OBJDIR: %s\n", $opts->objDir    if $opts->verbose(2);
printf "SUITE:  %s\n", $opts->suiteName if $opts->verbose(2);

# get the test file names
my @testFiles = $opts->getArgList;
my $testIds   = getTestIds(\@testFiles);

my $idFile;
if($opts->suiteName ne "not_a_suite")
{
    $idFile = $opts->objDir . "/" . $opts->suiteName . "_ID.c";
}
else
{
    $idFile = $opts->objDir . "/" . $testIds->[0] . "_ID.c";
}

print "ID: $idFile\n" if $opts->verbose(2);

my $IDFILE;
open($IDFILE, "> $idFile") || die "couldn't open $idFile";

# generate the TP_*_ID.c file
printIdFile($IDFILE, $testIds);
close $IDFILE;

if($opts->printObjNames)
{
    # print the object file names for make
    foreach my $testId (@{$testIds})
    {
        print $opts->objDir . "/" . $testId . "_ID.o";
    }
}

##############################################################################
exit; # end of processing ####################################################
##############################################################################

########################################################################
#################### Subroutines begin here ############################
########################################################################

########################################################################
# getTestIds
########################################################################
sub getTestIds
{
    my ($testSrcFiles) = @_;
    my @testIds;
    my $testId;


    foreach my $testSrc ( @{$testSrcFiles} )
    {
        my @pathParts = split '/', $testSrc;
        $testId = pop @pathParts;
        if($opts->idName)
        {
            $testId =~ s/_ID.[co]$//;
        }
        else
        {
            $testId =~ s/.[co]$//;
        }
        push @testIds, $testId;
    }

    return \@testIds;
}

########################################################################
# outputFileName
########################################################################
sub outputFileName
{
    my ($testSrcFile, $extension) = @_;
    my @idFilePath = split "/", $testSrcFile;
    my $fileName   = pop @idFilePath;
    $fileName      =~ s/\.c$/$extension/;

    return $opts->objDir . "/" . $fileName;
}

########################################################################
# printIdFile - print the ID file
########################################################################
sub printIdFile
{
    my ($out, $testIds) = @_;
    my $testCount       = 0;

    print $out "/* THIS FILE IS AUTOMATICALLY GENERATED: do not edit */\n";
    print $out "#include <TFX/TFX.h>\n\n";
    print $out "/* TEST_ID declarations */\n";
    
    # print the extern definitions
    foreach my $testId (@{$testIds})
    {
        print $out "extern TEST_ID " . $testId . "_ID;\n";
        $testCount ++
    }

    print $out "\nint g_TestCount = $testCount;\n\n";
    print $out "TEST_ID * g_TestIds[] =\n";
    print $out "{\n";

    # print the TEST_ID references
    foreach my $testId (@{$testIds})
    {
        print $out "\t&" . $testId . "_ID,\n";
    }

    print $out "};\n\n";
;
}