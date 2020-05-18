"""
    Copyright 2020 Brenden Davidson

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
"""

#!/usr/bin/env python3

from sys import argv

def to_c_string(file_name):
    """
        Converts a set of SQL queries to a C string.

        Arguments:

            file_name -- name of file to read from

        Returns:

            c-formatted string version of the text from the file .
    """

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