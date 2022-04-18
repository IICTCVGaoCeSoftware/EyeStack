import os
import sys
from xml.etree import ElementTree as et

if len(sys.argv) != 2:
    print(
        """Argument(s): color

    color: '#ddd' or '#dddddd' or 'currentColor'
"""
    )
    exit()

color = sys.argv[1]
if not (color.startswith("#") and (len(color) == 4 or len(color) == 7)):
    print("invalid color format")
    exit()


def strip_svg(filepath):
    """修改 svg path 颜色，同时修剪 svg 内容"""

    tree = et.parse(filepath)
    root = tree.getroot()

    xmlns, _, root.tag = root.tag[1:].rpartition("}")
    assert xmlns == "http://www.w3.org/2000/svg"

    viewbox = root.get("viewBox")
    root.attrib = {"xmlns": xmlns, "viewBox": viewbox}

    assert len(root) == 1
    el_path = root[0]

    assert el_path.tag.endswith("path")
    el_path.tag = "path"

    assert {"d", "fill"}.union(el_path.attrib.keys()) == {"d", "fill"}
    el_path.set("fill", color)

    assert len(el_path) == 0

    with open(filepath, "wb") as file:
        file.write(et.tostring(root))


done_counter = 0
fail_counter = 0
longest = 0

for name in os.listdir():
    if not name.endswith(".svg"):
        continue

    longest = max(longest, len(name))
    try:
        strip_svg(name)
    except BaseException as e:
        fail_counter += 1
        print(f"{name} fail:".ljust(longest), e)
    else:
        done_counter += 1
        print(name.ljust(longest), end="\r")


print(f"Total: {done_counter} done, {fail_counter} fail")
