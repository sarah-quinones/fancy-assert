from pathlib import Path
from shutil import copy
from subprocess import run
import os, sys

macro_prefix = "FANCY_ASSERT"

try:
    boost_dir = sys.argv[1]
except:
    boost_dir = "/usr/local/include/"

old_paths = [
    "/usr/local/include/boost/preprocessor/stringize.hpp",
    "/usr/local/include/boost/preprocessor/variadic/size.hpp",
    "/usr/local/include/boost/preprocessor/tuple/enum.hpp",
    "/usr/local/include/boost/preprocessor/seq/seq.hpp",
    "/usr/local/include/boost/preprocessor/detail/auto_rec.hpp",
    "/usr/local/include/boost/preprocessor/control/iif.hpp",
    "/usr/local/include/boost/preprocessor/punctuation/comma.hpp",
    "/usr/local/include/boost/preprocessor/list/adt.hpp",
    "/usr/local/include/boost/preprocessor/punctuation/comma_if.hpp",
    "/usr/local/include/boost/preprocessor/config/config.hpp",
    "/usr/local/include/boost/preprocessor/facilities/identity.hpp",
    "/usr/local/include/boost/preprocessor/punctuation/detail/is_begin_parens.hpp",
    "/usr/local/include/boost/preprocessor/array/elem.hpp",
    "/usr/local/include/boost/preprocessor/tuple/rem.hpp",
    "/usr/local/include/boost/preprocessor/arithmetic/dec.hpp",
    "/usr/local/include/boost/preprocessor/control/expr_iif.hpp",
    "/usr/local/include/boost/preprocessor/punctuation/remove_parens.hpp",
    "/usr/local/include/boost/preprocessor/control/detail/while.hpp",
    "/usr/local/include/boost/preprocessor/variadic/to_seq.hpp",
    "/usr/local/include/boost/preprocessor/logical/bitand.hpp",
    "/usr/local/include/boost/preprocessor/comparison/greater.hpp",
    "/usr/local/include/boost/preprocessor/comparison/less.hpp",
    "/usr/local/include/boost/preprocessor/tuple/to_array.hpp",
    "/usr/local/include/boost/preprocessor/facilities/overload.hpp",
    "/usr/local/include/boost/preprocessor/repetition/deduce_z.hpp",
    "/usr/local/include/boost/preprocessor/list/fold_right.hpp",
    "/usr/local/include/boost/preprocessor/array/size.hpp",
    "/usr/local/include/boost/preprocessor/cat.hpp",
    "/usr/local/include/boost/preprocessor/tuple/to_seq.hpp",
    "/usr/local/include/boost/preprocessor/array/data.hpp",
    "/usr/local/include/boost/preprocessor/detail/check.hpp",
    "/usr/local/include/boost/preprocessor/repetition/repeat.hpp",
    "/usr/local/include/boost/preprocessor/tuple/eat.hpp",
    "/usr/local/include/boost/preprocessor/list/detail/fold_right.hpp",
    "/usr/local/include/boost/preprocessor/variadic/elem.hpp",
    "/usr/local/include/boost/preprocessor/tuple/detail/is_single_return.hpp",
    "/usr/local/include/boost/preprocessor/comparison/not_equal.hpp",
    "/usr/local/include/boost/preprocessor/tuple/elem.hpp",
    "/usr/local/include/boost/preprocessor/logical/bool.hpp",
    "/usr/local/include/boost/preprocessor/logical/not.hpp",
    "/usr/local/include/boost/preprocessor/list/detail/fold_left.hpp",
    "/usr/local/include/boost/preprocessor/facilities/empty.hpp",
    "/usr/local/include/boost/preprocessor/control/while.hpp",
    "/usr/local/include/boost/preprocessor/list/fold_left.hpp",
    "/usr/local/include/boost/preprocessor/logical/compl.hpp",
    "/usr/local/include/boost/preprocessor/repetition/enum.hpp",
    "/usr/local/include/boost/preprocessor/list/reverse.hpp",
    "/usr/local/include/boost/preprocessor/detail/is_binary.hpp",
    "/usr/local/include/boost/preprocessor/repetition/for.hpp",
    "/usr/local/include/boost/preprocessor/seq/detail/is_empty.hpp",
    "/usr/local/include/boost/preprocessor/seq/for_each_i.hpp",
    "/usr/local/include/boost/preprocessor/repetition/detail/for.hpp",
    "/usr/local/include/boost/preprocessor/control/if.hpp",
    "/usr/local/include/boost/preprocessor/arithmetic/inc.hpp",
    "/usr/local/include/boost/preprocessor/comparison/less_equal.hpp",
    "/usr/local/include/boost/preprocessor/seq/elem.hpp",
    "/usr/local/include/boost/preprocessor/facilities/expand.hpp",
    "/usr/local/include/boost/preprocessor/tuple/size.hpp",
    "/usr/local/include/boost/preprocessor/array/pop_front.hpp",
    "/usr/local/include/boost/preprocessor/tuple/pop_front.hpp",
    "/usr/local/include/boost/preprocessor/arithmetic/sub.hpp",
    "/usr/local/include/boost/preprocessor/seq/for_each.hpp",
    "/usr/local/include/boost/preprocessor/seq/size.hpp",
    "/usr/local/include/boost/preprocessor/array/to_tuple.hpp",
    "/usr/local/include/boost/preprocessor/punctuation/is_begin_parens.hpp",
    "/usr/local/include/boost/preprocessor/debug/error.hpp",
]

old_paths = list(set(old_paths))

for i, name in enumerate(old_paths):
    if "config/config.hpp" in name:
        break
old_paths[i], old_paths[0] = old_paths[0], old_paths[i]

for name in old_paths:
    with open(name) as f:
        line: str = f.readline()
        while line:
            line = line.replace(
                "BOOST_PREPROCESSOR", f"Z_{macro_prefix}_BOOST_PREPROCESSOR"
            )
            line = line.replace("BOOST_PP_", f"Z_{macro_prefix}_IMPL_PP_")
            if "include" in line:
                line = ""
            print(line, end="")
            line = f.readline()
