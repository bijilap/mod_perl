use ExtUtils::MakeMaker;

use lib qw(../lib);
use Apache::src ();
my $src = Apache::src->new;

WriteMakefile(
    NAME	=> "Apache::Constants",
    VERSION_FROM => "Constants.pm",
    'INC'	=> $src->inc,
    'TYPEMAPS'  => $src->typemaps,
);

