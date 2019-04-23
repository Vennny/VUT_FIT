<?php
#####################
# IPP Project 1
# 2018/2019
# Václav Trampeška
#####################

#writes message to error log and exits
function message_die($message, $exit_code){
    error_log($message);
    die($exit_code);
}

#counts the amount of operands
#if the amount is different then required then exits with error code
function params_count_check($instruction, $expected_count){
    #instruction itself is included in $instruction, we need to subtract one to get the amount of operands
    if (count($instruction)-1 != $expected_count) {
        message_die("ERROR: Expected $expected_count operand(s) for instruction $instruction[0]", 23);
    }
}

#checks if the input variable is valid
function check_var($variable){
    if (!preg_match('/^(LF|GF|TF)@([\-$&%*!?_]|[a-zA-Z])([\-$&%*!?_]|[a-zA-Z]|[0-9])*$/', $variable)){
        message_die("ERROR: Invalid variable operand.",23);
    }
}

#checks if the input symbol is a valid variable or constant
function check_symb_and_generate($place, $symbol, $xml_buffer){
    if(!preg_match('/(^int@.+$)|(^string@.*$)|((^bool@)(true|false)$)|(^nil@nil$)|(^(LF|GF|TF)@([\-$&%*!?_]|[a-zA-Z])([a-zA-Z]|[0-9])*$)/', $symbol)){
        message_die("ERROR: Invalid variable operand.",23);
    }

    #get type by deleting the value
    $type = preg_replace('/@.*/','', $symbol);
    
    #checks if the type is a variable
    if (in_array($type, array("GF","LF", "TF"))){
        $type = "var";
    }
    else{
        #type is not a variable, get value by deleting type
        $symbol = preg_replace('/^(string|int|bool|nil)@/','', $symbol);
        
        #regex for checking escapes 
        if($type == "string" && preg_match('/\\\\(\d{0,2}($|\s|[^\d]))/', $symbol)){
            message_die("ERROR: Expected three digits after \\.", 23);
        }
    }

    #write operand to XML
    writeOperandElement($type, $place, $symbol, $xml_buffer);
}

#checks if the input label is valid
function check_label($label){
    if (!preg_match('/^([\-$&%*!?_]|[a-zA-Z])([a-zA-Z]|[0-9])*$/', $label)){
        message_die("ERROR: Invalid variable operand.",23);
    }
}

#checks if the input type is valid
function check_type($type){
    if (!in_array($type, array("bool", "string", "int"))){
        message_die("ERROR: Invalid variable operand.",23);
    }
}

#writes operand to XML
function writeOperandElement($operand, $place, $value, $xml_buffer){

    $xml_buffer -> startElement('arg'.$place);
    $xml_buffer -> writeAttribute('type', $operand);
    #if the operand has no value, make the XML tag shorter
    if ($value != ""){
        $xml_buffer -> text($value);
    }
    $xml_buffer -> endElement();
}

#handles a line from STDIN, checks validity of the instruction and operands, then writes XML
function check_instruction($line, $xml_buffer, $order){
    #instructions without operands
    $zero_op = array("CREATEFRAME","PUSHFRAME","POPFRAME","RETURN","BREAK");
    #operand pattern: <label>
    $one_op_label = array("CALL", "LABEL", "JUMP");
    #operand pattern: <var>
    $one_op_var = array("DEFVAR", "POPS");
    #operand pattern: <symb>
    $one_op_symb = array("PUSHS", "WRITE", "EXIT", "DPRINT");
    #operand pattern: <var> <symb>
    $two_op_symb = array("MOVE","INT2CHAR","STRLEN","TYPE", "NOT");
    #
    #DONT FORGET READ <var> <type>
    #
    #operand pattern: <var> <symb1> <symb2>
    $three_op_var = array("ADD","SUB","MUL","IDIV","LT","GT","EQ","AND","OR","STRI2INT","CONCAT","GETCHAR","SETCHAR");
    #operand pattern: <label> <symb1> <symb2>
    $three_op_symb = array("JUMPIFEQ", "JUMPIFNEQ");

    #seperate input line to words
    $instruction = explode(" ", $line);
    #handle instruction case insensitivity
    $instruction[0] = strtoupper($instruction[0]);

    #write XML instruction
    $xml_buffer -> startElement('instruction');
    $xml_buffer -> writeAttribute('order', $order);
    $xml_buffer -> writeAttribute('opcode', $instruction[0]);

    #finds loaded instruction in instruction arrays, checks operands and writes XML
    if(in_array($instruction[0], $zero_op)){
        #check operand count
        params_count_check($instruction, 0);
    }
    else if(in_array($instruction[0], $one_op_label)){
        #check operand count
        params_count_check($instruction, 1);
        #checks operand validity
        check_label($instruction[1]);
        #writes operand to XML
        writeOperandElement('label', 1, $instruction[1], $xml_buffer);
    }
    else if(in_array($instruction[0], $one_op_var)){
        #check operand count
        params_count_check($instruction, 1);
        #checks operand validity
        check_var($instruction[1]);
        #writes operand to XML
        writeOperandElement('var', 1, $instruction[1], $xml_buffer);
    }
    else if(in_array($instruction[0], $one_op_symb)){
        #check operand count
        params_count_check($instruction, 1);
        #checks symbol validity and writes XML
        check_symb_and_generate(1, $instruction[1], $xml_buffer);
    }
    else if(in_array($instruction[0], $two_op_symb)){
        #check operand count
        params_count_check($instruction, 2);
        #checks operand validity
        check_var($instruction[1]);
        #writes operand to XML
        writeOperandElement('var', 1, $instruction[1], $xml_buffer);
        #checks symbol validity and writes XML
        check_symb_and_generate(2, $instruction[2], $xml_buffer);
    }
    else if ($instruction[0] == "READ"){
        #check operand count
        params_count_check($instruction, 2);
        #checks operand validity
        check_var($instruction[1]);
        writeOperandElement('var', 1, $instruction[1], $xml_buffer);
        #checks operand validity
        check_type($instruction[2]);
        #writes operand to XML
        writeOperandElement('type', 2, $instruction[2], $xml_buffer);
    }
    else if(in_array($instruction[0], $three_op_var)){
        #check operand count
        params_count_check($instruction, 3);
        #checks operand validity
        check_var($instruction[1]);
        #writes operand to XML
        writeOperandElement('var', 1, $instruction[1], $xml_buffer);
        #checks symbol validity and writes XML
        check_symb_and_generate(2, $instruction[2], $xml_buffer);
        check_symb_and_generate(3, $instruction[3], $xml_buffer);
    }
    else if(in_array($instruction[0], $three_op_symb)){
        #check operand count
        params_count_check($instruction, 3);
        #checks operand validity
        check_label($instruction[1]);
        #writes operand to XML
        writeOperandElement('label', 1, $instruction[1], $xml_buffer);
        #checks symbol validity and writes XML
        check_symb_and_generate(2, $instruction[2], $xml_buffer);
        check_symb_and_generate(3, $instruction[3], $xml_buffer);
    }
    #instuction not found in arrays, error
    else{
        message_die("ERROR: Syntax error: Unknown instruction.", 22);
    }

    #write XML end instruction
    $xml_buffer -> endElement();
}

###############################################
#start of script
###############################################

# handle arguments
# -- help echoes help and exits
# other combination of arguments is not allowed
if ($argc == 2 && $argv[1] == "--help") {
    echo "Skript typu filtr (parse.php v jazyce PHP 7.3) načte ze standardního vstupu zdrojový kód v IPPcode19, zkontroluje lexikální a syntaktickou správnost kódu a vypíše na standardní výstup XML reprezentaci programu dle specifikace\n";
    die(0);
} else if ($argc != 1) {
    message_die("ERROR: Invalid arguments.", 10);
}

#get first line from STDIN
$header = fgets(STDIN);
#delete comments
$header = preg_replace('/#.*/', '', $header);
#delete redundant spaces and handle case insensitivity
$header = strtoupper(trim($header));

#no valid header results in exiting
if ($header != ".IPPCODE19") {
    message_die( "ERROR: Header is missing.",21);
}


# initialize XMLwriter
$xml_buffer = new XMLWriter();
$xml_buffer -> openURI('php://output');

#write XML header
$xml_buffer -> startDocument('1.0','utf-8');
$xml_buffer -> setIndent(true);
$xml_buffer -> setIndentString('    ');
$xml_buffer -> startElement('program');
$xml_buffer -> writeAttribute('language', 'IPPcode19');

#initialize variable for instruction count
$order = 1;

#loads lines from STDIN
while ($line = fgets(STDIN)) {
    #delete redundant spaces between words
    $line = preg_replace('/\s+/', ' ', $line);
    #delete comments
    $line = preg_replace('/#.*/', '', $line);
    #delete redundant spaces at the start/end
    $line = trim($line);

    #skip empty lines
    if ($line != "") {
        check_instruction($line, $xml_buffer, $order);
        $order++;
    }
}

#write XML footer and send to STDOUT
$xml_buffer -> endElement();
$xml_buffer -> endDocument();
$xml_buffer -> flush();
