package(default_visibility = ["//visibility:public"])

# Python binding support
cc_binary(
    name = "madara.so",
    srcs = glob(
        [
            "src/*.h",
            "src/*.cpp",
        ],
    ),
    copts = [
        "-w",
    ],
    linkshared = 1,
    deps = [
        "@madara//:madara_python",
    ],
)

py_library(
    name = "madara",
    data = [
        ":madara.so",
    ],
    imports = ["."],
)
