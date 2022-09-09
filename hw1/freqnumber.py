#!/usr/bin/env python3
import sys
from functools import *
from itertools import *
from typing import List, Tuple


def sort(lst: List[Tuple[str, int]]) -> List[Tuple[str, int]]:
    def low_level_sort(lst, idx, length):
        def compare(item, lst, acc, idx, length, added) -> List[Tuple[str, int]]:
            if idx == length:
                return acc + ([item] if not added else [])
            if lst[idx][1] >= item[1] and not added:
                return compare(item, lst, acc + [item], idx + 1, length, True)
            return compare(item, lst, acc + [lst[idx]], idx + 1, length, added)
        return reduce(lambda x, y: compare(y, x, [], 0, len(x), False), lst, [])
    return low_level_sort(lst, 0, len(lst))


def pair_up(lst_one, lst_two):
    def low_level_pair_up(lst_one, lst_two, idx, acc, length):
        if idx == length:
            return acc
        return low_level_pair_up(lst_one, lst_two, idx + 1, acc + [(lst_one[idx], lst_two[idx])], length)
    return low_level_pair_up(lst_one, lst_two, 0, [], len(lst_one))


def update_count(element, lst, keys):
    def low_level_update(element, lst, keys, acc, idx, length):
        if idx == length:
            return acc
        if keys[idx] == element:
            return low_level_update(element, lst, keys, acc + [lst[idx] + 1], idx + 1, length)
        return low_level_update(element, lst, keys, acc + [lst[idx]], idx + 1, length)
    return low_level_update(element, lst, keys, [], 0, len(keys))


def split(lst, split_char) -> List[str]:
    def low_level_split(lst, acc, curr_str, idx, length, split_char):
        if idx == length:
            return acc + [curr_str]
        if lst[idx] == split_char:
            return low_level_split(lst, acc + [curr_str], '', idx + 1, length, split_char)
        return low_level_split(lst, acc, curr_str + lst[idx], idx + 1, length, split_char)
    return low_level_split(lst, [], '', 0, len(lst), split_char)


def parse_arguments(arg: str) -> List[str]:
    converted_semis = list(filter(lambda x: x == ';', arg))
    if len(converted_semis) != 2:
        return []
    converted_equals = list(filter(lambda x: x == '=', arg))
    if len(converted_equals) != 3:
        return []
    key_values = split(arg, ';')
    if len(key_values) != 3:
        return []
    return key_values


def main():
    if len(sys.argv) != 2:
        print("Please run the python file like './file \"k=<k value>;input=<input file>.txt;output=<output file>.txt\"")
        return
    params = parse_arguments(sys.argv[1])
    if len(params) != 3:
        print("Please run the python file like './file \"k=<k value>;input=<input file>.txt;output=<output file>.txt\"")
        return
    k = params[0].split('=')[1]
    input_file = params[1].split('=')[1]
    output_file = params[2].split('=')[1]
    with open(input_file, 'r') as IPS:
        lines = list(map(lambda x: x.strip(), IPS.readlines()))

    def allowed(string):
        return len(string) > 0 and len(list(filter(lambda x: 48 <= ord(x) <= 57 or ord(x) == 46 or ord(x) == ord('-'), string))) == len(string)
    numbers = list(
        filter(
            allowed,
            reduce(
                lambda xt, line: xt + reduce(
                    lambda x, y: x + [y],
                    line.split(' '),
                    []
                ),
                lines,
                []
            )
        )
    )
    unique_numbers = list(
        filter(
            allowed,
            reduce(
                lambda xt, yt: xt + reduce(
                    lambda x, y: x +
                    [y] if y not in x and y not in xt else x,
                    yt.split(' '),
                    []
                ),
                lines,
                []
            )
        )
    )
    count = reduce(
        lambda x, y: update_count(y, x, unique_numbers),
        numbers,
        reduce(lambda _, __: _ + [0], unique_numbers, [])
    )
    pairs = pair_up(unique_numbers, count)
    integers = list(filter(lambda x: '.' not in x[0], pairs))
    reals = list(filter(lambda x: '.' in x[0], pairs))
    integers_sorted = sort(integers)
    reals_sorted = sort(reals)
    with open(output_file, 'w') as OFS:
        def output_to_file_int(idx, k_val, contents):
            if k_val == 0 or idx == -1:
                OFS.write(contents)
                return
            output_to_file_int(idx - 1, k_val - 1, contents +
                               f"{integers_sorted[idx][0]} {integers_sorted[idx][1]}\n")

        def output_to_file_real(idx, k_val, contents):
            if k_val == 0 or idx == -1:
                OFS.write(contents)
                return
            output_to_file_real(idx - 1, k_val - 1, contents +
                                f"{reals_sorted[idx][0]} {reals_sorted[idx][1]}\n")

        output_to_file_int(len(integers_sorted) - 1, int(k), "integer:\n")
        output_to_file_real(len(reals_sorted) - 1, int(k), "real:\n")


if __name__ == "__main__":
    main()
