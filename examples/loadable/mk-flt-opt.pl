local %filter_tree=();
local $dotest=0;
while ($sourcefile = shift) {
  ( $sourcefile=~/^--?t(?:est)/) && ($dotest=1) && next;
  unless (-e $sourcefile && -r $sourcefile && (open SOURCEFILE,$sourcefile)) {
    printf STDERR "can not open file: $sourcefile\n";
    next;
  }
  printf STDERR "processing: $sourcefile\n";
  $line="";
  while (<SOURCEFILE>) {
    chomp;
    $line.=$_;
    ( $line=~s/((?:\\\\)*)\\$/\1/) && next;
    if ($_=~/^[ \t]*(STRING|BOOL|INT|LIST)_OPTION[ \t]*\(([^\(\)]*)\)/) {
      $optiontype=$1;
      $argumentstring=$2;
      ( scalar (@argumentlist = split /[ \t]*,[ \t]*/,$argumentstring) == 5) ||
        (die "$sourcefile($.): Wrong number of arguments for ${optiontype}_OPTION".
             " macro\n");
      ( $filter=get_filter(destring($argumentlist[3],0))) ||
        (die "$sourcefile($.): Filter $argumentlist[3] unknown.\n");
      ( $option=add_option($filter,destring($argumentlist[1],0),$optiontype,
                           destring($argumentlist[4],0))) ||
        (die "$sourcefile($.): ${optiontype}_OPTION: redefinition with".
             " different type or unknown feature.\n");
      $line="";
      next;
    }
    if ($line=~/^[ \t]*((?:EN|DE)CODER|FILTER)[ \t]*\(([^\(\)]*)\)/) {
      $feature_name=$1;
      $argumentstring=$2;
      ( scalar (@argumentlist = split /[ \t]*,[ \t]*/,$argumentstring) == 3) ||
        (die "$sourcefile($.): Wrong number of arguments for $feature_name".
             " macro\n");
      
      ( $filter=add_filter(destring($argumentlist[0],0),$feature_name)) ||
        (die "$sourcefile($.): Invalid Filtername $argumentlist[0] or feature".
             " $feature_name allready used.\n");
      $line="";
      next;
    }
    if ($line=~/^[ \t]*((?:SET|ADD)_(?:DESCRIPTION|DEFAULT))[ \t]*\(([^\(\)]*)\)/) {
      $action=$1;
      $argumentstring=$2;
      ( scalar (@argumentlist = split /[ \t]*,[ \t]*/,$argumentstring) == 3) ||
        (die "$sourcefile($.): Wrong number of arguments for $action".
             " macro\n");
      ( $filter=get_filter(destring($argumentlist[2],0))) ||
        (die "$sourcefile($.): Filter $argumentlist[2] unknown.\n");
      if (($action eq "SET_DESCRIPTION") &&
          (destring($argumentlist[0],0) eq ${$filter}{"NAME"})) {
          ${$filter}{"DESCRIPTION"}=destring($argumentlist[1],1);
        $line="";
        next;
      }
      ( $option=add_to_option($filter,destring($argumentlist[0],0),
                              $action,destring($argumentlist[1],1))) ||
        (die "$sourcefile($.): Invalid option expansion via $action macro".
             " or unknown option.\n");
      $line="";
      next;
    }
    if ($line=~/^[ \t]*ACTIVATE_((?:EN|DE)CODER|FILTER)[ \t]*\(([^\(\)]*)\)/) {
      $feature_name=$1;
      $argumentstring=$2;
      ( scalar (@argumentlist = split /[ \t]*,[ \t]*/,$argumentstring) == 3) ||
        (die "$sourcefile($.): Wrong number of arguments for".
             " ACTIVATE_$feature_name macro\n");
      ( $filter=get_filter(destring($argumentlist[2],0))) ||
        (die "$sourcefile($.): Filter $argumentlist[2] unknown.\n");
      ( $feature=get_feature($filter,$feature_name)) ||
        (die "$sourcefile($.): tried to activate unknown feature $feature_name.\n");
      ( ${$feature}{"ACTIVE"}) && 
        (die "$sourcefile($.): reactivation of $feature_name feature.\n");
      ${$feature}{"ACTIVE"}=1;
      $line="";
      next;
    }
    if ($line=~/^[ \t]*ASPELL_VERSION[ \t]*\(([^\(\)]*)\)/) {
      $argumentstring=$1;
      ( scalar (@argumentlist = split /[ \t]*,[ \t]*/,$argumentstring) == 2) ||
        (die "$sourcefile($.): Wrong number of arguments for".
             " ASPELL_VERSION macro\n");
              destring($argumentlist[0],0)."\n";
      ( $filter=get_filter(destring($argumentlist[0],0))) ||
        (die "$sourcefile($.): Filter $argumentlist[0] unknown.\n");
      ( ($version=destring($argumentlist[1],0)) &&
         ($version=~/^(?:[<>]=?|=)?[0-9][0-9\.]*$/)) ||
        (die "$sourcefile($.): Invalid version string for Aspell version.\n");
      ${$filter}{"ASPELL_VERSION"}=$version;
      $line="";
      next;
    }  
    $line="";
  }
  close SOURCEFILE;
}      
while ($dotest && (($filter_name,$filter)=each %filter_tree)) {
  printf STDERR "Filter: $filter_name \n";
  @filter_keys=();
  @havekeys=keys %{$filter};
  @filter_keys=sort_keys(\@havekeys,"NAME","ASPELL_VERSION","DESCRIPTION",
                                    "ENCODER","FILTER","DECODER");
  @havekeys=();
  for($count_fkeys=0;$count_fkeys < scalar @filter_keys;$count_fkeys++) {
    if ($count_fkeys < 3) {
      printf STDERR "\t$filter_keys[$count_fkeys]:".
                      "${$filter}{$filter_keys[$count_fkeys]}\n";
      next;
    }
    $element=${$filter}{$filter_keys[$count_fkeys]};
    @elem_keys=();
    @havekeys=keys %{$element};
    if ($count_fkeys < 6) {
      printf STDERR "\tFeature: $filter_keys[$count_fkeys]\n";
      @elem_keys=sort_keys(\@havekeys,"USED","ACTIVE","ISOPTION");
    }
    else {
      printf STDERR "\tOption: $filter_keys[$count_fkeys]\n";
      @elem_keys=sort_keys(\@havekeys,"TYPE","DESCRIPTION","ISOPTION","DEFAULT");
    }
    @havekeys=();
    for($count_ekeys=0;$count_ekeys < scalar @elem_keys;$count_ekeys++) {
      $subelement=${$element}{$elem_keys[$count_ekeys]};
      if ($count_ekeys < 3) {
        printf STDERR "\t\t${elem_keys[$count_ekeys]}: $subelement\n";
        next;
      }
      if ($count_fkeys < 6) {
        printf STDERR "\t\tUses option: $elem_keys[$count_ekeys]\n";
        next;
      }
      printf STDERR "\t\t$elem_keys[$count_ekeys]:\n";
      foreach $default_value (@{$subelement}) {
        printf STDERR "\t\t\t$default_value\n";
      }
    }
  }
}
$_="";
( $dotest) && (printf STDERR  "\n\n");
while ($dotest && ($_ !~ /^([yY](?:[Ee][Ss])?|[nN][Oo]?)$/)) {
printf STDERR "Accept filter listing (yes|no): ";
  $_=<STDIN>;
}
( $dotest ) && ($_ =~ /[nN]/) &&
  (die "User abbort ...\n"); 


sub sort_keys{
my ($list_tosort,@preceede)=@_;
my ($sortcount,$eliminate,@locallist);
  @locallist=sort @{$list_tosort};
  for($sortcount=0;$sortcount< scalar @locallist ;$sortcount++) {
    foreach $eliminate (@preceede) {
      if ($locallist[$sortcount] eq $eliminate) {
        splice @locallist,$sortcount,1;
        $sortcount--;
      }
    }
  }
  return (@preceede,@locallist);
}
sub get_filter {
my ($filter_name)=@_;
my ($existing,$filter);
  while (($existing,$filter)=each %filter_tree) {
    if ($existing eq $filter_name) {
      scalar keys %filter_tree;
      return $filter;
    }
  }
  return undef;
}

sub get_option {
my ($filter,$option_name)=@_;
my ($existing,$option);
  while (($existing,$option)=each %{$filter}) {
    if (($existing eq $option_name) && 
        (${$option}{"ISOPTION"} == 1)) {
      scalar keys %{$filter};
      return $option;
    }
  }
  return undef;
}

sub get_feature {
my ($filter,$feature_name)=@_;
my ($existing,$feature);
  $feature_name=~tr/a-z/A-Z/;
  while (($existing,$feature)=each %{$filter}) {
    if (($existing eq $feature_name) && 
        ($existing=~/(?:ENCODER|FILTER|DECODER)/)) {
      scalar keys %{$filter};
      return $feature;
    }
  }
  return undef;
}

sub add_to_feature {
my ($feature,$option_name,$option)=@_;
my ($existing,$missing);
  while (($existing,$missing)=each %{$feature}){
    if ($existing eq $option_name) {
      scalar keys %{$feature};
      return $missing;
    }
  }
  return (${$feature}{$option_name}=$option);
}

sub add_to_option {
my ($filter,$option_name,$operation,$content)=@_;
my ($existing,$option,$element);
  ( $option = get_option($filter,$option_name)) || (return undef);
  unless ($operation=~s/(SET|ADD)_(DESCRIPTION|DEFAULT)/$1/) {
    return undef;
  }
  $element=$2;
  if (($operation eq "SET") && ($element eq "DESCRIPTION")) {
    ${$option}{$element}=$content;
    return $option;
  }
  if (($element eq "DEFAULT")&&
      ((scalar @{${$option}{"DEFAULT"}} == 0) ||
       (($operation eq "ADD") &&
        (${$option}{"TYPE"} eq "LIST")))) {
    push @{${$option}{"DEFAULT"}},$content;
    return $option;
  }
  return undef;
}

sub add_option {
my ($filter,$option_name,$option_type,$feature_name)=@_;
my ($option,$feature);
  unless ($feature=get_feature($filter,$feature_name)) {
    return undef;
  }
  unless ($option=get_option($filter,$option_name)) {
    $option=${$filter}{$option_name}={};
    ${$option}{"DESCRIPTION"}="";
    ${$option}{"DEFAULT"}=[];
    ${$option}{"ISOPTION"}=1;
    ${$option}{"TYPE"}=$option_type;
    ${$option}{"FEATURES"}=[];
  }
  if (${$option}{"TYPE"} ne $option_type) {
    return undef;
  }
  add_to_feature($feature,$option_name,$option);
  $feature_name=~tr/[a-z]/[A-Z]/;
  push @{${$option}{"FEATURES"}},$feature_name;
  return $option;
}

sub add_filter {
my ($filter_name,$feature_name)=@_;
my ($filter,$feature);
  unless (defined $filter_name) {
    return undef;
  }
  unless ($filter=get_filter($filter_name)) {
    $filter=$filter_tree{$filter_name}={};
    ${$filter}{"ENCODER"}={};
    ${$filter}{"FILTER"}={};
    ${$filter}{"DECODER"}={};
    ${$filter}{"NAME"}=$filter_name;
    ${$filter}{"ASPELL_VERSION"}=">=0.50";
    ${$filter}{"DESCRIPTION"}=">=0.50";
    ${${$filter}{"ENCODER"}}{"ACTIVE"}=0;
    ${${$filter}{"ENCODER"}}{"USED"}=0;
    ${${$filter}{"ENCODER"}}{"ISOPTION"}=0;
    ${${$filter}{"FILTER"}}{"ACTIVE"}=0;
    ${${$filter}{"FILTER"}}{"USED"}=0;
    ${${$filter}{"FILTER"}}{"ISOPTION"}=0;
    ${${$filter}{"DECODER"}}{"ACTIVE"}=0;
    ${${$filter}{"DECODER"}}{"USED"}=0;
    ${${$filter}{"DECODER"}}{"ISOPTION"}=0;
  }
  unless ($feature=get_feature($filter,$feature_name)){
    return undef;
  }
  if (${$feature}{"USED"} != 0) {
    return undef;
  }
  ${$feature}{"USED"}=1;
  return $filter;
}

sub set_aspell_version {
my ($filter_name,$version)=@_;
my ($filter);
  unless ($filter=get_filter($filter_name)) {
    return undef;
  }
  unless ($version=~/^(?:<=?|=|>=?)?[0-9][0-9\.]+$/) {
    return undef;
  }
  ${$filter}{"ASPELL_VERSION"}=$version;
  return $filter;
}

sub set_description {
my ($filter_name,$text)=@_;
my ($filter);
  unless ($filter=get_filter($filter_name)) {
    return undef;
  }
  ${$filter}{"DESCRIPTION"}=$text;
  return $filter;
}

sub destring {
my ($c_string,$replace)=@_;
  $c_string=~s/^[ \t]*\"//;
  $c_string=~s/\"[ \t\n]*\"//g;
  $c_string=~s/\"[ \t]*$//;
  ($c_string=~s/\\\{/\(/g) && !$replace && (return undef); 
  ($c_string=~s/\\\}/\)/g) && !$replace && (return undef); 
  ($c_string=~s/\\'/\"/g) && !$replace && (return undef); 
  ($c_string=~s/\\;/,/g) && !$replace && (return undef); 
  ($c_string=~s/([ \t]+)/\\ /g) && !$replace && (return undef); 
  return $c_string;
}


