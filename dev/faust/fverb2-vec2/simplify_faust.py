# simplify faust

import sys
import os

if len(sys.argv) != 4:
    print("Usage: python simplify_faust.py <input_file> <output_file> <class_name>")
    sys.exit(1)

lang = "cpp"
if sys.argv[2].endswith(".c"):
    lang = "c"
os.system(
    """faust -i -scn "" -i """
    + sys.argv[1]
    + """ -lang """
    + lang
    + """ -cn """
    + sys.argv[3]
    + """ -o """
    + sys.argv[2]
    + """ -light -nvi"""
)
os.system("clang-format -i " + sys.argv[2])

faust_code = open(sys.argv[2]).read()
# edit it line by line
lines = faust_code.split("\n")
new_lines = []
in_metadata = False
in_user_interface = False
for line in lines:
    if "FAUSTFLOAT *input" in line:
        continue
    if "FAUSTFLOAT *output" in line:
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
    new_lines.append(line)

new_faust_code = "\n".join(new_lines)

# global replacements
new_faust_code = new_faust_code.replace(
    "FAUSTFLOAT **RESTRICT inputs", "float *input0, float *input1"
)
new_faust_code = new_faust_code.replace(
    "FAUSTFLOAT **RESTRICT outputs", "float *output0, float *output1"
)

with open(sys.argv[2], "w") as f:
    f.write(new_faust_code)

# run clang-format on the file
os.system("clang-format -i " + sys.argv[2])
