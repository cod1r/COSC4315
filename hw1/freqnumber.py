#!/usr/bin/env python3
import sys
from functools import *
from itertools import *
from typing import List, Tuple


def sort(lst: List[Tuple[str, int]]) -> List[Tuple[str, int]]:
    def low_level_sort(lst, idx, length):
        def compare(item, lst, acc, idx, length) -> List[Tuple[str, int]]:
            if idx == length:
                return acc + [item]
            if lst[idx][1] >= item[1]:
                if lst[idx][1] == item[1]:
                    if float(lst[idx][0]) > float(item[0]):
                        return compare(item, lst, acc + [lst[idx]], idx + 1, length)
                return compare(lst[idx], lst, acc + [item], idx + 1, length)
            return compare(item, lst, acc + [lst[idx]], idx + 1, length)
        return reduce(lambda x, y: compare(y, x, [], 0, len(x)), lst, [])
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
        return len(string) > 0 \
                and len(list(filter(lambda x: ord('0') <= ord(x) <= ord('9') or ord(x) == ord('.') or ord(x) == ord('-'), string))) == len(string)
    def is_sep(string):
        return len(string) > 0 \
                and not ((ord('A') <= ord(string) <= ord('Z')) or (ord('a') <= ord(string) <= ord('z')))

    split_symbols = list(
        filter(
            lambda sym: not allowed(sym) and is_sep(sym),
            reduce(
                lambda acc, line: acc + reduce(
                    lambda cl, c: cl +
                    [c] if c not in cl and c not in acc else cl,
                    line,
                    []),
                lines,
                []
            )
        )
    )

    numbers = list(
        filter(
            allowed,
            reduce(
                lambda acc, line: acc + reduce(
                    lambda prev_split, split_sym: reduce(
                        lambda acc_splits, part: acc_splits +
                        part.split(split_sym),
                        prev_split,
                        []
                    ),
                    split_symbols,
                    [line]
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
                lambda acc, elem: acc + ([elem] if elem not in acc else []),
                numbers,
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
    reals = list(filter(lambda x: '.' in x[0] and len(x[0]) >= 3, pairs))
    integers_sorted = sort(integers)
    reals_sorted = sort(reals)

    def group_lst(lst):
        def lower_level_group(lst, idx, length):
            def insert(
                pair: Tuple[str, int],
                group_lst: List[List[Tuple[str, int]]],
                acc: List[List[Tuple[str, int]]],
                idx,
                length,
                added
            ):
                if idx == length:
                    return acc + ([[pair]] if not added else [])
                if pair[1] == group_lst[idx][0][1] and not added:
                    return insert(pair, group_lst, acc + [group_lst[idx] + [pair]], idx + 1, length, True)
                return insert(pair, group_lst, acc + [group_lst[idx]], idx + 1, length, False)
            return reduce(lambda grouped, pair: insert(pair, grouped, [], 0, len(grouped), False), lst, [])
        return lower_level_group(lst, 0, len(lst))

    integers_sorted_grouped = group_lst(integers_sorted)
    reals_sorted_grouped = group_lst(reals_sorted)

    with open(output_file, 'w') as OFS:
        def empty_out(lst, start_contents) -> str:
            def lower_level_empty(lower_lst: List[Tuple[str, int]], acc: str, idx: int):
                if idx == -1:
                    return acc
                return lower_level_empty(lst, acc + f"{lst[idx][0]} {lst[idx][1]}\n", idx - 1)
            return lower_level_empty(lst, start_contents, len(lst) - 1)

        def output_to_file(lst: List[List[Tuple[str, int]]], idx, k_val: int, contents) -> None:
            if idx == -1 or k_val == 0:
                OFS.write(contents)
                return
            return output_to_file(lst, idx - 1, k_val - 1, empty_out(lst[idx], contents))
        output_to_file(integers_sorted_grouped, len(
            integers_sorted_grouped) - 1, int(k), 'integer:\n')
        output_to_file(reals_sorted_grouped, len(
            reals_sorted_grouped) - 1, int(k), 'real:\n')


if __name__ == "__main__":
    main()
