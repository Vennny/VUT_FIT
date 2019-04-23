<?php

$int_only = False;
$parse_only = False;
$both = False;
$recursive = False;
$path = "";
$parse_script = "parse.php";
$parse_script_changed = False;
$int_script = "interpret.py";
$intepret_script_changed = False;
$html = "";

$args_selected = "";
#goes through all of the arguments
foreach ( $argv as $count => $arg )
{
  #skip test.php name
  if ($count < 1) continue;
  unset( $matches );

  if ( preg_match( '/^--help$/', $arg)){
    if ($parse_only or $int_only or $recursive or $path != "" or $parse_script_changed or $intepret_script_changed){
      error_log("ERROR: Wrong arguments in help.");
      exit(10);    
    }
    echo "---------------NAPOVEDA-------------\nSpusteni:\nphp test.php [--help] [--directory=\"path\"] [--recursive] [--parse-script=\"file\"] [--int-script=\"file\"] [--parse_only] [--int_only]\n";
    echo"      --help          -vypise napovedu\n";
    echo"      --directory     -cesta k adresari s testy, defaultne aktualni adresar\n";
    echo"      --recursive     -skript bude prochazet i podadresare daneho adresare\n";
    echo"      --parse-script  -cesta a nazev k parsovacimu skriptu, defaultne ./parse.php\n";
    echo"      --int-script    -cesta a nazev k interpretovacimu skriptu, defaultne ./interpret.py\n";
    echo"      --parse-only    -skript bude testovat jen parsovaci skript\n";
    echo"      --int-only      -skript bude testovat jen interpretovaci skript\n";
    exit(0);
  }
  else if ( preg_match( '/^--directory=(.*)$/', $arg, $matches)){
    $path = $matches[1];
  	$args_selected .= "--directory=$path ";
  }
  else if ( preg_match( '/^--recursive$/', $arg)){
    $recursive = True;
  	$args_selected .= "--recursive ";
  }
  else if ( preg_match( '/^--parse-script=(.*)$/', $arg, $matches)){

    if ($int_only){      
      error_log("ERROR: Wrong arguments.");
      exit(10);
    }

    $parse_script = $matches[1];
    $parse_script_changed = True;
  	$args_selected .= "--parse-script=$parse-script ";
  }
  else if ( preg_match( '/^--int-script=(.*)$/', $arg, $matches)){

    if ($parse_only){
      error_log("ERROR: Wrong arguments.");
      exit(10);
    }

    $int_script = $matches[1];
    $intepret_script_changed = True;
  	$args_selected .= "--int-script=$int-script ";
  }
  else if ( preg_match( '/^--parse-only$/', $arg)){

    if ($int_only or $intepret_script_changed){
      error_log("ERROR: Wrong arguments.");
      exit(10);
    }

    $parse_only = True;
  	$args_selected .= "--parse_only ";
  }
  else if ( preg_match( '/^--int-only$/', $arg)){

    if ($parse_only or $parse_script_changed){
      error_log("ERROR: Wrong arguments.");
      exit(10);
    }

    $int_only = True;
  	$args_selected .= "--int_only ";
  }
  else{
    error_log("ERROR: Wrong arguments.");
    exit(10);
  }
}

#add slash to path if not present
if ($path != "" and !preg_match('/\/$/', $path)){
  $path .="/";
}

#if tests not specified, do both
if (!$parse_only and !$int_only){
  $both = True;
}

$total_tests = 0;
$failed_tests = 0;

$out_stdin = "$path"."output.out";
$out_stderr = "$path"."stderr.out";

if ($recursive){
  #recursive iterator
  try{
  	$file = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path));
  } 
  catch(Exception $e){
  	error_log("ERROR: Dir not found");
  	exit(11);
  }

} else{
  $file = glob($path.'*.*');
}


foreach( $file as $test_name){
  #get filename to an array, first position is the name without the file extension, second is only the file extension
  $x = strrpos($test_name, '.');
  $test = array(substr($test_name, 0, strrpos($test_name, '.')), substr($test_name, $x + 1));

  #if test found
  if($test[1] == "src"){
    $input_file = $test[0]. ".in";
    $rc_file = $test[0]. ".rc";
    $out_file = $test[0]. ".out";
    $src_file = $test_name;

    #create .in and .out file if non-existent
    if (!file_exists($input_file)){
      $f = fopen($input_file, 'w');
      fclose($f);
    }

    if (!file_exists($out_file)){
      $f = fopen($out_file, 'w');
      fclose($f);
    }

    #test parser
    if ($parse_only or $both){
      if (!file_exists($parse_script)){ 
        error_log("ERROR: Parse.php not found");
        exit(11);
      }

      exec("php7.3 ".$parse_script." < \"".$src_file."\" 2> \"".$out_stderr."\"", $output, $return_value);
      #save output to a temp file
      $output = implode("\n", $output);
      $tempf = tmpfile();
      fwrite($tempf, $output);

      if ($both){
        #output becomes the new source file
        $src_file = stream_get_meta_data($tempf)['uri'];
      }
      else{
      	$out_stdin = stream_get_meta_data($tempf)['uri'];
      }
    }
    #test interpret
    if ($int_only or $both){
      if (!file_exists($int_script)){
        error_log("ERROR: Interpret not found");
        exit(11);
      }

      exec("python3.6 ".$int_script." --source=\"".$src_file. "\" < \"".$input_file. "\" > \"".$out_stdin."\" 2> \"".$out_stderr."\"", $output, $return_value);
    }
   
    # create .rc file if nonexistent and load value
    if (file_exists($rc_file)){
      $rc_file_opened = fopen($rc_file, 'r');
      $rc_value = fread($rc_file_opened, filesize($rc_file));
      $rc_value = str_replace("\n", "", $rc_value);
      $rc_value = (int)$rc_value;
    }
    else{
      $rc_file_opened = fopen($rc_file, 'w');
      fwrite($rc_file_opened, '0');    
      $rc_value = "0"; 
    }
    fclose($rc_file_opened);
    # check if returns match
    if ($rc_value != $return_value){
      $failed_tests++;

      $html .="
        <tr style=\"background-color: rgba(250,0,0,0.6)\">
          <td class=\"tg-0lax\" style=\"text-align: left\">$test_name</td>
          <td class=\"tg-0lax\">$rc_value</td>
          <td class=\"tg-0lax\">$return_value</td>
          <td class=\"tg-0lax\"></td>
          <td class=\"tg-0lax\">FAIL</td>
        </tr>";

    }
    #if returns match and are 0, output check is needed
    else if ($return_value == 0){
      if ($int_only or $both){
        exec("diff $out_file $out_stdin", $diff_output, $diff_return_value);
        if (count($diff_output) != 0){
          $html .="
            <tr style=\"background-color: rgba(250,0,0,0.6)\">
              <td class=\"tg-0lax\" style=\"text-align: left\">$test_name</td>
              <td class=\"tg-0lax\">$rc_value</td>
              <td class=\"tg-0lax\">$return_value</td>
              <td class=\"tg-0lax\">FAIL</td>
              <td class=\"tg-0lax\">FAIL</td>
            </tr>";

          $failed_tests++;
        }
        else{
          $html .="
            <tr style=\"background-color: rgba(59,252,72,0.4)\">
              <td class=\"tg-0lax\" style=\"text-align: left\">$test_name</td>
              <td class=\"tg-0lax\">$rc_value</td>
              <td class=\"tg-0lax\">$return_value</td>
              <td class=\"tg-0lax\">OK</td>
              <td class=\"tg-0lax\">OK</td>
            </tr>";
        }

        $diff_output = "";
      }
      else if ($parse_only){
        exec("java -jar /pub/courses/ipp/jexamxml/jexamxml.jar ".$out_stdin." ".$out_file." /D /pub/courses/ipp/jexamxml/options");

        $jexamxml_return = shell_exec('echo $?');

        if ($jexamxml_return == 0){
          $html .="
            <tr style=\"background-color: rgba(59,252,72,0.4)\">
              <td class=\"tg-0lax\" style=\"text-align: left\">$test_name</td>
              <td class=\"tg-0lax\">$rc_value</td>
              <td class=\"tg-0lax\">$return_value</td>
              <td class=\"tg-0lax\">OK</td>
              <td class=\"tg-0lax\">OK</td>
            </tr>";
        } else {
          $html .="
            <tr style=\"background-color: rgba(250,0,0,0.6)\">
              <td class=\"tg-0lax\" style=\"text-align: left\">$test_name</td>
              <td class=\"tg-0lax\">$rc_value</td>
              <td class=\"tg-0lax\">$return_value</td>
              <td class=\"tg-0lax\">FAIL</td>
              <td class=\"tg-0lax\">FAIL</td>
            </tr>";
        }
      }
    }
    else{
      $html .="
        <tr style=\"background-color: rgba(59,252,72,0.4)\">
          <td class=\"tg-0lax\" style=\"text-align: left\">$test_name</td>
          <td class=\"tg-0lax\">$rc_value</td>
          <td class=\"tg-0lax\">$return_value</td>
          <td class=\"tg-0lax\"></td>
          <td class=\"tg-0lax\">OK</td>
        </tr>";
    }
    $total_tests++;
  }
}

#delete files
if (file_exists($out_stdin)){
  unlink($out_stdin);
}
if (file_exists($out_stderr)){
  unlink($out_stderr);
}

$ok_tests = $total_tests - $failed_tests;
#create html file
echo "<!DOCTYPE html>
  <html>
    <head>
      <title>IPP2019 report</title>
      <meta charset=\"UTF-8\">
      <style type=\"text/css\">
      	* { box-sizing:border-box; box-sizing:border-box;}
        body {
          width: 1080px;
          margin: 0 auto;
          background-color: rgba(253,251,214);
        }
        h1 {
          font-size: 50px;
          text-align: center;          
        }
        h2 {
          font-size: 20px;
          text-align: center;          
        }
        .content {
          margin-top: 30px;
          background-color: white;
          padding: 10px;
        }
        .overview {
          text-align: center;
          font-size: 24px;
        }
        .overview span {
          margin-right: 20px;
        }
        .overview span .ok {
          color: green;
          margin-right: 5px;
        }
        .overview span .fail {
          color: red;
          margin-right: 5px;
        }
        table {width:1050px;margin-left: 5px; }                
        .tg  {border-collapse:collapse;border-spacing:0;}
        .tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 10px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;text-align: center;}
        .tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:5px 20px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:black;}
        .tg .tg-wp8o{text-align:center;vertical-align:top}
        .tg .tg-73oq{text-align:left;vertical-align:top}
      </style>
    </head>
    <body>
      <div class=\"content\">
        <h1>IPP2019: test.php report</h1>
        <h2>$args_selected</h2>
        <div class=\"overview\">
          <span>Total: $total_tests</span>
          <span><span class=\"ok\">OK:</span> $ok_tests</span>
          <span><span class=\"fail\">Failed:</span> $failed_tests</span>
          <hr>
        </div>
        <table class=\"tg\">
          <tr>
            <th class=\"tg-baqh\">Test name</th>
            <th class=\"tg-baqh\">Correct RC</th>
            <th class=\"tg-baqh\">Returned RC</th>
            <th class=\"tg-baqh\">Output Result</th>
            <th class=\"tg-baqh\">Result</th>
          </tr>";

echo $html;
echo "
        </table>
     </div>
    </body>
  </html>";
?>