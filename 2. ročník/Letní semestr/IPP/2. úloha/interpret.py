import sys
import getopt
import xml.etree.ElementTree as ET
import re

"""prints an error message and exits the program with a provided exit code"""	
def exit_with_error_msg(message, number):
	print(message, file=sys.stderr)
	sys.exit(number)	

"""takes the file and parses it into an internal representation of instructions and parameter"""
def parse_xml_to_lists(file):

	#count the lines
	lines = 0
	xml_input = ""
	for line in file:
		xml_input += line
		lines+=1

	#load the xml using the xml library
	try:
		xml_input = ET.fromstring(xml_input)
	except:
		exit_with_error_msg("ERROR: XML syntax is invalid", 31)

	#declare list 
	xml_list = [None]*(lines)

	try:
		language = xml_input.attrib["language"]
		header_tag = xml_input.tag
	except:
		exit_with_error_msg("ERROR: Syntax error in the header", 32)

	if ( language != "IPPcode19" or  header_tag != "program"):
		exit_with_error_msg("ERROR: Syntax error in the header", 32)

	for instruction in xml_input:

		if (instruction.tag != "instruction"):
			exit_with_error_msg("ERROR: Expected tag instruction, got "+instruction.tag,32)

		#declare list containing the name of the instruction and its arguments
		instruction_list = [None]*(len(instruction)+1)	
		try:
			instruction_list[0] = instruction.attrib["opcode"].upper()
		except:
			exit_with_error_msg("ERROR: Invalid opcode in instruction.", 32)

		if len(instruction.attrib) != 2:
			exit_with_error_msg("ERROR: Incorrect attributes in "+instruction_list[0]+".", 32)

		for argument in instruction:
			#declare list for the attribute type and value
			attribute_list = []

			if len(argument) != 0:
				exit_with_error_msg("ERROR: In instruction "+instruction.attrib["order"]+". Element found in an argument", 32)

			if len(argument.attrib) != 1:
				exit_with_error_msg("ERROR: Incorrect attributes in argument.", 32)

			if argument.tag[:-1] != "arg":
				exit_with_error_msg("ERROR: In instruction "+instruction.attrib["order"]+". Expected tag arg, got "+argument.tag[:-1], 32)

			try:
				argument_number = int(argument.tag[-1:])
			except:
				exit_with_error_msg("ERROR: In instruction "+instruction.attrib["order"]+". Expected number of argument, got" + argument.tag[-1:], 32)

			if argument_number < 1 or argument_number > 3:
				exit_with_error_msg("ERROR: In instruction "+instruction.attrib["order"]+". Argument has to be on a valid position.", 32)

			try:
				attribute_list.append(argument.attrib["type"])
			except:
				exit_with_error_msg("ERROR: No type attribute in argument found.", 32)

			if 	argument.attrib["type"] == "int":
				try:
					attribute_list.append(int(argument.text))
				except:
					exit_with_error_msg("ERROR: Integer has to be a valid number", 32)

			elif argument.attrib["type"] == "bool":
				if argument.text == "true":
					attribute_list.append(True)
				elif argument.text == "false":
					attribute_list.append(False)
				else:
					exit_with_error_msg("ERROR: Invalid bool value", 32)
			else:		
				attribute_list.append(argument.text)

			try:
				if instruction_list[argument_number] != None:
					sys.exit(32)

				#insert the argumetn list on the position given by the arg tag
				instruction_list[argument_number] = attribute_list
			except:
				exit_with_error_msg("ERROR: In instruction "+instruction.attrib["order"]+". Wrong arguments.", 32)	
		try:
			order = int(instruction.attrib["order"])-1
		except:
			exit_with_error_msg("ERROR: In instruction "+instruction.attrib["opcode"]+" position "+instruction.attrib["order"]+". Attribute order has to be a number.", 32)	

		try:
			xml_list[order] = instruction_list
		except:
			exit_with_error_msg("ERROR: Instruction(s) missing", 32)
	#cut all unnecessary list intems
	xml_list = xml_list[:len(xml_input)]
	return xml_list

"""gets the instruction and the expected amount of arguments, then checks it"""
def check_number_of_arguments(instruction, expected_number):
	if len(instruction) != expected_number:
		exit_with_error_msg("ERROR: Incorrect amount of arguments",32)

"""syntax check of the label type"""
def check_label(label):
	if label[0] != "label":
		exit_with_error_msg("ERROR: Expected label, got: " +label[0], 32)

	if not re.search("^([\-$&%*!?_]|[a-zA-Z])([a-zA-Z]|[0-9]|[\-$&%*!?_])*$", label[1]):
		exit_with_error_msg("ERROR: Label syntax error",32)

"""syntax check of the variable type, returns the frame and the value in a list"""
def check_var(variable):
	if not variable[0] == "var":
		exit_with_error_msg("ERROR: Expected var, got: "+variable[0], 32)

	value = variable[1].split("@",1)

	if not (value[0] in ["GF", "TF", "LF"] and re.search("^([\-$&%*!?_]|[a-zA-Z])([a-zA-Z]|[0-9]|[\-$&%*!?_])*$", value[1]) ):
		exit_with_error_msg("ERROR: Variable syntax error",32)

	return value

"""check if the variable is already defined od the frame"""
def is_var_definied(variable, global_frame, temporary_frame, local_frame):
	if variable[0] == "GF":
		if variable[1] not in global_frame:
			exit_with_error_msg("ERROR: Variable "+variable[0]+"@"+variable[1]+" not defined.", 54)

	elif variable[0] == "TF":
		if temporary_frame == None:
			exit_with_error_msg("ERROR: No local frame defined for "+variable[0]+"@"+variable[1], 55)

		if variable[1] not in temporary_frame:
			exit_with_error_msg("ERROR: Variable "+variable[0]+"@"+variable[1]+" not defined.", 54)

	elif variable[0] == "LF":
		if local_frame == None:
			exit_with_error_msg("ERROR: No local frame defined for "+variable[0]+"@"+variable[1], 55)

		if variable[1] not in local_frame:
			exit_with_error_msg("ERROR: Variable "+variable[0]+"@"+variable[1]+" not defined.", 54)	

"""syntax check of the symbol type, returns the symbol in a list"""
def check_symb(symbol, global_frame, temporary_frame, local_frame):
	if symbol[0] == "var":
		var = check_var(symbol)
		is_var_definied(var, global_frame, temporary_frame, local_frame)
		return var

	elif symbol[0] == "string":
		if symbol[1] == None:
			symbol[1] = ""
		else:
			if re.search("(\\\\\d{0,2}($|\s|[^\d]))|(#)", symbol[1]):
				exit_with_error_msg("ERROR: Invalid escape sequence in string",32)
			#decode escape sequences
			escape = []
			for i, char in enumerate(symbol[1]):
				if char == '\\':
					escape.append(i)

			escape.reverse()
			for i in escape:
				symbol[1] = symbol[1][0:i] + chr(int(symbol[1][i+1:i+4])) + symbol[1][i+4:]

	elif symbol[0] == "nil":
		if symbol[1] != "nil":
			exit_with_error_msg("ERROR: Expected nil as a value of type nil, got: "+symbol[1], 32) 
	elif symbol[0] == "int" or symbol[0] == "bool":
		pass
	else:
		exit_with_error_msg("ERROR: Expected var, string, int or bool, got: "+symbol[0],32)			
	return symbol

"""syntax check of the type type"""
def check_type(type_):
	if type_[0] != "type":
		exit_with_error_msg("ERROR: Expected type, got: "+type_[0],32)

	if not type_[1] in ["bool", "string", "int"]:
		exit_with_error_msg("ERROR: Expected type of bool, string or int, got: "+type_[1],32)

"""moves the provided value to a variable"""
def move_value_to_var(variable, value, global_frame, temporary_frame, local_frame):
	if value[0] != "string" and (value[0] == None or value[1] == None):
		exit_with_error_msg("ERROR: Ilegal use of uninitialized variable "+value[1], 56)
	if variable[0] == "GF":
		global_frame[variable[1]] = value
	elif variable[0] == "TF":
		temporary_frame[variable[1]] = value
	elif variable[0] == "LF":
		local_frame[variable[1]] = value

"""takes the provided symbol and returns it's value"""
def get_value_from_symbol(symbol, global_frame, temporary_frame, local_frame):

	if symbol[0] == "GF":
		new_value = global_frame[symbol[1]]
	elif symbol[0] == "TF":
		new_value = temporary_frame[symbol[1]]
	elif symbol[0] == "LF":
		new_value = local_frame[symbol[1]]
	else:
		new_value = symbol

	if  new_value[0] != "string" and (new_value[0] == None or new_value[1] == None):
		sys.exit(56)
	return new_value

"""takes the internal representation of instructions, checks the syntax of the instructions and executes the instructions"""
def execute_instructions(instruction, input_file):
	
	global_frame = {}
	temporary_frame = None
	local_frame = None
	frame_stack = []

	call_stack = []
	value_stack = []

	number_of_instructions = len(instruction)
	
	i = 0
	labels = {}
	#go through all of the instructions, find labels and save them to a dictionary
	while i < number_of_instructions:
		if instruction[i] == None:
			exit_with_error_msg("ERROR: Instruction number "+ str(i+1)+" missing.",32)	

		if instruction[i][0] == "LABEL":
			check_number_of_arguments(instruction[i], 2)
			check_label(instruction[i][1])

			if instruction[i][1][1] in labels:
				exit_with_error_msg("ERROR: Label \""+instruction[i][1][1]+"\" already defined.", 52)
			labels[instruction[i][1][1]] = i

		i+=1

	i = 0
	while i < number_of_instructions:
		
		if instruction[i][0] == "CREATEFRAME": 
			check_number_of_arguments(instruction[i], 1)
			temporary_frame = {}

		elif instruction[i][0] == "PUSHFRAME":
			check_number_of_arguments(instruction[i], 1)

			if temporary_frame == None:
				exit_with_error_msg("ERROR: Temporary frame not defined for "+instruction[i][0], 55)
				
			frame_stack.append(temporary_frame)
			local_frame = frame_stack[-1]

			temporary_frame = None

		elif instruction[i][0] == "POPFRAME":
			check_number_of_arguments(instruction[i], 1)

			if local_frame == None:
				exit_with_error_msg("ERROR: No local frame defined for "+instruction[i][0], 55)

			temporary_frame = frame_stack.pop()

			if len(frame_stack) != 0:
				local_frame = frame_stack[-1]
			else :
				local_frame = None

		elif instruction[i][0] == "RETURN":
			check_number_of_arguments(instruction[i], 1)

			try:
				i = call_stack.pop() 
			except:
				exit_with_error_msg("ERROR: Call stack for RETURN is empty.", 56)	

		elif instruction[i][0] == "BREAK":
			check_number_of_arguments(instruction[i], 1)

			print("------------BREAK------------", file=sys.stderr)
			print("Opcode of the current instruction:", i+1, file=sys.stderr)
			print("Total number of instructions", number_of_instructions, file=sys.stderr)
			print("Frames:", file=sys.stderr)
			print("Global frame: {'name': ['type', value]}", file=sys.stderr)
			print(global_frame, file=sys.stderr)
			print("Temporary frame: {'name': ['type', value]}", file=sys.stderr)
			print(temporary_frame, file=sys.stderr)
			print("Local frame: {'name': ['type', value]}", file=sys.stderr)
			print(local_frame, file=sys.stderr)
			print("Amount of frames on stack: ", len(frame_stack), file=sys.stderr)
			print("Call stack positions: ", call_stack, file=sys.stderr)
			print("---------END-OF-BREAK---------", file=sys.stderr)

		elif instruction[i][0] == "CALL":
			check_number_of_arguments(instruction[i], 2)			
			check_label(instruction[i][1])
				
			call_stack.append(i)

			try:
				i = labels[instruction[i][1][1]]
			except:
				exit_with_error_msg("ERROR: Label "+instruction[i][1][1]+" in instruction "+instruction[i][0]+" not defined.",52)

		elif instruction[i][0] == "LABEL":
			pass	

		elif instruction[i][0] == "JUMP":
			check_number_of_arguments(instruction[i], 2)
			check_label(instruction[i][1])

			try:
				i = labels[instruction[i][1][1]]
			except:
				exit_with_error_msg("ERROR: Label "+instruction[i][1][1]+" in instruction "+instruction[i][0]+" not defined.",52)

		elif instruction[i][0] == "DEFVAR":
			check_number_of_arguments(instruction[i], 2)

			var_value = check_var(instruction[i][1])

			if var_value[0] == "GF":
				if var_value[1] in global_frame:
					exit_with_error_msg("ERROR: Redefinition of a variable", 52)
				global_frame[var_value[1]] = [None, None]

			elif var_value[0] == "TF":
				if temporary_frame == None:
					exit_with_error_msg("ERROR: Temporary frame not defined for "+instruction[i][0], 55)
				if var_value[1] in temporary_frame:
					exit_with_error_msg("ERROR: Redefinition of a variable", 52)
				temporary_frame[var_value[1]] = [None, None]

			elif var_value[0] == "LF":
				if local_frame == None:
					exit_with_error_msg("ERROR: No local frame defined for "+instruction[i][0], 55)
				if var_value[1] in local_frame:
					exit_with_error_msg("ERROR: Redefinition of a variable", 52)
				local_frame[var_value[1]] = [None, None]

		elif instruction[i][0] == "PUSHS":
			check_number_of_arguments(instruction[i], 2)
			symb_value = check_symb(instruction[i][1], global_frame, temporary_frame, local_frame)
				
			to_move = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)

			value_stack.append(to_move)

		elif instruction[i][0] == "POPS":
			check_number_of_arguments(instruction[i], 2)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)

			try:
				to_move = value_stack.pop()
			except:
				exit_with_error_msg("ERROR: In POPS, stack is empty", 56)
			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)
				
		elif instruction[i][0] == "WRITE":
			check_number_of_arguments(instruction[i], 2)
			symb_value = check_symb(instruction[i][1], global_frame, temporary_frame, local_frame)

			symb_value = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)

			if symb_value[0] == "bool":
				if symb_value[1]:
					print("true",end='')
				else:
					print("false",end='')
			elif symb_value[0] != "nil":
				print(symb_value[1],end='')
				
		elif instruction[i][0] == "EXIT":
			check_number_of_arguments(instruction[i], 2)
			symb_value = check_symb(instruction[i][1], global_frame, temporary_frame, local_frame)

			symb_value = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)
			if symb_value[0] != "int":
				exit_with_error_msg("ERROR: Symbol in EXIT has to be an integer.",53)

			if symb_value[1] >=0 and symb_value[1] < 50:
				sys.exit(symb_value[1])
			else:
				exit_with_error_msg("ERROR: Error code in EXIT has to be an integer with a value of <0,49>", 57)
		
		elif instruction[i][0] == "DPRINT":
			check_number_of_arguments(instruction[i], 2)
			symb_value = check_symb(instruction[i][1], global_frame, temporary_frame, local_frame)

			value = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)
			print(value[1], file=sys.stderr)
				
		elif instruction[i][0] == "MOVE":
			check_number_of_arguments(instruction[i], 3)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)

			to_move = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)
				
		elif instruction[i][0] == "INT2CHAR":
			check_number_of_arguments(instruction[i], 3)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)

			loaded_int = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)

			if loaded_int[0] != "int":
				exit_with_error_msg("ERROR: Symbol in INT2CHAR has to be an integer", 53)
			try:
				int_to_char = int(loaded_int[1])
			except:
				exit_with_error_msg("ERROR: Not a valid integer in INT2CHAR,",53)
			try:
				char_value = chr(int_to_char)			
			except:
				exit_with_error_msg("ERROR: Not a valid integer in INT2CHAR,",58)

			to_move = []
			to_move.append("string")
			to_move.append(char_value)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "STRLEN":
			check_number_of_arguments(instruction[i], 3)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)

			symb_value = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)

			if symb_value[0] != "string":
				exit_with_error_msg("ERROR: In STRLEN, symbol has to be a string", 53)

			result = len(symb_value[1])

			to_move = []
			to_move.append("int")
			to_move.append(result)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)
				
		elif instruction[i][0] == "TYPE":
			check_number_of_arguments(instruction[i], 3)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			
			try:
				symb_value = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)
			except:
				symb_value = [""]

			to_move = []
			to_move.append("string")
			to_move.append(symb_value[0])

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)
			
		elif instruction[i][0] == "NOT":
			check_number_of_arguments(instruction[i], 3)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)

			symb_value = get_value_from_symbol(symb_value, global_frame, temporary_frame, local_frame)

			if symb_value[0] != "bool":
				exit_with_error_msg("ERROR: Symbol in NOT has to be a bool", 53)
			
			to_move = []
			to_move.append("bool")

			if symb_value[1]:
				to_move.append(False)
			else:
				to_move.append(True)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)
				
		elif instruction[i][0] == "READ":
			check_number_of_arguments(instruction[i], 3)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			check_type(instruction[i][2])

			expected_type = instruction[i][2][1]

			if (input_file == sys.stdin):
				try:
					read_input = input()
				except:
					read_input = None
			else:
				read_input = input_file.readline()
				#cut the end of line character
				if read_input[-1:] == "\n":
					read_input = read_input[:-1]

			to_move = []

			if expected_type == "string":
				to_move.append("string")

				if not read_input:					
					to_move.append("")
				else:
					to_move.append(read_input)

			elif expected_type == "int":
				to_move.append("int")
				try:
					to_move.append(int(read_input))
				except:
					to_move.append(0)

			elif expected_type == "bool":
				to_move.append("bool")

				try:
					read_input = read_input.upper()
				except:
					to_move.append(False)
					
				if read_input == "TRUE":
					to_move.append(True)
				else:
					to_move.append(False)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "ADD":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1, global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2, global_frame,temporary_frame, local_frame)
			
			if symb_value1[0] != "int" or symb_value2[0] != "int":
				exit_with_error_msg("ERROR: In ADD, both operands have to be of type integer", 53)

			to_move = []
			to_move.append("int")
			final_value = symb_value1[1] + symb_value2[1]
			to_move.append(final_value)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "SUB":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1, global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2, global_frame,temporary_frame, local_frame)
			
			if symb_value1[0] != "int" or symb_value2[0] != "int":
				exit_with_error_msg("ERROR: In SUB, both operands have to be of type integer", 53)

			to_move = []
			to_move.append("int")
			final_value = symb_value1[1] - symb_value2[1]
			to_move.append(final_value)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)	

		elif instruction[i][0] == "MUL":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1, global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2, global_frame,temporary_frame, local_frame)
			
			if symb_value1[0] != "int" or symb_value2[0] != "int":
				exit_with_error_msg("ERROR: In MUL, both operands have to be of type integer", 53)

			to_move = []
			to_move.append("int")
			final_value = symb_value1[1] * symb_value2[1]
			to_move.append(final_value)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "IDIV":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1, global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2, global_frame,temporary_frame, local_frame)
			
			if symb_value1[0] != "int" or symb_value2[0] != "int":
				exit_with_error_msg("ERROR: In ADD, both operands have to be of type integer", 53)

			if symb_value2[1] == 0:
				exit_with_error_msg("ERROR: Dividing by zero.", 57)

			to_move = []
			to_move.append("int")
			final_value = symb_value1[1] // symb_value2[1]
			to_move.append(final_value)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)	

		elif instruction[i][0] == "LT":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)
			
			if symb_value1[0] == "nil" or symb_value2[0] == "nil":
				exit_with_error_msg("ERROR: In LT, nil is not a valid type", 53)

			if symb_value1[0] != symb_value2[0]:
				exit_with_error_msg("ERROR: In LT, both symbols have to be the same type", 53)

			to_move = []
			to_move.append("bool")
			if symb_value1[1] < symb_value2[1]:
				to_move.append(True)
			else:
				to_move.append(False)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "GT":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)
			
			if symb_value1[0] == "nil" or symb_value2[0] == "nil":
				exit_with_error_msg("ERROR: In GT, nil is not a valid type", 53)

			if symb_value1[0] != symb_value2[0]:
				exit_with_error_msg("ERROR: In GT, both symbols have to be the same type", 53)

			to_move = []
			to_move.append("bool")
			if symb_value1[1] > symb_value2[1]:
				to_move.append(True)
			else:
				to_move.append(False)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)
				
		elif instruction[i][0] == "EQ":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)

			if symb_value1[0] != symb_value2[0] and symb_value1[0] != "nil" and symb_value2[0] != "nil":
				exit_with_error_msg("ERROR: In EQ, both symbols have to be the same type", 53)

			to_move = []
			to_move.append("bool")
			if symb_value1[1] == symb_value2[1]:
				to_move.append(True)
			else:
				to_move.append(False)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "AND":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)

			if symb_value1[0] != "bool" or symb_value2[0] != "bool":
				exit_with_error_msg("ERROR: Symbols in AND have to be a bool", 53)
			
			to_move = []
			to_move.append("bool")

			if symb_value1[1] and symb_value2[1]:
				to_move.append(True)
			else:
				to_move.append(False)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "OR":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)

			if symb_value1[0] != "bool" or symb_value2[0] != "bool":
				exit_with_error_msg("ERROR: Symbols in OR have to be a bool", 53)
			
			to_move = []
			to_move.append("bool")

			if symb_value1[1] or symb_value2[1]:
				to_move.append(True)
			else:
				to_move.append(False)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)	

		elif instruction[i][0] == "STRI2INT":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			#first symbol, has to be a string
			loaded_string = get_value_from_symbol(symb_value1, global_frame, temporary_frame, local_frame)

			if loaded_string[0] != "string":
				exit_with_error_msg("ERROR: Symbol in STRI2INT has to be a string", 53)

			#second symbol, has to be an integer
			loaded_int = get_value_from_symbol(symb_value2, global_frame, temporary_frame, local_frame)

			if loaded_int[0] != "int":
				exit_with_error_msg("ERROR: Symbol2 in STRI2INT has to be an integer", 53)

			try:
				index = int(loaded_int[1])	
			except:
				exit_with_error_msg("ERROR: Not a valid integer in STRI2INT symbol2",53)

			if index >= len(loaded_string[1]):
				exit_with_error_msg("ERROR: IN STRI2INT, index out of range", 58)
			try:
				char_to_int_value = ord(loaded_string[1][index])
			except:
				exit_with_error_msg("ERROR: Not a valid character in STRI2INT for ord",58)

			to_move = []
			to_move.append("int")
			to_move.append(char_to_int_value)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)
				
		elif instruction[i][0] == "CONCAT":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)	
			if symb_value1[0] != "string" or symb_value2[0] != "string":
				exit_with_error_msg("ERROR: In CONCAT symbols have to be a string", 53)

			to_move = []
			to_move.append("string")
			result = symb_value1[1] + symb_value2[1]
			to_move.append(result)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "GETCHAR":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)	

			#first symbol, has to be a string
			loaded_string = get_value_from_symbol(symb_value1, global_frame, temporary_frame, local_frame)

			if loaded_string[0] != "string":
				exit_with_error_msg("ERROR: Symbol in GETCHAR has to be a string", 53)
			#second symbol, has to be an integer
			loaded_int = get_value_from_symbol(symb_value2, global_frame, temporary_frame, local_frame)

			if loaded_int[0] != "int":
				exit_with_error_msg("ERROR: Symbol2 in GETCHAR has to be an integer", 53)

			try:
				index = int(loaded_int[1])	
			except:
				exit_with_error_msg("ERROR: Not a valid integer in GETCHAR symbol2",53)

			if index >= len(loaded_string[1]):
				exit_with_error_msg("ERROR: IN GETCHAR, index out of range", 58)
			try:
				character = loaded_string[1][index]
			except:
				exit_with_error_msg("ERROR: In GETCHAR, unexpected error.",58)

			to_move = []
			to_move.append("string")
			to_move.append(character)

			move_value_to_var(var_value, to_move, global_frame, temporary_frame, local_frame)			

		elif instruction[i][0] == "SETCHAR":
			check_number_of_arguments(instruction[i], 4)
			var_value = check_var(instruction[i][1])
			is_var_definied(var_value, global_frame, temporary_frame, local_frame)
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)	

			string_to_change = get_value_from_symbol(var_value, global_frame,temporary_frame, local_frame)

			if string_to_change[0] != "string" or symb_value1[0] != "int" or symb_value2[0] != "string":
				exit_with_error_msg("ERROR: In SETCHAR, invalid type of an operand", 53)

			if symb_value2[1] == "":
				exit_with_error_msg("ERROR: In SETCHAR, string in symbol2 cannot be empty", 58)

			index = symb_value1[1]

			if index >= len(string_to_change[1]) or string_to_change[1] == None:
				exit_with_error_msg("ERROR: In SETCHAR, index out of range", 58)

			new_string = string_to_change[1][:index] + symb_value2[1][0] + string_to_change[1][(index+1):]
			string_to_change[1] = new_string

			move_value_to_var(var_value, string_to_change, global_frame, temporary_frame, local_frame)

		elif instruction[i][0] == "JUMPIFEQ":
			check_number_of_arguments(instruction[i], 4)
			check_label(instruction[i][1])
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)	

			if symb_value1[0] != symb_value2[0]:
				exit_with_error_msg("ERROR: In JUMPIFEQ, both symbols have to be the same type", 53)

			if symb_value1[1] == symb_value2[1]:
				try:
					i = labels[instruction[i][1][1]]
				except:
					exit_with_error_msg("ERROR: Label "+instruction[i][1][1]+" in instruction "+instruction[i][0]+" not defined.",52)				

		elif instruction[i][0] == "JUMPIFNEQ":
			check_number_of_arguments(instruction[i], 4)
			check_label(instruction[i][1])
			symb_value1 = check_symb(instruction[i][2], global_frame, temporary_frame, local_frame)
			symb_value2 = check_symb(instruction[i][3], global_frame, temporary_frame, local_frame)

			symb_value1 = get_value_from_symbol(symb_value1,global_frame,temporary_frame, local_frame)
			symb_value2 = get_value_from_symbol(symb_value2,global_frame,temporary_frame, local_frame)

			if symb_value1[0] != symb_value2[0]:
				exit_with_error_msg("ERROR: In JUMPIFEQ, both symbols have to be the same type", 53)

			if symb_value1[1] != symb_value2[1]:
				try:
					i = labels[instruction[i][1][1]]
				except:
					exit_with_error_msg("ERROR: Label "+instruction[i][1][1]+" in instruction "+instruction[i][0]+" not defined.",52)		
		else:
			exit_with_error_msg("ERROR:Invalid instruction "+ instruction[i][0],32)
				
		i+=1

####################################################
# main function
####################################################
def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:], '', ['source=','input=', 'help'])
	except getopt.GetoptError:
		exit_with_error_msg("ERROR: Wrong combination of operands",10)
	
	source_file = sys.stdin
	input_file = sys.stdin
	#parse command line arguments
	for opt, arg in opts:
		if opt == '--help':
			if len(sys.argv) == 2:
				print("Nápověda:\nParametry příkazové řádky:\n--help : vypíše nápovědu\n--source=<file> : zdrojový soubor s jazykem IPPcode19 zapsaném v xml formátu\n--input=<file> : zdrojový soubor pro vstup\nAlespoň jeden z posledních dvou parametrů musí být zadán. Chybějící parametr je nahrazem vstupem ze standardního vstupu")
				sys.exit(0)
			else:
				exit_with_error_msg("ERROR: Wrong combination of operands",10)			
				
		elif opt == '--source':
			if source_file != sys.stdin:
				exit_with_error_msg("ERROR: Wrong combination of operands",10)			
			try:
				source_file = open(arg, 'r')
			except:
				exit_with_error_msg("ERROR: Can't open source file",11)

		elif opt == '--input':
			if input_file != sys.stdin:
				exit_with_error_msg("ERROR: Wrong combination of operands",10)			
			try:
				input_file = open(arg, 'r')
			except:
				exit_with_error_msg("ERROR: Can't open input file",11)
		else:
				exit_with_error_msg("ERROR: Wrong combination of operands",10)			

	if (source_file == sys.stdin and input_file == sys.stdin):
		exit_with_error_msg("ERROR: No arguments",10)

	instructions = parse_xml_to_lists(source_file)
	execute_instructions(instructions, input_file)

	sys.exit(0)

if __name__ == "__main__":
	main()