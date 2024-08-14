import sys

import numpy as np
import matplotlib.pyplot as plt


def run():
    sample_num = int(sys.argv[1])
    if sample_num == None:
        print("oops")
        return
    # Create a numpy array of x-values from 0 to 1
    x = np.linspace(0, 1, sample_num)

    # Calculate the sine of the x-values
    cos_out = np.cos(np.pi / 2 * x)
    cos_in = np.cos((1 - x) * np.pi / 2)
    sqrt_in = np.sqrt(np.sqrt(x))
    sqrt_out = np.sqrt(np.sqrt(1 - x))
    exp_out = np.exp(-x * np.exp(1.5))
    exp_in = np.exp(x * np.exp(1.5)) / np.exp(np.exp(1.5))
    line_out = 1 - x

    # fade out the last 10% of the exponential
    exp_out = np.multiply(exp_out, line_out)
    exp_in = np.multiply(exp_in, 1 - line_out)
    # create a header file `crossfade.h`
    with open("crossfade.h", "w") as f:
        f.write(
            """#ifndef CROSSFADE_LIB
#define CROSSFADE_LIB
#define CROSSFADE_LIB
#define CROSSFADE_COS 0      
#define CROSSFADE_SQRT 1      
#define CROSSFADE_EXP 2      
#define CROSSFADE_LINE 3
"""
        )
        f.write("#define CROSSFADE_LIMIT %d\n" % sample_num)
        f.write(f"extern float crossfade_cos_out[{sample_num}];\n")
        f.write(f"extern float crossfade_cos_in[{sample_num}];\n")
        f.write(f"extern float crossfade_sqrt_out[{sample_num}];\n")
        f.write(f"extern float crossfade_sqrt_in[{sample_num}];\n")
        f.write(f"extern float crossfade_exp_out[{sample_num}];\n")
        f.write(f"extern float crossfade_exp_in[{sample_num}];\n")
        f.write(f"extern float crossfade_line[{sample_num}];\n")
        f.write(
            """
#endif
"""
        )
    # now create the .cpp file
    f = open("crossfade.cpp", "w")
    f.write('#include "crossfade.h"\n')

    f.write(f"float crossfade_cos_out[{sample_num}]={{")
    s = ""
    for _, v in enumerate(cos_out):
        s += f"{round(v,4)},"
    f.write(s)
    f.write("};")
    f.write(f"float crossfade_cos_in[{sample_num}]={{")
    s = ""
    for _, v in enumerate(cos_in):
        s += f"{round(v,4)},"
    f.write(s)
    f.write("};")

    f.write(f"float crossfade_sqrt_out[{sample_num}]={{")
    s = ""
    for _, v in enumerate(sqrt_out):
        s += f"{round(v,4)},"
    f.write(s)
    f.write("};")

    f.write(f"float crossfade_sqrt_in[{sample_num}]={{")
    s = ""
    for _, v in enumerate(sqrt_in):
        s += f"{round(v,4)},"
    f.write(s)
    f.write("};")

    f.write(f"float crossfade_exp_out[{sample_num}]={{")
    s = ""
    for _, v in enumerate(exp_out):
        s += f"{round(v,4)},"
    f.write(s)
    f.write("};")

    f.write(f"float crossfade_exp_in[{sample_num}]={{")
    s = ""
    for _, v in enumerate(exp_in):
        s += f"{round(v,4)},"
    f.write(s)
    f.write("};")

    f.write(f"float crossfade_line[{sample_num}]={{")
    s = ""
    for _, v in enumerate(line_out):
        s += f"{round(v,4)},"
    f.write(s)
    f.write("};")

    # # Plot the sine wave
    # plt.plot(x, cos_out, label="Cos", color="r")
    # plt.plot(x, sqrt_out, label="Sqrt", color="b")
    # plt.plot(x, line_out, label="Line", color="g")
    # plt.plot(x, exp_out, label="Exp", color="y")
    # plt.plot(x, cos_in, color="r")
    # plt.plot(x, sqrt_in, color="b")
    # plt.plot(x, 1 - line_out, color="g")
    # plt.plot(x, exp_in, color="y")
    # plt.legend()
    # plt.xlabel("X")
    # plt.ylabel("Y")
    # plt.title("Curves")
    # plt.show()


if __name__ == "__main__":
    run()
