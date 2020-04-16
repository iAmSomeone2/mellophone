#!/usr/bin/env python3

from sys import argv

def to_c_string(file_name):
    lines = []
    with open(file_name, "r") as sql:
        lines = sql.readlines()

    new_lines = []
    for line in lines:
        if line != "\n":
            tmp = line.replace("\"", "\\\"")
            new_lines.append("\"" + tmp.strip() + "\" \\")

    out_str = ""
    for line in new_lines:
        out_str += line + "\n"

    return out_str

if __name__ == "__main__":
    file_name = argv[1]

    print(to_c_string(file_name))