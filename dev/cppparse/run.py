import clang.cindex
import sys
import re

# Set the library path manually
clang.cindex.Config.set_library_file("/usr/lib/llvm-17/lib/libclang.so.1")


def extract_variables_from_cpp(file_path):
    # Initialize the index
    index = clang.cindex.Index.create()

    # Parse the file
    translation_unit = index.parse(file_path)

    # Set to store the found variables (using a set for uniqueness)
    variables = set()

    # Traverse the AST
    for node in translation_unit.cursor.get_children():
        extract_variable_declarations(node, variables)

    # Filter and return only variables starting with 'f' followed by an uppercase letter
    pattern = re.compile(r"^f[A-Z]")
    return sorted(var for var in variables if pattern.match(var))


def extract_variable_declarations(node, variables):
    # Check if the node is a variable declaration
    if (
        node.kind == clang.cindex.CursorKind.VAR_DECL
        or node.kind == clang.cindex.CursorKind.FIELD_DECL
    ):
        # Get the variable name and add to set for uniqueness
        variable_name = node.spelling
        variables.add(variable_name)

    # Recursively check child nodes
    for child in node.get_children():
        extract_variable_declarations(child, variables)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python variable_extractor.py <cpp_file>")
        sys.exit(1)

    cpp_file = sys.argv[1]
    variables = extract_variables_from_cpp(cpp_file)
    if variables:
        print("Variables found in the file:")
        for var in variables:
            print(var)
    else:
        print("No variables found or file is empty.")
