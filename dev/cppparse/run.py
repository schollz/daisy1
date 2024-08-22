import clang.cindex
import sys

# Set the library path manually
clang.cindex.Config.set_library_file("/usr/lib/llvm-17/lib/libclang.so.1")


def extract_and_modify_variables_from_cpp(file_path, prefix, output_file):
    # Initialize the index
    index = clang.cindex.Index.create()

    # Parse the file
    translation_unit = index.parse(file_path)

    # Set to store the found variables (using a set for uniqueness)
    variables = set()

    # Modify the source code with the new variable names
    modified_code = []
    with open(file_path, "r") as f:
        lines = f.readlines()

    for node in translation_unit.cursor.get_children():
        modify_global_variable_declarations(node, variables, prefix, lines)

    # Write the modified code to the new file
    with open(output_file, "w") as f:
        f.writelines(lines)

    # Sort variables from longest to shortest
    variables = sorted(variables, key=len, reverse=True)
    return variables


def modify_global_variable_declarations(node, variables, prefix, lines):
    # Check if the node is a variable declaration and if it's in the global scope
    if (
        node.kind == clang.cindex.CursorKind.VAR_DECL
        and node.semantic_parent.kind == clang.cindex.CursorKind.TRANSLATION_UNIT
    ):
        # Get the variable name
        variable_name = node.spelling

        # Skip the variable "DSY_SDRAM_BSS"
        if variable_name == "DSY_SDRAM_BSS":
            return

        # Add the variable name to the set for uniqueness
        variables.add(variable_name)

        # Get the variable's location
        location = node.location

        # Modify the specific line of code
        line_number = location.line - 1
        column_number = location.column - 1

        # Replace the variable name with the prefixed one
        lines[line_number] = (
            lines[line_number][:column_number]
            + prefix
            + variable_name
            + lines[line_number][column_number + len(variable_name) :]
        )

    # Recursively check child nodes that might be in global scope (e.g., namespaces)
    for child in node.get_children():
        modify_global_variable_declarations(child, variables, prefix, lines)


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python variable_extractor.py <cpp_file> <prefix> <output_file>")
        sys.exit(1)

    cpp_file = sys.argv[1]
    prefix = sys.argv[2]
    output_file = sys.argv[3]

    variables = extract_and_modify_variables_from_cpp(cpp_file, prefix, output_file)
    if variables:
        print("Global variables found and modified in the file:")
        for var in variables:
            print(f"{prefix}{var}")
        print(f"Modified code saved to {output_file}")
    else:
        print("No global variables found or file is empty.")
