use strict;
use warnings FATAL => 'all';

use Apache::Test;
use Apache::TestRequest;
use Apache::TestUtil;

plan tests => 1;

my $expected = "F_O_O_b_a_r_";
my $location = '/TestFilter::out_str_remove';
my $response = GET_BODY $location;
ok t_cmp($expected, $response, "a filter that removes itself");

