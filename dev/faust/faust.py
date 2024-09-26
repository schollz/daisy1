import os
import argparse

# Set up argument parsing
parser = argparse.ArgumentParser(description="Simplify FAUST code.")
parser.add_argument("input_file", help="Path to the input FAUST file.")
parser.add_argument("output_file", help="Path to the output file.")
parser.add_argument("class_name", help="Name of the FAUST class to generate.")
parser.add_argument("--sram", action="store_true", help="Boolean flag for SRAM usage.")
parser.add_argument(
    "--vec", action="store_true", help="Boolean flag for vectorization."
)
# Parse the arguments
args = parser.parse_args()

# Determine the language based on the output file extension
lang = "cpp"
if args.output_file.endswith(".c"):
    lang = "c"

# Run the FAUST compiler
do_vec = ""
if args.vec:
    do_vec = " -vec "
os.system(
    """faust -i -scn "" -i """
    + do_vec
    + args.input_file
    + """ -lang """
    + lang
    + """ -cn """
    + args.class_name
    + """ -o """
    + args.output_file
    + """ -light -nvi"""
)

# Run clang-format on the output file
os.system("clang-format -i " + args.output_file)

# Read the generated FAUST code
faust_code = open(args.output_file).read()

# global replace before beginning
faust_code = faust_code.replace("FAUSTFLOAT", "float")

# Process the code line by line
lines = faust_code.split("\n")
new_lines = []
in_metadata = False
in_user_interface = False
line_with_class = 0
global_variables = []
setters = {}
for i, line in enumerate(lines):
    # check if variable
    if line.startswith("class " + args.class_name):
        line_with_class = i
    fields = line.split()
    if (
        args.sram
        and len(fields) == 3
        and fields[0] == "static"
        and fields[1] == "float"
    ):
        # comment out line
        line = "// " + line
        # copy to global variables
        global_variables.append(fields[2])
    if args.sram and len(fields) == 2 and fields[0] == "float" and "[" in fields[1]:
        # parse fRec[1000]; into variable 'fRec' and '1000'
        variable = fields[1].split("[")[0]
        size = fields[1].split("[")[1].split("]")[0]
        size_int = int(size)
        if size_int > 4:
            # comment out line
            line = "// " + line
            # copy to global variables
            global_variables.append(fields[1])
    if "float *input" in line:
        continue
    if "float *output" in line:
        continue
    if "void buildUserInterface" in line:
        in_user_interface = True
    if in_user_interface:
        if line.strip() == "}":
            in_user_interface = False
        # add // to the beginning of the line
        line = "// " + line
    if "void metadata" in line:
        in_metadata = True
    if in_metadata:
        if line.strip() == "}":
            in_metadata = False
        # add // to the beginning of the line
        line = "// " + line
    if '"symbol"' in line and "&" in line and "," in line:
        # collect the variable between & and ,
        variable = line.split("&")[1].split(",")[0]
        # collect the name
        fields = line.split('"')
        name = fields[len(fields) - 2]
        print(variable, name)
        setters[variable] = name
    new_lines.append(line)

if args.sram:
    # add back the globals
    for variable in global_variables:
        new_lines.insert(line_with_class - 1, "   float DSY_SDRAM_BSS " + variable)


# add in setters
line_with_compute = 0
for i, line in enumerate(new_lines):
    if "void compute" in line:
        line_with_compute = i
        break
for variable, name in setters.items():
    new_lines.insert(
        line_with_compute - 1,
        "   void set_" + name + "(float value) { " + variable + " = value; }",
    )


new_faust_code = "\n".join(new_lines)

if args.sram:
    # redo variable names
    for variable in global_variables:
        variable_name = variable.split("[")[0]
        variable_new_name = variable_name + "_" + args.class_name
        new_faust_code = new_faust_code.replace(
            variable_name + "[", variable_new_name + "["
        )
        new_faust_code = new_faust_code.replace(
            variable_name + ")", variable_new_name + ")"
        )

# Perform global replacements
new_faust_code = new_faust_code.replace(
    "float **RESTRICT inputs", "float *input0, float *input1"
)
new_faust_code = new_faust_code.replace(
    "float **RESTRICT outputs", "float *output0, float *output1"
)

# Write the modified FAUST code back to the output file
with open(args.output_file, "w") as f:
    f.write(new_faust_code)

# Run clang-format again on the file to ensure proper formatting
os.system("clang-format -i " + args.output_file)
