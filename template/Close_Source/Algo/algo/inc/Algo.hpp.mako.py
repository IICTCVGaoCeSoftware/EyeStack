import os

gen_file("Algo.hpp.makoignore", f"{cmake_vars['TEMPLATE_PROJECT_NAME']}.hpp")

os.mkdir(os.path.join(dst_dir, cmake_vars["TEMPLATE_PROJECT_NAME"]))
